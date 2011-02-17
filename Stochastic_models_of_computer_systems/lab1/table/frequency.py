# -*- coding: utf-8 -*-

import sys
import locale
import collections

def main(args):
    full_contents = sys.stdin.read()
    #contents = filter(lambda x: len(x.encode("utf-8")) >= 2, full_contents)
    contents = full_contents
    counters = collections.defaultdict(int) # dict(character: count)
    for ch in contents:
        counters[ch] += 1
    
    ch_entries = sorted(counters.items(), cmp=(lambda a, b: -cmp(a[1], b[1])))
    total_count = len(contents)
    for ch, count in ch_entries:
        print("%s %4.1f %%"%(ch, 100.0 * count / total_count))

if __name__ == "__main__":
    main(sys.argv)
