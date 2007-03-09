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
    $g.enter();
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
    {
	$g.exit();
	return;
    }
    try {
	$m.lock();
	$m.unlock();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
    $g.exit();
}

sub dt()
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
}

sub main()
{
    # internal deadlock with synchronized subroutines
    my $c = new Counter(2);
    background internal_dl_a($c);
    internal_dl_b($c);

    my $m = new Mutex();
    my $g = new Gate();
    
    # deadlock tests with qore classes and explicit locking
    # increment counter for synchronization
    $c.inc();
    $c.inc();
    $dl = False;
    background class_dl_a($c, $m, $g);
    class_dl_b($c, $m, $g);

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

    # test thread resource tracking checks
    background dt();
}

main();
