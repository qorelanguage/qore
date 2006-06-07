#!/usr/bin/env qore

# require global variables to be declared with "our" before use
%require-our

%include xmlrpc.ql
%include HTTPServer.qc
%include XmlRpcHandler.qc

# default port for server
const DefaultPort = 8081;

const XmlRpcMethods =
    (
     ( "name"     : "^sys\.shutdown\$",
       "text"     : "sys.shutdown",
       "function" : "shutdown",
       "help"     : "shuts down this XML-RPC server",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.arrayOfStructsTest\$",
       "text"     : "validator1.arrayOfStructsTest",
       "function" : "arrayOfStructsTest",
       "help"     : "the 'arrayOfStructsTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.countTheEntities\$",
       "text"     : "validator1.countTheEntities",
       "function" : "countTheEntities",
       "help"     : "the 'countTheEntities' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.easyStructTest\$",
       "text"     : "validator1.easyStructTest",
       "function" : "easyStructTest",
       "help"     : "the 'easyStructTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.echoStructTest\$",
       "text"     : "validator1.echoStructTest",
       "function" : "echoStructTest",
       "help"     : "the 'echoStructTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.manyTypesTest\$",
       "text"     : "validator1.manyTypesTest",
       "function" : "manyTypesTest",
       "help"     : "the 'manyTypesTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.nestedStructTest\$",
       "text"     : "validator1.nestedStructTest",
       "function" : "nestedStructTest",
       "help"     : "the 'nestedStructTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 ),
     ( "name"     : "^validator1\.simpleStructReturnTest\$",
       "text"     : "validator1.simpleStructReturnTest",
       "function" : "simpleStructReturnTest",
       "help"     : "the 'simpleStructReturnTest' method as per the official XML-RPC validator specifications",
       "logopt"   : 0 )
 );

our ($o, $http_server, $xml_rpc_handler);

sub usage()
{
    printf(
"usage: %s [options]
  -p,--port=arg     sets XML-RPC server port ([interface:]port)
  -h,--help         this help text
", get_program_name());
    exit(1);
}

sub log()
{
    my $str = sprintf("%s: ", format_date("YYYY-MM-DD HH:mm:SS", now()));
    vprintf($str + shift $argv + "\n", $argv); 
}

sub process_command_line()
{
    my $opts = 
        ( "port" : "port,p=s",
          "help" : "help,h" );

    my $g = new GetOpt($opts);
    $o = $g.parse(\$ARGV);
    if (exists $o{"_ERRORS_"})
    {
        printf("%s\n", $o{"_ERRORS_"}[0]);
        exit(1);
    }
    if ($o.help || elements $ARGV)
        usage();

    if (!exists $o.port)
	$o.port = DefaultPort;
}

sub arrayOfStructsTest($m)
{
    #printf("arrayOfStructsTest() arg=%N\n", $a);
    my $c;
    foreach my $elem in ($argv)
	$c += $elem.curly;
    #printf("arrayOfStructsTest() result=%N\n", $c);
    return $c;
}

sub countChar($char, $str)
{
    my $c;
    for (my $i = 0; $i < strlen($str); $i++)
	if (substr($str, $i, 1) == $char)
	    $c++;
    return $c;
}

sub countTheEntities($m, $str)
{
    my $h.ctLeftAngleBrackets = countChar("<", $str);
    $h.ctRightAngleBrackets = countChar(">", $str);
    $h.ctAmpersands = countChar("&", $str);
    $h.ctApostrophes = countChar("'", $str);
    $h.ctQuotes = countChar("\"", $str);
    return $h;
}

sub easyStructTest($m, $s)
{
    return $s.moe + $s.larry + $s.curly;
}

sub echoStructTest($m, $s)
{
    return $s;
}

# number, boolean, string, double, dateTime, base64
sub manyTypesTest($m)
{
    return $argv;
}

sub moderateSizeArrayCheck($m)
{
    #printf("moderateSizeArrayCheck() arg=%N\n", $a);
    return $argv[0] + $argv[(elements $argv) - 1];
}

sub nestedStructTest($m, $s)
{
    return $s.2000."04"."01".moe + $s.2000."04"."01".larry + $s.2000."04"."01".curly;
}

sub simpleStructReturnTest($m, $n)
{
    return ( "times10" : $n * 10,
	     "times100" : $n * 100,
	     "times1000" : $n * 1000 );
}

sub shutdown()
{
    background stop();
    return "OK";
}

sub stop()
{
    delete $http_server;
}

sub main()
{
    process_command_line();

    # start HTTP server
    $http_server = new HTTPServer($o.port, "log", "log");

    # create XML-RPC handler
    $xml_rpc_handler = new XmlRpcHandler(XmlRpcMethods);

    # add XML-RPC handler to HTTP server - will handle all requests with
    # URL RPC2* or content-type = "text/xml"
    $http_server.setHandler("xmlrpc", "^RPC2*", "text/xml", $xml_rpc_handler);
    
    printf("HTTP Server listening on port '%s' for XML-RPC requests\n", $o.port);
}

main();
