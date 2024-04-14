ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)gcc
else
CC = gcc
endif

all: writer.c
	$(CC) writer.c -o writer

.PHONY: clean
clean:
	rm -f writer
