#!/usr/bin/perl -w
my $ind = 0;
while (<>) {
	my $del = 0;
	if (/}/g) { 
		if (pos() < 2) {
			--$ind; 
		} else {
			$del = 1;
		}
	}
	for (my $i = 0; $i < $ind*2; ++$i) {
		print ' ';
	}
	print $_;
	$ind -= $del;
	while (/}/g) { --$ind; }
	while (/{/g) { ++$ind; }
}
