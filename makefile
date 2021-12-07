all:
	gcc -o brainfuck -Ofast -Wall -Wextra -Wpedantic src/brainfuck.c

test:
	cd tst; ./run-tests.sh
