#!/usr/bin/env qore

RWLock::methodGate($m)
{
    return 0;
}

$o = ( "key1" : "key1", 
       "key2" : "key2", 
       "key3" : "key3", 
       "key4" : "key4" );

$iters   = int(shift $ARGV);
$writers = int(shift $ARGV);
$readers = int(shift $ARGV);
if (!$iters)
    $iters = 1000;
if (!$writers)
    $writers = 10;
if (!$readers)
    $readers = 10;

printf("iters   = %d\n", $iters);
printf("writers = %d\n", $writers);
printf("readers = %d\n", $readers); 

sub read_thread()
{
    for (my $i = 0; $i < $iters; $i++)
    {
	my $t;
	$rwl.readLock();
	if ($o.key1 == $o.key2)
	    $t = 1;
	    #printf("read_thread (%02d) %d/%d:  key1 = key2\n", gettid(), $i, $iters);
	else
	    $t = 1;
	    #printf("read_thread (%02d) %d/%d: key1 != key2\n", gettid(), $i, $iters);
	
	$rwl.readLock();
	if ($o.key3 == $o.key4)
	    $t = 1;
	    #printf("read_thread (%02d) %d/%d: key3 = key4\n", gettid(), $i, $iters);
	else
	    $t = 1;
	    #printf("read_thread (%02d) %d/%d: key3 != key4\n", gettid(), $i, $iters);
	$rwl.readUnlock();

	if (my $n = $rwl.numWriters())
	{
	    printf("read error:  %d writers\n", $n);
	    exit(1);
	}
	$rwl.readUnlock();
    }
}

sub write_thread()
{
    for (my $i = 0; $i < $iters; $i++)
    {
	#printf("write_thread (%02d) %d/%d trying to get lock\n",
	#       gettid(), $i, $iters);
	$rwl.writeLock();
	my $t = $o.key2;
	$o.key2 = $o.key1;
	$o.key1 = $t;

	if ((my $n = $rwl.numWriters()) > 1)
	{
	    printf("write error: %d writers\n", $n);
	    exit(2);
	}

	$t = $o.key4;
	$o.key4 = $o.key3;
	$o.key3 = $t;

	$t = rand() % 100;
	$o.$t = rand() % 1000;

	#printf("write_thread (%02d) %d/%d: 1: %s 2: %s 3: %s 4: %s (new %s)\n",
	#       gettid(), $i, $iters, $o.key1, $o.key2, $o.key3, $o.key4, $t);

	$rwl.writeUnlock();
    }
}

$rwl = new RWLock();
while ($writers)
{
    background write_thread();
    $writers--;
}
while ($readers)
{
    background read_thread();
    $readers--;
}

#printf("%s\n", dbg_node_info($rwl));
