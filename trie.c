#include "trie.h"
#include <stdlib.h>

#include <stdio.h>


struct Trie *allocate_trie_node() {
  struct Trie *node = (struct Trie *)calloc(1, sizeof(struct Trie));
  return node;
}

int to_trie_index(char ch) {
  int val = ch - 'a';
  if (val < 0 || val > 25)
    return -1;

  return val;
}

int insert_in_trie(struct Trie *trie, char *str, lexeme lex, int i) {
  if(!str[i]) {
    trie->lex = lex;
    return 0;
  }

  int index = to_trie_index(str[i]);
  if(index < 0)
    return 1;

  if(!trie->children[index])
    trie->children[index] = allocate_trie_node();

  return insert_in_trie(trie->children[index], str, lex, i + 1);
}

struct Trie *initialize_trie_of_lexemes() {
  struct Trie *trie = allocate_trie_node();

  insert_in_trie(trie, "auto", AUTO, 0);
  insert_in_trie(trie, "double", DOUBLE, 0);
  insert_in_trie(trie, "int", INT, 0);
  insert_in_trie(trie, "struct", STRUCT, 0);
  insert_in_trie(trie, "break", BREAK, 0);
  insert_in_trie(trie, "else", ELSE, 0);
  insert_in_trie(trie, "long", LONG, 0);
  insert_in_trie(trie, "switch", SWITCH, 0);
  insert_in_trie(trie, "case", CASE, 0);
  insert_in_trie(trie, "enum", ENUM , 0);
  insert_in_trie(trie, "register", REGISTER , 0);
  insert_in_trie(trie, "typedef", TYPEDEF , 0);
  insert_in_trie(trie, "char", CHAR , 0);
  insert_in_trie(trie, "extern", EXTERN , 0);
  insert_in_trie(trie, "return", RETURN , 0);
  insert_in_trie(trie, "union", UNION , 0);
  insert_in_trie(trie, "const", CONST , 0);
  insert_in_trie(trie, "float", FLOAT , 0);
  insert_in_trie(trie, "short", SHORT , 0);
  insert_in_trie(trie, "unsigned", UNSIGNED , 0);
  insert_in_trie(trie, "continue", CONTINUE , 0);
  insert_in_trie(trie, "for", FOR , 0);
  insert_in_trie(trie, "signed", SIGNED , 0);
  insert_in_trie(trie, "void", VOID , 0);
  insert_in_trie(trie, "default", DEFAULT , 0);
  insert_in_trie(trie, "goto", GOTO , 0);
  insert_in_trie(trie, "sizeof", SIZEOF , 0);
  insert_in_trie(trie, "volatile", VOLATILE , 0);
  insert_in_trie(trie, "do", DO , 0);
  insert_in_trie(trie, "if", IF , 0);
  insert_in_trie(trie, "static", STATIC , 0);
  insert_in_trie(trie, "while", WHILE , 0);

  return trie;
}

