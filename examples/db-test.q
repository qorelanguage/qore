#!/usr/bin/env qore

# database example script
# edit the following parameters depending on which the example schema
# 1) mysql-test-db.sql 
# 2) oracle-test-db.sql 
# has been installed

%require-our
%enable-all-warnings
our $o;

const opts = 
    ( "help"   : "h,help",
      "host"   : "H,host=s",
      "pass"   : "p,pass=s",
      "db"     : "d,db=s",
      "user"   : "u,user=s",
      "oracle" : "o,oracle" );

# add a convenient method to the Datasource class 
Datasource::selectRow($sql)
{
    context ($.select($sql))
        return %%;
}

sub usage()
{
    printf("usage: %s [options]
 -h,--help         this help text
 -u,--user=ARG     set username
 -p,--pass=ARG     set password
 -d,--db=ARG       set database name
 -H,--host=ARG     set hostname (for MySQL connections)
 -o,--oracle       connect to an Oracle datasource\n",
	   basename($ENV."_"));
    exit();
}

sub parse_command_line()
{
    my $g = new GetOpt(opts);
    $o = $g.parse(\$ARGV);
    if ($o.help)
	usage();

    if (!strlen($o.db))
    {
	stderr.printf("set the login parameters with -u,-p,-d, etc (-h for help)\n");
	exit();
    }
}

sub getDS()
{
    my $ds = new Datasource($o.oracle ? DSOracle : DSMySQL, $o.user, $o.pass, $o.db);
    if (strlen($o.host))
	$ds.setHostName($o.host);
    return $ds;
}

sub doit($db)
{
    # frist we select all the data from the tables and then use context statements to order the output hierarchically

    my $people = $db.select("select * from people");

    my $attributes = $db.select("select * from attributes");

    my $today = format_date("YYYYMMDD", now());

    context family ($db.select("select * from family"))
    {
	printf("Family %d: %s\n", %family_id, %name);
	context people ($people) sortDescendingBy (int(find %value in $attributes where (%attribute == "eyes" && %person_id == %people:person_id))) where (%family_id == %family:family_id)
	{
	    printf("  %s, born %s\n", %name, format_date("Month DD, YYYY", %dob));
	    context ($attributes) sortBy (%attribute) where (%person_id == %people:person_id)
		printf("    has %s %s\n", %value, %attribute);
	}
    }
}

sub test_timeout($db, $q)
{
    $db.setTransactionLockTimeout(1s);
    try {
	# this should cause an exception to be thrown
	$db.exec("insert into family values (3, 'Test')\n");
	printf("FAILED TRANSACTION LOCK TEST\n");
    }
    catch ($ex)
    {
	printf("TRANSACTION LOCK TEST OK (%N)\n", $ex.err);
    }
    $q.push();
}

sub transaction_test($db)
{
    my $ndb = getDS();
    printf("db.autocommit=%N, ndb.autocommit=%N\n", $db.getAutoCommit(), $ndb.getAutoCommit());

    # first, we insert a new row into "family" but do not commit it
    $db.exec("insert into family values (3, 'Test')\n");

    # now we verify that the new row is not visible to the other datasource
    my $r;
    $r = $ndb.selectRow("select name from family where family_id = 3").name;
    if (exists $r)
	printf("FAILED TRANSACTION TEST, name=%N\n", $r);
    else
	printf("TRANSACTION TEST OK\n");

    # now we verify that the new row is visible to the inserting datasource
    $r = $db.selectRow("select name from family where family_id = 3").name;
    if (!exists $r)
	printf("FAILED TRANSACTION TEST, name=%N\n", $r);
    else
	printf("TRANSACTION TEST OK (name=%N)\n", $r);

    # test datasource timeout
    my $q = new Queue();
    background test_timeout($db, $q);

    $q.get();
    
    # now, we commit the transaction
    $db.commit();

    # now we verify that the new row is visible in the other datasource
    $r = $ndb.selectRow("select name from family where family_id = 3").name;
    if (!exists $r)
	printf("FAILED TRANSACTION TEST, name=%N\n", $r);
    else
	printf("TRANSACTION TEST OK\n");

    # now we delete the row we inserted (so we can repeat the test)
    $r = $ndb.exec("delete from family where family_id = 3");
    if ($r != 1)
	printf("FAILED TRANSACTION TEST, rows deleted=%N\n", $r);
    else
	printf("TRANSACTION TEST OK\n");
    $ndb.commit();
}

sub main()
{
    parse_command_line();
    my $db = getDS();

    doit($db);
    transaction_test($db);
}

main();
