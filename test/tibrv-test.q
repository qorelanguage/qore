#!/usr/bin/env qore

## include TIBCO Rendezvous functionality
%requires tibrv

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

sub listen_forever()
{
    my $subject = Subject + ".>";
    my $listener = new TibrvListener($subject);
    printf("listening on %s\n", $subject);
    while (True)
    {
	my $msg = $listener.getMessage();
	printf("%N\n", $msg);
    }
}

sub listener($q)
{
    my $subject = Subject + ".>";
    my $listener = new TibrvListener($subject);
    printf("listening on %s\n", $subject);

    # it takes a little time until RV actually starts listening in the listening thread
    # so we sleep for .5 seconds before we notify the sending thread that the listener
    # has started
    usleep(500000);
    $q.push();

    $msg = $listener.getMessage();
    printf("%N\n", $msg);
    my $sender = new TibrvSender();
    $sender.sendSubject($msg.replySubject, ( "answer" : "hello" ));
}

sub send()
{
    my $q = new Queue();
    background listener($q);
    $q.get();
    my $sender = new TibrvSender();
    my $ans = $sender.sendSubjectWithSyncReply(Subject + ".1", ( "test" : 9999-12-31-23:59:59 ), 5000);
    printf("reply=%N\n", $ans);
}		

sub do_ft_msg($f)
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

sub do_ft_mon($f)
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

sub ft_test()
{
    my $mon = new TibrvFtMonitor(FaultTolerantGroupName, ActivationInterval);
    background do_ft_mon($mon);

    ## it is an error for the same program to join the same fault-tolerant group more than once, 
    ## but we do it here just for testing purposes (if $num > 1)
    my $num = 1;
    my $f;
    for (my $i = 0; $i < $num; $i++)
    {
	$f[$i] = new TibrvFtMember(FaultTolerantGroupName, Weight, ActiveGoal, HeartbeatInterval, ActivationInterval, PreparationInterval);

	background do_ft_msg($f[$i]);
    }  

    sleep(5);

    # we have to stop the event loops in the other threads manually (we could 
    # also call delete to do this), because if there is an active method call 
    # in another thread, the object will not go out of scope even though the 
    # local variable containing the object is going out of scope

    for (my $i = 0; $i < $num; $i++)
	$f[$i].stop();  

    $mon.stop();
}

#printf("%N\n", getModuleList());

send();
#listen_forever();
ft_test();
