#!/usr/bin/python
import sys, random

def gen_pattern(k, w):
    return ''.join(random.choices("10", cum_weights=[w, 1], k=k))

if __name__ == "__main__":
    if len(sys.argv) > 2:
        print(gen_pattern(int(sys.argv[1]), float(sys.argv[2]) % 1))
    else:
        print(f"Usage: {sys.argv[0]} n_total ones_weight")
