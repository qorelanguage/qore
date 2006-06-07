#!/usr/bin/env qore 
#-ciso-8859-1

$iters = int(shift $ARGV);
$threads = shift $ARGV;
printf("iters=%d threads=%d\n", $iters, $threads);

$obj.key.500.hello = 0;

$c = new Counter();

sub add()
{
    for (my $i = 0; $i < $iters; $i++)
	$obj.key.500.hello++;
    #printf("add() %d\n", $obj.key.500.hello);
    $c.dec();
}

sub subtract()
{
    for (my $i = 0; $i < $iters; $i++)
	$obj.key.500.hello--;
    #printf("subtract() %d\n", $obj.key.500.hello);
    $c.dec();
}

sub do_threads()
{
    while ($threads--)
    {
	$c.inc();
	$c.inc();
	background add();
	background subtract();
    }
}

do_threads();

$c.waitForZero();
printf("result=%N\n", $obj);
