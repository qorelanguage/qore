# XML-RPC handler class definition
#
# Copyright (C) 2006, 2007 David Nichols
# to be registered as a handler to the Qore HTTP server

namespace XmlRpcHandler
{
    const Version = "0.3.3";

    const InternalMethods = (("function" : "help",
			      "help"     : "shows a list of XML-RPC methods registered with this handler",
			      "text"     : "help",
			      "logopt"   : 2
			      ),
			     ("function" : "listMethods",
			      "help"     : "lists XML-RPC method names registered with this handler",
			      "text"     : "system.listMethods",
			      "logopt"   : 2
			       ));
}

class XmlRpcHandler::XmlRpcHandler {
    private $.methods, $.mi;

    constructor($methods)
    {
	$.methods = ();

	# add internal methods
	foreach my $im in (XmlRpcHandler::InternalMethods)
	    $.addMethodInternal($im + ( "internal" : True ));
	
	foreach my $m in ($methods)
	{
	    if (!exists $m.name)
		throw "XML-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'name' key in method hash (%n)", $m);
	    if (!exists $m.function)
		throw "XML-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'function' key in method hash (%n)", $m);

	    if (!exists $m.text)
		throw "XML-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'text' key in method hash (%n)", $m);
	    delete $m.internal;
	    $.addMethodInternal($m);
	}
    }

    addMethod($name, $func, $text, $help, $logopt, $cmark)
    {
	if (!exists $func || !exists $text)
	    throw "XML-RPC-SERVER-ADD-METHOD-PARAMETER-ERROR", "expecting function name and text name as hash key in argument";

	$.addMethodInternal(( "name"     : $name,
			      "function" : $func,
			      "text"     : $text,
			      "help"     : $help,
			      "logopt"   : $logopt,
			      "cmark"    : $cmark ));
    }

    private addMethodInternal($h)
    {
	# check for duplicate in method index
	my $i = $.mi.($h.text);
	if (!exists $i)
	    $i = elements $.methods;

	if (!exists $h.name)
	    $h.name = sprintf("^%s\$", $h.text);
	$.methods[$i] = $h;
    }

    private help($c)
    {
	my $h;
	foreach my $m in ($.methods)
	{
	    $h.($m.text).description = $m.help;
	    if (exists $m.params)
		$h.($m.text).params  = $m.params;
	}
	return $h;
    }

    private listMethods($c)
    {
	my $l = ();
	foreach my $m in ($.methods)
	    $l += $m.text;
	return $l;
    }

    private log($func)
    {
	$argv[0] = "XmlRpcHandler " + $argv[0];
	call_function_args($func, $argv);
    }

    private callMethod($context, $params)
    {
	my $h;

	my $method = $context.method;
	# find method function
	my $found;
	foreach my $m in ($.methods)
	{
	    if (regex($method, $m.name))
	    {
		$found = $m;
		break;
	    }
	}
	
	if (exists $found)
	{
	    # add context marker, if any
	    $context.cmark = $found.cmark;
	    $context.function = $found.function;

	    if (($found.logopt & LP_LEVELMASK) <= $.loglevel && exists $context.logfunc)
	    {
		my $msg = sprintf("from %s: %s", $context.source, $method);
		if ($found.logopt & LP_LOGPARAMS)
		    $msg += sprintf(", params: %n", $params);
		$.log($context.logfunc, $msg);
	    }
	    #printf("about to call function '%s' (method=%s params=%n)\n", $found.function, $method, $params);
	    my $rv;

	    if (type($params) == Type::List)
		unshift $params, $context;
	    else if (exists $params)
		$params = ($context, $params);
	    else
		$params = $context;
	    
	    if ($found.internal)
		$rv = callObjectMethodArgs($self, $found.function, $params);
	    else
		$rv = call_function_args($found.function, $params);

	    $h.body = makeXMLRPCResponseString($rv);
	    return $h;
	}
	else
	    throw "XML-RPC-SERVER-UNKNOWN-METHOD", sprintf("unknown method %n", $method);
    }

    # method called by HTTPServer
    handleRequest($context, $hdr, $body, $close)
    {
	#printf("xmlrpc handler context=%n hdr=%n body=%n\n", $context, $hdr, $body);

	my $xmlrpc;

	if ($hdr.method == "GET")
	{
	    my $path = substr($hdr.path, index($hdr.path, "/") + 1);
	    
	    if (!strlen($path))
		return ( "code" : 501,
			 "desc" : "invalid HTTP GET: no path/XML-RPC method name given" ); 
	    
	    if (index($path, ".") == -1)
		$path = "omq.system." + $path;
	    $xmlrpc.methodName = $path;
	}
	else
	{
	    if ($hdr.method != "POST")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle method %n", $hdr.method) );

	    if ($hdr."content-type" != "text/xml")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle content-type %n", $hdr."content-type") );

	    try {
		$xmlrpc = parseXMLRPCCall($body);
	    }
	    catch ($ex)
	    {
		return ( "code"   : 200,
			 "errlog" : sprintf("%s: %s", $ex.err, $ex.desc),
			 "hdr"    : ( "Content-Type" : "text/xml" ),
			 "body"   : makeXMLRPCFaultResponseString(0, $ex.err) );
	    }
	}
	try {
	    #printf("msg=%s\nxmlrpc=%N\n", $body, $xmlrpc);fflush();
	    $context.method = $xmlrpc.methodName;
	    return ( "code" : 200, "hdr" : ( "Content-Type" : "text/xml" ) ) + $.callMethod($context, $xmlrpc.params);
	}
	catch ($ex)
	{
	    my $str = sprintf("%s: %s", $ex.err, $ex.desc);
	    #$.log($context.logfunc, $str);
	    return ( "code"   : 200,
		     "errlog" : $str,
		     "hdr"    : ( "Content-Type" : "text/xml" ),
		     "body"   : makeXMLRPCFaultResponseString(0, $str) );
	}
    }
}
