#!/usr/bin/env qore

## include TIBCO Rendezvous functionality
%requires tibrv

%require-our
%exec-class tibrv_test

const Subject = "qore.test";
const CmSubject = "qore.cmtest";

## some defaults
const defaultIterations = 2000;
const defaultTimeout = 5000;        # default timeout for reliable messaging is 5 seconds
const defaultCmTimeout = 20000;     # default timeout for certified messaging is 20 seconds

## fault tolerant params
const HeartbeatInterval = 1000;    # heartbeat interval is 1000 ms = 1s
const ActivationInterval = 2000;   # activation interval is 2s
const PreparationInterval = 3000;  # preparation interval is 3s
const Weight = 1;
const ActiveGoal = 2;
const FaultTolerantGroupName = "QORE.TEST.FT_GROUP.1";

const FT_MSG_HASH = 
    ( TIBRVFT_PREPARE_TO_ACTIVATE : "TIBRVFT_PREPARE_TO_ACTIVATE",
      TIBRVFT_ACTIVATE            : "TIBRVFT_ACTIVATE",
      TIBRVFT_DEACTIVATE          : "TIBRVFT_DEACTIVATE",
      TIBRVFT_QORE_STOP           : "TIBRVFT_QORE_STOP" );

const opts = 
    ( "verbose"   : "verbose,v:i+",
      "timeout"   : "timeout,t=i",
      "cmtimeout" : "cmtimeout,c=i",
      "advisory"  : "advisory,a",
      "iters"     : "iters,i=i",
      "send"      : "send-only,s",
      "recv"      : "receive-only,r",
      "help"      : "help,h"
     );

class tibrv_test {

    constructor()
    {
	$.parse_command_line();

	# setup test message
	$.test_msg = 
	    ( "string"     : "this is a string",
	      "int"        : 43991883840175,
	      "float"      : 2.95743823,
	      "bool"       : True,
	      "binary"     : binary("hello there"),
	      "date"       : 2006-08-07-17:04:52,
	      "list"       : ( 1, "two", 3.0, 1998-09-11, False, 4, 2, "string" ),
	      "hash"       : ( "key" : "value", "key2" : 400.1 ),
	      "i8"         : tibrv_i8(-10),
	      "u8"         : tibrv_u8(234),
	      "i16"        : tibrv_i16(23049),
	      "u16"        : tibrv_u16(65530),
	      "i32"        : tibrv_i32(23493922),
	      "u32"        : tibrv_u32(233929394),
	      "i64"        : tibrv_i64(34203480324802342),
	      "u64"        : tibrv_u64(99123939291923212),
	      "f32"        : tibrv_f32(3949.5),
	      "f64"        : tibrv_f64(-34993.12353491),
	      "ipport16"   : tibrv_ipport16(25),
	      "ipaddr32"   : tibrv_ipaddr32("192.168.0.1"),
	      "xml"        : tibrv_xml(makeXMLString(( "hello" : "val" ))) );

	$.test_msg_compare = $.make_comparison_hash($.test_msg);

	$.answer = 
	    ( "string" : "this is another string",
	      "int"    : 81923405,
	      "float"  : 3431.123983,
	      "bool"   : False,
	      "binary" : binary("strings are nice"),
	      "date"   : 9999-12-31-23:59:59,
	      "list"   : ( "one", 2.0, 3, 2001-12-28, 2005-03-11, True, ( "hash" : "value", "key" : 39293 )),
	      "hash"   : ( "key1" : "value1", "key-two" : 5001, "list" : (1, 2, 4, 5, 6) ) );

	printf("Rendezvous version is: %s\n", tibrvGetVersion());

	my $al;
	if ($.o.advisory)
	    $al = new AdvisoryListener();

	$.lq = new Queue();
	if (!$.o.send)
	{
	    background $.reliable_listener();
	    background $.cm_listener();
	}

	if (!$.o.recv)
	{
	    # if we are not just sending only, then wait for the listener to start
	    if (!$.o.send)
		$.lq.get();
	    $.reliable_sender();
	    $.cm_sender();
	}

	$.ft_test();
	
	if ($.o.advisory)
	    delete $al;

	#printf("%N\n", getAllThreadCallStacks());
    }

