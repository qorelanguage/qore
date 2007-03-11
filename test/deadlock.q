#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub internal_deadlock_a($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_deadlock_b();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

synchronized sub internal_deadlock_b($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_deadlock_a();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub class_deadlock_a($c, $m, $g)
{
    my $al = new AutoLock($m);
    $c.dec();
    $c.waitForZero();
    try {
	$g.enter();
	$g.exit();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub class_deadlock_b($c, $m, $g)
{
    my $ag = new AutoGate($g);
    $c.dec();
    $c.waitForZero();
    try {
	$m.lock();
	$m.unlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub class_deadlock_c($c, $rw1, $rw2)
{
    my $al = new AutoWriteLock($rw1);
    $c.dec();
    $c.waitForZero();
    printf("%d: rw1 write lock grabbed\n", gettid());
    try {
	printf("%d: about to get rw2 write lock\n", gettid());
	$rw2.writeLock();
	$rw2.writeUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub class_deadlock_d($c, $rw1, $rw2)
{
    #my $al = new AutoReadLock($rw2);
    my $al = new AutoWriteLock($rw2);
    $c.dec();
    $c.waitForZero();
    printf("%d: rw2 read lock grabbed\n", gettid());
    try {
	printf("%d: about to get rw1 read lock\n", gettid());
	$rw1.writeLock();
	$rw1.writeUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    printf("exited\n");
}

sub test_thread_resources()
{
    my $n = new Mutex();
    $n.lock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }        
    $n = new Gate();
    $n.enter();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    my $rw = new RWLock();
    $rw.readLock();
    $rw.readLock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw.writeLock();
    try {
	throwThreadResourceExceptions();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
}

sub cond_test($c, $cond, $m)
{
    $m.lock();
    $c.dec();
    try {
	$cond.wait($m);
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }        
}

sub main()
{
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
    background class_deadlock_a($c, $m, $g);
    class_deadlock_b($c, $m, $g);

    # deadlock tests with other classes
    my $rw1 = new RWLock();
    my $rw2 = new RWLock();

    # increment counter for synchronization
    $c.inc();
    $c.inc();
    background class_deadlock_c($c, $rw1, $rw2);
    class_deadlock_d($c, $rw1, $rw2);

    # mutex tests
    $m.lock();
    try {
	$m.lock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }    
    try {
	$m.unlock();
	$m.unlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    try {
	delete $m;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }    

    # Gate tests
    try {
	$g.exit();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $g.enter();
    try {
	delete $g;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # RWLock tests
    try {
	$rw1.writeUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    try {
	$rw1.readUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    # RWLock tests
    try {
	$rw1.writeLock();
	$rw1.readUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw1.writeUnlock();
    try {
	$rw1.readLock();
	$rw1.writeUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $rw1.readUnlock();

    my $cond = new Condition();
    $m = new Mutex();
    # increment counter for synchronization
    $c.inc();
    $c.inc();
    background cond_test($c, $cond, $m);
    background cond_test($c, $cond, $m);
    $c.waitForZero();
    # lock and unlock to ensure that cond.wait has been called
    usleep(100ms);
    $m.lock();
    $m.unlock();
    delete $m;

    # test thread resource tracking checks
    background test_thread_resources();
}

main();
