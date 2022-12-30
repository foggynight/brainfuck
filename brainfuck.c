#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_PROG_SIZE (1 << 8)
#define DATA_SIZE (1 << 16)

#define error(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while(0)

char *prog;
size_t prog_ptr;
size_t prog_cnt; // Number of commands in the program array.
size_t prog_siz; // Current size of the program array.

char data[DATA_SIZE];
size_t data_ptr;

// matches[i] = { if prog[i] is bracket: index of matching bracket
//              { otherwise:             0
size_t *matches;

#ifdef MODE_ACCUM
char accum;
#endif

int is_command(char c) { return strchr("<>+-[],.", c) != NULL; }

void add_command(char c) {
    if (prog_cnt >= prog_siz) {
        prog_siz *= 2;
        prog = realloc(prog, prog_siz);
        if (prog == NULL) error("brainfuck: memory error\n");
    }
    prog[prog_cnt] = c;
    ++prog_cnt;
}

void parse_program(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) error("brainfuck: failed to open file: %s\n", path);
    for (char c; (c = fgetc(fp)) != EOF;)
        if (is_command(c)) add_command(c);
    if (fclose(fp)) error("brainfuck: failed to close file: %s\n", path);
}

void compute_matches(void) {
    matches = calloc(prog_siz, sizeof(size_t));
    if (matches == NULL)
        error("brainfuck: memory error\n");
    for (prog_ptr = 0; prog_ptr < prog_cnt; ++prog_ptr) {
        size_t counter = 0;
        if (prog[prog_ptr] == '[') {
            for (size_t i = prog_ptr + 1; i < prog_cnt; ++i) {
                if (prog[i] == '[')
                    ++counter;
                else if (prog[i] == ']') {
                    if (counter == 0) {
                        matches[prog_ptr] = i;
                        break;
                    }
                    --counter;
                }
            }
            if (matches[prog_ptr] == 0)
                error("brainfuck: unmatched open bracket\n");
        } else if (prog[prog_ptr] == ']') {
            matches[prog_ptr] = prog_ptr;
            for (size_t i = prog_ptr - 1; 1; --i) {
                if (prog[i] == ']')
                    ++counter;
                else if (prog[i] == '[') {
                    if (counter == 0) {
                        matches[prog_ptr] = i;
                        break;
                    }
                    --counter;
                }
                if (i == 0) break;
            }
            if (matches[prog_ptr] == prog_ptr)
                error("brainfuck: unmatched closed bracket\n");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) error("brainfuck: invalid argument count\n");
    prog = malloc((prog_siz = INIT_PROG_SIZE));
    if (prog == NULL) error("brainfuck: memory error\n");
    parse_program(argv[1]);
    compute_matches();
    int inp; // Used in ',' word.
    for (prog_ptr = 0; prog_ptr < prog_cnt; ++prog_ptr) {
        switch(prog[prog_ptr]) {
        case '>': ++data_ptr; break;
        case '<': --data_ptr; break;
        case '+': ++data[data_ptr]; break;
        case '-': --data[data_ptr]; break;
        case '[': if (data[data_ptr] == 0) prog_ptr = matches[prog_ptr]; break;
        case ']': prog_ptr = matches[prog_ptr] - 1; break;
        case ',': data[data_ptr] = ((inp = getchar()) == EOF) ? 0 : inp; break;
#ifdef MODE_ACCUM
        case '.': accum += data[data_ptr]; break;
#else
        case '.': putchar(data[data_ptr]); break;
#endif
        }
    }
#ifdef MODE_ACCUM
    printf("%X", accum);
#endif
    return 0;
}
