#!/bin/sh

gcc -o long-to-size_t long-to-size_t.c
./long-to-size_t
rm long-to-size_t
