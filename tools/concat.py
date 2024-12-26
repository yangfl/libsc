#!/usr/bin/env python3

import sys


with open(sys.argv[-1], 'w') as f:
    for fn in sorted(sys.argv[1:-1]):
        print(f'#include "{fn}"', file=f)
        #with open(fn) as g:
        #    f.write(g.read())
