#!/usr/bin/env qore

%require-our
%enable-all-warnings
%exec-class socket_test

const opts = 
    ( "help"       : "h,help",
      "server"     : "S,server=s",
      "servonly"   : "O,server-only",
      "ssl"        : "s,ssl",
      "key"        : "k,private-key=s",
      "cert"       : "c,cert=s",
      "clientkey"  : "K,client-private-key=s",
      "clientcert" : "C,client-cert=s",
      "events"     : "e,show-events",
      "verbose"    : "v,verbose" );

const i1 = 10;
const i2 = 5121;
const i4 = 2393921;
const i8 = 12309309203932;

const http_headers =  
    ( "Accept"       : "text",
      "Content-Type" : "text",
      "User-Agent"   : "Qore HTTP Test Agent",
      "Connection"   : "Keep-Alive" );

class socket_test {

    constructor() {
	$.process_command_line();

	$.string = "This is a binary string";
	$.binary = binary($.string);

	$.counter = new Counter(1);

	# create event queue and start listening thread if necessary
	if ($.o.events) {
	    $.queue = new Queue();
	    background $.listen();
	}

	$.stopc = new Counter();
	if (!exists $.o.server) {
	    $.stopc.inc();
	    background $.server_thread();
	}
	if (!$.o.servonly) {
	    $.stopc.inc();
	    background $.client_thread();   
	}

	$.stopc.waitForZero();
	if ($.o.events)
	    $.queue.push();
    }

    private server_thread() {
	on_exit $.stopc.dec();
	socket_test::printf("listening for incoming connections on %s\n", $.server_port);
	my $s = new Socket();
	# setting the callback will output far too much data
	if ($.o.events) {
	    $s.setEventQueue($.queue);
	}

	if ($s.bind($.server_port, True) == -1){
	    socket_test::printf("server_thread: error opening socket! (%s)\n", strerror(errno()));
	    thread_exit;
	}
	
	if ($s.listen()) {
	    socket_test::printf("listen error (%s)\n", strerror(errno()));
	    thread_exit;
	}
    
	# socket created, now wake up client
	$.counter.dec();
	try {
	    if ($.o.ssl) {
		if (strlen($.o.cert)) {
		    $s.setCertificate($.o.cert);
		    if (!strlen($.o.key))
			$s.setPrivateKey($.o.cert);
		}
		if (strlen($.o.key))
		    $s.setPrivateKey($.o.key);
		
		$s = $s.acceptSSL();
		socket_test::printf("returned from Socket::acceptSSL() s=%N\n", $s);
		socket_test::printf("server: secure connection (%s %s) from %s (%s)\n", $s.getSSLCipherName(), $s.getSSLCipherVersion(), $s.source, $s.source_host);
		my $str = $s.verifyPeerCertificate();
		if (!exists $str)
		    socket_test::printf("server: no client certificate\n");
		else
		    socket_test::printf("server: client certificate: %n %s: %s\n", $str, X509_VerificationReasons.$str);
	    }
	    else {
		$s = $s.accept();
		socket_test::printf("server: cleartext connection from %s (%s)\n", $s.source, $s.source_host);
	    }
	    if ($.o.events)
		$s.setEventQueue($.queue);
	}
	catch ($ex) {
	    socket_test::printf("server error: %s: %s\n", $ex.err, $ex.desc);
	    thread_exit;
	}
	
	$.receive_messages($s, "server");
	$.send_messages($s);

	$s.close();
    }

    private client_thread() {
	on_exit $.stopc.dec();
	if (!exists $.o.server)
	    $.counter.waitForZero();
	my $s = new Socket();
	# setting the callback will output far too much data
	if ($.o.events)
	    $s.setEventQueue($.queue);

	try {
	    if ($.o.ssl) {
		if (strlen($.o.clientcert)) {
		    $s.setCertificate($.o.clientcert);
		    if (!strlen($.o.clientkey))
			$s.setPrivateKey($.o.clientcert);
		}
		if (strlen($.o.clientkey))
		    $s.setPrivateKey($.o.clientkey);
		$s.connectSSL($.client_port);
		
		my $str = $s.verifyPeerCertificate();
		socket_test::printf("client: server certificate: %s: %s\n", $str, X509_VerificationReasons.$str);
	    }
	    else
		$s.connect($.client_port);
	}
	catch ($ex) {
	    socket_test::printf("client error: %s: %s\n", $ex.err, $ex.desc);
	    thread_exit;
	}

	$.send_messages($s);
	$.receive_messages($s, "client");
    }

