#!/usr/bin/perl
#use strict;
#use warnings;

#use Time::HiRes qw(usleep nanosleep);

$gc = "gpio write ";
$u = " up";
$d = " down";
$pc = "./led-pcf ";

@gx = (0, 1, 2, 3, 4, 5, 6, 7);
@gy = reverse(@gx);
@px = (255, 127, 63, 31, 15, 7, 3, 1, 0); #, 1, 3, 7, 15, 31, 63, 127, 255);
@py = reverse(@px);

$debug = ($ARGV[0] eq "debug" ? 1 : 0);

foreach $gpio (0..7) {
 system("gpio mode $gpio out");
 system("gpio write $gpio down");
}
system("$pc 255");

$swap = 0;
#$sleep = 5000000; # 1 milisecond = 1000 microseconds
$sleep = 0.05;
foreach $num (0..10) {
  if ($swap == 0) {
    print "swap 0\n" if ($debug == 1);
    foreach $led (0..7) {
      #print "u gpio-$led\n";
      $ngc = $gc . $gx[$led] . $u;
      print "gx: $gx[$led]   " if ($debug == 1);
      system($ngc) if ($debug == 0);
      select(undef, undef, undef, $sleep);
    }
    print "\n" if ($debug == 1);
    foreach $led (0..8) {
      #print "on pcf-$led ($px[$led])\n";
      $npc = $pc . $px[$led];
      print "px: $px[$led]   " if ($debug == 1);
      system($npc) if ($debug == 0);
      #usleep($sleep);
      select(undef, undef, undef, $sleep);
    }
    print "\n" if ($debug == 1);
    $swap = 1;
  } else {
    print "swap 1\n" if ($debug == 1);
    foreach $led (0..8) {
      #print "off pcf-$led ($py[$led])\n";
      $npc = $pc . $py[$led];
      print "py: $py[$led]   " if ($debug == 1);
      system($npc) if ($debug == 0);
      #usleep($sleep);
      select(undef, undef, undef, $sleep);
    }
    print "\n" if ($debug == 1);
    foreach $led (0..7) {
      #print "d-gpio-$led\n";
      $ngc = $gc . $gy[$led] . $d;
      print "gy $gy[$led]   " if ($debug == 1);
      system($ngc) if ($debug == 0);
      #usleep($sleep);
      select(undef, undef, undef, $sleep);
    }
    print "\n" if ($debug == 1);
    $swap = 0;
  }
}
# clean up (turn off leds
system("./led-pcf 255");
foreach $i (0..7) { system("gpio write $i down"); }
