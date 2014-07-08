#!/usr/bin/env qore

# little script to test hash accesses on a large hash
# by David Nichols

# default 10000 hash keys
my int $size = int(shift $ARGV);
if (!$size)
    $size = 10000;

# default minimum hash key length = 20 characters
my int $min_length = int(shift $ARGV);
if (!$min_length)
    $min_length = 20;

# times to search the hash
my int $num_loops = int(shift $ARGV);
if (!$num_loops)
    $num_loops = 2;

string sub rstr(int $len) {
    my string $str = "";

    for (my int $i = 0; $i < $len; $i++)
	$str += doChar(rand() % 52);

    return $str;
}

string sub doChar(int $v) {
    return $v < 26 ? chr($v + ord("A")) : chr($v - 26 + ord("a"));
}

string sub getKey(int $n) {
    my string $str = "";

    my int $v = $n % 52;
    $str += doChar($v) + rstr(2);
    $n -= $v;
    while ($n > 51) {
	$n /= 52;
	$str += doChar(($n - 1) % 52) + rstr(2);
    }
    $str += rstr($min_length - $str.size());
    return $str;
}

sub hash_test() {
    srand(now());

    my hash $h;
    my list $l = ();

    printf("creating hash key list (%d entries, %d char key len): ", $size, $min_length); flush();
    # first we get a list of all the hash keys
    my date $list_start = now_us();
    for (my int $i = 0; $i < $size; $i++)
	$l += getKey($i);

    my date $start = now_us();
    printf("created in %y\n", $start - $list_start); flush();
    print("running insert: ");
    for (my int $i = 0; $i < $size; $i++)
	$h{$l[$i]} = True;

    my date $search = now_us();
    #$l = keys $h;
    printf("done in %y (%d keys), running search: ", $search - $start, $h.size()); flush();
    $search = now_us();
    
    for (my int $loop = 0; $loop < $num_loops; ++$loop) {
	for (my int $i = 0; $i < $size; $i++)
	    my bool $v = $h.($l[$i]);
    }

    my date $et = now_us();
    printf("%d searches in %y, total time: %y\n", $size & $num_loops, $et - $search, $et - $start);
}

hash_test();
