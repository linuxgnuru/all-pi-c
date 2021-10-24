#!/bin/bash
for z in {0..10};
   do for x in {0..7};
      do gpio write $x up;
   done;
   sleep 0.4;
   for y in 7 6 5 4 3 2 1 0;
      do gpio write $y down;
   done;
done
