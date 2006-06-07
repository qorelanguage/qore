#!/usr/bin/env qore

$prog = "
sub thread()
{
    my $x = 5;
    while ($x--)
	sleep(1);
}

background thread();
";


$a = new Program();
$a.parse($prog, "prog");
$a.run();
sleep(2);
delete $a;
