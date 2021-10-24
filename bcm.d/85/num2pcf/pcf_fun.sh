#!/bin/bash

#for fu in {0..7}; do ./num2pcf $fu; done

./led-pcf 255

for z in {0..7};
   do
   gpio write $z up;
   for x in {0..7}; # 255 127 63 31 15 7 3 1 0 1 3 7 15 31 63 127 255;
      do
	  echo "x: $x"
          ./num2pcf $x;
          sleep .03;
   done;
   for a in {0..7}; # 255 127 63 31 15 7 3 1 0 1 3 7 15 31 63 127 255;
      do
      	  abs=$((a - 7))
	  newa=${abs#-}
	  echo "newa: $newa"
          ./num2pcf $newa;
          sleep .03;
   done;
   for y in {0..7}; #254 252 248 240 224 192 128 0 128 192 224 240 248 252 254;
      do
	      ./num2pcf $y;
	      echo "y: $y"
	      sleep .03;
   done;
   for b in {0..7}; #254 252 248 240 224 192 128 0 128 192 224 240 248 252 254;
      do
          abs=$((b - 7))
	  newb=${abs#-}
	  echo "newb: $newb"
          ./num2pcf $newb;
          sleep .03;
    done;
done

./led-pcf 255

for z in {0..7}; do gpio write $z down; done
