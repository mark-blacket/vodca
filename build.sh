#!/usr/bin/bash

set -xe
gcc -O2 -Wall main.c -o main `sdl2-config --cflags --libs`
