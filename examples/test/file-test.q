#!/usr/bin/env qore

my ReadOnlyFile $f("file-test.q");
$f.setPos($ARGV[0]);
while (exists (my *string $line = $f.readLine()))
    printf("%d: %s", ++$c, $line);

printf("end position: %d\n", $f.getPos());

foreach my string $line in (new FileLineIterator("file-test.q"))
    printf("%d: %s\n", $# + 1, $line);
