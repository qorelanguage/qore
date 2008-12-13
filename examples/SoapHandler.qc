# SOAP handler class definition
#
# to be registered as a handler to the Qore HTTP server

# v0.2.2: soap 1.2 improvements
# v0.2.1: matched to new WSDL and HTTPServer implementation
# v0.2.0: better WSDL support
# v0.1.0: initial WSDL-based SOAP support (still incomplete)

namespace SoapHandler {
    const Version = "0.2.2";
}

class SoapHandler::SoapHandler {
    private $.methods, $.mi;

    constructor($methods) {
	#foreach my $m in (InternalMethods)
	#    $.addMethodInternal($m);

	foreach my $m in ($methods) {
	    if (!($m.operation instanceof WSOperation))
		throw "SOAP-CONSTRUCTOR-ERROR", sprintf("expecting 'operation' key in method hash to be an instance of class WEOperation (%n)", $m);
	    if (!exists $m.function)
		throw "SOAP-CONSTRUCTOR-ERROR", sprintf("expecting 'function' key in method hash (%n)", $m);

	    delete $m.internal;
	    $.addMethodInternal($m);
	}
    }

    addMethod($op, $func, $help, $logopt, $cmark) {
	if (!($op instanceof WSOperation))
	    throw "SOAP-SERVER-ADDMETHOD-PARAMETER-ERROR", "first argument is not a WSOperation object";

	if (!exists $func)
	    throw "SOAP-SERVER-ADDMETHOD-PARAMETER-ERROR", "second argument is not a function name";

	$.addMethodInternal(( "operation" : $op,
			      "name"      : $op.name,
			      "function"  : $func,
			      "help"      : $help,
			      "logopt"    : $logopt,
			      "cmark"     : $cmark ));
	#printf("methods=%N\n", $.methods);
    }

    private addMethodInternal($method) {
	# map top-level element to method
	$.methods.($method.operation.getTopLevelRequestName()) = $method;
    }

    private help($c) {
	my $h;
	foreach my $cmd in (keys $.methods) {
	    $h.$cmd.description = $.methods.$cmd.help;
	    if (exists $.methods.$cmd.params)
		$h.$cmd.params  = $.methods.$cmd.params;
	}
	return $h;
    }

    private log($context, $str) {
        my $msg = "SOAP ";
        if (exists $context.user)
            $msg += sprintf("user %s ", $context.user);
        $msg += sprintf("from %s: ", $context.source);
        $msg += vsprintf($str, $argv);

        call_function_args($context.logfunc, $msg);
    }

    private static makeSoapFaultResponse($soap12, $err, $desc) {
	my $o = $soap12 ? ENVELOPE_12_NS : ENVELOPE_11_NS;
	$o."soapenv:Envelope"."soapenv:Body" =
	    ( "soapenv:Fault" :
	      ( "faultcode"   : $err,
		"faultstring" : $desc,
		"desc"        : "" ) );

	return makeXMLString($o);
    }

    private callOperation($context, $args) {
	my $element = $context.element;

	my $method = $.methods.$element;

	#printf("DEBUG: found element %N method %N\n", $element, $method);
	if (!exists $method) {
	    if (!exists $element)
		throw "SOAP-SERVER-UNKNOWN-OPERATION", sprintf("cannot find operation element in SOAP call; call context=%N", $context);
	    throw "SOAP-SERVER-UNKNOWN-OPERATION", sprintf("cannot map top-level element %n to a SOAP operation (currently recognized top-level elements: %n)", $element, keys $.methods);
	}

	# NOTE: internal methods have no operation definition and can take no parameters
	my $h;
	if ($method.internal) {
	    $h.body = callObjectMethod($self, $method.function);
	    return $h;
	}

	my $soap12 = $method.operation.isSoap12();

	try {
	    $args = $method.operation.deserializeRequest($args);
	}
	catch ($ex) {
	    return ( "errlog" : sprintf("%s: %s", $ex.err, $ex.desc),
		     "body"   : SoapHandler::makeSoapFaultResponse($soap12, $ex.err, $ex.desc),
		     "hdr"    : ( "Content-Type" : $soap12 ? "application/soap+xml" : "text/xml" ) );
	}

	# add context marker, if any
	$context.cmark = $method.cmark;
	$context.function = $method.function;
	$context.operation = $method.operation;

	if (($method.logopt & LP_LEVELMASK) <= $.loglevel && exists $context.logfunc) {
	    my $msg;

	    $msg = sprintf("SOAP call from %s: %s", $context.source, $method.name);
	    if ($method.logopt & LP_LOGPARAMS) {
		$msg += sprintf("(");
		my $i = 0;
		foreach my $arg in ($args) {
		    if (inlist($i++, $method.maskargs))
			$msg += "<masked>, ";
		    else if (type($arg) == Type::Hash && elements $arg) {
			$msg += "(";
			foreach my $k in (keys $arg) {
			    if ($k == $method.maskkey)
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
	    $h.logreply = True;
	}
	#printf("about to call function '%s' (method=%s params=%N)\n", $method.function, $method.name, $args);flush();
	my $rv;

	if (type($args) == Type::List)
	    unshift $args, $context;
	else if (exists $args)
	    $args = ($context, $args);
	else
	    $args = $context;

	if ($method.internal)
	    $rv = callObjectMethodArgs($self, $method.function, $args);
	else
	    $rv = call_function_args($method.function, $args);

	# if there is an output message, then serialize the response
	if (exists $method.operation.output)
	    $h.body = $method.operation.serializeResponse($rv);

	#$.log($context, "body=%N", $h.body);
	
	$h.hdr."Content-Type" = $soap12 ? "application/soap+xml" : "text/xml";
	return $h;
    }

    # method called by HTTPServer
    handleRequest($context, $hdr, $body) {
	#printf("soap handler context=%n hdr=%n body=%n\n", $context, $hdr, $body);
	$context.http_header = $hdr;
	$context.http_body = $body;
	my $args;
	if ($hdr.method == "GET") {
	    my $path = $hdr.path;
	    
	    if (!strlen($path))
		return ( "code" : 501,
			 "desc" : "invalid HTTP GET: no SOAP method name given" ); 
	     
	    $path = substr($path, index($path, "/") + 1);
	    $context.method = $path;
	}
	else {
	    if ($hdr.method != "POST")
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle method %n", $hdr.method) );

	    if (!inlist($hdr."content-type", ("application/soap+xml", "text/xml")))
		return ( "code" : 501,
			 "body" : sprintf("don't know how to handle content-type %n (expecting 'application/soap+xml')", $hdr."content-type") );

	    try {
		$args = parseXML($body);

		# get SOAP operation name
		my $element;
		if (!exists ($element = ($hdr."qore-orig-content-type" =~ x/action=".*\/(.*)"/)[0])) {
		    WSDL::XSDBase::removeNS(\$args);
		    WSDL::XSDBase::removeNS(\$args.Envelope);

		    my $sbody = $args.Envelope.Body;
		    if (!exists $sbody)
			throw "SOAP-CALL-ERROR", "missing SOAP body in SOAP envelope in SOAP operation call";

		    foreach my $k in (keys $sbody) {
			if ($k == "multiRef" || $k == "^attributes^")
			    continue;
			$element = $k =~ x/.*:(.*)/[0];
			break;
		    }
		}
		    
		if (!exists $element)
		    throw "SOAP-CALL-ERROR", "no operation call found in message";
		
		$context.element = $element;
	    }
	    catch ($ex) {
		my $str = sprintf("exception in %s:%d: %s: %s (1: %N)", $ex.file, $ex.line, $ex.err, $ex.desc, $ex.callstack);
		return ( "code"   : 200,
			 "errlog" : sprintf("%s: %s", $ex.err, $ex.desc),
			 "body"   : SoapHandler::makeSoapFaultResponse(True, $ex.err, $ex.desc),
			 "hdr"    : ( "Content-Type" : "application/soap+xml" )
		    );
	    }
	}
	try {
	    #printf("DEBUG: context: %N\nargs: %N\n", $context, $args);
	    return ( "code" : 200 ) + $.callOperation($context, $args);
	}
	catch ($ex) {
	    my $str = sprintf("exception in %s:%d: %s: %s (2: %N)", $ex.file, $ex.line, $ex.err, $ex.desc, $ex.callstack);
	    return ( "code"   : 200,
		     "errlog" : $str,
		     "body"   : SoapHandler::makeSoapFaultResponse(True, $ex.err, $ex.desc),
		     "hdr"    : ( "Content-Type" : "application/soap+xml" )
		);
	}
    }
}
