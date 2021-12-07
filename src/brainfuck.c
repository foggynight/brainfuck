#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PROG_INIT_SIZE (2 << 8)
#define DATA_SIZE (2 << 16)

char *prog;
size_t prog_ptr;
size_t prog_cnt;  // Number of commands in the program array.
size_t prog_size; // Current size of the program array.

char data[DATA_SIZE];
uint16_t data_ptr;

// Determine if a character is a valid command.
int is_command(char c) {
    static const char commands[] = { '>', '<', '+', '-', '.', ',', '[', ']' };
    for (int i = 0; i < 8; ++i)
        if (c == commands[i])
            return 1;
    return 0;
}

// Add a command to the prog array, increasing its size if necessary.
void add_command(char c) {
    if (prog_cnt >= prog_size) {
        prog_size *= 2;
        prog = realloc(prog, prog_size);
        if (prog == NULL) {
            fputs("brainfuck: memory error\n", stderr);
            exit(1);
        }
    }
    prog[prog_cnt] = c;
    ++prog_cnt;
}

// Parse the program contained within the file at path.
void parse_program(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "brainfuck: failed to open file: %s\n", path);
        exit(1);
    }
    if (fseek(fp, 0L, SEEK_END)) {
        fprintf(stderr, "brainfuck: file read error: %s\n", path);
        exit(1);
    }
    const long file_size = ftell(fp);
    if (file_size == -1L) {
        fprintf(stderr, "brainfuck: file read error: %s\n", path);
        exit(1);
    }
    rewind(fp);
    char *file_str = malloc(file_size);
    if (file_str == NULL) {
        fputs("brainfuck: memory error\n", stderr);
        exit(1);
    }
    if (fread(file_str, 1, file_size, fp) != file_size) {
        fprintf(stderr, "brainfuck: file read error: %s\n", path);
        exit(1);
    }
    for (int i = 0; i < file_size; ++i)
        if (is_command(file_str[i]))
            add_command(file_str[i]);
    free(file_str);
}

// Execute the open bracket command.
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
    fputs("brainfuck: unmatched open bracket\n", stderr);
    exit(1);
}

// Execute the closed bracket command.
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
    fputs("brainfuck: unmatched closed bracket\n", stderr);
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fputs("brainfuck: invalid argument count\n", stderr);
        return 1;
    }
    prog_size = PROG_INIT_SIZE;
    prog = malloc(PROG_INIT_SIZE);
    if (prog == NULL) {
        fputs("brainfuck: memory error\n", stderr);
        return 1;
    }
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
    return 0;
}
