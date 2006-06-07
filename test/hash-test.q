#!/usr/bin/env qore

# little script to test random hash accesses on a large hash
# test with "time hash-test.q"
# by David Nichols

$size = int(shift $ARGV);
if (!$size)
    $size = 10000;

sub rstr()
{
    my $str = "";

    for (my $i = 0; $i < 4; $i++)
	$str += doChar(rand() % 52);

    return $str;
}

sub doChar($v)
{
    if ($v < 26)
	return chr($v + ord("A"));
    return chr($v - 26 + ord("a"));    
}

sub getKey($n)
{
    my $str = "";

    my $v = $n % 52;
    $str += doChar($v) + rstr();
    $n -= $v;
    while ($n > 51)
    {
	$n = $n / 52;
	$str += doChar(($n - 1) % 52) + rstr();
    }
    return $str;
}

sub hash_test()
{
    srand();

    my $h;

    for (my $i = 0; $i < $size; $i++)
	$h{getKey($i)} = True;

    my $l = keys $h;
    for (my $i = 0; $i < $size / 10; $i++)
	my $v = $h.($l[$i]);

    #printf("l=%N\n", $l);
}

hash_test();
