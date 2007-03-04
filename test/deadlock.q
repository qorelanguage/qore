#!/usr/bin/env qore

%require-our

our $dl;  # deadlock flag

synchronized sub a()
{
    if ($dl)
	return;
    usleep(500ms);
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
    usleep(500ms);
    try {
	return a() + 1;
    }
    catch ($ex)
    {
	printf("%s: %s\n", $ex.err, $ex.desc); 
	$dl = True;
    }
}

background a();
b();
