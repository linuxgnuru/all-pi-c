#!/bin/bash

array[0]=127
array[1]=63
array[2]=31
array[3]=15
array[4]=7
array[5]=3
array[6]=1
array[7]=0

for x in {0..7}; do gpio mode $x out; done
for z in {0..7};
   do
     for x in 0 1 2 3 4 5 6 7;
       do
         gpio write $x up;
         sleep .03;
     done;
     for y in 7 6 5 4 3 2 1 0;
        do
          gpio write $y down;
         sleep .03;
     done;
     for x in 7 6 5 4 3 2 1 0;
       do
         gpio write $x up;
         sleep .03;
     done;
     for y in 0 1 2 3 4 5 6 7;
        do
          gpio write $y down;
         sleep .03;
     done;
     ./led-pcf ${array[$z]};
done
./led-pcf 255
