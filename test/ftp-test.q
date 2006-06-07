#!/usr/bin/env qore

$f = new FtpClient(shift $ARGV);
$file = shift $ARGV;

sub get_program_name()
{
    my $l = split("/", $ENV{"_"});
    return $l[elements $l - 1];
}

if (!exists $file)
{
    printf("syntax: %s <host> <file>\n", get_program_name());
    exit(1);
}

if ($f.connect())
{
    printf("%s\n", strerror(errno()));
    exit();
}


$f.cwd("/tmp");
printf("PWD: %s\n", $f.pwd());
#printf("LIST: %s", $f.list());
printf("LIST: %s", $f.list("*.q"));
if (!$f.put($file))
    printf("successfully sent %s\n", $file);
else
    printf("ERROR: %s\n", strerror(errno()));

printf("LIST: %s", $f.list("*.q"));
$f.disconnect();
