# -*- mode: qore; indent-tabs-mode: nil -*-
# JSON-RPC handler class definition
#

# to be registered as a handler in the HTTPServer class

namespace JsonRpcHandler {
    const Version = "0.1";

    const InternalMethods = (("function" : "help",
			      "help"     : "shows a list of JSON-RPC methods registered with this handler",
			      "text"     : "help",
			      "logopt"   : 2
			      ),
			     ("function" : "listMethods",
			      "help"     : "lists JSON-RPC method names registered with this handler",
			      "text"     : "system.listMethods",
			      "logopt"   : 2
			       ),
			     ("function" : "system_describe",
			      "help"     : "returns the service description object as per the JSON-RPC 1.1 spec",
			      "text"     : "system.describe",
			      "logopt"   : 2
			      ));
}

class JsonRpcHandler::JsonRpcHandler inherits public HTTPRequestHandler {
    private {
        list $.methods = ();
        hash $.mi;
        int $.loglevel;
    }

    constructor(list $methods) {
	# add internal methods
	foreach my hash $im in (JsonRpcHandler::InternalMethods)
	    $.addMethodInternal($im + ( "internal" : True ));
	
	foreach my hash $m in ($methods) {
	    if (!exists $m.name)
		throw "JSON-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'name' key in method hash (%n)", $m);
	    if (!exists $m.function)
		throw "JSON-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'function' key in method hash (%n)", $m);

