CC=gcc
CFLAGS=-I. -g
DEPS = $(wildcard *.h) $(wildcard */*.h)
SRC = $(wildcard *.c) $(wildcard */*.c)
OBJ = $(SRC:.c=.o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

viva: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	find . -name "*.o" -type f -delete
	rm -f viva