#!/bin/bash

cat pg2600.txt | sed 's/[^A-Za-z ]//g' | tr -d '\n' | tr 'A-Z' 'a-z' | \
    python frequency.py > table.dat

# vim: set ts=2 sw=2 et:
