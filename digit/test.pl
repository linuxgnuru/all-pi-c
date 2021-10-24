#!/usr/bin/perl

$j = 0;
foreach $i (1..9) {
	$fu[$j++] = $i;
}
$fu[$j++] = 0;

foreach $i (a..g) {
	$fu[$j++] = $i;
}
$fu[$j++] = 'p';
$fu[$j++] = 's';
$fu[$j++] = 'u';

foreach $i (@fu) {
	print "$i\n";
	chop($l = `sudo ./digit2 $i`);
	sleep(1);
}
