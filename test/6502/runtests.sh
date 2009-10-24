#!/bin/bash
# Filenames should not contain any dot other than the one in the "extension"

set -e
for BINARY in t/*.bin
do
    EXP_TRACE=`cut -d. -f1 <<< $BINARY`.trace
    ./$1 $BINARY $EXP_TRACE > /dev/null
done
