import sys
import random

for i in xrange(1000):
    sys.stdout.write(chr(ord('a') + int(round(random.randint(0, 25)))))

# vim: set ts=4 sw=4 et:
