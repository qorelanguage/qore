#!/usr/bin/env qore

%requires tibco

const Subject = "Qore.Test.QORETest";

$iters = int(shift $ARGV);
if (!$iters)
    $iters = 100;

$threads = int(shift $ARGV);
if (!$threads)
    $threads = 1;

printf("%d iteration(s) %d thread(s)\n", $iters, $threads);

$seq = new Sequence();

class Waiter {
    constructor() { $.c = new Condition(); $.m = new Mutex(); $.t = False; }
    wait() { $.m.lock(); if (!$.t) $.c.wait($.m); $.m.unlock(); }
    broadcast() { $.m.lock(); $.t = True; $.c.broadcast(); $.m.unlock(); }
}

sub testInit($q)
{
    # Application properties for adapter
    my $props.AppVersion = Qore::VersionString;
    $props.AppInfo = "test";
    $props.AppName = "testAdapter";
    $props.RepoURL = "./new.dat";
    $props.ConfigURL = "/tibco/private/adapter/testAdapter";
    my $classes.Test = "/tibco/public/class/ae/Test";

    print("initializing TIBCO session: \n");
    my $adapter = new TibcoAdapter("rvSession", $props, $classes);#, "8504", "172.23.3.137", "172.23.5.143:7500");
    print("done\n");
    $q = new Queue();
    return $adapter;
}

sub sendTest($adapter, $subject)
{
    my $function = "Test";
    my $msg = ( "STRING"   : "hello there",
		"INTEGER"  : $seq.next(),
		"DATE"     : now(),
		"DATETIME" : now(), #9999-12-31,
		"BOOLEAN"  : True,
		"FLOAT"    : 123.23443 );
    $adapter.sendSubjectWithSyncReply($subject, $function, $msg, 1);
}

sub sendExitMsg($adapter, $subject)
{
    my $function = "Test";
    my $msg = ( "STRING"   : "exit" );
    $adapter.sendSubject($subject, $function, $msg);
}

sub send($adapter, $subject)
{
    $subject += ".1";
    printf("sending %s\n", $subject);
    my $cnt = 0;
    $go.wait();
    printf("running sender %s\n", $subject);
    for (my $i; $i < $iters; $i++)
    {
	sendTest($adapter, $subject);
	if (!(++$cnt % 10))
	    print("S");
    }
    sendExitMsg($adapter, $subject);
    printf("%d messages sent\n", $iters + 1);
}

sub receive($adapter, $subject)
{
    my $cnt = 0;
    my $sc;

    $subject += ".>";
    printf("listening on %s\n", $subject);
    # set up the listener
    $adapter.receive($subject, 1);
    $go.wait();
    printf("running listener %s\n", $subject);
    while (True)
    {
	my $msg = $adapter.receive($subject, 5000);
	usleep(5000);
	if (!exists $msg)
	    break;
	if (!(++$cnt % 10))
	    print("R");
	if ($msg.msg.STRING == "exit")
	    break;
	if (!exists $sc)
	    $sc = $msg.msg.INTEGER;
	else if ($msg.msg.INTEGER == ($sc + 1))
	    $sc++;
	#else
	    #printf("SEQUENCE ERROR: sc=%n msg=%n\n", $sc, $msg.msg);
    }
    printf("\n%d messages received\n", $cnt);
}

$go = new Waiter();

sub doTest()
{

    my $a = ();
    for (my $i = 0; $i < $threads; $i++)
	$a += testInit();

    for (my $i = 0; $i < $threads; $i++)
    {
	my $subject = sprintf("%s.%d.1", Subject, $i);

	background receive($a[$i], $subject);
	background send($a[$i], $subject);
    }

    $go.broadcast();
}

sub newTest()
{
    my $subject = "DEV.GB.Hutchison3G.QORETest.1";

    $string = "hello: äüöß";
    $string = convert_encoding($string, "ISO-8859-1");
    print(dbg_node_info($string));

    my $function = "Test";
    my $msg = ( "STRING"   : $string,
		"INTEGER"  : $seq.next(),
		"DATE"     : now(),
		"DATETIME" : now(), #9999-12-31,
		"BOOLEAN"  : True,
		"FLOAT"    : 123.23443 );

    my $adapter = testInit();
    $adapter.sendSubjectWithSyncReply($subject, $function, $msg, 1);
}

doTest();

#newTest();
