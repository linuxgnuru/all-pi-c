#!/bin/bash

for x in 0 1 3 7 15 31 63 127 255;
 do
 ./led-pcf $x;
 sleep .03;
done
