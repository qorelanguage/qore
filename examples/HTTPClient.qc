# HTTPClient class definition
#
# authors: Petr Matejka, David Nichols
# v0.3.1: added SSL support, https protocol recognition

namespace HTTPClient {

    const DEFAULT_METHODS = ("OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT");

    # the following headers will be included in every message but can be overridden
    const DEFAULT_HEADERS = 
	( "Accept"       : "text/html",
	  "Content-Type" : "text/html",
	  "User-Agent"   : "Qore HTTP Client v" + Version,
	  "Connection"   : "Keep-Alive" );

    # the following headers will be overridden by HTTPClient objects if set
    const DEFAULT_HEADER_IGNORE = ("Host","User-Agent","Content-Length");
    
    # hash of known protocols and default connection parameters
    const DEFAULT_PROTOCOLS = ( "http"  : ( "port" : 80,  "ssl" : False ),
				"https" : ( "port" : 443, "ssl" : True ) );

    const DEFAULT_HTTP_VERSION = "1.1";
    const allowed_versions = ( "1.0", "1.1" );

    const Version = "0.3.1";

    # default timeout set to 5 minutes
    const defaultTimeout = 300000;
}

class HTTPClient::HTTPClient inherits private Qore::Socket, private Qore::Thread::Mutex
{
    private $.timeout, $.http_version, $.ssl, $.protocollist;

    constructor($opts)
    {
        my $h;

	$.protocols = HTTPClient::DEFAULT_PROTOCOLS;

	if (exists $opts.protocols)
	    $.protocols += $opts.protocols;

        if (exists $opts.url)
        {
	    $h = parseURL($opts.url);
	    #printf("%s\n", eval_node($h));
	    if (!exists $h)
	    {
		delete $self;
		throw "HTTP-CLIENT-PARAMETER-ERROR", "url could not be parsed";
	    }
	    if (exists $h.protocol)
		if (!inlist($h.protocol, keys $.protocols))
	        {
		    my $p = $h.protocol;
		    my $l = keys $.protocols;
		    delete $self;
		    throw "HTTP-CLIENT-PARAMETER-ERROR", sprintf("protocol '%s' not supported (expecting: %n)", $p, $l);
		}
	        else 
		{
		    $.ssl   = $.protocols.($h.protocol).ssl;
		    if (!exists $h.port)
			$h.port = $.protocols.($h.protocol).port;
		}
        }

        # check if host is really a local port
        if (!exists $h.port && int($h.host) == $h.host)
        {
	    $h.port = $h.host;
	    $h.host = "localhost";
        }

        if (!exists $h.host)
	    $h.host = "localhost";

	# set the port number if it's not already given
        if (!exists $h.port)
	    $h.port = exists $opts.default_port ? $opts.default_port : 80;

	if (exists $h.path)
	    $.default_path = "/" + $h.path;
	else if (exists $opts.default_path)
	    $.default_path = "/" + $opts.default_path;

	if (exists $opts.timeout)
	    $.timeout = $opts.timeout;
	else
	    $.timeout = HTTPClient::defaultTimeout;

	if (exists $opts.http_version)
	    $.setHTTPVersion($opts.http_version);
	else
	    $.http_version = DEFAULT_HTTP_VERSION;

	#printf("h=%N\n", $h);

        $.socketpath = sprintf("%s:%s", $h.host, $h.port);
    }

    copy()
    {
	$.connected = False;
    }
	
    setHTTPVersion($v)
    {
	if (type($v) != Type::String || !inlist($v, HTTPClient::allowed_versions))
	    throw "HTTP-VERSION-ERROR", sprintf("version %n is not supported (valid versions: %n)", $v, HTTPClient::allowed_versions);
	$.http_version = $v;
    }

    getHTTPVersion()
    {
	return $.http_version;
    }

    setSecure($opt)
    {
	$.ssl = $opt;
    }

    isSecure()
    {
	return Socket::$.isSecure();
    }

    verifyPeerCertificate()
    {
	return Socket::$.verifyPeerCertificate();
    }

    getSSLCipherName()
    {
	return Socket::$.getSSLCipherName();
    }

    getSSLCipherVersion()
    {
	return Socket::$.getSSLCipherVersion();
    }

    connect()
    {
        $.lock();
        if (!$.connected)
        {
	    try {
		if ($.ssl)
		    Qore::Socket::$.connectSSL($.socketpath);
		else
		    Qore::Socket::$.connect($.socketpath);
	    }
	    catch ()
	    {
		$.unlock();
		rethrow;
	    }
        }
        $.connected = True;
        $.unlock();
    }
    
