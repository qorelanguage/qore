#!/usr/bin/env qore

# a generic XML-RPC client
# usage: xml-rpc-client.q [-uURL] method [parameters]

# disable the use of global variables
%no-global-vars
# execute the application class
%exec-class xml_rpc_client

#include some XML-RPC helper functions and class
%include xmlrpc.ql
%include XmlRpcClient.qc

# define command-line options for GetOpt class
const xml_rpc_opts = 
    ( "url"  : "url,u=s",
      "xml"  : "xml,x",
      "lxml" : "literal-xml,X",
      "verb" : "verbose,v",
      "help" : "help,h" );

# define our application class
class xml_rpc_client
{
    private $.o;

    constructor()
    {
	$.process_command_line();
	if (!elements $ARGV)
	    $.usage();

	if (!exists $.o.url)
	    $.o.url = "http://localhost:8081";
    
	#printf("sending command to \"%s\"\n", $s);

	my $cmd = shift $ARGV;

	my $rs;
	try
	{
	    my $xrc = new XmlRpcClient(( "url" : $.o.url ));
	    my $args;
	    foreach my $arg in ($ARGV)
		# in case make_option() returns a list
		$args[elements $args] = $.make_option($arg);
	    
	    if ($.o.verb)
	    {
		if ($.o.xml)
		    printf("outgoing message:\n%s\n", makeFormattedXMLRPCCallStringArgs($cmd, $args));
		else if ($.o.lxml)
		    printf("outgoing message:\n%s\n", makeXMLRPCCallStringArgs($cmd, $args));
		else
		    printf("args=%N\n", $args);
	    }
	    #printf("%s", dbg_node_info($args));
	    $rs = $xrc.callArgs($cmd, $args);
	}
	catch ($ex)
	{
	    printf("%s: %s\n", $ex.err, $ex.desc);
	    exit(1);
	}
	if ($.o.lxml)
	{
	    printf("response:\n%s\n", exists $rs.fault ? makeXMLRPCFaultResponseString($rs.fault.faultCode, $rs.fault.faultString) : makeXMLRPCResponseString($rs.params));
	    return;
	}
	if ($.o.xml)
	{
	    printf("response:\n%s\n", exists $rs.fault ? makeFormattedXMLRPCFaultResponseString($rs.fault.faultCode, $rs.fault.faultString) : makeFormattedXMLRPCResponseString($rs.params));
	    return;
	}
    
	if (exists $rs.fault)
	{
	    printf("ERROR: %s\n", $rs.fault.faultString);
	    exit(1);
	}
	my $info = $rs.params;
	
	if (exists $info)
	{
	    if (type($info) == Type::String)
		print($info);
	    else
		printf("%N", $info);
	    if (type($info) != String || substr($info, -1) != "\n")
		print("\n");
	}
	else
	    print("OK\n");
    }

    private usage()
    {
	printf(
"usage: %s [options] <command> [parameters...]
  -u,--url=arg      sets XML-RPC command url (ex: xmlrpc://host:port)
  -x,--xml          shows literal xml response (formatted)
  -X,--literal-xml  shows literal xml response (unformatted)
  -v,--verbose      shows more information
  -h,--help         this help text
", get_program_name());
	exit(1);
    }

    private process_command_line()
    {
	my $g = new GetOpt(xml_rpc_opts);
	$.o = $g.parse(\$ARGV);
	if (exists $.o{"_ERRORS_"})
	{
	    printf("%s\n", $.o{"_ERRORS_"}[0]);
	    exit(1);
	}
	if ($.o.help)
	    $.usage();
    }

    private make_option($arg)
    {
	if (!strlen($arg))
	    return;

	# see if it's an int
	if (int($arg) == $arg)
	{
	    if (int($arg) >= 2147483648)
		return $arg;
	    return int($arg);
	}
	
	# see if it's an object or list
	my $str  = sprintf("sub get() { return %s; }", $arg);
	#printf("%s\n", $str);
	my $prog = new Program();
	try {
	    $prog.parse($str, "main");
	    my $rv = $prog.callFunction("get");
	    #printf("no exception, rv=%s (%n)\nstr=%s\n", $rv, $rv, $str);
	    # if it's a float, then return a string to preseve formatting
	    if (type($rv) == Type::Float || !exists $rv)
		return $arg;
	    return $rv;
	}

	catch ($ex)
	{
	    #printf("exception %s\n", $ex.err);
	    # must be a string
	    # see if it's a string like "key=val"
	    if ((my $i = index($arg, "=")) != -1)
	    {
		my $h{substr($arg, 0, $i)} = substr($arg, $i + 1);
		return $h;
	    }
	    return $arg;
	}
    }
}
