#!/usr/bin/env qore

# this used to cause qore to crash...

$prog = "
sub thread()
{
    printf('embedded program thread sleeping for 5 seconds\n');
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
stdout.printf("main program calling delete while child is running in the background: "); stdout.sync();
delete $a;
printf("done\n");
