#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffered_read.h"
#include "lexer.h"
#include "token.h"
#include "trie.h"


int main(int argc, char *argv[]) {
  int err = tratamento_input(argc, argv);
  if (err)
    return 1.0;

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    printf("[WARNING] Houve um erro na hora "
      "de abrir o arquivo\n");
    return 1;
  }

  struct Buffer buffer = allocate_buffer(file);
  struct TokenBuffer *tb = allocate_token_buffer(1024);

  struct Trie *reserved = initialize_trie_of_lexemes();
  struct Trie *ptr = reserved;

  int i = 0;

  // The lexer needs to detect valid lexemes and turn them into tokens. In order
  // to do that, it needs to act like a state machine capable of accept a regular
  // language. To represent the current state, we can use a bunch of flags that
  // persists through iterations of a loop reading chars from the file. The
  // flags are updated based on the current state and the current char read by
  // the loop

  int is_reserved_flag = 1;
  int is_lexeme_start_flag = 1;
  int is_number_flag = 0;
  int is_float_flag = 0;
  int is_octal_flag = 0;
  int past_arithmetic_operator_flag = 0;
  int is_assignment_operator_flag = 0;
  int past_equal_flag = 0;
  int is_relational_operator_flag = 0;
  int past_negator = 0;
  int is_char_literal_flag = 0;
  int past_escape_char_flag = 0;
  int is_literal_flag = 0;
  int is_string_literal_flag = 0;
  int past_slash_flag = 0;
  int comment_until_end_of_line_flag = 0;
  int multi_line_comment_flag = 0;
  int directive_flag = 0;
  int past_asterisk_flag = 0;

  int line_number = 0;

  int size = 0;
  char lexeme_buffer[65] = {0};


  while(!buffer.is_eof) {

    if(comment_until_end_of_line_flag || directive_flag) {
      if(buffer.line_number == line_number) {
        get_next_char(&buffer);
        continue;
      }

      comment_until_end_of_line_flag = 0;
      directive_flag = 0;
    }

    char ch = get_next_char(&buffer);

    if(ch == '#' && is_lexeme_start_flag && !is_literal_flag) {
      directive_flag = 1;
      line_number = buffer.line_number;
      continue;
    }

    if(multi_line_comment_flag) {
      if(ch == '*')
        past_asterisk_flag = 1;

      if(ch == '/' && past_asterisk_flag)
        multi_line_comment_flag = 0;

      if(past_asterisk_flag && ch != '*')
        past_asterisk_flag = 0;

      continue;
    }

    if(ch != '/' && ch != '*' && past_slash_flag && !is_literal_flag) {
      past_slash_flag = 0;
      past_arithmetic_operator_flag = 1;
    }

    // Checks if it is needed to change current token being parsed
    // (NLP = and we are not in a literal parsing)
    //
    // * the current char is a separator NLP
    // * the current char is a lexeme by itself NLP
    // * the lexeme buffer has a single char lexeme NLP
    // * a second point is found during float parsing
    // * a non digit is found during a number parsing
    // * the current char is an arithmetic operator 
    //
    //    - that's neither a slash nor an asterisk that appeared just after a
    //    slash, because that would be framed as a comment start, and the slash
    //    in the lexeme buffer must not be computed as a token
    //
    //    - that's neither a plus after another plus nor a minus after another
    //    minus, because that would be framed as an increment or decrement, and
    //    the two operators must not be computed separately
    //
    //    - NLP
    //
    // * the lexeme buffer has an assignment operator NLP
    // * we just passed an equal symbol and we don't find another one NLP
    // * we are parsing a relational operator with two digits
    // * we are parsing a relational operator and the current char isn't an equal
    // * we are parsing a literal char and we find a non escaped single quote
    // * we are parsing a literal string and we find a non escaped double quote
    // * the past character is an arithmetic operator and
    //
    //    - the lexeme buffer size is equal to one, so we are neither
    //    computing the increment nor decrement yet
    //
    //    - the current char isn't an equal, so we are certain that's not the
    //    case of a special assignment
    //
    //    - the current char is different from the one on the buffer, so is
    //    neither the case of an increment nor decrement, 
    //
    // * the past character is an arithmetic operator and the lexeme buffer size
    // is equal to two
    //
    // In case of changing the current token being parsed, all flags
    // are reset for its original values. It's also important to note that the if
    // block doesn't parse the current char, even if it is a single char lexeme.
    // The if block only create tokens based on what is stored on the lexeme
    // buffer, the current char is used just to decide if the lexer may end the
    // current token being parsed and start a new one

    if((is_separator(ch) && !is_literal_flag)
        || (is_single_char_lexeme(ch) && !is_literal_flag)
        || (size == 1 && is_single_char_lexeme(lexeme_buffer[0])
          && !is_literal_flag)
        || (ch == '.' && is_float_flag)
        || (ch != '.' && !isdigit(ch) && is_number_flag)
        || (is_arithmetic_operator(ch)
          && !(ch == '/' && past_slash_flag)
          && !(ch == '*' && past_slash_flag)
          && !(ch == '+' && lexeme_buffer[0] == '+')
          && !(ch == '-' && lexeme_buffer[0] == '-')
          && !is_literal_flag)
        || is_assignment_operator_flag
        || (past_equal_flag && ch != '=')
        || (is_relational_operator_flag && size == 2)
        || (is_relational_operator_flag && size == 1 && ch != '=')
        || (is_char_literal_flag && !past_escape_char_flag && ch == '\'')
        || (is_string_literal_flag && !past_escape_char_flag && ch == '\"')
        || (past_arithmetic_operator_flag && size == 1 && ch != '='
          && !(lexeme_buffer[0] == '+' && ch == '+')
          && !(lexeme_buffer[0] == '-' && ch == '-'))
        || (past_arithmetic_operator_flag && size == 2)) {

      // When we find a separator just after another separator, the
      // lexeme buffer will be empty. That is common when we use several
      // spaces for tabulation

      if(size == 0 && is_separator(ch))
        continue;

      // Adds a null char after the last valid char on the lexeme buffer, making
      // the lexeme inside the buffer a valid null terminated string.

      lexeme_buffer[size] = 0;

      if(past_equal_flag && ch != '=' && !is_literal_flag)
        is_assignment_operator_flag = 1;

      if(size) {
        printf("%d: %s", i, lexeme_buffer);
        i++;
      }

      if(size) {
        if(is_reserved_flag && ptr->lex) {
          struct Token token = {
            .lex = ptr->lex,
            .str = NULL
          };
          push_token(tb, token);

        } else if(is_number_flag) {
          if(is_float_flag)
            printf(" [FLOAT]");
          else if(is_octal_flag)
            printf(" [OCTAL]");
          else
            printf(" [INTEGER]");

        } else if(size == 1 && is_single_char_lexeme(lexeme_buffer[0])) {
          if(lexeme_buffer[0] == '(')
            printf(" [OPEN_BRACKET]");
          if(lexeme_buffer[0] == ')')
            printf(" [CLOSE_BRACKET]");
          if(lexeme_buffer[0] == '{')
            printf(" [OPEN_CURLY_BRACKET]");
          if(lexeme_buffer[0] == '}')
            printf(" [CLOSE_CURLY_BRACKET]");
          if(lexeme_buffer[0] == '[')
            printf(" [OPEN_SQUARE_BRACKET]");
          if(lexeme_buffer[0] == ']')
            printf(" [CLOSE_SQUARE_BRACKET]");
          if(lexeme_buffer[0] == ';')
            printf(" [SEMI_COLON]");

        } else if(is_relational_operator_flag)
          printf(" [RELATIONAL_OP]");
        else if(is_assignment_operator_flag)
          printf(" [ASSIGNMENT_OP]");
        else if(is_char_literal_flag)
          printf(" [CHAR_LITERAL]");
        else if(is_string_literal_flag)
          printf(" [STRING_LITERAL]");
        else if(past_arithmetic_operator_flag)
          printf(" [ARITHMETIC_OP]");
        else
          printf(" [IDENTIFIER]");
      }

      ptr = reserved;

      is_reserved_flag = 1;
      is_lexeme_start_flag = 1;
      is_number_flag = 0;
      is_float_flag = 0;
      is_octal_flag = 0;
      past_arithmetic_operator_flag = 0;
      is_assignment_operator_flag = 0;
      past_equal_flag = 0;
      is_relational_operator_flag = 0;
      past_negator = 0;
      past_slash_flag = 0;

      if(size)
        printf("\n");

      size = 0;

      if(is_literal_flag) {
        is_literal_flag = 0;
        is_string_literal_flag = 0;
        is_char_literal_flag = 0;
        continue;
      }

      if(is_separator(ch))
        continue;
    }

    // Choices prior to adding the current char to the lexeme buffer. This
    // region is important if the current char must be ignored when parsing the
    // current token, as this char may represent quotes that start or end a
    // literal, or the start or end of a comment.

    if(!is_literal_flag && (ch == '\'' || ch == '\"')) {
      if(ch == '\'')
        is_char_literal_flag = 1;

      if(ch == '\"')
        is_string_literal_flag = 1;

      is_literal_flag = 1;
      is_lexeme_start_flag = 0;
      is_reserved_flag = 0;
      continue;
    }

    if(ch == '/') {
      if(past_slash_flag) {
        past_slash_flag = 0;
        comment_until_end_of_line_flag = 1;
        line_number = buffer.line_number;
        size = 0;
        continue;

      } else
        past_slash_flag = 1;
    }

    if(ch == '*' && past_slash_flag) {
      past_slash_flag = 0;
      multi_line_comment_flag = 1;
      size = 0;
      continue;
    }

    // Add current char to lexeme buffer. If the loop iteration reaches here, the
    // current char was judged able to be added to the lexeme buffer. Post this
    // operation, we recompute flags based on the current char and its relation
    // with current active flags

    lexeme_buffer[size] = ch;
    size++;

    if(is_literal_flag) {
      if(past_escape_char_flag) {
        past_escape_char_flag = 0;
        continue;
      }

      if(ch == '\\')
        past_escape_char_flag = 1;

      continue;
    }
    
    if(is_lexeme_start_flag) {
      is_lexeme_start_flag = 0;

      if(isdigit(ch)) {
        is_number_flag = 1;
        is_reserved_flag = 0;

        if(ch == '0')
          is_octal_flag = 1;
      }
    }

    if(is_arithmetic_operator(ch) && ch != '/' && !is_literal_flag)
      past_arithmetic_operator_flag = 1;

    if(is_relational_operator(ch))
      is_relational_operator_flag = 1;

    if(ch == '!')
      past_negator = 1;

    if(ch == '=') {
      if(past_arithmetic_operator_flag)
        is_assignment_operator_flag = 1;

      if(past_equal_flag || past_negator)
        is_relational_operator_flag = 1;

      past_equal_flag = 1;
    }

    if(ch == '.' && is_number_flag)
      is_float_flag = 1;

    if(is_reserved_flag) {
      int index = to_trie_index(ch);

      if (index < 0) {
        is_reserved_flag = 0;
        continue;
      }

      if(ptr->children[index])
        ptr = ptr->children[index];
    }
  }


  deallocate_buffer(buffer);
  fclose(file);

  return 0;
}

int tratamento_input(int argc, char *argv[]) {
  if (argc != 2) {
    printf("[WARNIG] Numero incorreto de argumentos\n\n");
    printf("Forma correta de utilização:\n");
    printf("  ./executavel nome_do_arquivo\n");
    return 1;
  }

  return 0;
}

int is_separator(char ch) {
  return (ch == ' ' || ch == '\t' || ch == '\n');
}

int is_bracket(char ch) {
  return (
    ch == '('
    || ch == ')'
    || ch == '{'
    || ch == '}'
    || ch == '['
    || ch == ']'
  );
}

int is_single_char_lexeme(char ch) {
  return (
    ch == ';'
    || ch == ','
    || is_bracket(ch)
  );
}

int is_arithmetic_operator(char ch) {
  return (
    ch == '*'
    || ch == '/'
    || ch == '+'
    || ch == '-'
    || ch == '%'
  );
}

int is_relational_operator(char ch) {
  return (
    ch == '>'
    || ch == '<'
  );
}

