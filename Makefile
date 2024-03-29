PROG=brainfuck
SRCS=brainfuck.cpp
FLAGS=-Ofast -Wall -Wextra -Wpedantic
INST=/usr/local/bin

.PHONY: all
all:
	g++ -o $(PROG) $(FLAGS) $(SRCS)

.PHONY: accum
accum:
	g++ -o $(PROG) -D MODE_ACCUM $(FLAGS) $(SRCS)

.PHONY: test
test:
	cd tst; ./run-tests.sh

.PHONY: install
install:
	cp $(PROG) $(INST)

.PHONY: uninstall
uninstall:
	rm $(INST)/$(PROG)
