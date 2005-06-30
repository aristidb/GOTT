#!/usr/bin/perl -w
$ind = 0;
while (<>) {
	while (/}/g) { --$ind; }
	for (my $i = 0; $i < $ind*2; ++$i) {
		print ' ';
	}
	print $_;
	while (/{/g) { ++$ind; }
}
