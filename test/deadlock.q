#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub a($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    #usleep(1s);
    try {
	return b();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

synchronized sub b($c)
{
    if (exists $c)
    {
	$c.dec();
	$c.waitForZero();
    }
    if ($dl)
	return;
    #usleep(1ms);
    try {
	return a();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
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
    background a($c);
    b($c);

    my $m = new Mutex();
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

    # test Gate
    my $m = new Gate();
    try {
	$m.exit();
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }
    $m.enter();
    try {
	delete $m;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
    }

    # test thread resource tracking: Mutex
    background dt();
}

main();
