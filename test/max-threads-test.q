#!/usr/bin/env qore

our $q = new Queue();

sub t()
{
    $q.get();
    $q.push(1);
}

try 
{
    while (True)
   	background t();
}
catch ($ex)
{
    printf("%s threads\n", num_threads());
    $q.push(1);
    printf("%s: %s\n", $ex.err, $ex.desc);
}
while (num_threads() > 1)
{
    printf("size=%d, threads=%d, sleeping for 0.5 seconds\n", $q.size(), num_threads());
    usleep(500ms);
}
printf("done\n");
