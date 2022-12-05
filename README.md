# vodca - variable one-dimensional cellular automaton

Takes a binary pattern as an input, recursively applies the rule and visualises it using SDL
A python script (pattern.py) can be used to generate randomized input patterns

Example:
vodca -r90 -w801 -h800 1001001001

Flags:
 - -r    - rule number, must be between 0 and 255
 - -w    - width in pixels
 - -h    - height in pixels
 - -L/-R - left/right alignment, by default the pattern is centered
