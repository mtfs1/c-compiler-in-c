#include "lexemes.h"


struct Trie {
  struct Trie *children[26];
  lexeme lex;
};

struct Trie *allocate_trie_node();
int insert_in_trie(struct Trie *trie, char *str, lexeme lex, int i);
int to_trie_index(char ch);
struct Trie *initialize_trie_of_lexemes();