    private receive_messages($s, $who) {
	my $m = $s.recv();
	$.test_value($who, $.string, $m, "string");
	$s.send("OK");

        $m = $s.recvBinary();
	$.test_value($who, $.binary, $m, "binary");
	$s.send("OK");

	$m = $s.recvi1();
	$.test_value($who, $m, i1, "sendi1");
	$s.send("OK");

	$m = $s.recvi2();
	$.test_value($who, $m, i2, "sendi2");
	$s.send("OK");

	$m = $s.recvi4();
	$.test_value($who, $m, i4, "sendi4");
	$s.send("OK");

	$m = $s.recvi8();
	$.test_value($who, $m, i8, "sendi8");
	$s.send("OK");

	$m = $s.recvi2LSB();
	$.test_value($who, $m, i2, "sendi2LSB");
	$s.send("OK");

	$m = $s.recvi4LSB();
	$.test_value($who, $m, i4, "sendi4LSB");
	$s.send("OK");

	$m = $s.recvi8LSB();
	$.test_value($who, $m, i8, "sendi8LSB");
	$s.send("OK");

        $m = $s.readHTTPHeader();
        $.test_value($who, $m.method, "POST", "HTTP header method");
        $m = $s.recv($m."content-length");
        $.test_value($who, $m, $.string, "HTTP message body");

	$s.sendHTTPResponse(200, "OK", "1.1", http_headers, "OK");
    }
    
    private send_messages($s) {
	$s.send($.string);
	$.get_response($s);

	$s.send($.binary);
	$.get_response($s);

	$s.sendi1(i1);
	$.get_response($s);
	$s.sendi2(i2);
	$.get_response($s);
	$s.sendi4(i4);
	$.get_response($s);
	$s.sendi8(i8);
	$.get_response($s);
	
	$s.sendi2LSB(i2);
	$.get_response($s);
	$s.sendi4LSB(i4);
	$.get_response($s);
	$s.sendi8LSB(i8);
	$.get_response($s);
	$s.sendHTTPMessage("POST", "none", "1.1", http_headers, $.string);
	$.get_http_response($s);
    }

    private get_response($s) {
	my $m = $s.recv(2);
	if ($m != "OK")
	    throw "RESPONSE-ERROR", sprintf("expecting 'OK', got: %N", $m);
    }

    private get_http_response($s) {
        my $m = $s.readHTTPHeader();
        $m = $s.recv($m."content-length");
	if ($m != "OK")
	    throw "RESPONSE-ERROR", sprintf("expecting 'OK', got: %N", $m);
    }

    private test_value($who, $v1, $v2, $msg) {
	if ($v1 === $v2)
	    socket_test::printf("%s: OK: %s test\n", $who, $msg);
	else {
	    socket_test::printf("%s: ERROR: %s test failed! (%n != %n)\n", $who, $msg, $v1, $v2);
	    $.errors++;
	}
    }

    private static usage() {
	socket_test::printf("usage: %s -[options] [port]
  -h,--help                    this help text
  -S,--server=ip:port          no server thread; connect to remote server
  -O,--server-only             no client thread; wait for remote clients
  -s,--ssl                     use secure connections
  -c,--cert=arg                set server SSL x509 certificate
  -k,--private-key=arg         set server SSL private key
  -C,--client-cert=arg         set client SSL x509 certificate
  -K,--client-private-key=arg  set client SSL private key
  -e,--show-events             monitor socket events
", basename($ENV."_"));
	exit();
    }

    private process_command_line() {
	my $g = new GetOpt(opts);
	$.o = $g.parse(\$ARGV);
	
	if (exists $.o{"_ERRORS_"}) {
	    socket_test::printf("%s\n", $.o{"_ERRORS_"}[0]);
	    exit(1);
	}
	
	if ($.o.help)
	    socket_test::usage();
	
	if (exists $.o.server && $.o.servonly) {
	    socket_test::printf("server only flag set and remote server option=%n set - aborting\n", $.o.server);
	    exit(1);
	}
	
	if (!($.server_port = int(shift $ARGV)))
	    $.server_port = 9001;
	
	if (exists $.o.server) {
	    $.client_port = $.o.server;
	    if ($.client_port == int($.client_port))
		$.client_port = "localhost:" + $.client_port;
	}
	else
	    $.client_port = sprintf("localhost:%d", $.server_port);
    }

    private listen() {
        while (True) {

            # get a message from the event queue; a hash is returned with at
            # least the following keys: "event" with the event code, and
            # "source" with the event source identifier

            my $e = $.queue.get();

	    # stop listening when empty event posted to queue in constructor()
	    if (!exists $e)
		return;

	    socket_test::printf("%s %s: %s\n", EVENT_SOURCE_MAP.($e.source), EVENT_MAP.($e.event), socket_test::getstr($e - ("event", "source")));
	    flush(); # flush output
	}
    }

    # ensure all output is synchronized
    synchronized static printf($fmt) {
	vprintf($fmt, $argv);
    }

    static getstr($h) {
        my $str;

        # we create the string by mapping our string format function on a list
        # of the hash keys.  this is more consise than iterating the keys with 
        # a "foreach" statement, for example

        map ($str += sprintf("%s=%n ", $1, $h.$1)), keys $h;
        return $str;
    }
}
