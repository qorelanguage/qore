# xmlrpc.ql
#
# by David Nichols

sub get_program_name()
{
    my $l = split("/", $ENV{"_"});
    return $l[elements $l - 1];
}

sub trim($str)
{
    while (substr($str, 0, 1) == " ")
        $str = substr($str, 1);
    while (substr($str, -1) == " ")
        $str = substr($str, 0, -1);
    return $str;
}

sub inlist($val, $list)
{
    foreach my $v in ($list)
        if ($val == $v)
            return True;
    return False;
}
