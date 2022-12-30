# brainfuck

Brainfuck interpreter written in C; uses a constant number of 8-bit cells and
precomputes matching brackets.

There is also an accumulator mode which can be built using the `accum` make
target. In this mode, the `.` command adds the value of the current cell to an
accumulator, the value of the accumulator is output in hexadecimal at the end of
program execution.

## Installation

Execute one of the following to build the interpreter in normal or accumulator
mode respectively:

    make        # normal mode
    make accum  # accum mode

This will create the `brainfuck` executable, which can be installed in the
`/usr/local/bin` directory manually or using:

    make install    # cp brainfuck /usr/local/bin
    make uninstall  # rm /usr/local/bin/brainfuck

## Usage

    brainfuck FILE

This will interpret the program in `FILE` and accept input from `stdin`.

Inputs to the interpreter are just characters, and as such it is difficult to
pass numbers as input directly, use `printf`:

    printf '\x1\xF' | brainfuck ...

Programs may additionally be passed directly to the interpreter (instead of
being written to/read from a file) as follows:

    brainfuck <(printf ',[.,]')
