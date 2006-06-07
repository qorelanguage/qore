#!/usr/bin/env qore

$f = new File();
$f.open("file-test.q");
$f.setPos($ARGV[0]);
while (exists ($line = $f.readLine()))
    printf("%d: %s", ++$c, $line);

printf("end position: %d\n", $f.getPos());
