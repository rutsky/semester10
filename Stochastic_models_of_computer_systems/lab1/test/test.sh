#!/bin/bash

prg=../build/src/main

cat test_text.txt | sed 's/[^A-Za-z ]//g' | tr -d '\n' | tr 'A-Z' 'a-z' | \
    $prg ../table/table.dat

# vim: set ts=2 sw=2 et:
