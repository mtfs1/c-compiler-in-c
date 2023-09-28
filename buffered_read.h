#include <stdio.h>


struct Buffer {
  int size;
  int capacity;
  int char_number;
  int line_number;

  FILE *file;
  int cursor_position;
  int char_number_buff;
  int is_eof;

  char *buff;
};

struct Buffer allocate_buffer(FILE *file);
char get_next_char(struct Buffer *buffer);
void deallocate_buffer(struct Buffer buffer);
void read_from_file_to_buffer(struct Buffer *buffer);

