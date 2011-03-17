# -*- coding: utf-8 -*-

import sys
import locale
import collections
import itertools

def main(args):
    full_contents = sys.stdin.read()
    #contents = filter(lambda x: len(x.encode("utf-8")) >= 2, full_contents)
    contents = full_contents

    # dict(character: count)
    counters = dict([(a + b, 0)
            for (a, b) in itertools.product(set(contents), repeat=2)])

    for ch in map(
	    lambda x: ''.join(x), 
	    zip(contents[:-1], contents[1:])):
        counters[ch] += 1
    
    ch_entries = sorted(counters.items(), cmp=(lambda a, b: -cmp(a[1], b[1])))
    total_count = len(contents)
    for ch, count in ch_entries:
        print("{0}\n{1:.16}".format(ch, float(count) / total_count))

if __name__ == "__main__":
    main(sys.argv)

# vim: set ts=4 sw=4 et:
