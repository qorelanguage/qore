#!/usr/bin/env qore 

%require-our
%enable-all-warnings
%exec-class thread_object_test

const opts = 
    ( "help"       : "h,help",
      "verbose"    : "v,verbose" );

class thread_object_test {
    constructor() {
	$.process_command_line();
	new CounterTest($.threads, $.iters);
	new QueueTest($.threads, $.iters);
        new ThreadTest($.threads, $.iters);
    }

    usage() {
        printf("usage: %s -[options] [iterations [threads]]
  -h,--help           this help text
  -v,--verbose        more information
", basename($ENV."_"));
        exit();
    }

    process_command_line() {
        my GetOpt $g(opts);
        $.o = $g.parse(\$ARGV);
        
        if (exists $.o."_ERRORS_") {
            printf("%s\n", $.o."_ERRORS_"[0]);
            exit(1);
        }
        
        if ($.o.help)
            $.usage();

	$.iters = int(shift $ARGV);
	if (!$.iters)
	    $.iters = 2000;
	$.threads = int(shift $ARGV);
	if (!$.threads)
	    $.threads = 5;
	printf("iterations=%d threads=%d\n", $.iters, $.threads);
    }
}

class CounterTest {
    constructor(int $threads, int $iters) {
	printf("counter test: "); flush();
	$.threads = $threads;
	$.iters = $iters;
	$.obj.key.500.hello = 0;

	$.c = new Counter();
	$.do_threads();
	$.c.waitForZero();
	if (!$.obj.key.500.hello)
	    print("OK\n");
	else
	    printf("ERROR (%d) %d\n", $.obj.key.500.hello, $.c.getCount());
    }

    private do_threads() {
	while ($.threads--) {
	    $.c.inc();
	    background $.add();
	    $.c.inc();
	    background $.subtract();
	}
    }

    private add() {
	for (my int $i = 0; $i < $.iters; $i++)
	    $.obj.key.500.hello++;
	$.c.dec();
    }

    private subtract() {
	for (my int $i = 0; $i < $.iters; $i++)
	    $.obj.key.500.hello--;
	$.c.dec();
    }
}

class QueueTest {
    constructor(softint $threads, softint $iters) {
	printf("queue test: "); flush();
	$.q = new Queue();
	$.x = new Counter();
	
	for (my int $i; $i < $threads; $i++) {
	    $.x.inc();
	    background $.qt($threads, $iters);
	}
	my int $c = $threads * $iters;
	while ($c--)
	    if (rand() % 2)
	        $.q.pop();
	    else
		$.q.get();

	$.x.waitForZero();
	if (!$.q.size())
	    print("OK\n");
	else
	    printf("ERROR: q=%N size=%d (%s)\n", $.q, $.q.size());
    }

    private qt(int $threads, int $iters) {
	for (my $i; $i < $iters; $i++)
	    $.q.push(sprintf("tid-%d-%d", gettid(), $i));
	$.x.dec();
    }
}

class ThreadTest inherits Mutex {
    constructor(softint $threads, softint $iters) {
	print("thread object tests: "); flush();
	$.iters = $iters;
        $.drw = new RWLock();
        $.g   = new Gate();
        $.c   = new Counter();
        while ($threads) {
	    $.c.inc();
	    background $.worker();
            $threads--;
        }
	$.c.waitForZero();
	print("OK\n");
    }
    getData(list $list) {
        my $rv;
        $.lock();
        foreach my string $key in ($list)
            $rv.$key = $.data.$key;
        $.unlock();
        return $rv;
    }
    setData(hash $hash) {
        $.lock();
        foreach my string $key in (keys $hash)
            $.data.$key = $hash.$key;
        $.unlock();
    }
    getRWData(list $list) {
        my $rv;
        $.drw.readLock();
        foreach my string $key in ($list)
            $rv.$key = $.rwdata.$key;
        $.drw.readUnlock();
        return $rv;
    }
    setRWData(hash $hash) {
        $.drw.writeLock();
        foreach my string $key in (keys $hash)
            $.rwdata.$key = $hash.$key;
        $.drw.writeUnlock();
    }
    getGateData(list $list) {
        my $rv;
        $.g.enter();
        foreach my string $key in ($list)
            $rv.$key = $.gdata.$key;
        $.g.exit();
        return $rv;
    }
    setGateData($hash) {
        $.g.enter();
        foreach my string $key in (keys $hash)
            $.gdata.$key = $hash.$key;
        $.g.exit();
    }
    worker() {
        for (my int $i = 0; $i < $.iters; $i++) {
            #if (!($i % 1000))
            #    printf("TID %3d: %d/%d\n", gettid(), $i, $.iters);
            my $c = rand() % 6;
            my $key1 = sprintf("key%d", rand() % 10);
            my $key2 = sprintf("key%d", (rand() % 10) + 10);
            my $key3 = sprintf("key%d", rand() % 20);
            if (!$c) {
                my $hash.$key1 = rand() % 10;
                $hash.$key2 = rand() % 10;
                $.setData($hash);
                continue;
            }
            if ($c == 1) {
                $.getData(($key1, $key2, $key3));
                continue;
            }
            if ($c == 2) {
                my $hash.$key1 = rand() % 10;
                $hash.$key2 = rand() % 10;
                $.setRWData($hash);
                continue;
            }
            if ($c == 3) {
                $.getRWData(($key1, $key2, $key3));
                continue;
            }
            if ($c == 4) {
                my $hash.$key1 = rand() % 10;
                $hash.$key2 = rand() % 10;
                $.setGateData($hash);
                continue;
            }
            if ($c == 5) {
                $.getGateData(($key1, $key2, $key3));
                continue;
            }
        }
	$.c.dec();
    }
}

