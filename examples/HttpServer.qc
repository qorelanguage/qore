# -*- mode: qore; indent-tabs-mode: nil -*-
# @file HttpServer.qc HTTP multi-threaded server class definition

# note that this server suffers from the following limitations, among others:
# *) not really HTTP 1.1 compliant although claims to be
# *) no HTTP mime/multipart handling (support for this ideally needs to be implemented in the qore library)

# changes for v0.3.0:
# added the ability to start and stop listeners on demand

# changes for v0.2.9:
# * updates for new SSL and timeout behavior with with Socket class with qore 0.8.1+
# * set socket encoding to UTF-8 by default
# * add "charset=utf-8" to Content-Type header if not already present
# * add "text/html" to Content-Type header if no content-type is given by the handler
# * fixed setting X.509 certificate and private key for HTTPS listeners
# * require qore >= 0.8.1 for new Socket features

# changes for v0.2.8:
# converted to hard typing for use with %require-types
# require qore >= 0.8.0 for new Socket features

# changes for v0.2.7:
# * set TCP_NODELAY on all sockets to ensure that clients get all data
#   before closing the socket, especially in case of errors
# * require qore >= 0.7.4 for new Socket features

# changes for v0.2.6:
# * minor fixes for SOAP support
# * improved handler matching

# changes for v0.2.5:
# * minor fixes for SOAP support

# changes for v0.2.4:
# * improved Content-Type handling
# * improved URL/path support

# changes for v0.2.3:
# * bzip2 content-encoding support 

# changes for v0.2.2:
# * basic authentication

# changes for v0.2.1:
# * implementing logic to handle "deflate" and "gzip" content-encoding
# * chunked content-encoding supported for POSTs
# * Date: header always sent as per HTTP 1.1 spec

# changes for v0.2.0: 
# * modular/multiple listener support added
# * https support added

%requires qore >= 0.8.1

my hash $tld;

#! abstract base class for external authentication
/** This class should be inherited by a class providing real authentication
  */
class AbstractAuthenticator {
    #! called to check if the connection requires authentication
    /** @return True if authentication is enabled, False if not (False is the default value returned)
      */
    bool requiresAuthentication() {
        return False;
    }
    #! returns the authentication realm as a string
    /** @return the authentication realm as a string (default: \c "Default Authentication Realm")
      */
    string getRealm() {
        return "Default Authentication Realm";
    }
    #! called to authenticate a user for a connection; should return a list of roles for the user if the user is authenticated
    /** This method will only be called if requiresAuthentication() returns True
        @param $user the username to authenticate
        @param $pass the password for the user
        @return a list of roles for the user if the user can be authenticated (this method returns an empty list by default)
        @throw AUTHENTICATION-ERROR subclasses should throw this exception if the user cannot be authenticated (however this method will never throw any exceptions)
      */
    list authenticate(string $user, string $pass = "") {
        return ();
    }
    #! called when the connection requires authentication, but no authentication credentials were supplied, to try to authenticate the connection based on the source IP address
    /** This method will only be called if requiresAuthentication() returns True and no authentication credentials are supplied with the request
        @param $ip the IP address of the source connection
        @param $user a reference to a string that will be populated with the username to use if the request can be authenticated based on the source IP address (this method returns the string \c "admin")
        @return a list of roles if the authentication request is successful (this method returns an empty list by default)
        @throw AUTHENTICATION-ERROR subclasses should throw this exception if the user cannot be authenticated (however this method will never throw any exceptions)
      */
    list authenticateByIP(string $ip, reference $user) {
        return ();
    }
}

#! abstract class that all HTTP request handler objects must inherit from
/** reimplement handleRequest) in subclasses
    @param $cx call context hash
    @param $hdr incoming header hash
    @param $body message body, if any
    @param $close set this boolean reference to True if the connection should be unconditionally closed when the handler returns
    @return a hash with the following keys:
    - \c "code": the HTTP return code (@see HttpCodes)
    - \c "body": the message body to return in the response
  */
class AbstractHttpRequestHandler {
    public {
        #! the optional AbstractAuthenticator for requests to this handler
        *AbstractAuthenticator $.auth;
    }

    #! create the object optionally with the given AbstractAuthenticator
    /** @param $auth the authentication object to use to authenticate connections (see AbstractAuthenticator); if no AbstractAuthenticator object is passed, then by default no authentication will be required
      */
    constructor(*AbstractAuthenticator $auth) {
        $.auth = $auth;
    }

    #! will be called when a request is received that should be directed to the handler
    hash handleRequest(hash $cx, hash $hdr, *data $body, reference $close) {
        return ("code": 501,
                "body": sprintf("default handler has no implementation"));
        
    }
}

