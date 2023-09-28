#include <stdio.h>
#include <stdlib.h>

#include "buffered_read.h"


struct Buffer allocate_buffer(FILE *file) {
  struct Buffer buffer;
  buffer.size = 0;
  buffer.capacity = 256;
  buffer.line_number = 1;
  buffer.char_number = 0;
  buffer.cursor_position = 0;
  buffer.buff = (char*) malloc(256);
  buffer.file = file;
  buffer.char_number_buff = 0;
  buffer.is_eof = 0;
  read_from_file_to_buffer(&buffer);
  return buffer;
}

void deallocate_buffer(struct Buffer buffer) {
  free(buffer.buff);
}

char get_next_char(struct Buffer *buffer) {
  if(buffer->char_number == buffer->cursor_position)
    read_from_file_to_buffer(buffer);

  if(buffer->char_number == buffer->cursor_position - 1)
    buffer->is_eof = feof(buffer->file);

  if(buffer->char_number_buff == buffer->size
      && buffer->size < buffer->capacity) {
    buffer->is_eof = 1;
    return EOF;
  }

  char ch = buffer->buff[buffer->char_number_buff];
  buffer->char_number_buff++;
  buffer->char_number++;

  if (ch == '\n')
    buffer->line_number++;

  return ch;
}

void read_from_file_to_buffer(struct Buffer *buffer) {
  if (feof(buffer->file))
    return;

  int num = fread(buffer->buff, 1,
                  buffer->capacity, buffer->file);
  buffer->cursor_position += num;
  buffer->size = num;
  buffer->char_number_buff = 0;
}

