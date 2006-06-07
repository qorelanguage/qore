#!/usr/bin/env qore

sub empty()
{
}

class T { constructor() {} }

for ($i = 0; $i < 10; $i++)
{
}

while (--$i)
{
}

do
{
} while (++$i < 10);

context (("hi": (1, 2, 3)))
{
}

if (True)
{
}

if (False)
{
}
else
{
}

foreach my $i in (1, 2, 3)
{
}

empty();

$a = new T();

