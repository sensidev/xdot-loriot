#!/usr/bin/python

import sys

x = str(sys.argv[1])

l = ['0x%s%s' % (x[i], x[i+1]) for i in range(0,len(x)-1, 2)]
ret = ', '.join(l)

print '{%s};' % ret
