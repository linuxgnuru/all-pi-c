#!/bin/bash
#up
for x in 255 127 63 31 15 7 3 1 0;
 do ./led-pcf $x;
 sleep .03;
done
#down
for x in 0 1 3 7 15 31 63 127 255;
 do ./led-pcf $x;
 sleep .03;
done
