#!/usr/bin/env qore

%require-our
%enable-all-warnings

class GateTest {
    constructor()
    {
	$.g    = new Gate();
	$.key1 = "key1";
	$.key2 = "key2";
	$.key3 = "key3";
	$.key4 = "key4";
    }
    shuffle()
    {
	# do writes
	$.g.enter();
	my $v = $.key1;
	my $x = $.key2;
	delete $.key2;
	$.key2 = $v;
	delete $.key1;
	$.g.enter();
	$.key1 = $x;
	$self{rand() % 100} = rand();
	delete $self{rand() % 100};
	$.g.exit();
	$.g.exit();
	$v = int();
	delete $v;
    }
    read()
    {
	$.g.enter();
	my $v = $.key3;
	my $x = $.key4;
	$.g.exit();
	$v = int();
	delete $v;
    }
}

our $delay       = int(shift $ARGV);
our $num_threads = int(shift $ARGV);
our $iters       = int(shift $ARGV);
our $overall     = int(shift $ARGV);

printf("delay   = %d\n", $delay);
printf("threads = %d\n", $num_threads);
printf("iters   = %d\n", $iters);
printf("overall = %d\n", $overall); 

our $obj1 = new GateTest();
our $obj2 = new GateTest();

our $queue = new Queue();

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

sub gee($arg)
{
    for (my $i = 0; $i < $iters; $i++)
    {
	$queue.push(sprintf("TID %3d (%3d/%3d) %s\n", gettid(), $i, $iters, $arg));
	#printf("TID %d (%d/%d) %s\n", gettid(), $i, $iters, $arg);
	delay($delay);
    }
    save_thread_data("key", "value");
    $queue.push(sprintf("key=%s\n", get_thread_data("key")));
}

sub output()
{
    my $v;

    while (($v = $queue.get()) != "exit")
	print($v);
	#continue;
}

# start message thread
our $otid = background output();

#printf("thread 1 TID: %d\n", background gee("geronimo", $delay));
for (my $i = 1; $i <= $num_threads; $i++)
    $queue.push(sprintf("thread %d TID: %d\n", $i, background gee(sprintf("thread %d", $i))));

our $o = $num_threads;
try 
{
    while (1)
    {
	if ($o >= $overall)
	    break;
	$queue.push(sprintf("threads=%d/%d (running=%d)\n", $o, $overall, num_threads()));
	my $start = $o;
	if ((num_threads() - 1) < $num_threads)
	    for (my $i = 0; $i < ($num_threads - num_threads() + 1); $i++)
        {
	    my $tidrc;
	    $queue.push(sprintf("thread %d TID: %d\n", $i + $start, ($tidrc = background gee(sprintf("thread %d", $i + $start)))));
	    if (exists($tidrc))
		$o++;
	    if ($o == $overall)
		break;
	}
	sleep(1);
    }

    while (1)
    {
	my $l;
	$queue.push(sprintf("this TID   = %d\n", gettid()));
	$queue.push(sprintf("output TID = %d\n", $otid));
	$queue.push(sprintf("total %d threads\n", num_threads()));
	foreach my $t in ($l = thread_list())
	    $queue.push(sprintf("TID %d still running\n", $t));
	if (elements $l == 3)
	    break;
	sleep(2);
    }
}
catch ($ex)
{
    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
        $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
}

$queue.push(sprintf("key1=%s key2=%s mutex=%s\n", $obj1.key1, $obj1.key2, typename($obj1.g)));

# tell output thread to terminate
$queue.push("exit");

#print("list of all global variables:\n");
#foreach my $v in (dbg_global_vars())
#    printf("%s\n", substr($v, 1));
