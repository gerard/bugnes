#!/bin/bash
URL="http://www.6502asm.com/examples"

if [ $1 = "--download" ]
then
    [ -z "$2" ] && exit 1
    wget $URL/$2
    exit 0
fi
[ -z "$1" ] && exit 1

ASM=$1.asm
CRASM=$1.crasm
SREC=$1.srec
BIN=$1.bin

cd `dirname $0`
[ -f $BIN ] && echo "File already assembled" && exit 1
[ -f $ASM ] || wget $URL/$ASM
[ -f $CRASM ] && rm -f $CRASM


# Code generation starts here
echo "cpu 6502" >> $CRASM
printf "\t* = \$600\n" >> $CRASM
# Declare lo and hi labels in the cpy header
grep '#<' $ASM | sed 's/^.*#<\([a-zA-Z0-9]*\)/\t\1lo = \1 \& 0xFF/' >> $CRASM
grep '#>' $ASM | sed 's/^.*#>\([a-zA-Z0-9]*\)/\t\1hi = (\1 \& 0xFF00) >> 8/' >> $CRASM
echo "code" >> $CRASM

# Suppress ^M, comments, empty lines, and then...
# Remove empty dcb directives that segfault crasm (yep, really...)
# Reformat labels, db/dcb directives and lo/hi labels
# Splitted for debugging purposes: is the longest sed line I've ever written...
cat $ASM | \
sed -e 's///' | \
sed -e 's/\;.*//' | \
sed -e '/^[[:blank:]]*$/d' | \
sed -e '/^[[:blank:]]*dcb[[:blank:]]*$/d' | \
sed -e 's/:/= \*\n/' | \
sed -e 's/dcb/db/' | \
sed -e 's/#<\([a-zA-Z0-9]*\)/#\1lo/' | \
sed -e 's/#>\([a-zA-Z0-9]*\)/#\1hi/' | \
cat >> $CRASM

echo "code" >> $CRASM


# Now assembly and conert to binary
crasm -s -l -x -o $SREC $CRASM > /dev/null
srec_cat $SREC -output $BIN -binary

# If successful delete intermediate steps
[ -f $BIN ] && rm -f $ASM $CRASM $SREC
