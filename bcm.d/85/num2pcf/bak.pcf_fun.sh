#!/bin/bash

for z in {0..7};
   do
   gpio write $z up;
   for x in 255 127 63 31 15 7 3 1 0 1 3 7 15 31 63 127 255;
      do
      ./led-pcf $x;
      sleep .03;
   done;
   for y in 254 252 248 240 224 192 128 0 128 192 224 240 248 252 254;
      do
      ./led-pcf $y;
      sleep .03;
   done;
done
./led-pcf 255
for z in {0..7};
do gpio write $z down;
done
