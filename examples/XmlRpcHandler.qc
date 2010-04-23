# -*- mode: qore; indent-tabs-mode: nil -*-
# XML-RPC handler class definition
#
# Copyright (C) 2006 - 2010 David Nichols
# to be registered as a handler to the Qore HTTP server

namespace XmlRpcHandler {
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

class XmlRpcHandler::XmlRpcHandler inherits public HTTPRequestHandler {
    private {
        list $.methods = ();
        hash $.mi = hash();
        string $.force_gui_encoding;
        int $.loglevel;
    }

    constructor(list $methods) {
	# add internal methods
	foreach my hash $im in (XmlRpcHandler::InternalMethods)
	    $.addMethodInternal($im + ( "internal" : True ));
	
	foreach my hash $m in ($methods) {
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

    addMethod(string $name, any $func, string $text, string $help, int $logopt, any $cmark) {
	if (!exists $func)
	    throw "XML-RPC-SERVER-ADD-METHOD-PARAMETER-ERROR", "expecting function name and text name as hash key in argument";

	$.addMethodInternal(( "name"     : $name,
			      "function" : $func,
			      "text"     : $text,
			      "help"     : $help,
			      "logopt"   : $logopt,
			      "cmark"    : $cmark ));
    }

    # hack to allow encodings for the Qorus GUI to be forced
    # as a workaround until the GUI's encoding support can be fixed
    forceGuiEncoding(string $enc) {
	$.force_gui_encoding = $enc;
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

    private listMethods() returns list {
	my list $l;
	foreach my hash $m in ($.methods)
	    $l += $m.text;
	return $l;
    }

    private log(hash $context, string $str) {
	my string $msg = "XML-RPC ";
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
	
	if (!exists $found)
	    throw "XML-RPC-SERVER-UNKNOWN-METHOD", sprintf("unknown method %n", $method);
        
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
        #printf("DEBUG: about to call function '%s' (method=%s params=%n)\n", $found.function, $method, $params);

        if (type($params) == Type::List)
            unshift $params, $context;
        else if (exists $params)
            $params = ($context, $params);
        else
            $params = $context;

        my any $rv;        
        if ($found.internal)
            $rv = callObjectMethodArgs($self, $found.function, $params);
        else
            $rv = call_function_args($found.function, $params);

	my hash $h;

        # HACK: if request comes from the GUI and the "force-gui-encoding" option is set, then use it
        if ($context.gui && exists $.force_gui_encoding)
            $h.body = makeXMLRPCResponseStringWithEncoding($.force_gui_encoding, $rv);
        else
            $h.body = makeXMLRPCResponseString($rv);

        return $h;
    }

    # method called by HTTPServer
    handleRequest(hash $context, hash $hdr, any $body, reference $close) returns hash {
	#printf("xmlrpc handler context=%n hdr=%n body=%n\n", $context, $hdr, $body);

	my hash $xmlrpc;

	if ($hdr.method == "GET") {
	    my string $path = substr($hdr.path, index($hdr.path, "/") + 1);
	    
	    if (!strlen($path))
		return ( "code" : 501,
			 "desc" : "invalid HTTP GET: no path/XML-RPC method name given" ); 
	    
	    if (index($path, ".") == -1)
		$path = "omq.system." + $path;
	    $xmlrpc.methodName = $path;
	}
	else {
	    if ($hdr.method != "POST")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle method %n", $hdr.method) );

	    if ($hdr."content-type" != "text/xml")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle content-type %n", $hdr."content-type") );

	    try {
		$xmlrpc = parseXMLRPCCall($body);
	    }
	    catch (hash $ex) {
		$.log($context, "%s: %s", $ex.err, $ex.desc);
		$.log($context, "error parsing XML-RPC string: %s", $body);
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
	catch (hash $ex) {
            $.log($context, "%s: %s", $ex.err, $ex.desc);
	    #log(LL_CRITICAL, "context=%N\nxmlrpc=%N\n", $context, $xmlrpc);
	    #log(LL_CRITICAL, getExceptionString($ex));
	    my string $str = sprintf("%s: %s", $ex.err, $ex.desc);
	    return ( "code"   : 200,
		     "errlog" : $str,
		     "hdr"    : ( "Content-Type" : "text/xml" ),
		     "body"   : makeXMLRPCFaultResponseString($ex.arg, $str) );
	}
    }
}
