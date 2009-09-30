#!/bin/bash

cd `dirname $0`
[ $1 = "-c" ] && echo "Cleaning binaries" && rm *.bin && exit
for ASM in alive backandforth byterun colors compo-May07-1st compo-May07-2nd \
         compo-May07-3rd demoscene disco fullscreenlogo noise random rorshach \
         selfmodify skier softsprites spacer splashscreen starfield2d \
         zookeeper
do
    ./00compile-6502-example.sh $ASM
done
