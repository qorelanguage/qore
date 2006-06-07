#!/usr/bin/env qore

$q = new Queue();

sub t()
{
    $q.get();
    $q.push(1);
}

try 
{
    while (1)
   	background t();
}
catch ($ex)
{
    printf("%s threads\n", num_threads());
    $q.push(1);
    printf("%s: %s\n", $ex.err, $ex.desc);
}
