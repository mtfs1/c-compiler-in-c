#include "lexemes.h"


struct Token {
  lexeme lex;
  void *str;
};

struct TokenBuffer {
  int capacity;
  int size;
  struct TokenBuffer *next;
  struct Token *buff;
};

struct TokenBufferCursor {
  struct TokenBuffer *root;
  struct TokenBuffer *actual_buffer;
  int index;
};

struct TokenBuffer *allocate_token_buffer(int size);
int push_token(struct TokenBuffer *tb, struct Token token);
struct TokenBufferCursor *get_token_cursor(struct TokenBuffer *tb);
struct Token get_token(struct TokenBufferCursor *cur);

