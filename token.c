#include <stdlib.h>

#include "token.h"


struct TokenBuffer *allocate_token_buffer(int size) {
  struct TokenBuffer *buffer =
    (struct TokenBuffer *) malloc(sizeof(struct TokenBuffer));

  buffer->capacity = size;
  buffer->size = 0;
  buffer->next = NULL;
  buffer->buff =
    (struct Token *) calloc(buffer->capacity, sizeof(struct Token));

  return buffer;
}

int push_token(struct TokenBuffer *tb, struct Token token) {
  if(tb->size < tb->capacity) {
    tb->buff[tb->size] = token;
    tb->size++;
    return 0;
  }

  if(!tb->next)
    tb->next = allocate_token_buffer(tb->capacity);

  return push_token(tb->next, token);
}

struct TokenBufferCursor *get_token_cursor(struct TokenBuffer *tb) {
  struct TokenBufferCursor *cur =
    (struct TokenBufferCursor *)malloc(sizeof(struct TokenBufferCursor));

  cur->root = tb;
  cur->actual_buffer = tb;
  cur->index = 0;

  return cur;
}

struct Token get_token(struct TokenBufferCursor *cur) {
  struct Token token = {0};
  if(!cur->actual_buffer)
    return token;

  token = cur->actual_buffer->buff[cur->index];
  cur->index++;

  if(cur->index == cur->actual_buffer->size)
    cur->actual_buffer = cur->actual_buffer->next;

  return token;
}

