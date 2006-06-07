#!/usr/bin/env qore

class ThreadTest {
    constructor($threads)
    {
	$.dm  = new Mutex();
	$.drw = new RWLock();
	$.m   = new Mutex();
	$.g   = new Gate();
	$.c   = new Condition();
	#$.s   = new SingleExitGate();
	while ($threads)
	{
	    $.addChild();
	    $threads--;
	}
    }
    destructor()
    {
	$.waitForChildren();
    }
    lock() { $.m.lock(); }
    unlock() { $.m.unlock(); }
    private addChild()
    {
	$.m.lock();
	$.children++;
	$.m.unlock();
	background $.worker();
    }
    subtractChild()
    {
	$.m.lock();
	if (!(--$.children))
	    $.c.signal();
	$.m.unlock();
    }
    waitForChildren()
    {
	$.m.lock();
	if ($.children)
	    $.c.wait($.m);
	$.m.unlock();
    }
    getData($list)
    {
	my $rv;
	$.dm.lock();
	foreach my $key in ($list)
	    $rv.$key = $.data.$key;
	$.dm.unlock();
	return $rv;
    }
    setData($hash)
    {
	$.dm.lock();
	foreach my $key in (keys $hash)
	    $.data.$key = $hash.$key;
	$.dm.unlock();
    }
    getRWData($list)
    {
	my $rv;
	$.drw.readLock();
	foreach my $key in ($list)
	    $rv.$key = $.rwdata.$key;
	$.drw.readUnlock();
	return $rv;
    }
    setRWData($hash)
    {
	$.drw.writeLock();
	foreach my $key in (keys $hash)
	    $.rwdata.$key = $hash.$key;
	$.drw.writeUnlock();
    }
    getGateData($list)
    {
	my $rv;
	$.g.enter();
	foreach my $key in ($list)
	    $rv.$key = $.gdata.$key;
	$.g.exit();
	return $rv;
    }
    setGateData($hash)
    {
	$.g.enter();
	foreach my $key in (keys $hash)
	    $.gdata.$key = $hash.$key;
	$.g.exit();
    }
    worker()
    {
	for (my $i = 0; $i < $iters; $i++)
	{
	    if (!($i % 1000))
		printf("TID %3d: %d/%d\n", gettid(), $i, $iters);
	    my $c = rand() % 7;
	    my $key1 = sprintf("key%d", rand() % 10);
	    my $key2 = sprintf("key%d", (rand() % 10) + 10);
	    my $key3 = sprintf("key%d", rand() % 20);
	    if (!$c)
	    {
		my $hash.$key1 = rand() % 10;
		$hash.$key2 = rand() % 10;
		$.setData($hash);
		continue;
	    }
	    if ($c == 1)
	    {
		my $data = $.getData(($key1, $key2, $key3));
		continue;
	    }
	    if ($c == 2)
	    {
		my $hash.$key1 = rand() % 10;
		$hash.$key2 = rand() % 10;
		$.setRWData($hash);
		continue;
	    }
	    if ($c == 3)
	    {
		my $data = $.getRWData(($key1, $key2, $key3));
		continue;
	    }
	    if ($c == 4)
	    {
		my $hash.$key1 = rand() % 10;
		$hash.$key2 = rand() % 10;
		$.setGateData($hash);
		continue;
	    }
	    if ($c == 5)
	    {
		my $data = $.getGateData(($key1, $key2, $key3));
		continue;
	    }
	    if ($c == 6)
	    {
		my $str = makeXMLRPCValueString($.getData());
		my $d = parseXML($str);
	    }
	}
	$.subtractChild();
    }
}

$num_threads = int(shift $ARGV);
$iters       = int(shift $ARGV);

printf("threads = %d\n", $num_threads);
printf("iters   = %d\n", $iters);

new ThreadTest($num_threads);
#printf("global vars:\n");
#foreach my $v in (dbg_global_vars())
#    printf("%s\n", $v);
