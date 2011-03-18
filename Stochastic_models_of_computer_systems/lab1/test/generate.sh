#!/bin/bash

i=1500

python gen_random.py $i > random_$i.txt

cat ../data/war_and_peace.txt | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
    sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
    dd of=war_and_peace_$i.txt bs=1 count=$i skip=10000

cat ../data/crime_and_punishment.txt | sed 's/[^A-Za-z ]/ /g' | tr '\n' ' ' | \
    sed -r 's/[ ]+/ /g' | tr 'A-Z' 'a-z' | \
    dd of=crime_and_punishment_$i.txt bs=1 count=$i skip=10000

# vim: set ts=2 sw=2 et:
