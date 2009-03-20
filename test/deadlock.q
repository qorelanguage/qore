#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub internal_deadlock_a($c) {
    if (exists $c) {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_deadlock_b();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

synchronized sub internal_deadlock_b($c) {
    if (exists $c) {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_deadlock_a();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub mutex_gate_deadlock_a($c, $m, $g) {
    my $al = new AutoLock($m);
    $c.dec();
    $c.waitForZero();
    try {
	$g.enter();
	$g.exit();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub mutex_gate_deadlock_b($c, $m, $g) {
    my $ag = new AutoGate($g);
    $c.dec();
    $c.waitForZero();
    try {
	$m.lock();
	$m.unlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub readwrite_deadlock_c($c, $rw1, $rw2) {
    my $al = new AutoWriteLock($rw1);
    $c.dec();
    $c.waitForZero();
    try {
	$rw2.writeLock();
	$rw2.writeUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub readwrite_deadlock_d($c, $rw1, $rw2) {
    my $al = new AutoReadLock($rw2);
    $c.dec();
    $c.waitForZero();
    try {
	$rw1.readLock();
	$rw1.readUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub test_thread_resources() {
    my $n = new Mutex();
    $n.lock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }        
    $n = new Gate();
    $n.enter();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    my $rw = new RWLock();
    $rw.readLock();
    $rw.readLock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw.writeLock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub cond_test($c, $cond, $m) {
    $m.lock();
    $c.dec();
    try {
	$cond.wait($m);
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }        
}

sub rwl_cond_test($c, $cond, $rwl) {
    $rwl.readLock();
    $c.dec();
    try {
	$cond.wait($rwl);
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }        
}

sub counter_test($c) {
    try {
	$c.waitForZero();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub queue_test($q) {
    try {
	$q.get();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub main() {
    # internal deadlock with synchronized subroutines
    my $c = new Counter(2);
    background internal_deadlock_a($c);
    internal_deadlock_b($c);

    # deadlock tests with qore classes and explicit locking
    my $m = new Mutex();
    my $g = new Gate();
    
    # increment counter for synchronization
    $c.inc();
    $c.inc();
    background mutex_gate_deadlock_a($c, $m, $g);
    mutex_gate_deadlock_b($c, $m, $g);

    # deadlock tests with other classes
    my $rw1 = new RWLock();
    my $rw2 = new RWLock();

    # increment counter for synchronization
    $c.inc();
    $c.inc();
    background readwrite_deadlock_c($c, $rw1, $rw2);
    readwrite_deadlock_d($c, $rw1, $rw2);

    # mutex tests
    $m.lock();
    try {
	$m.lock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }    
    try {
	$m.unlock();
	$m.unlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    try {
	delete $m;
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }    

    # Gate tests
    try {
	$g.exit();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $g.enter();
    try {
	delete $g;
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # RWLock tests
    try {
	$rw1.writeUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    try {
	$rw1.readUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    # RWLock tests
    try {
	$rw1.writeLock();
	$rw1.readUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw1.writeUnlock();
    try {
	$rw1.readLock();
	$rw1.writeUnlock();
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw1.readUnlock();

    # make sure threads sleeping on Condition variable wake up with an exception 
    # if the mutex object is deleted in another thread
    my $cond = new Condition();
    $m = new Mutex();
    # increment counter for synchronization
    $c.inc();
    $c.inc();
    background cond_test($c, $cond, $m);
    background cond_test($c, $cond, $m);
    $c.waitForZero();
    # lock and unlock to ensure until there are 2 condition variables waiting on this Mutex
    $m.lock();
    $m.unlock();
    try {
	delete $m;
	throw "NO-EXCEPTION-ERROR";
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # try it with a RWLock object
    $c.inc();
    $c.inc();
    background rwl_cond_test($c, $cond, $rw1);
    background rwl_cond_test($c, $cond, $rw1);
    $c.waitForZero();
    # lock and unlock to ensure until there are 2 condition variables waiting on this Mutex
    $rw1.writeLock();
    $rw1.writeUnlock();
    try {
	delete $rw1;
	throw "NO-EXCEPTION-ERROR";
    }
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # make sure threads sleeping on a counter wake up with an exception 
    # when the counter is deleted
    my $c1 = new Counter();
    $c1.inc();
    background counter_test($c1);
    background counter_test($c1);
    # sleep until there are 2 counter variables waiting on this Mutex
    while ($c1.getWaiting() != 2)
	usleep(100ms);
    try {
	delete $c1;
    }    
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }


    # make sure threads sleeping on a Queue wake up with an exception 
    # when the Queue is deleted
    my $q = new Queue();
    background queue_test($q);
    background queue_test($q);
    # sleep until there are 2 threads waiting on this Queue
    while ($q.getWaiting() != 2)
	usleep(100ms);
    try {
	delete $q;
    }    
    catch ($ex) {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # test thread resource tracking checks
    background test_thread_resources();
}

main();
