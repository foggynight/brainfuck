#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROG_INIT_SIZE (2 << 8)
#define DATA_SIZE (2 << 16)

#define error(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while(0)

char *prog;
size_t prog_ptr;
size_t prog_cnt; // Number of commands in the program array.
size_t prog_siz; // Current size of the program array.

char data[DATA_SIZE];
uint16_t data_ptr;

// Each element in this array contains the index within prog of the bracket
// which matches the bracket at the same index within prog as the index of the
// element in this array. If the character within prog at a given index of this
// array is not a bracket, the value at that index in this array is zero.
size_t *matches;

int is_command(char c) {
    static const char commands[] = { '>', '<', '+', '-', '.', ',', '[', ']' };
    for (int i = 0; i < 8; ++i)
        if (c == commands[i])
            return 1;
    return 0;
}

void add_command(char c) {
    if (prog_cnt >= prog_siz) {
        prog_siz *= 2;
        prog = realloc(prog, prog_siz);
        if (prog == NULL)
            error("brainfuck: memory error\n");
    }
    prog[prog_cnt] = c;
    ++prog_cnt;
}

void parse_program(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
        error("brainfuck: failed to open file: %s\n", path);
    if (fseek(fp, 0L, SEEK_END) != 0)
        error("brainfuck: file read error: %s\n", path);
    const long file_size = ftell(fp);
    if (file_size == -1L)
        error("brainfuck: file read error: %s\n", path);
    rewind(fp);
    char *file_str = malloc(file_size);
    if (file_str == NULL)
        error("brainfuck: memory error\n");
    if (fread(file_str, 1, file_size, fp) != (size_t)file_size)
        error("brainfuck: file read error: %s\n", path);
    for (int i = 0; i < file_size; ++i)
        if (is_command(file_str[i]))
            add_command(file_str[i]);
    free(file_str);
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
                if (i == 0)
                    break;
            }
            if (matches[prog_ptr] == prog_ptr)
                error("brainfuck: unmatched closed bracket\n");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2)
        error("brainfuck: invalid argument count\n");
    prog = malloc((prog_siz = PROG_INIT_SIZE));
    if (prog == NULL)
        error("brainfuck: memory error\n");
    parse_program(argv[1]);
    compute_matches();
    for (prog_ptr = 0; prog_ptr < prog_cnt; ++prog_ptr) {
        switch(prog[prog_ptr]) {
            case '>': ++data_ptr; break;
            case '<': --data_ptr; break;
            case '+': ++data[data_ptr]; break;
            case '-': --data[data_ptr]; break;
            case '.': putchar(data[data_ptr]); break;
            case ',': data[data_ptr] = getchar(); break;
            case '[': if (data[data_ptr] == 0)
                          prog_ptr = matches[prog_ptr];
                      break;
            case ']': prog_ptr = matches[prog_ptr] - 1; break;
        }
    }
}