# Listener will be assigned to private members of the HttpServer; is not directly accessible from code outside the HttpServer
class HttpListener inherits Qore::Socket {
    private {
        HttpServer $.serv;
        Sequence $.ss;
        *SSLCertificate $.cert;
        *SSLPrivateKey $.key;
        bool $.ssl = False;
        any $.socket;

	# connection counter
	Counter $.cThreads();
        bool $.exit = False;
        bool $.stopped = False;
        int $.id;

        # stop mutex
        Mutex $.sm();
    }

    public {
        # TID of the background listener thread
        int $.tid;
    }

    # params: server, id, session ID sequence object, socket, rbac obj, [cert, key]
    constructor(HttpServer $server, int $id, Sequence $ss, any $sock, *SSLCertificate $cert, *SSLPrivateKey $key) {
        # set character encoding to utf-8
        $.setCharset("utf-8");
        
	# save a reference to the server object
	$.serv = $server;
        $.id = $id;
	$.ss = $ss;

	# convert socket argument to an integer if it's a port number so a UNIX domain socket will not be opened
	if (int($sock) == $sock)
	    $sock = int($sock);
	else if ($sock =~ /^\//) # try to unlink the file first if it's a UNIX domain socket
	    unlink($sock);

	# create SSL certificate and private key objects from PEM files if passed
        if (exists $cert) {
	    $.cert = $cert;
	    $.setCertificate($.cert);
            $.key = $key;
	    $.setPrivateKey($.key);
	    $.ssl = True;
	}

	# bind to socket and reuse address
	if ($.bind($sock, True))
	    throw "BIND-ERROR", strerror(errno());

	# save socket port/path
	$.socket = $sock;

	# set TCP_NODELAY
	$.setNoDelay(True);

	# set listening state on socket
	if ($.listen())
	    throw "HTTP-LISTEN-ERROR", sprintf("listen error on socket %s", $sock);

	# start main listener thread
	$.cThreads.inc();
	$.tid = background $.mainThread();
    }

    copy() {
	throw "COPY-ERROR", "this object cannot be copied";
    }

    destructor() {
	$.stop();
    }
    
    any getAddress() {
        return $.socket;
    }

    int getID() {
        return $.id;
    }

    bool isSecure() {
        return $.ssl;
    }

    hash getInfo() {
        return ( "proto" : $.ssl ? "https" : "http",
                 "bind" : $.socket );
    }

    stopNoWait() {
        background $.stop();
    }

    stop() {
        {
            $.sm.lock();
            on_exit $.sm.unlock();

            if (!exists $.cThreads || $.stopped)
                return;

            if ($.exit)
                return;

            $.exit = True;
            # wake up main listener thread
            try {
                my Socket $s();
                if (type($.socket) == Type::Int)
                    $s.connect("127.0.0.1:" + $.socket);
                else
                    $s.connect($.socket);
                $s.send("quit");
            }
            catch (hash $ex) {
                printf("LISTENER-SHUTDOWN-ERROR: %s: %s\n", $ex.err, $ex.desc);
            }
            
            # wait for all connection threads to terminate
            $.cThreads.waitForZero();
            
            $.stopped = True;
            $.log("stopped listener");
        }

        $.serv.listenerStopped($self);
    }

    private log() {
	$argv[0] = sprintf("listener(%s): %s", $.socket, $argv[0]);
	$.serv.logArgs($argv);
    }
    
    private logError() {
	$argv[0] = sprintf("listener(%s): %s", $.socket, $argv[0]);
	$.serv.logErrorArgs($argv);
    }
    
    private mainThread() {
        on_exit	$.cThreads.dec();

        # DEBUG
        #printf("HTTP DEBUG: listener(%s): mainThread() started TID %d\n", $.socket, gettid());

	# start listening
	while (True) {
	    my Socket $r;
	    try {
		if ($.ssl) {
		    $r = $.acceptSSL();
		    $.log("secure connection (%s %s) from %s", $r.getSSLCipherName(), $r.getSSLCipherVersion(), $r.source);
		}
		else
		    $r = $.accept();
	    }
	    catch ($ex) {
                if ($ex.err != "SOCKET-CLOSED")
                    $.log("error accepting %sconnection: %s: %s", $.ssl ? "secure " : "", $ex.err, $ex.desc);
		delete $r;
            }

	    if ($.exit)
		break;
	    
	    if (!exists $r)
		continue;

	    # DEBUG
	    #printf("HTTP DEBUG: listener(%s): accepting HTTP connection from %s\n", $.socket, $r.source);
	    #$.log("accepting HTTP connection from %s", $r.source);
	    
	    $.cThreads.inc();
	    background $.connectionThread($r);
	}
	#printf("HTTP DEBUG: HttpListener::mainThread() closing socket %s\n", $.socket);
	$.shutdown();
	$.close();
	#printf("HTTP DEBUG: HttpListener::mainThread() TID %d terminating\n", gettid());
    }

    # thread for handling communication per connection
    private connectionThread(Socket $s) {
        on_exit	$.cThreads.dec();

	my hash $cx = 
	    ( "source"      : $s.source,
	      "source-host" : $s.source_host,
	      "socket"      : $.socket,
	      "id"          : $.ss.next() );
	
	my (any $hdr, any $body);

	# set TCP_NODELAY on incoming socket
	$s.setNoDelay(True);
	
	try {
	    while (True) {
		if ($.exit)
		    break;
		
		delete $body;
		delete $hdr;

		if (!$s.isDataAvailable(HttpServer::PollTimeout))
		    continue;
		
		try {
		    $hdr = $s.readHTTPHeader(HttpServer::ReadTimeout);
		}
		catch (hash $ex) {
		    # according to RFC 2616 sec 8.1.2.1, clients claiming http 1.1 
		    # protocol compatibility SHOULD only close the connection after
		    # sending a "connection: close" header, but in 
		    # case they don't, we simply close the connection silently
		    if ($ex.err == "SOCKET-CLOSED") {
			#printf("HTTP DEBUG: socket closed id=%d source=%n\n", $cx.id, $s.source);
			break;
		    }
                    else if ($ex.err == "SOCKET-TIMEOUT") {
                        # log error and close connection on timeout
                        my string $err = sprintf("timed out reading HTTP header after %d ms", HttpServer::ReadTimeout);
                        my string $str = sprintf("%s from %s via %s", $err, $s.source, $.socket);
                        $.logError($str);
                        $.serv.sendHttpError($s, 400, $err, True);
                        break;
                    }
		    my string $etxt = sprintf("ERROR reading HTTP header: %s: %s", $ex.err, $ex.desc);
		    my string $str = sprintf("%s: received from %s via %s", $etxt, $s.source, $.socket);
		    $.logError($str);
		    break;
		}

		# DEBUG:
		#printf("HTTP DEBUG: id: %d, hdr=%n\n", $cx.id, $hdr);

		if (type($hdr) != Type::Hash) {
		    my string $err = "malformed HTTP header received";
		    my string $str = sprintf("%s from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.serv.sendHttpError($s, 400, $err);
		    continue;
		}
		
		# remove leading / if present in path
		if ($hdr.path =~ /^\//)
		    $hdr.path = substr($hdr.path, 1);

		# process ascii encodings in url if present
		$hdr.path = decode_url($hdr.path);

		if (exists $hdr.authorization)
		    $cx.auth = $hdr.authorization;

		$cx.encoding = $.getCharset();

		# save original content-type
		$hdr."_qore_orig_content_type" = $hdr."content-type";

		# split content-type fields
		my list $ct = split(";", $hdr."content-type");
		trim $ct;

		# check for character encoding specification in content-type
		foreach my string $c in (\$ct)	{
		    if ($c =~ /charset=/) {
			$c =~ s/charset=//;
			$s.setCharset($c);
			$cx.encoding = $c;
		    }
		    else if ($c !~ /=/)
			$hdr."content-type" = $c;
		}

		# check if we need to close the connection
		my any $conn = split(",", tolower($hdr.connection));
		# remove leading and trailing whitespace
		trim $conn;

		my bool $close = (inlist("close", $conn) || ($hdr.http_version != "1.1" && !inlist("keep-alive", $conn)));
		#printf("HTTP DEBUG: conn=%n, close=%n\n", $conn, $close);

		if ($hdr.method != "POST" && ($hdr."content-length" || $hdr."transfer-encoding" == "chunked")) {
		    my string $err = sprintf("body sent with %s method", $hdr.method);
		    my string $str = sprintf("%s: received from %s via %s (header=%n)", $err, $s.source, $.socket, $hdr);
		    $.logError($str);
		    $.serv.sendHttpError($s, 411, $err, True);
		    break;
		}

		# check if the content-encoding header is misused to specify the character encoding
		if ($hdr."content-encoding" =~ /iso-/i || $hdr."content-encoding" =~ /utf-/i) {
		    $.setCharset($hdr."content-encoding");
		    $cx.encoding = $hdr."content-encoding";
		    delete $hdr."content-encoding";
		}

		# if we need to get a body
		switch ($hdr.method) {
		case "HEAD":
		    $.serv.handleRequest($.socket, $s, $cx, $hdr, NOTHING, \$close, True);
		    break;

		case "GET":
		    $.serv.handleRequest($.socket, $s, $cx, $hdr, NOTHING, \$close);
		    break;
		
		case "POST":
		    if ($hdr."transfer-encoding" == "chunked") {
			try {
			    if (exists $hdr."content-encoding")
				$hdr += $s.readHTTPChunkedBodyBinary(HttpServer::ReadTimeout);
			    else
				$hdr += $s.readHTTPChunkedBody(HttpServer::ReadTimeout);
			    $body = $hdr.body;
			    $hdr -= "body";
			}
			catch (hash $ex) {
			    my string $etxt = sprintf("error reading chunked body in POST: %s: %s", $ex.err, $ex.desc);
			    my string $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHttpError($s, 400, $etxt, True);
			    $close = True;
			    break;
			}
		    }
		    else if (!$hdr."content-length") {
			my string $err = "missing body (no 'Content-Length' header, no chunked encoding)";
			my string $str = sprintf("%s: received from %s via %s (header=%n)", $err, $s.source, $.socket, $hdr);
			$.logError($str);
			$.serv.sendHttpError($s, 411, $err, True);
			$close = True;
			break;
		    }
		    else {
			try {
			    if (exists $hdr."content-encoding")
				$body = $s.recvBinary($hdr."content-length", HttpServer::ReadTimeout);
			    else
				$body = $s.recv($hdr."content-length", HttpServer::ReadTimeout);
				#printf("HTTP DEBUG: %s\n", $body);
			}
			catch (hash $ex) {
			    my string $etxt = sprintf("error reading body in POST (Content-Length: %d): %s: %s", $hdr."content-length", $ex.err, $ex.desc);
			    my string $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHttpError($s, 400, $etxt, True);
			    $close = True;
			    break;
			}
		    }
		    #printf("HTTP DEBUG: id: %d, body: %n\n", $cx.id, substr($body, 0, 120));
		    # handle content-encoding
		    if (exists $hdr."content-encoding") {
			#printf("HTTP DEBUG: context=%n\nhdr=%n\nSHA1(body)=%n, body=%N\n", $cx, $hdr, SHA1($body), $body);
			try {
			    switch ($hdr."content-encoding") {
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
   				case "bzip2":
				case "x-bzip2":
				    $body = bunzip2_to_string(binary($body));
				    break;
			        default:
				    throw "UNSUPPORTED-CONTENT-ENCODING", "don't know how to handle this content-encoding";
			    }
			}
			catch (hash $ex) {
			    my string $etxt = sprintf("error processing content encoding %s: %s: %s", $hdr."content-encoding", $ex.err, $ex.desc);
			    my string $str = sprintf("%s: received from %s via %s (header=%n)", $etxt, $s.source, $.socket, $hdr);
			    $.logError($str);
			    $.serv.sendHttpError($s, 501, $etxt, True);
			    $close = True;
			    break;
			}
		    }
	            $.serv.handleRequest($.socket, $s, \$cx, $hdr, $body, \$close);
	            break;

  	        default:
		    my string $err = sprintf("unknown HTTP method %n", $hdr.method);
		    my string $str = sprintf("%s: received from %s via %s", $err, $s.source, $.socket);
		    $.logError($str);
		    $.serv.sendHttpError($s, 501, $err, True);
		    $close = True;
		    break;
	        }

		if ($close)
		    break;
	    }
	}
	catch (hash $ex) {
	    my string $etxt = sprintf("%s: %s", $ex.err, $ex.desc);
	    my string $str = sprintf("%s: received from %s via %s", $etxt, $s.source, $.socket);
	    $.logError($str);
	    $.logError(sprintf("hdr=%n", $hdr));
	    #$.logError(sprintf("msg=%n", $body));
	    $.serv.sendHttpError($s, 500, $etxt, True);
	}

	$s.close();
    }
}

#! HttpServer class implements a multithreaded HTTP server primarily for serving RPC-style services
class HttpServer {
    #! version of the HttpServer's implementation
    const Version = "0.3.0";
    #! default read timeout in ms
    const ReadTimeout = 30000;  # recvs timeout after 30 seconds
    #! default poll timeout in ms
    const PollTimeout = 5000;   # check for exit every 5 seconds while waiting

    # logging options
    const LP_LOGPARAMS = 1 << 16;
    const LP_LEVELMASK = LP_LOGPARAMS - 1;

    #! map of HTTP result codes and text messages
    const HttpCodes = 
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

    private {
	any $.logfunc;
	any $.errlogfunc; 

	# quit server flag
	bool $.exit = False;

	Sequence $.seqSessions();
	Sequence $.seqListeners();

	string $.httpserverstring = sprintf("Qore HTTP Server v%s", HttpServer::Version);

	bool $.stopped = False;

	hash $.handlers;
	hash $.defaultHandler;
	hash $.listeners;

        # map of bind addresses to listener IDs
        hash $.smap;

        # listener Gate
	Gate $.lm();
    }

    #! creates the HttpServer; call addListener() to add and start listeners
    constructor(any $logfunc, any $errlogfunc) {
	$.logfunc = $logfunc;
	$.errlogfunc = $errlogfunc;

	if (exists $logfunc && !existsFunction($logfunc))
	    throw "HTTP-SERVER-ERROR", sprintf("log function '%s' does not exist", $logfunc);
	
	if (exists $errlogfunc && !existsFunction($errlogfunc))
	    throw "HTTP-SERVER-ERROR", sprintf("error function '%s' does not exist", $errlogfunc);
    }

    #! calls HttpServer::stop()
    destructor() {
	$.stop();
    }
    
    private int addListenerIntern(string $sock, *SSLCertificate $cert, *SSLPrivateKey $key) {
	$.lm.enter();
	on_exit $.lm.exit();

	my int $id = $.seqListeners.next();
	my HttpListener $l($self, $id, $.seqSessions, $sock, $cert, $key);
	$.listeners.$id = $l;
        $.smap.$sock = $id;
	return $id;
    }

    #! adds and starts a listener and returns the listener ID
    int addListener(softstring $sock, *string $cert_path, *string $key_path) {
        if (!exists $cert_path)
            return $.addListenerIntern($sock);

        my File $f();
        # read in X.509 certificate file
        $f.open2($cert_path);

        my any $cert_data = $cert_path =~ /\.der$/ ? $f.readBinary(-1) : $f.read(-1);
        my SSLCertificate $cert($cert_data);

        my SSLPrivateKey $key;

        # read in private key file
        if (strlen($key_path)) {
            $f.open2($key_path);
            $key = $key_path =~ /\.der$/ ? new SSLPrivateKey($f.readBinary(-1)) : new SSLPrivateKey($f.read(-1));
        }
        else
            $key = new SSLPrivateKey($cert_data);

        return $.addListenerIntern($sock, $cert, $key);
    }

    #! throws an exception; these objects to not support copying
    copy() {
	throw "COPY-ERROR", "this object cannot be copied";
    }

    #! returns a hash of listener information
    hash getListeners() {
        my hash $h;

        map $h.$1 = $.listeners.$1.getInfo(), keys $.listeners;
        return exists $h ? $h : hash();
    }

    #! stops all listeners; does not wait for all connections on the listeners to close
    stopNoWait() {
	# stop all listeners
	$.lm.enter();
	on_exit $.lm.exit();

        map $.listeners.$1.stopNoWait(), keys $.listeners;
    }

    listenerStopped(HttpListener $l) {
	$.lm.enter();
	on_exit $.lm.exit();

        delete $.smap.($l.getAddress());
        remove $.listeners.($l.getID());
    }

    #! stops all listeners; only returns when all connections are closed on all listeners
    stop() {
	# shutdown all listeners
	$.lm.enter();
	on_exit $.lm.exit();

        map $.listeners.$1.stop(), keys $.listeners;
    }

    #! stops a single listener based on its bind address; does not return until all connections on the listener have closed
    stopListener(softstring $bind) {
	$.lm.enter();
	on_exit $.lm.exit();

        my any $id = $.smap.$bind;

	if (!exists $id)
            throw "HTTP-SERVER-ERROR", sprintf("there is no listener with bind address %n", $bind);

        if (elements $.smap == 1)
            throw "HTTP-SERVER-ERROR", "cannot stop last listener";

        $.listeners.$id.stop();
    }

    #! stops a single listener based on its listener ID; does not return until all connections on the listener have closed
    any stopListenerID(softint $id) {
	$.lm.enter();
	on_exit $.lm.exit();

	if (!exists $.listeners.$id)
            throw "HTTP-SERVER-ERROR", sprintf("there is no listener with ID %d", $id);

        if (elements $.smap == 1)
            throw "HTTP-SERVER-ERROR", "cannot stop last listener";

        #my softstring $bind = $.listeners.$id.getAddress();
        $.listeners.$id.stop();
    }

    #! gets the TID of a listener based on its listener ID
    int getListenerTID(softint $id) {
	return $.listeners.$id.tid;
    }
    
    #! sets the default request handler when no other handler can be matched
    setDefaultHandler(string $name, AbstractHttpRequestHandler $obj) {
	$.defaultHandler = ( "name" : $name,
			     "obj"  : $obj );
    }
    
    #! sets a request handler according to the arguments given 
    setHandler(string $name, any $url, any $content, AbstractHttpRequestHandler $obj, any $special_headers) {
	$.handlers.$name = 
	    ( "obj"     : $obj,
	      "content" : $content,
	      "url"     : $url,
	      "shdr"    : $special_headers );
    }

    private log() {
	if (exists $.logfunc) {
	    $argv[0] = "HttpServer: " + $argv[0];
	    call_function_args($.logfunc, $argv);
	}
    }
    
    private logError() {
	if (exists $.errlogfunc) {
	    $argv[0] = "HttpServer: " + $argv[0];
	    call_function_args($.errlogfunc, $argv);
	}
    }
    
    #! calls the log function/closure with the given args
    logArgs(list $args = ()) {
	if (exists $.logfunc) {
	    $args[0] = "HttpServer: " + $args[0];
	    call_function_args($.logfunc, $args);
	}
    }
    
    #! calls the error log function/closure with the given args
    logErrorArgs(list $args = ()) {
	if (exists $.errlogfunc) {
	    $args[0] = "HttpServer: " + $args[0];
	    call_function_args($.errlogfunc, $args);
	}
    }
    
    private hash noHandlerError(hash $cx, hash $hdr, any $body, reference $close) {
	my string $str = "";
	if (strlen($hdr.path))
	    $str = sprintf("url=%n", $hdr.path);
	else
	    $str = "<no URL>";

	if (strlen($hdr."content-type"))
	    $str += sprintf(", content-type=%n", $hdr."content-type");
	else
	    $str += ", <no content-type>";

	$.log("no handler for %s (from %s) hdr=%n", $str, $cx.source, $hdr);
	return ( "code" : 501,
		 "body" : sprintf("no handler has been registered for %s", $str) );
    }
    
    #! sends an HTTP error message on the socket
    sendHttpError(Socket $s, int $code, string $msg, bool $close = False, any $extra_hdrs) {
	my string $str = sprintf("<html><head><title>%s %s</title></head><body><h1>%s</h1>%s<p><hr><address>%s on %s</address></body></html>",
                                 $code, HttpServer::HttpCodes.$code, HttpServer::HttpCodes.$code, html_encode($msg), $.httpserverstring, gethostname());
	
	my hash $hdr = ( "Content-Type" : "text/html;charset=utf-8",
                         "Server"       : $.httpserverstring );
	if ($close)
	    $hdr += ( "Connection" : "close" );
	else
	    $hdr += ( "Connection" : "Keep-Alive" );

	if (type($extra_hdrs) == Type::Hash)
	    $hdr += $extra_hdrs;

        # log exceptions if not "SOCKET-SEND-ERROR" (probably broken pipe)
        try {
	    $s.sendHTTPResponse($code, HttpServer::HttpCodes.$code, "1.1", $hdr, $str);
        }
        catch (hash $ex) {
            if ($ex.err != "SOCKET-SEND-ERROR") {
		my string $estr = sprintf("%s:%d: %s: %s", $ex.file, $ex.line, $ex.err, $ex.desc);
		$.logError($estr);
            }
        }
    }

    #! handles an incoming request
    handleRequest(any $sock, Socket $s, reference $cx, hash $hdr, any $body, reference $close, bool $head = False) {
	# handle accept-encoding to compress data if necessary        
	if (exists $hdr."accept-encoding") {
	    # remove all writespace
	    $hdr."accept-encoding" =~ s/\s//g;
	    my any $mq = 0;
	    my any $me;
	    foreach my string $acc in (split(",", $hdr."accept-encoding")) {
		my any $q;
		# get encoding
		my any $enc = $acc =~ x/^(\w+|\*)/[0];
		# ignore encodings we don't recognize
		if (!inlist($enc, ("gzip", "deflate", "x-deflate", "x-gzip", "bzip2", "x-bzip2")))
		    continue;
		# get q value
		$q = $acc =~ x/;q=(.*)$/[0];
		$q = exists $q ? float($q) : 1.0;
		if ($q > $mq) {
		    if ($enc =~ /x-/)
			$enc =~ s/x-//;
		    $me = $enc;
		    $mq = $q;
		}
	    }
	    if (exists $me)
		$cx.encoding = $me;
	}
        # erase the encoding string on exit
        on_exit remove $cx.encoding;
        
        # parse the path in the request
        my any $url = parseURL($hdr.path);
	if (elements $url == 1 && exists $url.host) {
	    $url = ( "path" : $url.host );
	}

	# add logging functions and url
	$cx += 
	    ( "logfunc"     : $.logfunc,
	      "errlogfunc"  : $.errlogfunc,
	      "url"         : $url 
	    );

	# find a handler for the request
        my AbstractHttpRequestHandler $handler;
	my string $handlerName;

	#printf("HTTP DEBUG: handleRequest() hdr=%n, handlers=%n\n", $hdr, $.handlers);
	foreach my string $h in (keys $.handlers) {
            #printf("HTTP DEBUG: %s: ct=%n, path=%n, hurlre=%n, hcon=%n (url=%n) shdr=%n\n", $h, $hdr."content-type", $hdr.path, $.handlers.$h.url, $.handlers.$h.content, $url, $.handlers.$h.shdr);
	    if (exists $hdr."content-type" && $hdr."content-type" == $.handlers.$h.content) {
		if (strlen($.handlers.$h.url) && strlen($url.path) && regex($url.path, $.handlers.$h.url)) {
                    $handler = $.handlers.$h.obj;
		    $handlerName = $h;
		    break;
		}
		if (!exists $handlerName) {
                    $handler = $.handlers.$h.obj;
		    $handlerName = $h;
                }
		continue;
	    }
	    if (exists $.handlers.$h.shdr) {
		#printf("HTTP DEBUG: checking special headers %n: %n\n", $.handlers.$h.shdr, $hdr);
		foreach my any $shdr in ($.handlers.$h.shdr) {
		    if (exists $hdr.$shdr) {
			#printf("HTTP DEBUG: got special header %s: %s\n", $shdr, $hdr.$shdr);
                        $handler = $.handlers.$h.obj;
			$handlerName = $h;
			break;
		    }
		}
	    }

	    # path trumps content-type
	    #printf("regex(%n, %s, %n, %s) %n && %n && %n\n", $hdr.path, type($hdr.path), $.handlers.$h.url, type($.handlers.$h.url), strlen($.handlers.$h.url), strlen($hdr.path), regex($url.path, $.handlers.$h.url));
	    if (strlen($.handlers.$h.url) && strlen($hdr.path) && regex($url.path, $.handlers.$h.url)) {
                $handler = $.handlers.$h.obj;
		$handlerName = $h;
            }
	}
	try {
	    my any $rv;
	    
	    #printf("HTTP DEBUG: handler=%n: context=%n, hdr=%n, body=%n\n", $handlerName, $cx, $hdr, $body);
	    #printf("HTTP DEBUG: BEFORE handler=%s", dbg_node_info($.handlers.$handlerName));

            if (!exists $handler && exists $.defaultHandler) {
                $handler = $.defaultHandler.obj;
                $handlerName = $.defaultHandler.name;
            }

            if (exists $handler) {
		# check for authentication info
                #printf("HTTP DEBUG: handler: %n handler=%N\n", $handlerName, $handler);
		if (exists $handler.auth && $handler.auth.requiresAuthentication()) {
		    if (exists $hdr.authorization) {
			if ($hdr.authorization !~ /basic /i) {
			    $.sendHttpError($s, 401, "Only basic authentication is supported", True, ( "WWW-Authenticate" : sprintf("Basic realm=\"%s\"", $handler.auth.getRealm())));
                            return;
			}
			my string $bstr = ($hdr.authorization =~ x/basic (.*)/i)[0];
			if (!strlen($bstr)) {
			    $.sendHttpError($s, 401, "Authentication is required to access this server", True, ( "WWW-Authenticate" : sprintf("Basic realm=\"%s\"", $handler.auth.getRealm())));
                            return;
			}
			my string $str = parseBase64StringToString($bstr);
			my int $i = index($str, ":");
			if ($i == -1) {
			    $.sendHttpError($s, 401, "Authentication is required to access this server", True, ( "WWW-Authenticate" : sprintf("Basic realm=\"%s\"", $handler.auth.getRealm())));
                            return;
			}
			my string $user = substr($str, 0, $i);
			my *string $pass = substr($str, $i + 1);
			my list $roles;
			try
			    $roles = $handler.auth.authenticate($user, $pass);
			catch (hash $ex) {
			    if ($ex.err == "AUTHENTICATION-ERROR") {
				$.sendHttpError($s, 401, "Authentication is required to access this server", True, ( "WWW-Authenticate" : sprintf("Basic realm=\"%s\"", $handler.auth.getRealm())));
				return;
			    }
			    else
				rethrow;
			}
			# save username in thread-local data
			# since this thread is dedicated to this connection, the data will be 
			# automatically deleted when the thread exits
			$tld.rbac_authorized_user = $user;
			$cx += ( "user"  : $user,
                                 "roles" : $roles );
		    }
		    else if (!exists $tld.rbac_authorized_user) {
			my (list $roles, string $user);
			# only try ip-based authentication if the connection is not already authenticated
			try {
			    $roles = $handler.auth.authenticateByIP($s.source, \$user);
			}
			catch (hash $ex) {
			    # log the error
			    my string $str = sprintf("%s:%d: %s: %s: received from %s via %s", $ex.file, $ex.line, $ex.err, $ex.desc, $s.source, $cx.socket);
			    $.logError($str);
			}
			if (elements $roles) {
			    # save username in thread-local data
			    $tld.rbac_authorized_user = $user;
			    $cx += ( "user"  : $user,
                                     "roles" : $roles );
			}
			else {
			    $.sendHttpError($s, 401, "Authentication is required to access this server", True, ( "WWW-Authenticate" : sprintf("Basic realm=\"%s\"", $handler.auth.getRealm())));
			    return;
			}
		    }
		}

                $rv = $handler.handleRequest($cx, $hdr, $body, \$close);
            }
            else {
                $handlerName = "error";
                $rv = $.noHandlerError($cx, $hdr, $body, \$close);
	    }

	    if (type($rv) == Type::String)
		$rv = ( "code" : 200,
			"body" : $rv );
	    else if (!exists HttpCodes.($rv.code)) { # if the handler returns an invalid hash
		my string $str = sprintf("%s handler returned an invalid response", $handlerName);
		$.sendHttpError($s, 500, $str, $close);
		$.logError($str);
		return;
	    }

	    #printf("HTTP DEBUG: handler %s returned: %n\n", $handlerName, $rv);

	    $rv.hdr.Server = $.httpserverstring;
	    if ($rv.code != 200) {
		if (!exists $rv.body)
		    $rv.body = sprintf("unknown error in %s handler", $handlerName);
		
		$.sendHttpError($s, $rv.code, $rv.body, $close);
	    }
	    else {
		#printf("\n**** REQUEST: %N\n", $body);
		#printf("\n**** RESPONSE: %N\n", $rv.body);
		if ($close)
		    $rv.hdr.Connection = "close";
		else
		    $rv.hdr.Connection = "Keep-Alive";

		if (!exists $rv.hdr.Date)
		    $rv.hdr.Date = format_date("Dy, DD Mon YYYY HH:mm:SS", gmtime()) + " GMT";

		if (!exists $rv.hdr.Server)
		    $rv.hdr.Server = $.httpserverstring;

                if (!exists $rv.hdr."Content-Type")
                    $rv.hdr."Content-Type" = "text/html;charset=utf-8";
                else if ($rv.hdr."Content-Type" !~ /charset=/ && (type($rv.body) == Type::String))
                    $rv.hdr."Content-Type" += ";charset=utf-8";

		if ($head)
		    $s.sendHTTPResponse($rv.code, HttpServer::HttpCodes.($rv.code), "1.1", $rv.hdr);
		else {
		    if ($cx.encoding == "deflate") {
			$rv.hdr += ( "Content-Encoding" : "deflate" );
			$rv.body = compress($rv.body);
		    }
		    else if ($cx.encoding == "gzip") {
			$rv.hdr += ( "Content-Encoding" : "gzip" );
			$rv.body = gzip($rv.body);
		    }
		    else if ($cx.encoding == "bzip2") {
			$rv.hdr += ( "Content-Encoding" : "bzip2" );
			$rv.body = bzip2($rv.body);
		    }
		    
		    $s.sendHTTPResponse($rv.code, HttpServer::HttpCodes.($rv.code), "1.1", $rv.hdr, $rv.body);
		    #$.log("HTTP DEBUG hdr=%N\nbody=%s", $rv.hdr, $rv.body);
		    #printf("HTTP DEBUG: response hdr=%n\n", $rv.hdr);
		}
	    }
	    
	    if (exists $rv.log)
		$.log("%s (from %s): %s", $handlerName, $s.source, $rv.log);
	    if (exists $rv.errlog)
		$.logError("%s (from %s): %s", $handlerName, $s.source, $rv.errlog);
	}
	catch (hash $ex) {
	    my string $str = sprintf("handler: %s: %s:%d: %s: %s", $handlerName, $ex.file, $ex.line, $ex.err, $ex.desc);
	    $.sendHttpError($s, 500, $str);
	    $.logError($str);
	}
    }

    #! returns a complete URL from a bind address
    static string getURLFromBind(string $bind, any $host) {
	my hash $h = parse_url($bind);
	# if there is only a port number, it will appear in the "host" key
	if (elements $h == 1 && exists $h.host && int($h.host) == $h.host) {
	    $h.port = int($h.host);
	    delete $h.host;
	}

	if (!exists $h.host)
	    $h.host = (!exists $host || $host == gethostname()) ? "localhost" : $host;

	my string $login = exists $h.password ? ($h.password + ":") : "";    
	if (exists $h.username)
	    $login += sprintf("%s@", $h.password);

	return sprintf("%s://%s%s%s%s", exists $h.protocol ? $h.protocol : "http", 
		       $login, 
		       $h.host, 
		       $h.port ? sprintf(":%d", $h.port) : "", 
		       $h.path);
    }
}
