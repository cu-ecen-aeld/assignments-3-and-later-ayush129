COMPILER_CALL=gcc writer.c -o writer
CROSS_COMPILE?=
COMPILER_FCALL=$(CROSS_COMPILE)$(COMPILER_CALL)
all: default
default:
	$(COMPILER_FCALL)

clean:
	rm -f writer
	rm -f *.o
