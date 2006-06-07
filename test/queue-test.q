#!/usr/bin/env qore 

$q = new Queue();
$x = new Counter();

$threads = int(shift $ARGV);
$iters   = int(shift $ARGV);

if (!$threads)
    $threads = 10;
if (!$iters)
    $iters = 1000;

printf("threads = %d\n", $threads);
printf("iters   = %d\n", $iters);

sub qt()
{
    for (my $i; $i < $iters; $i++)
	$q.push(sprintf("tid-%d-%d", gettid(), $i));
    $x.dec();
}

sub main()
{
    for (my $i; $i < $threads; $i++)
    {
	$x.inc();
	background qt();
    }
    my $c = $threads * $iters;
    while ($c--)
	$q.pop();

    #$x.waitForZero();
    printf("q=%N size=%d (%s)\n", $q, $q.size(), $q.size() ? "ERROR" : "OK");
}

main();

