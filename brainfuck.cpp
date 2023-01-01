#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define error(...) do {\
    fprintf(stderr, "brainfuck: " __VA_ARGS__); exit(1);\
} while (0)

std::vector<char> prog;

#define DATA_SIZE (1 << 16)
char data[DATA_SIZE];
size_t data_ptr;

// match[i] = { prog[i] is bracket: index of matching bracket
//            { else:               undefined
size_t *match;

#ifdef MODE_ACCUM
char accum;
#endif

bool is_cmd(char c) { return strchr("<>+-[],.", c) != NULL; }
void parse_program(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) error("failed to open file: %s\n", path);
    for (char c; (c = fgetc(fp)) != EOF;)
        if (is_cmd(c)) prog.push_back(c);
    if (fclose(fp)) error("failed to close file: %s\n", path);
}

void matching_brackets() {
    match = (size_t*)malloc(prog.size() * sizeof(size_t));
    if (match == NULL) error("memory error\n");
    std::vector<size_t> stack;
    for (size_t i = 0; i < prog.size(); ++i) {
        switch (prog[i]) {
            case '[': stack.push_back(i); break;
            case ']':
                if (stack.size() == 0) error("unmatched right bracket\n");
                const size_t left = stack.back();
                stack.pop_back();
                match[left] = i;
                match[i] = left;
                break;
        }
    }
    if (stack.size() > 0) error("unmatched left bracket\n");
}

int main(int argc, char **argv) {
    if (argc != 2) error("invalid argument count\n");
    parse_program(argv[1]);
    matching_brackets();
    int inp; // Used in ',' word.
    for (size_t prog_ptr = 0; prog_ptr < prog.size(); ++prog_ptr) {
        switch (prog[prog_ptr]) {
            case '<': --data_ptr; break;
            case '>': ++data_ptr; break;
            case '-': --data[data_ptr]; break;
            case '+': ++data[data_ptr]; break;
            case '[': if (data[data_ptr] == 0) prog_ptr = match[prog_ptr]; break;
            case ']': prog_ptr = match[prog_ptr] - 1; break;
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
