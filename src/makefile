CC=gcc
CFLAGS=-I. -g -O3
DEPS = termui/termui.h termui/native_termui.h termui/termui_types.h textengine/textengine.h
SRC = termui/posix_native_termui.c termui/win32_native_termui.c termui/termui.c textengine/textengine.c viva.c
OBJ = $(SRC:.c=.o)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

viva: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	find . -name "*.o" -type f -delete
	rm -f viva