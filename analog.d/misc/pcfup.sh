#!/bin/bash

for x in 255 127 63 31 15 7 3 1 0;
 do
 ./led-pcf $x;
 sleep .03;
done
