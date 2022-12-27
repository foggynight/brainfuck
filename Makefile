all:
	gcc -o brainfuck -Ofast -Wall -Wextra -Wpedantic src/brainfuck.c

.PHONY: test
test:
	cd tst; ./run-tests.sh
