#!/usr/bin/env qore

%requires tibae
%require-our
%enable-all-warnings

#const Subject = "Qore.Test.QORETest";
const Subject = "test";

our $iters = int(shift $ARGV);
if (!$iters)
    $iters = 100;

our $threads = int(shift $ARGV);
if (!$threads)
    $threads = 1;

printf("%d iteration(s) %d thread(s)\n", $iters, $threads);

our $seq = new Sequence();

const test_class = "/tibco/public/class/ae/Test";

sub testInit($q)
{
    # Application properties for adapter
    my $props.AppVersion = Qore::VersionString;
    $props.AppInfo = "test";
    $props.AppName = "testAdapter";
    $props.RepoURL = "./new.dat";
    $props.ConfigURL = "/tibco/private/adapter/testAdapter";
    my $classes.Test = test_class;

    print("initializing TIBCO session: \n");
    my $adapter = new TibcoAdapter("rvSession", $props, $classes);#, "8504", "172.23.3.137", "172.23.5.143:7500");
    print("done\n");
    $q = new Queue();
    return $adapter;
}

const msg_in = ( "STRING"   : "hello there",
		 "INTEGER"  : 2501234,
		 "DATE"     : 2005-03-10,
		 "DATETIME" : 2007-06-05T15:48:37.145,
		 "BOOLEAN"  : True,
		 "FLOAT"    : 123.23443,
		 "I1"       : 110,
		 "I2"       : 21312,
		 "I4"       : 24983942,
		 "I8"       : 349389023848234,
		 "U1"       : -76,
		 "U2"       : -20534,
                 "U4"       : 34904932,
                 "U8"       : 491934783039821,
                 "R4"       : 154.0,
                 "R8"       : 239349871094.2334,
                 "BINARY"   : <beadface>,
                 "TIME"     : 12:35:01.145,
                 "INTERVAL" : 439395s );

our $msg_out = ( "STRING"   : msg_in.STRING,
		 "INTEGER"  : msg_in.INTEGER,
		 "DATE"     : msg_in.DATE,
		 "DATETIME" : msg_in.DATETIME,
		 "BOOLEAN"  : msg_in.BOOLEAN,
		 "FLOAT"    : msg_in.FLOAT,
                 "BINARY"   : msg_in.BINARY,
		 "I1"       : tibae_type(TIBAE_I1, msg_in.I1),
		 "I2"       : tibae_type(TIBAE_I2, msg_in.I2),
		 "I4"       : tibae_type(TIBAE_I4, msg_in.I4),
		 "I8"       : tibae_type(TIBAE_I8, msg_in.I8),
		 "U1"       : tibae_type(TIBAE_U1, msg_in.U1),
		 "U2"       : tibae_type(TIBAE_U2, msg_in.U2),
                 "U4"       : tibae_type(TIBAE_U4, msg_in.U4),
                 "U8"       : tibae_type(TIBAE_U8, msg_in.U8),
                 "R4"       : tibae_type(TIBAE_R4, msg_in.R4),
                 "R8"       : tibae_type(TIBAE_R8, msg_in.R8),
                 "TIME"     : tibae_type(TIBAE_TIME, msg_in.TIME),
                 "INTERVAL" : tibae_type(TIBAE_INTERVAL, 5D + 2h + 3m + 15s + 251ms ) );

sub sendTest($adapter, $subject)
{
    # use the class repository path directly
    $adapter.sendSubject($subject, test_class, $msg_out);
}

sub sendExitMsg($adapter, $subject)
{
    my $msg = ( "STRING"   : "exit" );
    # look up the class path using the class hash passed to the TibcoAdapter constructor
    $adapter.sendSubject($subject, "Test", $msg);
}

sub send($adapter, $subject)
{
    $subject += ".1";
    printf("sending %s\n", $subject);
    my $cnt = 0;
    $go.waitForZero();
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

    $subject += ".>";
    printf("listening on %s\n", $subject);
    # set up the listener
    $adapter.receive($subject, 1);
    $go.waitForZero();
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

	foreach my $k in (keys msg_in)
	{
	    if ($msg.msg.$k != msg_in.$k)
	    {
		printf("error in field %n: recvd=%N, expected=%N\n", $k, $msg.msg.$k, msg_in.$k);
		return;
	    }
	}
    }
    printf("\n%d messages received\n", $cnt);
}

our $go = new Counter();

sub doTest()
{
    my $a = ();
    for (my $i = 0; $i < $threads; $i++)
	$a += testInit();

    $go.inc();
    for (my $i = 0; $i < $threads; $i++)
    {
	my $subject = sprintf("%s.%d.1", Subject, $i);

	background receive($a[$i], $subject);
	background send($a[$i], $subject);
    }

    $go.dec();
}

doTest();

