#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub a()
{
    if ($dl)
	return;
    usleep(1s);
    try {
	return b() + 1;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

synchronized sub b()
{
    if ($dl)
	return;
    usleep(1ms);
    try {
	return a() + 1;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

sub main()
{
    # internal deadlock with synchronized subroutines
    background a();
    b();

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
}

main();
