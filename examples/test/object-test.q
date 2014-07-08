#!/usr/bin/env qore

%require-our
%enable-all-warnings

our $home;
if (gethostname() =~ /^ren/)
    $home = 1;

class Test {
    constructor($a)
    {
	$.data = $a;
	printf("new object created (%s)\n", $.data);
    }
    destructor()
    {
	printf("%s destructor() called\n", $.data);
    }
    getData()
    {
	return $.data;
    }
    getType()
    {
	return getClassName($self);
    }
    private p1()
    {
	printf("p1()\n");
    }
    showMembers()
    {
	foreach my $a in (keys $self)
	    printf("%s\n", $a);
	#delete $self;
	#return $.data;
    }
}

private Test::p2()
{
    $.p1();
    printf("p2()\n");
}

Test::hello()
{
    $.p2();
    printf("data is %s, hello!\n", $.data);
}
sub et($t)
{
    $t.a = "123";
    #$t.getType();
    #my $t = new Test(1);
    #throw("gee");
}

our $t = new Test(1);
(new Test(3)).hello();
#$t.p1();
#$t.p2();
printf("object type=%s\n", $t.getType());
$t.hello();
$t.showMembers();
et($t);
$t.showMembers();

#$q = new Test(4, 5, 6);
#printf("data=%s\n", $t.getData());
#printf("data=%s\n", $q.getData());

foreach my $n in (getMethodList($t))
    printf("Test::%s()\n", $n);