	    if (!exists $m.text)
		throw "JSON-RPC-CONSTRUCTOR-ERROR", sprintf("expecting 'text' key in method hash (%n)", $m);
	    delete $m.internal;
	    $.addMethodInternal($m);
	}
    }

    addMethod(string $name, any $func, string $text, string $help, any $logopt, any $cmark) {
	if (!exists $func)
	    throw "JSON-RPC-HANDLER-ADD-METHOD-ERROR", "expecting function name and text name as hash key in argument";

	$.addMethodInternal(( "name"     : $name,
			      "function" : $func,
			      "text"     : $text,
			      "help"     : $help,
			      "logopt"   : $logopt,
			      "cmark"    : $cmark ));
    }

    private addMethodInternal(hash $h) {
	# check for duplicate in method index
	my any $i = $.mi.($h.text);
	if (!exists $i)
	    $i = elements $.methods;

	if (!exists $h.name)
	    $h.name = sprintf("^%s\$", $h.text);
	$.methods[$i] = $h;
    }

    private help() returns hash {
	my hash $h;
	foreach my hash $m in ($.methods) {
	    $h.($m.text).description = $m.help;
	    if (exists $m.params)
		$h.($m.text).params  = $m.params;
	}
	return $h;
    }

    private system_describe() returns hash {
	my string $address = "http://localhost/JSON";
	my list $procs = ();

	foreach my hash $m in ($.methods)
	    if ($m.text != "service.describe")
		$procs += ( "name" : $m.text,
			    "summary" : $m.help );

	return ( "sdversion" : "1.0",
		 "name" : "Qore JSON-RPC Handler",
		 "id" : $address,
		 "version" : JsonRpcHandler::Version,
		 "summary" : "provides a JSON-RPC handler to the HTTP server",
		 #"address" : $address,
		 "procs" : $procs ); 
    }

    private listMethods() returns list {
	my list $l = ();
	foreach my hash $m in ($.methods)
	    $l += $m.text;
	return $l;
    }

    private log(hash $context, string $str) {
        my string $msg = "JSON-RPC ";
        if (exists $context.user)
            $msg += sprintf("user %s ", $context.user);
        $msg += sprintf("from %s: ", $context.source);
        $msg += vsprintf($str, $argv);

        call_function_args($context.logfunc, $msg);
    }

    private callMethod(hash $context, any $params) returns hash {
	my string $method = $context.method;
	# find method function
	my hash $found;
	foreach my hash $m in ($.methods) {
	    if (regex($method, $m.name)) {
		$found = $m;
		break;
	    }
	}
	
	if (exists $found) {
	    # add context marker, if any
	    $context.cmark = $found.cmark;
	    $context.function = $found.function;

	    if (($found.logopt & LP_LEVELMASK) <= $.loglevel && exists $context.logfunc) {
		my string $msg = $method;
		# add arguments to log message
		if ($found.logopt & LP_LOGPARAMS) {
		    $msg += sprintf("(");
		    my int $i = 0;
                    foreach my any $arg in ($params) {
			if (inlist($i++, $found.maskargs))
			    $msg += "<masked>, ";
			else if (type($arg) == Type::Hash && elements $arg) {
			    $msg += "(";
			    foreach my string $k in (keys $arg) {
				if ($k == $found.maskkey)
				    $msg += sprintf("%s=<masked>, ", $k);
			        else
				    $msg += sprintf("%s=%n, ", $k, $arg.$k);
			    }
			    splice $msg, -2, 2;
			    $msg += "), ";
			}
			else
			    $msg += sprintf("%n, ", $arg);
		    }
		    # remove the last two characters from the string if any were added
		    if ($i)
			splice $msg, -2, 2;
		    $msg += ")";
		}

                $.log($context, $msg);
	    }
	    #printf("about to call function '%s' (method=%s params=%n)\n", $found.function, $method, $params);
	    my any $rv;
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

            my hash $h.body = makeJSONRPCResponseString("1.1", $context.jsonid, $rv);
	    return $h;
	}
	else
	    return ( "code" : 500, "body" : sprintf("JSON-RPC-SERVER-UNKNOWN-METHOD: unknown method %n", $method) );
    }

    do_param(any $value, reference $param) {
	if (exists $param) {
	    if (type($param) != Type::List)
		$param = list($param);
	    $param += $value;
	}
	else
	    $param = $value;
    }

    # method called by HTTPServer
    handleRequest(hash $context, hash $hdr, any $body, any $close) returns hash {
	#printf("jsonrpc handler context=%n hdr=%n body=%n\n", $context, $hdr, $body);

	my hash $jsonrpc;

	if ($hdr.method == "GET") {
	    my string $path = substr($hdr.path, index($hdr.path, "/") + 1);
	    
	    if (!strlen($path))
		return ( "code" : 501,
			 "desc" : "invalid HTTP GET: no path/JSON-RPC method name given" ); 

	    my list $args = split("?", $path);
	    $jsonrpc.method = shift $args;

	    $args = split("&", $args[0]);

	    # process arguments
	    if (elements $args) {
		my any $params;
		foreach my any $arg in ($args) {
		    my int $i;
		    if (($i = index($arg, "=")) == -1)
			continue;

		    my string $key = substr($arg, 0, $i);
		    my string $value = substr($arg, $i + 1);
		    # if it is a positional parameter
		    if (int($key) == $key) {
			# check for reasonable argument limits, otherwise ignore
			if ($key > 0 && $key < 9999)
			    $.do_param($value, \$params[$key - 1]);
		    }
		    else
			$.do_param($value, \$params.$key);
		}
		$jsonrpc.params = $params;
		#printf("params=%N\n", $params);
	    }
	    
	    if (index($jsonrpc.method, ".") == -1)
		$jsonrpc.method = "omq.system." + $jsonrpc.method;
	}
	else {
	    if ($hdr.method != "POST")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle method %n", $hdr.method) );

	    # accept any content-type with "json" in it, otherwise throw an error
	    #if ($hdr."content-type" !~ /json/)
	    #	return ( "code" : 501,
	    #		 "body" : sprintf("don't know how to handle content-type %n", $hdr."content-type") );

	    try {
		$jsonrpc = parseJSON($body);
	    }
	    catch (hash $ex) {
		my string $estr = sprintf("%s: %s", $ex.err, $ex.desc);
		return ( "code"   : 500,
			 "errlog" : $estr,
			 "body"   : $estr );
	    }
	}
	$context.method = $jsonrpc.method;
	$context.jsonid = $jsonrpc.id;
	try {
	    #printf("msg=%s\nxmlrpc=%N\n", $body, $jsonrpc);fflush();
	    return ( "code" : 200, "hdr" : ( "Content-Type" : "application/json" ) ) + $.callMethod($context, $jsonrpc.params);
	}
	catch (hash $ex) {
	    my string $str = sprintf("%s: %s", $ex.err, $ex.desc);
	    return ( "code"   : 200,
		     "errlog" : $str,
		     "hdr"    : ( "Content-Type" : "application/json" ),
		     "body"   : makeJSONRPC11ErrorString(104, $str, $jsonrpc.id, $ex.arg) );
	}
    }
}
