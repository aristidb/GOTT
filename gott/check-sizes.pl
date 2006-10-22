#!/usr/bin/perl -w
%entries = ();
while (<>) {
	if (/^~.*:(\d+)/) {
		print "- $1 <- $_";
		print "deleted unallocated entry: $_\n" if !delete $entries[$1];
	} elsif (/:(\d+)/) {
		print "+ $1 <- $_";
		$entries[$1] = $_;
	} else {
		print "unrecognized sizes.log message";
	}
}
print "$_\n" foreach %entries;
