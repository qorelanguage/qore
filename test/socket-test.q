#!/usr/bin/env qore

%require-our

#$sp = "/tmp/sock-test";
#$cp = "/tmp/sock-test";

our ($o, $sp, $cp, $q, $errors);

const opts = 
    ( "help"       : "h,help",
      "ssl"        : "s,ssl",
      "key"        : "k,private-key=s",
      "cert"       : "c,cert=s",
      "clientkey"  : "K,client-private-key=s",
      "clientcert" : "C,client-cert=s",
      "verbose"    : "v,verbose" );

sub test_value($v1, $v2, $msg)
{
    if ($v1 === $v2)
	printf("OK: %s test\n", $msg);
    else
    {
        printf("ERROR: %s test failed! (%n != %n)\n", $msg, $v1, $v2);
        $errors++;
    }
}

sub usage()
{
    printf("usage: %s -[options] [port]
  -h,--help                    this help text
  -s,--ssl                     use secure connections
  -c,--cert=arg                set server SSL x509 certificate
  -k,--private-key=arg         set server SSL private key
  -C,--client-cert=arg         set client SSL x509 certificate
  -K,--client-private-key=arg  set client SSL private key
", basename($ENV."_"));
    exit();
}

sub process_command_line()
{
    my $g = new GetOpt(opts);
    $o = $g.parse(\$ARGV);

    if (exists $o{"_ERRORS_"})
    {
        printf("%s\n", $o{"_ERRORS_"}[0]);
        exit(1);
    }

    if ($o.help)
	usage();

    if (!($sp = int(shift $ARGV)))
	$sp = 9001;

    $cp = sprintf("localhost:%d", $sp);
}

const i1 = 10;
const i2 = 5121;
const i4 = 2393921;
const i8 = 12309309203932;

sub server_thread()
{
    my $s = new Socket();
    if ($s.bind($sp, True) == -1)
    {
	printf("server_thread: error opening socket! (%s)\n", strerror(errno()));
	exit(2);
    }
    
    if ($s.listen())
    {
	printf("listen error (%s)\n", strerror(errno()));
	exit(2);
    }
    
    # socket created, now wake up client
    $q.push("hi");
    my $r;
    try {
	if ($o.ssl)
	{
	    if (strlen($o.clientcert))
	    {
		$s.setCertificate($o.clientcert);
		if (!strlen($o.clientkey))
		    $s.setPrivateKey($o.clientcert);
	    }
	    if (strlen($o.clientkey))
		$s.setPrivateKey($o.clientkey);

	    $r = $s.acceptSSL();
	    printf("server: secure connection (%s %s) from %s (%s)\n", $r.getSSLCipherName(), $r.getSSLCipherVersion(), $r.source, $r.source_host);
	    my $code = $r.verifyPeerCertificate();
	    if ($code == -1)
		printf("server: client certificate could not be verified\n");
	    else
	    {
		my $str = getSSLCertVerificationCodeString($code);
		printf("server: client certificate: %n %s: %s\n", $str, X509_VerificationReasons.$str);
	    }
	}
	else
	{
	    $r = $s.accept();
	    printf("server: normal socket connection from %s (%s)\n", $r.source, $r.source_host);
	}
    }
    catch ($ex)
    {
	printf("server error: %s: %s\n", $ex.err, $ex.desc);
	exit(1);
    }
    
    my $m = $r.recv();
    if ($m == -1)
    {
	printf("recv error (%s)\n", strerror(errno()));
	exit(2);
    }
    
    printf("server: message from client: %s (%s)\n", $m, typename($m));
    $r.send("OK");

    my $i = $r.recvi1();
    test_value($i, i1, "sendi1");
    $i = $r.recvi2();
    test_value($i, i2, "sendi2");
    $i = $r.recvi4();
    test_value($i, i4, "sendi4");
    $i = $r.recvi8();
    test_value($i, i8, "sendi8");

    $i = $r.recvi2LSB();
    test_value($i, i2, "sendi2LSB");
    $i = $r.recvi4LSB();
    test_value($i, i4, "sendi4LSB");
    $i = $r.recvi8LSB();
    test_value($i, i8, "sendi8LSB");

    $m = $r.recv();
    if ($m == -1)
    {
	printf("recv error (%s)\n", strerror(errno()));
	exit(2);
    }
    
    printf("server: message from client: %s (%s)\n", $m, typename($m));

    $r.close();
    $s.close();
}

sub client_thread()
{
    $q.get();
    my $s = new Socket();

    try {
	if ($o.ssl)
	{
	    if (strlen($o.cert))
	    {
		$s.setCertificate($o.cert);
		if (!strlen($o.key))
		    $s.setPrivateKey($o.cert);
	    }
	    if (strlen($o.key))
		$s.setPrivateKey($o.key);
	    $s.connectSSL($cp);

	    my $code = $s.verifyPeerCertificate();
	    my $str = getSSLCertVerificationCodeString($code);
	    printf("client: server certificate: %s: %s\n", $str, X509_VerificationReasons.$str);
	}
	else
	    $s.connect($cp);
    }
    catch ($ex)
    {
	printf("client error: %s: %s\n", $ex.err, $ex.desc);
	exit(1);
    }

    $s.send("Hi there!!!");
    my $m = $s.recv();
    if ($m == -1)
    {
	printf("recv error (%s)\n", strerror(errno()));
	exit(2);
    }
    
    printf("client: message from server: %s (%s)\n", $m, typename($m));
    $s.sendi1(i1);
    $s.sendi2(i2);
    $s.sendi4(i4);
    $s.sendi8(i8);

    $s.sendi2LSB(i2);
    $s.sendi4LSB(i4);
    $s.sendi8LSB(i8);
    $s.send("goodbye!");
}

sub main()
{
    process_command_line();

    $q = new Queue();

    background server_thread();
    background client_thread();   
}

main();
