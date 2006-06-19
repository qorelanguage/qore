#!/usr/bin/env qore

%require-our

#$sp = "/tmp/sock-test";
#$cp = "/tmp/sock-test";

our ($o, $sp, $cp, $q);

const opts = 
    ( "help" : "h,help",
      "ssl"  : "s,ssl",
      "key"  : "p,private-key=s",
      "cert" : "c,cert=s" );

sub usage()
{
    printf("usage: %s -[options] [port]
  -h,--help             this help text
  -s,--ssl              use secure connections
  -c,--cert=arg         set SSL x509 certificate
  -p,--private-key=arg  set SSL private key\n", basename($ENV."_"));
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
	    if (strlen($o.cert))
	    {
		$s.setCertificate($o.cert);
		if (!strlen($o.key))
		    $s.setPrivateKey($o.cert);
	    }
	    if (strlen($o.key))
		$s.setPrivateKey($o.key);

	    $r = $s.acceptSSL();
	    printf("secure connection: %s %s\n", $r.getSSLCipherName(), $r.getSSLCipherVersion());
	}
	else
	    $r = $s.accept();
    }
    catch ($ex)
    {
	printf("server error: %s: %s\n", $ex.err, $ex.desc);
	exit(1);
    }

    printf("connection from %s\n", $r.source);
    
    my $m = $r.recv();
    if ($m == -1)
    {
	printf("recv error (%s)\n", strerror(errno()));
	exit(2);
    }
    
    printf("server: message from client: %s (%s)\n", $m, typename($m));
    $r.send("OK");

    my $i = $r.recvi1();
    printf("Socket::sendi1(), Socket::recvi1() test: %s\n", $i == i1 ? "PASSED" : "FAILED");
    $i = $r.recvi2();
    printf("Socket::sendi2(), Socket::recvi2() test: %s\n", $i == i2 ? "PASSED" : "FAILED");
    $i = $r.recvi4();
    printf("Socket::sendi4(), Socket::recvi4() test: %s\n", $i == i4 ? "PASSED" : "FAILED");

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
	    $s.connectSSL($cp);
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
