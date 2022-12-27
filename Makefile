PROG=brainfuck
SRCS=brainfuck.c
FLAGS=-Ofast -Wall -Wextra -Wpedantic

.PHONY: all
all:
	gcc -o $(PROG) $(FLAGS) $(SRCS)

.PHONY: accum
accum:
	gcc -o $(PROG) -D MODE_ACCUM $(FLAGS) $(SRCS)

.PHONY: test
test:
	cd tst; ./run-tests.sh
