#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub internal_dl_a($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_dl_b();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

synchronized sub internal_dl_b($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	return internal_dl_a();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub class_dl_a($c, $m, $g)
{
    my $al = new AutoLock($m);
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	$g.enter();
	$g.exit();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub class_dl_b($c, $m, $g)
{
    my $ag = new AutoGate($g);
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    try {
	$m.lock();
	$m.unlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub class_dl_c($c, $rw1, $rw2)
{
    $rw1.readLock();
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
    {
	$rw1.readUnlock();
	return;
    }
    try {
	$rw2.readLock();
	$rw2.readUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
    $rw1.readUnlock();
}

sub class_dl_d($c, $rw1, $rw2)
{
    $rw2.writeLock();
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
    {
	$rw2.writeUnlock();
	return;
    }
    try {
	$rw1.writeLock();
	$rw1.writeUnlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
    $rw2.writeUnlock();
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
    background internal_dl_a($c);
    internal_dl_b($c);

    # deadlock tests with qore classes and explicit locking
    my $m = new Mutex();
    my $g = new Gate();
    
    # increment counter for synchronization
    $c.inc();
    $c.inc();
    $dl = False;
    background class_dl_a($c, $m, $g);
    class_dl_b($c, $m, $g);

    # deadlock tests with other classes
    my $rw1 = new RWLock();
    my $rw2 = new RWLock();

    # increment counter for synchronization
    $c.inc();
    $c.inc();
    $dl = False;
    background class_dl_c($c, $rw1, $rw2);
    class_dl_d($c, $rw1, $rw2);

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
