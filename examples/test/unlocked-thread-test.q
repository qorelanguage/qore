#!/usr/bin/env qore

class ThreadTest {
    constructor() {
	$.key1 = "key1";
	$.key2 = "key2";
	$.key3 = "key3";
	$.key4 = "key4";
    }
    shuffle() {
	my $v = $.key1;
	my $x = $.key2;
	delete $.key2;
	$.key2 = $v;
	delete $.key1;
	$.key1 = $x;
	$self{rand() % 100} = rand();
	delete $self{rand() % 100};
	$v = int();
	delete $v;
    }
    read() {
	my $v = $.key3;
	my $x = $.key4;
	$v = int();
	delete $v;
    }
}

$delay       = int(shift $ARGV);
$num_threads = int(shift $ARGV);
$iters       = int(shift $ARGV);
$overall     = int(shift $ARGV);

printf("delay   = %d\n", $delay);
printf("threads = %d\n", $num_threads);
printf("iters   = %d\n", $iters);
printf("overall = %d\n", $overall); 

$obj1 = new ThreadTest();
$obj2 = new ThreadTest();

$queue = new Queue();

sub delay($val)
{
    my $i = 0;
    while ($i < $val)
    {
	if (gettid() % 2)
	{
	    $obj1.shuffle();
	    my $v = $obj2.key1;
	    $v += " " + $obj2.key2;
	}
	else
	{
	    $obj1.read();
	    my $v += "extra";
	    my $x += "extra";
	    $v += $obj2.key1;
	    $v += " " + $obj2.key2;
	}
	$i++;
    }
}

sub gee($arg) {
    my $ds;

    if ($arg == "geronimo")
	$ds = $s2;
    else
	$ds = $s1;

    for (my $i = 0; $i < $iters; $i++) {
	$queue.push(sprintf("TID %3d (%3d/%3d) %s\n", gettid(), $i, $iters, $arg));
	#printf("TID %d (%d/%d) %s\n", gettid(), $i, $iters, $arg);
	delay($delay);
    }
    save_thread_data("key", "value");
    $queue.push(sprintf("key=%s\n", get_thread_data("key")));
}

sub output() {
    my $v;

    while (($v = $queue.get()) != "exit")
	print($v);
	#continue;
}

# start message thread
$otid = background output();

for (my $i = 1; $i <= $num_threads; $i++)
    $queue.push(sprintf("thread %d TID: %d\n", $i, background gee(sprintf("thread %d", $i))));

$o = $num_threads;
try {
    while (1) {
	if ($o >= $overall)
	    break;
	$queue.push(sprintf("threads=%d/%d (running=%d)\n", $o, $overall, num_threads()));
	my $start = $o;
	if ((num_threads() - 1) < $num_threads)
	    for (my $i = 0; $i < ($num_threads - num_threads() + 1); $i++) {
	    my $tidrc;
	    $queue.push(sprintf("thread %d TID: %d\n", $i + $start, ($tidrc = background gee(sprintf("thread %d", $i + $start)))));
	    if (exists($tidrc))
		$o++;
	    if ($o == $overall)
		break;
	}
	sleep(1);
    }

    while (1) {
	my $l;
	$queue.push(sprintf("this TID   = %d\n", gettid()));
	$queue.push(sprintf("output TID = %d\n", $otid));
	$queue.push(sprintf("total %d threads\n", num_threads()));
	foreach my $t in ($l = thread_list())
	    $queue.push(sprintf("TID %d still running\n", $t));
	if (elements $l == 2)
	    break;
	sleep(2);
    }
}
catch ($ex) {
    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
        $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
}

$queue.push(sprintf("key1=%s key2=%s mutex=%s\n", $obj.key1, $obj.key2, type($obj.m)));

# tell output thread to terminate
$queue.push("exit");
