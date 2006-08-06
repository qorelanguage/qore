#!/usr/bin/env qore

## include TIBCO Rendezvous functionality
%requires tibrv

%require-our
%exec-class tibrv_test

const Subject = "test";

## fault tolerant params
const HeartbeatInterval = 1000;    # heartbeat interval is 1000 ms = 1s
const ActivationInterval = 2000;   # activation interval is 2s
const PreparationInterval = 3000;  # preparation interval is 3s
const Weight = 1;
const ActiveGoal = 2;
const FaultTolerantGroupName = "TEST.FT_GROUP.1";

const FT_MSG_HASH = 
    ( TIBRVFT_PREPARE_TO_ACTIVATE : "TIBRVFT_PREPARE_TO_ACTIVATE",
      TIBRVFT_ACTIVATE            : "TIBRVFT_ACTIVATE",
      TIBRVFT_DEACTIVATE          : "TIBRVFT_DEACTIVATE",
      TIBRVFT_QORE_STOP           : "TIBRVFT_QORE_STOP" );

# objects of this class will run in the background and report rendezvous advisory messages until deleted
class AdvisoryListener {
    private 
	$.run,  # flag for stopping the object
	$.q;    # blocking confirmation that the object has stopped

    constructor()
    {
	$.q = new Queue();
	$.run = True;
	# start the listening thread
	background $.listen();
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

    private listen()
    {
	my $listener = new TibrvListener("_RV.>");
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

class tibrv_test {

    constructor()
    {
	my $al = new AdvisoryListener();
	$.reliable_send_test();
	$.cm_send();
	$.ft_test();
	delete $al;
    }

    private reliable_send_test()
    {
	my $q = new Queue();
	background $.listener($q);
	$q.get();
	my $sender = new TibrvSender();
	my $ans = $sender.sendSubjectWithSyncReply(Subject + ".1", ( "test" : 9999-12-31-23:59:59 ), 5000);
	printf("RELIABLE SENDER: reply=%N\n", $ans);
    }

    private listener($q)
    {
	my $subject = Subject + ".>";
	my $listener = new TibrvListener($subject);
	printf("RELIABLE LISTENER: listening on %s\n", $subject);

	# it takes a little time until RV actually starts listening in the listening thread
	# so we sleep for .5 seconds before we notify the sending thread that the listener
	# has started
	usleep(500000);
	$q.push();

	my $msg;
	while (True)
	{
	    $msg = $listener.getMessage();
	    printf("RELIABLE LISTENER: %N\n", $msg);
	    if (exists $msg.msg.test)
		break;
	}
	
	my $sender = new TibrvSender();
	$sender.sendSubject($msg.replySubject, ( "answer" : "hello" ));
    }

    private wait_for_cm_complete($o)
    {
	while (True)
	{
	    printf("CERTIFIED SENDER: checking ledger: %N\n", $o.reviewLedger(">"));
	    my $count = 0;
	    foreach my $entry in ($o.reviewLedger(">"))
		$count += $entry.total_msgs;
	    if (!$count)
	    {
		printf("CERTIFIED SENDER: ledger OK\n");
		break;
	    }
	    # sleep for 1/2 second and check again
	    usleep(500000);
	}
    }

    private cm_listener($q)
    {
	my $subject = Subject + ".>";
	my $listener = new TibrvCmListener($subject);
	printf("CERTIFIED LISTENER: listening on %s\n", $subject);
	
	# it takes a little time until RV actually starts listening in the listening thread
	# so we sleep for .5 seconds before we notify the sending thread that the listener
	# has started
	usleep(500000);
	$q.push();
	
	my $sender = new TibrvCmSender();
	
	while (True)
	{
	    my $msg = $listener.getMessage();
	    if (!exists $msg)
		continue;
	    printf("CERTIFIED LISTENER: %N\n", $msg);
	    $sender.sendSubject($msg.replySubject, ( "answer" : "hello" ));
	    if (exists $msg.msg.test)
		break;
	}
	# if we don't sleep here, the sending thread will normally not receive the 2nd message, why?
	# for some reason the ledger does not show if the messages have been received or not
	usleep(500000);
    }

    private cm_send()
    {
	my $q = new Queue();
	background $.cm_listener($q);
	$q.get();
	my $sender = new TibrvCmSender();
	my $ans;
	$ans = $sender.sendSubjectWithSyncReply(Subject + ".1", ( "date" : 9999-12-31-23:59:59 ), 5000);
	printf("CERTIFIED SENDER: 1st reply=%N\n", $ans);
	$ans = $sender.sendSubjectWithSyncReply(Subject + ".1", ( "test" : 9999-12-31-23:59:59 ), 5000);
	printf("CERTIFIED SENDER: 2nd reply=%N\n", $ans);
	# here we can check the ledger that all messages have been received
	$.wait_for_cm_complete($sender);
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
