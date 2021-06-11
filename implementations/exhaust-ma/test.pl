#! /usr/bin/perl
# $Id: test.pl,v 1.1 2003/06/06 12:08:39 martinus Exp $
#
# tests for differences between pmars and exhaust
#


die "usage: $0: start_ix warriorA warriorB\n" if @ARGV < 3;

$start = $ARGV[0];
$WA = $ARGV[1];
$WB = $ARGV[2];

for ($i = $start; $i < 8192; $i++) {
    $F = gray($i);
    $F = perm13($F);
    if ( $F >= 100 && $F <= 8000-100 ) {
	if (1) {
	    print( "(i=$i): $F\n")  if $i % 25 == 0;
	    if ( 0 != play( $F ) ) {
		print "(i=$i): difference at position $F\n";
		last;
	    }
	}
    }
}
exit 0;





sub gray {
    my $i=shift;
    return $i ^ ($i>>1);
}

# reverse the bits 0..12
sub perm13 {
    my $x = shift;
    my $y = 0;

    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1; $y = $y << 1;
    $y |= ($x & 1); $x = $x >> 1;
    return $y;
}


sub play {
    my $F = shift;
    my $p, $e;
    do { $p = `pmars-server -bkF $F $WA $WB 2>/dev/null`} && $? == 0
	or die "$0: execution of  'pmars-server' failed: $?\n";
    do { $e = `./exhaust -bkF $F $WA $WB` } && $? == 0
	or die "$0: execution of 'exhaust' failed: $?\n";
    return $p cmp $e;
}