    disconnect()
    {
        $.lock();
        if ($.connected)
	    $.close();

        $.connected = False;
        $.unlock();
    }

    get($path, $headers)
    {
	return $.sendInternal(NOTHING, "GET", $path, $headers, True).body;
    }
    
    head($path, $headers)
    {
	return $.sendInternal(NOTHING, "GET", $path, $headers, False);
    }

    post($path, $data, $headers)
    {
	return $.sendInternal($data, "POST", $path, $headers, True).body;
    }

    private getResponse()
    {
        my $ans= $.readHTTPHeader($.timeout);
	
	# if there was a timeout
	if (!exists $ans)
	    throw "HTTP-CLIENT-TIMEOUT", sprintf("timed out waiting for response on socket %n", $.socketpath);

	#printf("%03d: got ans=%n\n", gettid(), $ans);
        if ($ans == -1)
	    throw "HTTP-CLIENT-RECEIVE-ERROR",
	    sprintf("error receiving message on socket %n: %s",
		    $.socketpath, strerror(errno()));

	
	#printf("ans=%N\n", $ans);
        if (type($ans) != Type::Hash)
	    throw "HTTP-CLIENT-RECEIVE-ERROR",
	    sprintf("malformed HTTP header received from socket %n: could not parse header",
		    $.socketpath);

	# check for character set spec in content-type header
	if (exists $ans."content-type")
	{
	    my $l = ();
	    foreach my $v in (split(";", $ans."content-type"))
	    {
		$l += trim(\$v);
		if ($v =~ /charset=/)
		{
		    $v =~ s/charset=//;
		    $.setCharset($v);
		}
	    }
	    if (elements $l == 1)
		$ans."content-type" = $l[0];
	    else
		$ans."content-type" = $l;
	}

	# check HTTP status code
	if (!exists $ans.status_code)
	    throw "HTTP-CLIENT-RECEIVE-ERROR", sprintf("no HTTP status code received in response: %n", $ans);

	# process redirect responses
	if ($ans.status_code >= 300 && $ans.status_code < 400)
	    throw "HTTP-CLIENT-RECEIVE-ERROR", sprintf("HTTP redirect %d (%s) to %n ignored", $ans.status_code, $ans.status_message, $ans.location);

	if ($ans.status_code < 200 || $ans.status_code >= 300)
	    throw "HTTP-CLIENT-RECEIVE-ERROR", sprintf("HTTP status code %d (%s) received", $ans.status_code, $ans.status_message);

        #printf("HTTP-Header: %n\n", $ans);

	if ($ans."content-length")
	    $ans.body = $.recv($ans."content-length", $.timeout);

        #printf("%03d: HTTP: %s\n", gettid(), $msg);
        return $ans;
    }

    send($data, $method, $path, $headers, $getbody)
    {
	return $.sendInternal($data, $method, $path, $headers, $getbody).body;
    }

    private sendInternal($data, $method, $path, $headers, $getbody)
    {
        $.connect();

        if (!inlist($method, HTTPClient::DEFAULT_METHODS))
            throw "HTTP-CLIENT-METHOD-ERROR", sprintf("HTTP method (%n) not recognized.", $method);

        my $hdrs = ( "Host" : gethostname() );

        foreach my $defkey in (keys HTTPClient::DEFAULT_HEADERS) {
            if (exists $headers.$defkey)
                $hdrs.$defkey = $headers.$defkey;
            else
                $hdrs.$defkey = HTTPClient::DEFAULT_HEADERS.$defkey;
        }

        foreach my $headerkey in (keys $headers) {
            if ((!inlist($headerkey, keys HTTPClient::DEFAULT_HEADERS))
                && (!inlist($headerkey, HTTPClient::DEFAULT_HEADER_IGNORE))) {
                $hdrs.$headerkey = $headers.$headerkey;
            }
        }
        #printf("path=%s, hdr=%N", $path, $hdrs);

        $.sendHTTPMessage($method, $path, $.http_version, $hdrs, $data);

	my $ans = $.getResponse();

	# if no "content-length" header was present and we need a body
	if ($getbody && !exists $ans.body)
	    $ans.body = $.recv(0, $.timeout);

	# close the connection if the server tells us to
	if (tolower($ans.connection) == "close")
	    $.disconnect();

	return $ans;
    }

    setTimeout($t)
    {
	$.timeout = $t;
    }

    getTimeout()
    {
	return $.timeout;
    }
}
