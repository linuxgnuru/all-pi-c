#!/bin/bash

# abs=-42; echo ${abs#-}

#for z in {0..10}; do
   for x in {0..1}; do
       for y in 0 1 2 3 4 5 6 7; do # 6 5 4 3 2 1 0; do
           if [ $x == 0 ] ; then
              gpio write $y up;
              sleep .03
           else
              abs=$((y - 7))
              newy=${abs#-}
              gpio write $newy down;
              sleep .03
           fi
       done
       #echo $x
   done
#done
