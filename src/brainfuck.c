#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROG_INIT_SIZE (2 << 8)
#define DATA_SIZE (2 << 16)

#define error(...) do { fprintf(stderr, __VA_ARGS__); exit(1); } while(0)

char *prog;
size_t prog_ptr;
size_t prog_cnt;  // Number of commands in the program array.
size_t prog_size; // Current size of the program array.

char data[DATA_SIZE];
uint16_t data_ptr;

int is_command(char c) {
    static const char commands[] = { '>', '<', '+', '-', '.', ',', '[', ']' };
    for (int i = 0; i < 8; ++i)
        if (c == commands[i])
            return 1;
    return 0;
}

void add_command(char c) {
    if (prog_cnt >= prog_size) {
        prog_size *= 2;
        prog = realloc(prog, prog_size);
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
    if (fseek(fp, 0L, SEEK_END))
        error("brainfuck: file read error: %s\n", path);
    const long file_size = ftell(fp);
    if (file_size == -1L)
        error("brainfuck: file read error: %s\n", path);
    rewind(fp);
    char *file_str = malloc(file_size);
    if (file_str == NULL)
        error("brainfuck: memory error\n");
    if (fread(file_str, 1, file_size, fp) != file_size)
        error("brainfuck: file read error: %s\n", path);
    for (int i = 0; i < file_size; ++i)
        if (is_command(file_str[i]))
            add_command(file_str[i]);
    free(file_str);
}

void exec_open_bracket(char **com_ptr) {
    if (data[data_ptr] != 0)
        return;
    size_t match = 0;
    for (char *com = *com_ptr + 1; com < prog + prog_cnt; ++com) {
        if (*com == '[')
            ++match;
        else if (*com == ']') {
            if (match == 0) {
                *com_ptr = com;
                return;
            }
            --match;
        }
    }
    error("brainfuck: unmatched open bracket\n");
}

void exec_closed_bracket(char **com_ptr) {
    size_t match = 0;
    for (char *com = *com_ptr - 1; com >= prog; --com) {
        if (*com == ']')
            ++match;
        else if (*com == '[') {
            if (match == 0) {
                *com_ptr = com - 1;
                return;
            }
            --match;
        }
    }
    error("brainfuck: unmatched closed bracket\n");
}

int main(int argc, char **argv) {
    if (argc != 2)
        error("brainfuck: invalid argument count\n");
    prog_size = PROG_INIT_SIZE;
    prog = malloc(PROG_INIT_SIZE);
    if (prog == NULL)
        error("brainfuck: memory error\n");
    parse_program(argv[1]);
    for (char *com = prog; com < prog + prog_cnt; ++com) {
        switch(*com) {
            case '>': ++data_ptr; break;
            case '<': --data_ptr; break;
            case '+': ++data[data_ptr]; break;
            case '-': --data[data_ptr]; break;
            case '.': putchar(data[data_ptr]); break;
            case ',': data[data_ptr] = getchar(); break;
            case '[': exec_open_bracket(&com); break;
            case ']': exec_closed_bracket(&com); break;
        }
    }
}
