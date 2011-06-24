#!/usr/bin/env python

import sys
import pprint

with open("mueller-base", 'r') as f:
    source = f.read()

words = filter(None, source.split("_____\n\n")[1:])

d = {}
for word_text in words:
    word, trans = word_text.split('\n\n', 1)
    #d[unicode(word, 'utf-8')] = unicode(trans, 'utf-8')
    d[word.lower()] = trans

def chunks(l, n):
    """ Yield successive n-sized chunks from l.
    """
    for i in xrange(0, len(l), n):
        yield l[i:i+n]

d_list = d.items()
d_list.sort()
for i, chunk in enumerate(chunks(d_list, 10000)):
    di = dict(chunk)
    
    with open("dict_{0}.py".format(i), 'w') as f:
        f.write("en2ru_{0} = {1}".format(i, pprint.pformat(di)))

# vim: set ts=4 sw=4 et:
