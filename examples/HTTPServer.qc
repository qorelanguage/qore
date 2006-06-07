# HTTP multi-threaded server class definition
# note that this server suffers from the following limitations:
# *) not really HTTP 1.1 compliant
# *) can only listen on one port

namespace HTTPServer
{
    const Version = "0.1.4";

    const ReadTimeout = 30000;  # recvs timeout after 60 seconds
    const PollTimeout = 5000;   # check for exit every 5 seconds while waiting

    # logging options
    const LP_LOGPARAMS = 1 << 16;
    const LP_LEVELMASK = LP_LOGPARAMS - 1;

    const HTTPCodes = 
	( 
	  # 100s: Informational
	  "100" : "Continue",
	  "101" : "Switching Protocols",

          # 200s: Success
          "200" : "OK",
          "201" : "Created",
          "202" : "Accepted",
          "203" : "Non-Authoritative Information",
          "204" : "No Content",
          "205" : "Reset Content",
          "206" : "Partial Content",
        
          # 300s: Redirection
          "300" : "Multiple Choices",
          "301" : "Moved Permanently",
          "302" : "Found",
          "303" : "See Other",
          "304" : "Not Modified",
          "305" : "Use Proxy",
          #"306" : "(Reserved)",
          "307" : "Temporary Redirect",

	  # 400s: Client Errors
	  "400" : "Bad Request",
	  "401" : "Unauthorized",
	  "402" : "Payment Required",
	  "403" : "Forbidden",
	  "404" : "Not Found",
	  "405" : "Method Not Allowed",
	  "406" : "Not Acceptable",
	  "407" : "Proxy Authentication Required",
	  "408" : "Request Timeout",
	  "409" : "Conflict",
	  "410" : "Gone",
	  "411" : "Length Required",
	  "412" : "Precondition Failed",
	  "413" : "Request Entity Too Large",
	  "414" : "Request-URI Too Long",
	  "415" : "Unsupported Media Type",
	  "416" : "Requested Range Not Satisfiable",
	  "417" : "Expectation Failed",

	  # 500s: Server Errors
	  "500" : "Internal Server Error",
	  "501" : "Not Implemented",
	  "502" : "Bad Gateway",
	  "503" : "Service Unavailable",
	  "504" : "Gateway Timeout",
	  "505" : "HTTP Version Not Supported",
	  "509" : "Bandwidth Limit Exceeded"
	  );
}

class HTTPServer::HTTPServer inherits private Qore::Socket {
    private $.socket, $.logfunc, $.errlogfunc, $.cThreads, $.exit, $.seqSessions,
            $.httpserverstring, $.orig, $.tid, $.stopped, $.handlers, $.defaultHandler;

    constructor($sock, $logfunc, $errlogfunc, $loglevel)
    {
	$.logfunc = $logfunc;
	$.errlogfunc = $errlogfunc;
	$.loglevel = $loglevel; # + 20;
	
	if (exists $logfunc && !existsFunction($logfunc))
	{
	    delete $self;
	    throw "HTTP-SERVER-ERROR", sprintf("log function '%s' does not exist", $logfunc);
	}
	
	if (exists $errlogfunc && !existsFunction($errlogfunc))
	{
	    delete $self;
	    throw "HTTP-SERVER-ERROR", sprintf("error function '%s' does not exist", $errlogfunc);
	}
	
	# convert socket argument to an integer if it's a port number so a
	# UNIX domain socket will not be opened
	if (int($sock) == $sock)
	    $sock = int($sock);
	
	# bind to socket and reuse address
	if (($.bind($sock, True)) == -1)
	{
	    #printf("can't bind to socket: %s\n", strerror(errno()));fflush();
	    delete $self;
	    throw "HTTP-SERVER-ERROR", 
	    sprintf("could not bind to socket '%s': %s", $sock,
		    strerror(errno()));
	}
	#printf("bound to socket %s\n", $sock);

	# save socket port/path
	$.socket = $sock;
	
	# quit server flag
	$.exit = False;
	
	# connection counter
	$.cThreads = new Counter();
	
	# set listening state on socket
	if ($.listen())
	{
	    delete $self;
	    throw "HTTP-LISTEN-ERROR",
	    sprintf("socket_thread: listen error on socket %s", $sock);
	}
	$.httpserverstring = sprintf("Qore HTTP Server v%s", HTTPServer::Version);
	$.seqSessions = new Sequence();
	
	$.orig = 1;
	$.cThreads.inc();
	$.tid = background $.mainThread();
    }

    copy()
    {
	throw "COPY-ERROR", "this object cannot be copied";
    }

    synchronized stop()
    {
	if (!exists $.cThreads)
	    return;

	$.exit = True;
	# wake up server thread
	try {
	    my $s = new Socket();
	    if (type($.socket) == Type::Int)
		$s.connect("localhost:" + $.socket);
	    else
		$s.connect($.socket);
	    $s.send("quit");
	}
	catch ($ex)
	{
	    # DEBUG
	    printf("%N\n", $ex);
	}
	
	# wait for all connection threads to terminate
	$.cThreads.waitForZero();
	
	$.stopped = True;
    }
    
