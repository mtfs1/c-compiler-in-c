CC = gcc
TARGET = main
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(OBJ)
	$(CC) -o $(TARGET) $(SRC)

%.o: %.c
	$(CC) -o $@ -c $^

clean: $(OBJ) $(TARGET)
	rm $(OBJ) $(TARGET)

