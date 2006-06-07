# HTTPClient class definition
#
# authors: Petr Matejka, David Nichols

namespace HTTPClient {

    const DEFAULT_METHODS = ("OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT");

    const DEFAULT_HEADERS = 
	( "Accept"       : "text/xml",
	  "Content-Type" : "text/xml",
	  "User-Agent"   : "Qore HTTP Client v" + Version,
	  "Connection"   : "Keep-Alive" );

    const DEFAULT_HEADER_IGNORE = ("Host","User-Agent","Content-Length");

    const DEFAULT_PROTOCOLS = "http";

    const DEFAULT_HTTP_VERSION = "1.1";
    const allowed_versions = ( "1.0", "1.1" );

    const Version = "0.3";

    # default timeout set to 5 minutes
    const defaultTimeout = 300000;
}

class HTTPClient::HTTPClient inherits private Qore::Socket, private Qore::Thread::Mutex
{
    private $.timeout, $.http_version;

    constructor($opts)
    {
        my $h;

	# ensure that protocol list is a list
	$.protocollist = list(HTTPClient::DEFAULT_PROTOCOLS);

	if (exists $opts.protocols)
	    $.protocollist += $opts.protocols;

        if (exists $opts.url)
        {
	    $h = parseURL($opts.url);
	    #printf("%s\n", eval_node($h));
	    if (!exists $h)
	    {
		delete $self;
		throw "HTTP-CLIENT-PARAMETER-ERROR", "url could not be parsed";
	    }
	    if (exists $h.protocol && !inlist($h.protocol, $.protocollist))
	    {
		delete $self;
		throw "HTTP-CLIENT-PARAMETER-ERROR", sprintf("protocol '%s' not supported (expecting: %n)", $h.protocol, $.protocollist);
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

    connect()
    {
        $.lock();
        if (!$.connected)
        {
	    if (Qore::Socket::$.connect($.socketpath) == -1)
	    {
		$.unlock();
		throw "HTTP-CLIENT-CONNECT-ERROR", sprintf("cannot connect to socket \"%s\": %s", $.socketpath, strerror(errno()));
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
	return $.send(NOTHING, "GET", $path, $headers);
    }

    post($path, $data, $headers)
    {
	return $.send($data, "POST", $path, $headers);
    }

    send($data, $method, $path, $headers)
    {
        $.connect();

        #POST /RPC2 HTTP/1.0
        #User-Agent: Frontier/5.1.2 (WinNT)
        #Host: betty.userland.com
        #Content-Type: text/xml
        #Content-length: 181

        # Accept-Charset: ISO-8859-1,UTF-8
        # Accept-Encoding: gzip,deflate
        # Keep-Alive: <seconds>
        # Connection: keep-alive

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

        my $msg;
        if (exists $ans.body)
	{
	    $msg = $ans.body;
	    #printf("%03d: BODY!!! %n\n", gettid(), $ans.body);
	}
        else
        {
	    # ignore if content-length is not present
	    $msg = $.recv($ans."content-length", $.timeout);
	    if ($msg == -1)
		throw "HTTP-CLIENT-RECEIVE-ERROR",
		sprintf("socket error while receiving xml body from socket \"%s\": %s",
			$.socketpath, strerror(errno()));
	    #printf("got body=%n\n", $msg);
        }

	# close the connection if the server tells us to
	if (tolower($ans.connection) == "close")
	    $.disconnect();

        #printf("%03d: HTTP: %s\n", gettid(), $msg);
        return $msg;
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