    private usage()
    {
	printf(
"usage: %s -[options]
  -h,--help           this help text
  -t,--timeout=ARG    timeout in ms (default %d ms)
  -i,--iters=ARG      send ARG messages for each iterated test
  -a,--advisory       show advisory messages
  -v,--verbose        display messages sent and received
", basename($ENV."_"), defaultTimeout);
	exit();
    }

    private make_comparison_hash($h)
    {
	my $nh;
	foreach my $key in (keys $h)
	{
	    if (exists $h.$key."^type^")
		$nh.$key = $h.$key."^value^";
	    else
		$nh.$key = $h.$key;
	}
	return $nh;
    }

    private printf($msg)
    {
	stdout.vprintf($msg, $argv);
	stdout.sync();
    }

    private parse_command_line()
    {
	my $g = new GetOpt(opts);
	$.o = $g.parse(\$ARGV);

	if (exists $.o."_ERRORS_")
	{
	    printf("%s\n", $.o."_ERRORS_"[0]);
	    exit(1);
	}

	if ($.o.help)
	    $.usage();

	if ($.o.send && $.o.recv)
	{
	    print("ERROR: both send-only and receive-only arguments given (-h for help)\n");
	    exit(1);
	}
	    
	if (!$.o.iters)
	    $.o.iters = defaultIterations;

	if (!$.o.timeout)
	    $.o.timeout = defaultTimeout;

	if (!$.o.cmtimeout)
	    $.o.cmtimeout = defaultCmTimeout;
    }

    private compareMessage($msg, $type)
    {
	if ($msg.msg != $.test_msg_compare)
	{
	    printf("%s MESSAGING ERROR: msg=%n != test_msg=%n\n", $type, $msg.msg, $.test_msg_compare);
	    exit(1);
	}
    }

    private compareAnswer($ans, $type, $to)
    {
	if (!exists $ans)
	{
	    printf("TIMEOUT: no answer received from %s LISTENER in timeout period (%d ms)\n", $type, $to);
	    exit(1);
	}
	if ($ans.msg != $.answer)
	{
	    printf("%s MESSAGING ERROR: ans=%n != answer=%n\n", $type, $ans.msg, $.answer);
	    exit(1);    
	}
	if ($.o.verbose)
	    printf("%s SENDER: RESPONSE: %N\n", $type, $ans);
    }

    private reliable_sender()
    {
	$.printf("RELIABLE SENDER: sending on %s\n", Subject + ".1");
	if (!$.o.verbose)
	    $.printf("RELIABLE MESSAGING: ");

	my $sender = new TibrvSender();
	my $ans;
	for (my $i = 0; $i < $.o.iters; $i++)
	{
	    if (!$.o.verbose && !($i % 100))
		$.printf(".");
	    $ans = $sender.sendSubjectWithSyncReply(Subject + ".1", $.test_msg, $.o.timeout);
	    $.compareAnswer($ans, "RELIABLE", $.o.timeout);
	}
	if ($.o.verbose)
	    $.printf("RELIABLE TESTS");
	$.printf(" OK (%d message%s)\n", $.o.iters, $.o.iters == 1 ? "" : "s");
    }

    private reliable_listener()
    {
	my $subject = Subject + ".>";
	my $listener = new TibrvListener($subject);
	printf("RELIABLE LISTENER: listening on %s\n", $subject);
	if ($.o.verbose)
	    printf("RELIABLE LISTENER: listening on %s\n", $subject);

	# if we are not just receiving, then wait for the listening thread to start
	# and signal the sending thread to begin
	if (!$.o.recv)
	{
	    # sleep for 1/2 second
	    usleep(500000);
	    $.lq.push();
	}

	my $msg;
	my $sender = new TibrvSender();
	
	for (my $i = 0; $i < $.o.iters; $i++)
	{
	    #printf("rl %d/%d\n", $i, $.o.iters); stdout.sync();
	    $msg = $listener.getMessage();
	    $.compareMessage($msg, "RELIABLE");
	    if ($.o.verbose)
		printf("RELIABLE LISTENER: %N\n", $msg);
	    $sender.sendSubject($msg.replySubject, $.answer);
	}
    }

    private wait_for_cm_complete($o)
    {
	while (True)
	{
	    if ($.o.verbose)
		printf("CERTIFIED SENDER: checking ledger: %N\n", $o.reviewLedger(">"));

	    my $count = 0;
	    foreach my $entry in ($o.reviewLedger(">"))
		$count += $entry.total_msgs;
	    if (!$count)
	    {
		if ($.o.verbose)
		    printf("CERTIFIED SENDER: ledger OK\n");
		break;
	    }
	    # sleep for 1/2 second and check again
	    usleep(500000);
	}
    }

    private cm_listener()
    {
	my $subject = CmSubject + ".>";
	my $listener = new TibrvCmListener($subject);
	printf("CERTIFIED LISTENER (%s): listening on %s\n", $listener.getName(), $subject);

	my $sender = new TibrvCmSender();
	
	for (my $i = 0; $i < $.o.iters; $i++)
	{
	    #printf("cm %d/%d\n", $i, $.o.iters); stdout.sync();
	    my $msg = $listener.getMessage();
	    $.compareMessage($msg, "CERTIFIED");
	    if ($.o.verbose)
		printf("CERTIFIED LISTENER: %N\n", $msg);
	    $sender.sendSubject($msg.replySubject, $.answer );
	}
	#printf("1: CERTIFIED SENDER: checking ledger: %N\n", $sender.reviewLedger(">"));

	# if we don't sleep here, the cm_sender thread will normally not receive the 2nd message (why?)
	# for some reason the ledger does not show if the messages have been received or not
	usleep(500000);

	#printf("2: CERTIFIED SENDER: checking ledger: %N\n", $sender.reviewLedger(">"));
    }

    private cm_sender()
    {
	my $sender = new TibrvCmSender();
	$.printf("CERTIFIED SENDER (%s): sending on %s\n", $sender.getName(), CmSubject + ".1");
	if (!$.o.verbose)
	    $.printf("CERTIFIED MESSAGING: ");

	my $ans;
	for (my $i = 0; $i < $.o.iters; $i++)
	{
	    if (!$.o.verbose && !($i % 100))
		$.printf(".");
	    $ans = $sender.sendSubjectWithSyncReply(CmSubject + ".1", $.test_msg, $.o.cmtimeout);
	    $.compareAnswer($ans, "CERTIFIED", $.o.cmtimeout);
	}
	# here we can check the ledger that all messages have been received
	$.wait_for_cm_complete($sender);

	if ($.o.verbose)
	    $.printf("CERTIFIED TESTS");
	$.printf(" OK (%d message%s)\n", $.o.iters, $.o.iters == 1 ? "" : "s");
    }

    private do_ft_msg($f)
    {
	my $tid = gettid();
	printf("FT MSG thread (TID %d): starting event loop for %s\n", $tid, $f.getGroupName());
	while (True)
	{
	    my $msg = $f.getEvent();
	    printf("FT MSG thread (TID %d): dequeued event: %s\n", $tid, FT_MSG_HASH.($msg.action));
	    if ($msg.action == TIBRVFT_QORE_STOP)
	    {
		printf("FT MSG thread (TID %d): received stop command\n", $tid);
		break;
	    }
	}
    }

    private do_ft_mon($f)
    {
	my $tid = gettid();
	printf("FT MON thread (TID %d): starting event loop for %s\n", $tid, $f.getGroupName());
	while (True)
	{
	    my $msg = $f.getEvent();
	    printf("FT MON thread (TID %d): %d active member%s\n", $tid, $msg.numActiveMembers, $msg.numActiveMembers == 1 ? "" : "s");
	    if ($msg.action == TIBRVFT_QORE_STOP)
	    {
		printf("FT MON thread (TID %d): received stop command\n", $tid);
		break;
	    }
	}
    }

    private ft_test()
    {
	my $mon = new TibrvFtMonitor(FaultTolerantGroupName, ActivationInterval);
	background $.do_ft_mon($mon);

	## it is an error for the same program to join the same fault-tolerant group more than once, 
	## but we do it here just for testing purposes (if $num > 1)
	my $num = 1;
	my $f;
	for (my $i = 0; $i < $num; $i++)
	{
	    $f[$i] = new TibrvFtMember(FaultTolerantGroupName, Weight, ActiveGoal, HeartbeatInterval, ActivationInterval, PreparationInterval);
	    
	    background $.do_ft_msg($f[$i]);
	}  
	
	# wait 5 seconds for all the messages to be processed
	sleep(5);

	# we have to stop the event loops in the other threads manually (we could 
	# also call delete to do this), because if there is an active method call 
	# in another thread, the object will not go out of scope even though the 
	# local variable containing the object is going out of scope
	
	for (my $i = 0; $i < $num; $i++)
	    $f[$i].stop();  
	
	$mon.stop();
    }
}

# objects of this class will run in the background and report rendezvous advisory messages until deleted
class AdvisoryListener {
    private 
	$.run,  # flag for stopping the object
	$.q;    # blocking confirmation that the object has stopped

    constructor($cm)
    {
	$.q = new Queue();
	$.run = True;
	# start the listening thread
	background $.listen($cm);
    }

    destructor()
    {
	$.stop();
    }

    stop()
    {
	if ($.run)
	{
	    $.run = False;
	    $.q.get();
	}
    }

    private listen($cm)
    {
	my $listener = $cm ? new TibrvCmListener("_RV.INFO.RVCM.>") : new TibrvListener("_RV.>");
	while ($.run)
	{
	    my $msg = $listener.getMessage(1000);
	    if (!exists $msg)
		continue;
	    printf("ADVISORY LISTENER: %s: %s\n", $msg.msg.ADV_CLASS, $msg.msg.ADV_NAME);
	    #printf("ADVISORY LISTENER: %n\n", $msg);
	}    
	$.q.push();
    }
}

