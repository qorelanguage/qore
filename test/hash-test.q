#!/usr/bin/env qore

# little script to test hash accesses on a large hash
# by David Nichols

# default 10000 hash keys
$size = int(shift $ARGV);
if (!$size)
    $size = 10000;

# default minimum hash key length = 20 characters
$min_length = int(shift $ARGV);
if (!$min_length)
    $min_length = 20;

sub rstr($len)
{
    my $str = "";

    for (my $i = 0; $i < $len; $i++)
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
    $str += doChar($v) + rstr(4);
    $n -= $v;
    while ($n > 51)
    {
	$n = $n / 52;
	$str += doChar(($n - 1) % 52) + rstr(4);
    }
    $str += rstr($min_length - strlen($str));
    return $str;
}

sub hash_test()
{
    srand();

    my $h;
    my $l = ();

    printf("creating hash key list (%d entries, %d char key len): ", $size, $min_length); flush();
    # first we get a list of all the hash keys
    my $list_start = clock_getmicros();
    for (my $i = 0; $i < $size; $i++)
	$l += getKey($i);

    my $start = clock_getmicros();
    printf("created in %.6fs\n", ($start - $list_start) / 1000000.0); flush();
    print("running insert: ");
    for (my $i = 0; $i < $size; $i++)
	$h{$l[$i]} = True;

    my $search = clock_getmicros();
    printf("done in %.6fs, running search: ", ($search - $start) / 1000000.0); flush();
    my $l = keys $h;
    my $end = $size / 2;
    for (my $i = 0; $i < $end; $i++)
	my $v = $h.($l[$i]);

    my $et = clock_getmicros();
    printf("%d searches in %.6fs, total time: %.6fs\n", $end, ($et - $search) / 1000000.0, ($et - $start) / 1000000.0);
}

hash_test();