    getTID()
    {
	return $.tid;
    }
    
    destructor()
    {
	$.stop();
    }
    
    setDefaultHandler($name, $obj)
    {
	$.defaultHandler = ( "name" : $name,
			     "obj"  : $obj );
    }
    
    setHandler($name, $url, $content, $obj)
    {
	$.handlers.$name = 
	    ( "obj"     : $obj,
	      "content" : $content,
	      "url"     : $url );
    }

    private log()
    {
	if (exists $.logfunc)
	{
	    $argv[0] = "HTTPServer: " + $argv[0];
	    call_function_args($.logfunc, $argv);
	}
    }
    
    private logError()
    {
	if (exists $.errlogfunc)
	{
	    $argv[0] = "HTTPServer: " + $argv[0];
	    call_function_args($.errlogfunc, $argv);
	}
    }
    
    private defaultHandler($context, $hdr, $body, $close)
    {
	my $str = "";
	if (strlen($hdr.path))
	    $str = sprintf("url=%n", $hdr.path);
	else
	    $str = "<no URL>";

	if (strlen($hdr."content-type"))
	    $str += sprintf(", content-type=%n", $hdr."content-type");
	else
	    $str += ", <no content-type>";

	$.log("no handler for %s (from %s) hdr=%n", $str, $context.source, $hdr);
	return ( "code" : 501,
		 "body" : "no handler has been registered for this request" );
    }
    
    private sendHTTPError($s, $code, $msg, $close)
    {
	my $str = sprintf("<html><head><title>%s %s</title></head><body><h1>%s</h1>%s<p><hr><address>%s on %s</address></body></html>",
			  $code, HTTPServer::HTTPCodes.$code, HTTPServer::HTTPCodes.$code, html_encode($msg), $.httpserverstring, $.socket);
	
	my $hdr = ( "Content-Type" : "text/html",
		    "Server"       : $.httpserverstring );
	if ($close)
	    $hdr += ( "Connection" : "close" );
	else
	    $hdr += ( "Connection" : "Keep-Alive" );
	
	$s.sendHTTPResponse($code, HTTPServer::HTTPCodes.$code, "1.1", $hdr, $str);
    }

    private handleRequest($s, $context, $hdr, $body, $close, $head)
    {
	# find a handler for the request
	my $m;
	
	#printf("handleRequest() hdr=%n, handlers=%n\n", $hdr, $.handlers);
	foreach my $h in (keys $.handlers)
	{
	    if (exists $hdr."content-type" 
		&& $hdr."content-type" == $.handlers.$h.content)
	    {
		if (strlen($.handlers.$h.url) && strlen($hdr.path) && regex($hdr.path, $.handlers.$h.url))
		{
		    $m = $h;
		    break;
		}
		if (!exists $m)
		    $m = $h;
		continue;
	    }
	    if (strlen($.handlers.$h.url) && strlen($hdr.path) 
		&& regex($hdr.path, $.handlers.$h.url))
		if (!exists $m)
	            $m = $h;
	}
	try {
	    my $rv;
	    
	    #printf("handler=%n: context=%n, hdr=%n, body=%n\n", $m, $context, $hdr, $body);
	    #printf("BEFORE handler=%s", dbg_node_info($.handlers.$m));
	    if (!exists $m)
	    {
		if (exists $.defaultHandler)
		{
		    $m = $.defaultHandler.name;
		    $rv = $.defaultHandler.obj.handleRequest($context, $hdr, $body, \$close);
		}
		else
		{
		    $m = "default";
		    $rv = $.defaultHandler($context, $hdr, $body, \$close);
		}
	    }
	    else
		$rv = $.handlers.$m.obj.handleRequest($context, $hdr, $body, \$close);

	    # if the handler returns an invalid hash
	    if (!exists HTTPCodes.($rv.code))
	    {
		my $str = sprintf("%s handler returned an invalid response", $m);
		$.sendHTTPError($s, 500, $str, $close);
		$.logError($str);
		return;
	    }

	    #printf("handler %s returned: %n\n", $m, $rv);

	    $rv.hdr.Server = $.httpserverstring;
	    if ($rv.code != 200)
	    {
		if (!exists $rv.body)
		    $rv.body = sprintf("unknown error in %s handler", $m);
		
		$.sendHTTPError($s, $rv.code, $rv.body, $close);
	    }
	    else
	    {
		if ($close)
		    $rv.hdr.Connection = "close";
		else
		    $rv.hdr.Connection = "Keep-Alive";
		if ($head)
		    $s.sendHTTPResponse($rv.code, HTTPServer::HTTPCodes.($rv.code), "1.1", $rv.hdr);
		else
		    $s.sendHTTPResponse($rv.code, HTTPServer::HTTPCodes.($rv.code), "1.1", $rv.hdr, $rv.body);
	    }
	    
	    if (exists $rv.log)
		$.log("%s (from %s): %s", $m, $s.source, $rv.log);
	    if (exists $rv.errlog)
		$.logError("%s (from %s): %s", $m, $s.source, $rv.errlog);
	}
	catch ($ex)
	{
	    my $str = sprintf("%s:%d: %s: %s", $ex.file, $ex.line, $ex.err, $ex.desc);
	    $.sendHTTPError($s, 500, $str);
	    $.logError($str);
	}
    }

    private mainThread()
    {
	# start listening
	while (True)
	{
	    my $r = $.accept();

	    if ($.exit)
		break;
	    
	    if (!exists $r)
		continue;

	    # DEBUG
	    #$.log("accepting HTTP connection from %s (%s)", $r.source, $.socket);
	    
	    $.cThreads.inc();
	    background $.connectionThread($r);
	}
	#printf("HTTPServer::mainThread() closing socket %s\n", $.socket);
	$.shutdown();
	$.close();
	$.cThreads.dec();
	#printf("HTTPServer::mainThread() TID %d terminating\n", gettid());
    }

    # thread for handling communication per connection
    private connectionThread($s)
    {
	my $context.source = $s.source;
	$context.id = $.seqSessions.next();
	
	my ($hdr, $body);
	
	try {
	    while (True)
	    {
		if ($.exit)
		    break;
		
		delete $body;
		delete $hdr;

		if (!$s.isDataAvailable(HTTPServer::PollTimeout))
		    continue;
		
		try {
		    $hdr = $s.readHTTPHeader(HTTPServer::ReadTimeout);
		}
		catch ($ex)
		{
		    # according to RFC 2616, sec 8.1.2.1 clients claiming http 1.1 protocol compatibility
		    # SHOULD only close the connection after sending a "connection: close" header, but in 
		    # case they don't, we simply close the connection silently
		    if ($ex.err == "SOCKET-CLOSED")
			break;
		    my $etxt = sprintf("ERROR reading HTTP header: %s: %s", $ex.err, $ex.desc);
		    my $str = sprintf("%s: received from %s via %s", $etxt, $s.source, $.socket);
		    $.logError($str);
		    break;
		}

		# log error and close connection on timeout
		if (!exists $hdr)
		{
		    my $err = sprintf("timed out reading HTTP header after %d ms", HTTPServer::ReadTimeout);
		    my $str = sprintf("%s from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.sendHTTPError($s, 400, $err, True);
		    break;
		}
		#printf("id: %d, hdr=%n\n", $context.id, $hdr);

		if (type($hdr) != Type::Hash)
		{
		    my $err = "malformed HTTP header received";
		    my $str = sprintf("%s from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.sendHTTPError($s, 400, $err);
		    continue;
		}
		
		# remove leading / if present in path
		if ($hdr.path =~ /^\//)
		    $hdr.path = substr($hdr.path, 1);

		# split content-type fields
		my $ct = split(";", $hdr."content-type");
		foreach my $c in (\$ct)
		{
		    trim(\$c);
		    if ($c =~ /charset=/)
		    {
			$c =~ s/charset=//;
			$s.setCharset($c);
		    }
		    else
			$hdr."content-type" = $c;
		}

		# check if we need to close the connection
		my $conn = split(",", tolower($hdr.connection));
		# remove leading and trailing whitespace
		foreach my $c in (\$conn)
		    trim(\$c);

		my $close = (inlist("close", $conn) || ($hdr.http_version != "1.1" && !inlist("keep-alive", $conn)));
		#printf("conn=%n, close=%n\n", $conn, $close);

		# if we need to get a body
		switch ($hdr.method)
		{
		case "HEAD":
		    $.handleRequest($s, $context, $hdr, NOTHING, \$close, True);
		    break;

		case "GET":
		    $.handleRequest($s, $context, $hdr, NOTHING, \$close);
		    break;
		
		case "POST":
		    if (!$hdr."content-length")
		    {
			my $err = "missing 'Content-Length' header";
			my $str = sprintf("%s: received from %s via %s (header=%n)", $err, $s.source, $.socket, $hdr);
			$.logError($str);
			$.sendHTTPError($s, 411, $err);
			break;
		    }
		    try {
			$body = $s.recv($hdr."content-length", HTTPServer::ReadTimeout);
			if (!exists $body)
			    throw "SOCKET-TIMEOUT", sprintf("expected %d bytes, got timeout", $hdr."content-length");
		    }
		    catch ($ex)
		    {
			my $etxt = sprintf("error reading body in POST: %s: %s", $ex.err, $ex.desc);
			my $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			$.logError($str);
			$.sendHTTPError($s, 400, $ex.err, True);
			$close = True;
		    }
	            $.handleRequest($s, $context, $hdr, $body, \$close);
	            break;

  	        default:
		    my $err = sprintf("unknown HTTP method %n", $hdr.method);
		    my $str = sprintf("%s: received from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.sendHTTPError($s, 501, $err);
		    break;
	        }

		if ($close)
		    break;
	    }
	}
	catch ($ex)
	{
	    my $etxt = sprintf("%s: %s", $ex.err, $ex.desc);
	    my $str = sprintf("%s: received from %s via %s", $etxt, $s.source, $.socket);
	    $.logError($str);
	    $.logError(sprintf("hdr=%n", $hdr));
	    $.logError(sprintf("msg=%n", $body));
	}
	
	$s.close();
	$.cThreads.dec();
    }
}
