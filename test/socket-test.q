#!/usr/bin/env qore

#$sp = "/tmp/sock-test";
#$cp = "/tmp/sock-test";

if (!($sp = int(shift $ARGV)))
    $sp = 9001;

$cp = sprintf("localhost:%d", $sp);

const i1 = 10;
const i2 = 5121;
const i4 = 2393921;

sub server_thread()
{
    my $s = new Socket();
    if ($s.bind($sp) == -1)
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
    my $r = $s.accept();
    printf("connection from %s\n", $r.source);
    
    $m = $r.recv();
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

    if ($s.connect($cp) == -1)
    {
	printf("client_thread: error opening socket! (%s)\n", strerror(errno()));
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

$q = new Queue();

background server_thread();
background client_thread();

