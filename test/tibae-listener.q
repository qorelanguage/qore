#!/usr/bin/env qore

%requires tibae

sub getAdapter()
{
    # Application properties for adapter
    my $props.AppVersion = Qore::VersionString;
    $props.AppInfo = "test";

    $props.AppName = "testAdapter";
    $props.RepoURL = "./new.dat";
    $props.ConfigURL = "/tibco/private/adapter/testAdapter";
    my $classes.Test = "/tibco/public/class/ae/Test";

    print("initializing TIBCO session: \n");
    my $adapter = new TibcoAdapter("rvSession", $props, $classes);
    #my $adapter = new TibcoAdapter("rvSession", $props, $classes, "7505", "172.23.65.0;239.255.43.30", "172.23.60.122:7500");
    #my $adapter = new TibcoAdapter("rvSession", $props, $classes, "8504", NOTHING, "172.23.5.159:7500");
    print("done\n");
    return $adapter;
}

$a = getAdapter();

$subject = "QORE.Test.QORETest.>";

while (True)
{
    printf("listening on %s\n", $subject);
    my $msg = $a.receive($subject);
    printf("%n: ", now());
    print("%N\n", $msg);
}
