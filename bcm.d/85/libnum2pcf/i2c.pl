#!/usr/bin/perl
$cmd = "i2cset -f -y 1 0x20 ";
$list = "255 127 63 31 15 7 3 1 0";
$nl = "254 253 251 247 239 223 191 127";

@lazy = split(/\s+/, $list);
@reallazy = reverse(@lazy);

@ll = split(/\s+/, $nl);
@rl = reverse(@ll);

foreach $w (1..2) {
 @fu = ($w == 1 ? @ll : @rl);
 foreach $item (@fu) {
  $n = $cmd . $item;
  system("$n");
  print "$n\n";
  sleep(1);
 }
}

__DATA__
foreach $w (1..2) {
 @fu = ($w == 1 ? @lazy : @reallazy);
 foreach $item (@fu) {
  $n = $cmd . $item;
  system("$n");
  print "$n\n";
  sleep(1);
 }
}

__DATA__
foreach $num (200..255) {
 $n = $cmd . $num;
 system("$n");
 print "$n\n";
 sleep(1);
}
254 253 251 247 239 223 191 127
