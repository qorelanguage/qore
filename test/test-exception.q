#!/usr/bin/env qore

$e = new Datasource(SQL::DSMySQL);

try 
{
    throw "testing", "123", "test";
}
catch ($ex)
{
    printf("QORE %s Exception in line %d of file %s: %s: %s\n", $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
}
try
{
    $e.open();
}
catch ()
{
    print("gee\n");
}
try
{
    try
    {
	try
	{
	    printf("%s\n", $snope.refresh());
	}
	catch ($ex)
	{
	    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
		   $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
	    try
	    {
		try
		{
		    context gee ($gee) where (%foo == "gee")
			printf("%s\n", %sdfdas);
		}
		catch ($ex)
		{
		    $desc = shift $argv;
		    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
			   $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
		    throw $snope.blah();
		}		
		throw $snope.sdfds();
	    }
	    catch ($ex) 
	    {
		printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
		       $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
		throw "oh no";
	    }
	}
    }
    catch ($ex)
    {
	printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
	       $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
    }
}
catch ($ex)
{
    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
	   $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
    context ($gee) where (%whiz == "wdsf")
	printf("%s\n", %dsfdf);
}

try
{
    throw "TEST-ERROR", "this is a test";
}
catch ()
{
    rethrow;
}
