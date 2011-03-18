import sys
import random

# TODO
n = int(sys.argv[1])

random.seed(0)

chars = [chr(ord('a') + i) for i in range(26)] + [' ']

for i in xrange(n):
    sys.stdout.write(random.choice(chars))

# vim: set ts=4 sw=4 et:
