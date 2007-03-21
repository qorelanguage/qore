# HTTP multi-threaded server class definition
# note that this server suffers from the following limitations, among others:
# *) not really HTTP 1.1 compliant
# *) no real content-type handling

# changes for v0.2.1:
# * implementing logic to handle "deflate" and "gzip" content-encoding
# * chunked content-encoding supported for POSTs
# * Date: header always sent as per HTTP 1.1 spec

# changes for v0.2.0: 
# * modular/multiple listener support added
# * https support added

namespace HTTPServer
{
    const Version = "0.2.1";

    const ReadTimeout = 30000;  # recvs timeout after 30 seconds
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

# Listener will be assigned to private members of the HTTPServer, so it does not need to declare private members, etc
class HTTPServer::Listener inherits Qore::Socket {
    
    # params: server, socket, [cert, key]
    constructor($server, $ss, $sock, $cert, $key)
    {
	# save a reference to the server object
	$.serv = $server;
	$.ss = $ss;

	# convert socket argument to an integer if it's a port number so a UNIX domain socket will not be opened
	if (int($sock) == $sock)
	    $sock = int($sock);

	# create SSL certificate and private key objects from PEM files if passed
	if (strlen($cert))
	{
	    $.cert = new SSLCertificate($cert);
	    $.setCertificate($.cert);
	    if (strlen($key))
		$.key = new SSLPrivateKey($key);
	    else
		$.key = new SSLPrivateKey($cert);
	    $.setPrivateKey($.key);
	    $.ssl = True;
	}
	else if (strlen($key))
	    throw "LISTENER-CONSTRUCTOR-ERROR", "a certificate PEM file must be given if a private key file is passed";

	# bind to socket and reuse address
	if ($.bind($sock, True))
	    throw "BIND-ERROR", strerror(errno());

	# save socket port/path
	$.socket = $sock;
	
	# connection counter
	$.cThreads = new Counter();
	
	# set listening state on socket
	if ($.listen())
	    throw "HTTP-LISTEN-ERROR", sprintf("listen error on socket %s", $sock);

	# start main listener thread
	$.cThreads.inc();
	$.tid = background $.mainThread();
    }

    copy()
    {
	throw "COPY-ERROR", "this object cannot be copied";
    }

    destructor()
    {
	$.stop();
    }
    
    synchronized stop()
    {
	if (!exists $.cThreads || $.stopped)
	    return;

	$.exit = True;
	# wake up main listener thread
	try {
	    my $s = new Socket();
	    #printf("Listener::stop() socket=%N\n", $.socket);
	    if (type($.socket) == Type::Int)
		$s.connect("127.0.0.1:" + $.socket);
	    else
		$s.connect($.socket);
	    $s.send("quit");
	}
	catch ($ex)
	{
	    printf("LISTENER-SHUTDOWN-ERROR: %s: %s\n", $ex.err, $ex.desc);
	}
	
	# wait for all connection threads to terminate
	$.cThreads.waitForZero();
	
	$.stopped = True;
	$.log("stopped listener");
    }

    private log()
    {
	$argv[0] = sprintf("listener(%s): %s", $.socket, $argv[0]);
	$.serv.logArgs($argv);
    }
    
    private logError()
    {
	$argv[0] = sprintf("listener(%s): %s", $.socket, $argv[0]);
	$.serv.logErrorArgs($argv);
    }
    
    private mainThread()
    {
	# start listening
	while (True)
	{
	    my $r;
	    try
	    {
		if ($.ssl)
		{
		    $r = $.acceptSSL();
		    $.log("secure connection (%s %s) from %s", $r.getSSLCipherName(), $r.getSSLCipherVersion(), $r.source);
		}
		else
		    $r = $.accept();
	    }
	    catch ()
		delete $r;

	    if ($.exit)
		break;
	    
	    if (!exists $r)
		continue;

	    # DEBUG
	    #printf("listener(%s): accepting HTTP connection from %s (%s)\n", $.socket, $r.source);
	    #$.log("accepting HTTP connection from %s", $r.source);
	    
	    $.cThreads.inc();
	    background $.connectionThread($r);
	}
	#printf("Listener::mainThread() closing socket %s\n", $.socket);
	$.shutdown();
	$.close();
	$.cThreads.dec();
	#printf("Listener::mainThread() TID %d terminating\n", gettid());
    }

    # thread for handling communication per connection
    private connectionThread($s)
    {
	my $context = 
	    ( "source"     : $s.source,
	      "socket"     : $.socket,
	      "id"         : $.ss.next() );
	
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
		    $.serv.sendHTTPError($s, 400, $err, True);
		    break;
		}
		#printf("HTTP DEBUG: id: %d, hdr=%n\n", $context.id, $hdr);

		if (type($hdr) != Type::Hash)
		{
		    my $err = "malformed HTTP header received";
		    my $str = sprintf("%s from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.serv.sendHTTPError($s, 400, $err);
		    continue;
		}
		
		# remove leading / if present in path
		if ($hdr.path =~ /^\//)
		    $hdr.path = substr($hdr.path, 1);

		# process ascii encodings in url if present
		if (elements (my $xl = ($hdr.path =~ x/(%[0-9a-fA-F][0-9a-fA-F])/)))
		{
		    foreach my $x in ($xl)
		    {
			my $i = index($hdr.path, $x);
			#printf("x=%n i=%d %n %n %n\n", $x, $i, substr($hdr.path, 0, $i), chr(hexToInt(substr($x, 1))), substr($hdr.path, $i + 3));
			try {
			    $hdr.path = substr($hdr.path, 0, $i) + chr(hexToInt(substr($x, 1))) + substr($hdr.path, $i + 3);
			}
			catch ()
			{
			    # break if there are invalid hex digits
			    break;
			}
		    }
		}

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

		if ($hdr.method != "POST" && ($hdr."content-length" || $hdr."transfer-encoding" == "chunked"))
		{
		    my $err = sprintf("body sent with %s method", $hdr.method);
		    my $str = sprintf("%s: received from %s via %s (header=%n)", $err, $s.source, $.socket, $hdr);
		    $.logError($str);
		    $.serv.sendHTTPError($s, 411, $err, True);
		    break;
		}

		# check if the content-encoding header is misused to specify the character encoding
		if ($hdr."content-encoding" =~ /iso-/i || $hdr."content-encoding" =~ /utf-/i)
		{
		    $.setCharset($hdr."content-encoding");
		    delete $hdr."content-encoding";
		}

		# if we need to get a body
		switch ($hdr.method)
		{
		case "HEAD":
		    $.serv.handleRequest($s, $context, $hdr, NOTHING, \$close, True);
		    break;

		case "GET":
		    $.serv.handleRequest($s, $context, $hdr, NOTHING, \$close);
		    break;
		
		case "POST":
		    if ($hdr."transfer-encoding" == "chunked")
		    {
			try {
			    if (exists $hdr."content-encoding")
				$hdr += $s.readHTTPChunkedBodyBinary(HTTPServer::ReadTimeout);
			    else
				$hdr += $s.readHTTPChunkedBody(HTTPServer::ReadTimeout);
			    $body = $hdr.body;
			    $hdr -= "body";
			}
			catch ($ex)
			{
			    my $etxt = sprintf("error reading chunked body in POST: %s: %s", $ex.err, $ex.desc);
			    my $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHTTPError($s, 400, $etxt, True);
			    $close = True;
			    break;
			}
		    }
		    else if (!$hdr."content-length")
		    {
			my $err = "missing body (no 'Content-Length' header, no chunked encoding)";
			my $str = sprintf("%s: received from %s via %s (header=%n)", $err, $s.source, $.socket, $hdr);
			$.logError($str);
			$.serv.sendHTTPError($s, 411, $err, True);
			$close = True;
			break;
		    }
		    else
		    {
			try {
			    if (exists $hdr."content-encoding")
				$body = $s.recvBinary($hdr."content-length", HTTPServer::ReadTimeout);
			    else
				$body = $s.recv($hdr."content-length", HTTPServer::ReadTimeout);
			    #printf("%s\n", $body);
			    if (!exists $body)
				throw "SOCKET-TIMEOUT", sprintf("expected %d bytes, got timeout", $hdr."content-length");
			}
			catch ($ex)
			{
			    my $etxt = sprintf("error reading body in POST (Content-Length: %d): %s: %s", $hdr."content-length", $ex.err, $ex.desc);
			    my $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHTTPError($s, 400, $etxt, True);
			    $close = True;
			    break;
			}
		    }
		    # handle content-encoding
		    if (exists $hdr."content-encoding")
		    {
			#printf("context=%n\nhdr=%n\nSHA1(body)=%n, body=%N\n", $context, $hdr, SHA1($body), $body);
			try {
			    switch ($hdr."content-encoding")
			    {
				case "deflate":
				case "x-deflate":
				    $body = uncompress_to_string(binary($body));
				    #printf("AFTER body=%N\n", $body);
				    break;
				case "gzip":
				case "x-gzip":
				    $body = gunzip_to_string(binary($body));
				    #printf("AFTER body=%N\n", $body);
				    break;
			        default:
				    throw "UNSUPPORTED-CONTENT-ENCODING", "don't know how to handle this content-encoding";
			    }
			}
			catch ($ex)
			{
			    my $etxt = sprintf("error processing content encoding %s: %s: %s", $hdr."content-encoding", $ex.err, $ex.desc);
			    my $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHTTPError($s, 501, $etxt, True);
			    $close = True;
			    break;
			}
		    }
	            $.serv.handleRequest($s, $context, $hdr, $body, \$close);
	            break;

  	        default:
		    my $err = sprintf("unknown HTTP method %n", $hdr.method);
		    my $str = sprintf("%s: received from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.serv.sendHTTPError($s, 501, $err, True);
		    $close = True;
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

class HTTPServer::HTTPServer {
    private 
	$.logfunc, 
	$.errlogfunc, 
	$.cThreads, 
	$.exit, 
	$.seqSessions,
	$.httpserverstring, 
	$.stopped, 
	$.handlers, 
	$.defaultHandler,
	$.listeners, 
	$.seqListeners, 
	$.lm;                   # listener mutex

    constructor($logfunc, $errlogfunc)
    {
	$.logfunc = $logfunc;
	$.errlogfunc = $errlogfunc;
	
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
	
	# quit server flag
	$.exit = False;
	
	$.httpserverstring = sprintf("Qore HTTP Server v%s", HTTPServer::Version);
	$.seqSessions  = new Sequence();
	$.seqListeners = new Sequence();
	$.lm = new Mutex();
    }

    addListener($sock, $cert, $key)
    {
	$.lm.lock();
	on_exit $.lm.unlock();

	my $l = new Listener($self, $.seqSessions, $sock, $cert, $key);
	my $id = $.seqListeners.next();
	$l.id = $id;
	$.listeners.$id = $l;
	return $id;
    }

    copy()
    {
	throw "COPY-ERROR", "this object cannot be copied";
    }

    stop()
    {
	# shutdown all listeners
	$.lm.lock();
	on_exit $.lm.unlock();

	foreach my $id in (keys $.listeners)
	    $.listeners.$id.stop();
    }

    stopListener($id)
    {
	$.lm.lock();
	on_exit $.lm.unlock();

	if (!exists $.listeners.$id)
	    return -1;

	$.listeners.$id.stop();
    }

    getListenerTID($id)
    {
	return $.listeners.$id.tid;
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
    
    logArgs($args)
    {
	if (exists $.logfunc)
	{
	    $args[0] = "HTTPServer: " + $args[0];
	    call_function_args($.logfunc, $args);
	}
    }
    
    logErrorArgs($args)
    {
	if (exists $.errlogfunc)
	{
	    $args[0] = "HTTPServer: " + $args[0];
	    call_function_args($.errlogfunc, $args);
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
		 "body" : sprintf("no handler has been registered for %s", $str) );
    }
    
    sendHTTPError($s, $code, $msg, $close)
    {
	my $str = sprintf("<html><head><title>%s %s</title></head><body><h1>%s</h1>%s<p><hr><address>%s on %s</address></body></html>",
			  $code, HTTPServer::HTTPCodes.$code, HTTPServer::HTTPCodes.$code, html_encode($msg), $.httpserverstring, gethostname());
	
	my $hdr = ( "Content-Type" : "text/html",
		    "Server"       : $.httpserverstring );
	if ($close)
	    $hdr += ( "Connection" : "close" );
	else
	    $hdr += ( "Connection" : "Keep-Alive" );
	
	$s.sendHTTPResponse($code, HTTPServer::HTTPCodes.$code, "1.1", $hdr, $str);
    }

    handleRequest($s, $context, $hdr, $body, $close, $head)
    {
	# handle accept-encoding to compress data if necessary
	if (exists $hdr."accept-encoding")
	{
	    # remove all writespace
	    $hdr."accept-encoding" =~ s/\s//g;
	    my $mq = 0;
	    my $me;
	    foreach my $acc in (split(",", $hdr."accept-encoding"))
	    {
		my $q;
		# get encoding
		my $enc = $acc =~ x/^(\w+|\*)/[0];
		# ignore encodings we don't recognize
		if (!inlist($enc, ("gzip", "deflate", "x-deflate", "x-gzip")))
		    continue;
		# get q value
		$q = $acc =~ x/;q=(.*)$/[0];
		$q = exists $q ? float($q) : 1.0;
		if ($q > $mq)
		{
		    if ($enc =~ /x-/)
			$enc =~ s/x-//;
		    $me = $enc;
		    $mq = $q;
		}
	    }
	    if (exists $me)
		$context.encoding = $me;
	}
		

	# add logging functions
	$context += 
	    ( "logfunc"    : $.logfunc,
	      "errlogfunc" : $.errlogfunc );

	# find a handler for the request
	my $m;
	
	#printf("handleRequest() hdr=%n, handlers=%n\n", $hdr, $.handlers);
	foreach my $h in (keys $.handlers)
	{
	    if (exists $hdr."content-type" && $hdr."content-type" == $.handlers.$h.content)
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

	    if (type($rv) == Type::String)
		$rv = ( "code" : 200,
			"body" : $rv );
	    else if (!exists HTTPCodes.($rv.code)) # if the handler returns an invalid hash

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
		#printf("\n\n\n**** REQUEST: %N\n", $body);
		#printf("\n\n\n**** RESPONSE: %N\n", $rv.body);
		if ($close)
		    $rv.hdr.Connection = "close";
		else
		    $rv.hdr.Connection = "Keep-Alive";

		if (!exists $rv.hdr.Date)
		    $rv.hdr.Date = format_date("Dy, DD Mon YYYY HH:mm:SS", gmtime()) + " GMT";

		if (!exists $rv.hdr.Server)
		    $rv.hdr.Server = $.httpserverstring;

		if ($head)
		    $s.sendHTTPResponse($rv.code, HTTPServer::HTTPCodes.($rv.code), "1.1", $rv.hdr);
		else
		{
		    if ($context.encoding == "deflate")
		    {
			$rv.hdr += ( "Content-Encoding" : "deflate" );
			$rv.body = compress($rv.body);
		    }
		    else if ($context.encoding == "gzip")
		    {
			$rv.hdr += ( "Content-Encoding" : "gzip" );
			$rv.body = gzip($rv.body);
		    }
		    $s.sendHTTPResponse($rv.code, HTTPServer::HTTPCodes.($rv.code), "1.1", $rv.hdr, $rv.body);
		}
	    }
	    
	    if (exists $rv.log)
		$.log("%s (from %s): %s", $m, $s.source, $rv.log);
	    if (exists $rv.errlog)
		$.logError("%s (from %s): %s", $m, $s.source, $rv.errlog);
	}
	catch ($ex)
	{
	    my $str = sprintf("handler: %s: %s:%d: %s: %s", $m, $ex.file, $ex.line, $ex.err, $ex.desc);
	    $.sendHTTPError($s, 500, $str);
	    $.logError($str);
	}
    }

}
