#!/bin/bash

# Note: tr -d 'euioazqjxkv'

cat ../data/war_and_peace.txt | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
    sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
    python frequency.py > table.dat

cat ../data/war_and_peace.txt | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
    sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
    python frequency2.py > table2.dat

# vim: set ts=2 sw=2 et:
