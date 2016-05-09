#!/bin/bash
find . \( -name \*.c -or -name \*.cpp -or -name \*.cc -or -name \*.h \) | xargs -n12 -P4 cf -i
