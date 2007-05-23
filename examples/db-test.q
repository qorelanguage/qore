#!/usr/bin/env qore

# database example script, depends on schemas:
# 1) mysql-test-db.sql 
# 2) oracle-test-db.sql 
# 3) pgsql-test-db.sql

%require-our
%enable-all-warnings

our $o;

const opts = 
    ( "help"   : "h,help",
      "host"   : "H,host=s",
      "pass"   : "p,pass=s",
      "db"     : "d,db=s",
      "user"   : "u,user=s",
      "type"   : "t,type=s" );

sub usage()
{
    printf("usage: %s [options]
 -h,--help         this help text
 -u,--user=ARG     set username
 -p,--pass=ARG     set password
 -d,--db=ARG       set database name
 -H,--host=ARG     set hostname (for MySQL and PostgreSQL connections)
 -t,--type         set database driver (default mysql)\n",
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
    if (!strlen($o.type))
	$o.type = "mysql";
}

sub getDS()
{
    my $ds = new Datasource($o.type, $o.user, $o.pass, $o.db);
    if (strlen($o.host))
	$ds.setHostName($o.host);
    return $ds;
}

sub doit($db)
{
    # first we select all the data from the tables and then use context statements to order the output hierarchically
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
	$db.exec("delete from family where name = 'Test'");
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
    my $r;
    printf("db.autocommit=%N, ndb.autocommit=%N\n", $db.getAutoCommit(), $ndb.getAutoCommit());

    # first, we insert a new row into "family" but do not commit it
    my $rows = $db.exec("insert into family values (3, 'Test')\n");
    if ($rows !== 1)
	printf("FAILED INSERT, rows=%N\n", $rows);

    # now we verify that the new row is not visible to the other datasource
    # unless it's a sybase datasource, in which case this would deadlock :-(
    if ($o.type != "sybase" && $o.type != "mssql")
    {
	$r = $ndb.selectRow("select name from family where family_id = 3").name;
	if (exists $r)
	    printf("FAILED TRANSACTION TEST, name=%N\n", $r);
	else
	    printf("TRANSACTION TEST OK\n");
    }

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

sub oracle_test()
{
}

sub pgsql_test()
{
}

sub mysql_test()
{
}

const syb_table = "
create table data_test (
	null_f char(1) null,

	varchar_f varchar(40) not null,
	char_f char(40) not null,
	unichar_f unichar(40) not null,
	univarchar_f univarchar(40) not null,
	text_f text not null,
	unitext_f unitext not null, -- note that unitext is stored as 'image'

	tinyint_f tinyint not null,
	smallint_f smallint not null,
	int_f int not null,

	decimal_f decimal(10,4) not null,

	float_f float not null,     -- 8-bytes
	real_f real not null,       -- 4-bytes
	money_f money not null,
	smallmoney_f smallmoney not null,

	date_f date not null,
	time_f time not null,
	datetime_f datetime not null,
	smalldatetime_f smalldatetime not null
)
";

sub sybase_test($db)
{
    # create test table, ignore any exceptions (assuming it's already there)
    try	$db.exec(syb_table); catch() {}

    my $args = ( NULL, "test", "test", "test", "test", "test", "test" );

    # insert data
    my $rows = $db.vexec("
insert into data_test values (
	%v,
	%v,
	%v,
	%v,
	%v,
	%v,
	%v,

	55,
	4285,
	405402,

	500.1231,

	23443.234324234,
	213.123,
	3434234250.2034,
	211100.1012,

	'2007-05-01',
	'10:30:01',
	'3459-01-01 11:15:02.251',
	'2007-12-01 12:01'
)
", $args);

    my $q = $db.selectRow("select * from data_test");
    foreach my $k in (keys $q)
	printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    $db.commit();
    $db.exec("drop table data_test");
    $db.commit();
    #printf("%N\n", $q);
}

const mssql_table = "
create table data_test (
	null_f char(1) null,

	varchar_f varchar(40) not null,
	char_f char(40) not null,
	text_f text not null,
	unitext_f unitext not null, -- note that unitext is stored as 'image'

	tinyint_f tinyint not null,
	smallint_f smallint not null,
	int_f int not null,

	decimal_f decimal(10,4) not null,

	float_f float not null,     -- 8-bytes
	real_f real not null,       -- 4-bytes
	money_f money not null,
	smallmoney_f smallmoney not null,

	date_f date not null,
	time_f time not null,
	datetime_f datetime not null,
	smalldatetime_f smalldatetime not null
)
";

sub mssql_test($db)
{
    # freetds doesn't support the following column types as far as I can tell:
    # unichar, univarchar

    # create test table
    $db.exec(mssql_table);

    # insert data
    my $rows = $db.exec("
insert into data_test values (
	null,
	'test',
	'test',
	'test',
	'test',

	55,
	4285,
	405402,

	500.1231,

	23443.234324234,
	213.123,
	3434234250.2034,
	211100.1012,

	'2007-05-01',
	'10:30:01',
	'3459-01-01 11:15:02.251',
	'2007-12-01 12:01'
)
");

    my $q = $db.selectRow("select * from data_test");
    foreach my $k in (keys $q)
	printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    $db.commit();
    $db.exec("drop table data_test");
    $db.commit();
    #printf("%N\n", $q);
}

sub main()
{
    my $test_map = 
	( "sybase" : \sybase_test(),
	  "mssql"  : \mssql_test(),
	  "mysql"  : \mysql_test(),
	  "pgsql"  : \pgsql_test(),
	  "oracle" : \oracle_test());

    parse_command_line();
    my $db = getDS();

    if ($db.getDriverName() == "mssql") $db.exec("set chained on");

    #doit($db);
    transaction_test($db);
    my $test = $test_map.($db.getDriverName());
    if (exists $test)
	$test($db);
}

main();
