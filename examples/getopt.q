#!/usr/bin/env qore

# require global variables to be declared
%require-our
%enable-all-warnings

sub usage()
{
    printf(
"usage: %s [options]
  -s,--string=arg  returns a list of strings in 'string'
  -t,--int=arg     returns an added integer sum of the arguments in 'int'
  -f,--float       returns a floating point value in 'float' 
  -b,--bool        returns a boolean value in 'bool'
  -d,--date        returns a date value in 'date'
  -t               returns a boolean value in 'test'
  -o,--opt         returns an integer sum of arguments in 'opt'\n",
	   basename($ENV."_"));
    exit();
}

sub process_command_line()
{
    if (!elements $ARGV)
	usage();

    my $opts = 
	( # --string,-s will give a list of strings 
	  "string"  : "string,s=s@",  
	  # --int,-t    will give an added integer sum of the arguments
	  "int"     : "int,i=i+",
	  # --float,-f  will give a floating point value 
	  "float"   : "float,f=f",
	  # --bool,-b   will give a boolean value
	  "bool"    : "bool,b=b",
	  # --date,-d   will give a date value
          "date"    : "date,d=d",
	  # --err       will give a list of strings
	  #"err"     : "err=@",
	  # -t          will give a boolean value
	  "test"    : "t",
	  # --opt,-o    will give an integer sum of arguments
	  "opt"     : "opt,o:i+",
	  "help"    : "help,h",
	  #"err1"    : "err,e=+",
	  #"err2"    : "x,err"
	  );
    my $g = new GetOpt($opts);
    # NOTE: by passing a reference to the list, the arguments parsed will be removed from the list
    my $o = $g.parse(\$ARGV);
    if ($o.help)
	usage();
    print("qore GetOpt class test script\n");
    # NOTE: errors will be returned in the $o."_ERRORS_" structure
    printf("o=%N\n", $o);
    printf("ARGV=%N\n", $ARGV);
}

process_command_line();
