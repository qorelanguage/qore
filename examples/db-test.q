#!/usr/bin/env qore

# database example script
# edit the following parameters depending on which the example schema
# 1) mysql-test-db.sql 
# 2) oracle-test-db.sql 
# has been installed

$type = SQL::DSMySQL;
# comment-out the above line and uncomment the next if the oracle schema was installed
#$type = SQL::DSOracle;
$user = "";
$pass = "";
$db   = "test";
$host = "";

# add a convenient method to the Datasource class 
Datasource::selectRow($sql)
{
    context ($.select($sql))
        return %%;
}

sub getDS()
{
    my $ds = new Datasource($type, $user, $pass, $db);
    if (strlen($host))
	$ds.setHostName($host);
    return $ds;
}

sub doit($db)
{
    # frist we select all the data from the tables and then use context statements to order the output hierarchically

    $people = $db.select("select * from people");

    $attributes = $db.select("select * from attributes");

    $today = format_date("YYYYMMDD", now());

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

sub transaction_test($db)
{
    $ndb = getDS();
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
    my $db = getDS();

    doit($db);
    transaction_test($db);
}

main();
