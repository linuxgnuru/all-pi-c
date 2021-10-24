#!/bin/bash

#   for x in 255 127 63 31 15 7 3 1 0 1 3 7 15 31 63 127 255; do
#      ./led-pcf $x;
#      sleep .03;
#   done;
for y in 254 252 248 240 224 192 128 0 128 192 224 240 248 252 254; do
   echo $y;
    ./led-pcf $y;
   sleep .03;
done
#./led-pcf 255
