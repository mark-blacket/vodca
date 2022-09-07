#!/usr/bin/bash

set -xe
gcc -O2 -Wall main.c -o vodca `sdl2-config --cflags --libs`
