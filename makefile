CC = gcc
CFLAGS = -Wall -g -Werror

showFDtables: a2.o
	$(CC) $(CFLAGS) -o $@ $^

showFDtables.o: showFDtables.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm *.o
