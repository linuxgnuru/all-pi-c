#!/usr/bin/perl

$p = "./led-pcf ";

$toggle = 0;
#    print "$fu\t";
#foreach $a (0..255) { foreach $b (0..127) { foreach $c (0..63) { foreach $d (0..31) { foreach $e (0..15) { foreach $f (0..7) { foreach $g (0..3) { foreach $h (0..1) { $num = abs($a - 255); $array[$num][$f] = $h; }  } } } } } } }

$h = 0;
foreach $a (0..255) {
  foreach $f (0..7) {
    $num = abs($a - 255);
    $array[$num][$f] = $h;
  }
  $h = ($h == 0 ? 1 : 0);
}

foreach $a (0..255) {
  foreach $f (0..7) {
   print "$array[$a][$f]\t";
  }
  print "\n";
 }

__DATA__
foreach $item (0..255) {
  $np = $p . $item;
  system("$np");
  foreach $num (0..7) {
   print "$array[$num]\t";
  }
  print "\n";
}


__DATA__
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
