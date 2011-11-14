#!/usr/bin/env qore

$e = new Datasource(SQL::DSMySQL);

try  {
    throw "testing", "123", "test";
}
catch ($ex) {
    printf("%s\n", $ex.err == "testing" && $ex.desc == "123" && $ex.arg == "test" ? "OK" : "ERROR");
}
try {
    $e.open();
}
catch ($ex) {
    printf("%s\n", $ex.err == "DATASOURCE-MISSING-DBNAME" && !exists $ex.arg ? "OK" : "ERROR");
}
try {
    try {
	try {
	    printf("%s\n", $snope.refresh());
	}
	catch ($ex) {
	    printf("%s\n", ($ex.err == "PSEUDO-METHOD-DOES-NOT-EXIST" && !exists $ex.arg) ? "OK" : "ERROR: " + $ex.err);

	    try {
		try {
		    context gee ($gee) where (%foo == "gee")
			printf("%s\n", %sdfdas);
		}
		catch ($ex) {
		    $desc = shift $argv;
		    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
			   $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
		    throw $snope.blah();
		}		
		throw $snope.sdfds();
	    }
	    catch ($ex) {
		printf("%s\n", ($ex.err == "PSEUDO-METHOD-DOES-NOT-EXIST" && !exists $ex.arg) ? "OK" : "ERROR: " + $ex.err);
		throw "TEST";
	    }
	}
    }
    catch ($ex) {
	printf("%s\n", $ex.err == "TEST" && !exists $ex.arg ? "OK" : "ERROR");
    }
}
catch ($ex) {
    printf("QORE %s Exception in line %d of file %s: %s: %s\n", 
	   $ex.type, $ex.line, $ex.file, $ex.err, $ex.desc);
    context ($gee) where (%whiz == "wdsf")
	printf("%s\n", %dsfdf);
}

try {
    try {
	throw "TEST-ERROR", "this is a test";
    }
    catch () {
	rethrow;
    }
}
catch ($ex) {
    printf("%s\n", $ex.err == "TEST-ERROR" && $ex.callstack[0].type == "rethrow" ? "OK" : "ERROR");
}
