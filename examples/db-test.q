#!/usr/bin/env qore

# database test script
# databases users must be able to create and destroy tables and procedures, etc
# in order to execute all tests

%require-our
%enable-all-warnings

our ($o, $errors, $test_count);

const opts = 
    ( "help"    : "h,help",
      "host"    : "H,host=s",
      "pass"    : "p,pass=s",
      "db"      : "d,db=s",
      "user"    : "u,user=s",
      "type"    : "t,type=s",
      "verbose" : "v,verbose:i+",
      "leave"   : "l,leave"
 );

sub usage()
{
    printf("usage: %s [options]
 -h,--help         this help text
 -u,--user=ARG     set username
 -p,--pass=ARG     set password
 -d,--db=ARG       set database name
 -H,--host=ARG     set hostname (for MySQL and PostgreSQL connections)
 -t,--type         set database driver (default mysql)
 -v,--verbose      more v's = more information
 -l,--leave        leave test tables in schema at end\n",
	   basename($ENV."_"));
    exit();
}

const table_map = 
 ( "oracle" : ora_tables,
   "mysql"  : mysql_tables,
   "pgsql"  : pgsql_tables,
   "sybase" : syb_tables,
   "mssql"  : mssql_tables );

const table_list = ( "family", "people", "attributes", "data_test" );

const ora_tables = ("create table family (
   family_id int not null,
   name varchar2(80) not null
)", "create table people (
   person_id int not null,
   family_id int not null,
   name varchar2(250) not null,
   dob date not null
)", "create table attributes (
   person_id int not null,
   attribute varchar2(80) not null,
   value varchar2(160) not null
)");

const mysql_tables = (
"create table family (
   family_id int not null,
   name varchar(80) not null
) type = innodb",
"create table people (
   person_id int not null,
   family_id int not null,
   name varchar(250) not null,
   dob date not null
) type = innodb",
"create table attributes (
   person_id int not null,
   attribute varchar(80) not null,
   value varchar(160) not null
) type = innodb" );

const pgsql_tables = ("create table family (
   family_id int not null,
   name varchar(80) not null )", 
"create table people (
   person_id int not null,
   family_id int not null,
   name varchar(250) not null,
   dob date not null )",
"create table attributes (
   person_id int not null,
   attribute varchar(80) not null,
   value varchar(160) not null)",
"create table data_test (
        int2_f smallint not null,
        int4_f integer not null,
        int8_f int8 not null,
        bool_f boolean not null,
        
        float4_f real not null,
        float8_f double precision not null,
        
        number_f numeric(16,3) not null,
        money_f money not null,

        text_f text not null,
        varchar_f varchar(40) not null,
        char_f char(40) not null,
        name_f name not null,

        date_f date not null,
        abstime_f abstime not null,
        reltime_f reltime not null,
        interval_f interval not null,
        time_f time not null,
        timetz_f time with time zone not null,
        timestamp_f timestamp not null,
        timestamptz_f timestamp with time zone not null,
        tinterval_f tinterval not null,
        
        bytea_f bytea not null
        --bit_f bit(11) not null,
        --varbit_f bit varying(11) not null
)" );

const syb_tables = "
create table family (
   family_id int not null,
   name varchar(80) not null
)

create table people (
   person_id int not null,
   family_id int not null,
   name varchar(250) not null,
   dob date not null
)

create table attributes (
   person_id int not null,
   attribute varchar(80) not null,
   value varchar(160) not null
)

create table data_test (
	null_f char(1) null,

	varchar_f varchar(40) not null,
	char_f char(40) not null,
	unichar_f unichar(40) not null,
	univarchar_f univarchar(40) not null,
	text_f text not null,
	unitext_f unitext not null, -- note that unitext is stored as 'image'

        bit_f bit not null,
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
	smalldatetime_f smalldatetime not null,

	binary_f binary(4) not null,
	varbinary_f varbinary(4) not null,
	image_f image not null
)
";

const mssql_tables = "
create table family (
   family_id int not null,
   name varchar(80) not null
)

create table people (
   person_id int not null,
   family_id int not null,
   name varchar(250) not null,
   dob date not null
)

create table attributes (
   person_id int not null,
   attribute varchar(80) not null,
   value varchar(160) not null
)

create table data_test (
	null_f char(1) null,

	varchar_f varchar(40) not null,
	char_f char(40) not null,
	text_f text not null,
	unitext_f unitext not null, -- note that unitext is stored as 'image'

        bit_f bit not null,
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
	smalldatetime_f smalldatetime not null,

	binary_f binary(4) not null,
	varbinary_f varbinary(4) not null,
	image_f image not null
)
";

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

sub create_datamodel($db)
{
    drop_test_datamodel($db);
  
    foreach my $sql in (table_map.($db.getDriverName()))
    {
	$db.exec($sql);
    }

    $db.exec("insert into family values ( 1, 'Smith' )");
    $db.exec("insert into family values ( 2, 'Jones' )");

    # we insert the dates here using binding by value so we don't have
    # to worry about each database's specific date format
    $db.exec("insert into people values ( 1, 1, 'Arnie', %v)", 1983-05-13);
    $db.exec("insert into people values ( 2, 1, 'Sylvia', %v)", 1994-11-10);
    $db.exec("insert into people values ( 3, 1, 'Carol', %v)", 2003-07-23);
    $db.exec("insert into people values ( 4, 1, 'Bernard', %v)", 1979-02-27);
    $db.exec("insert into people values ( 5, 1, 'Isaac', %v)", 2000-04-04);
    $db.exec("insert into people values ( 6, 2, 'Alan', %v)", 1992-06-04);
    $db.exec("insert into people values ( 7, 2, 'John', %v)", 1995-03-23);

    $db.exec("insert into attributes values ( 1, 'hair', 'blond' )");
    $db.exec("insert into attributes values ( 1, 'eyes', 'hazel' )");
    $db.exec("insert into attributes values ( 2, 'hair', 'blond' )");
    $db.exec("insert into attributes values ( 2, 'eyes', 'blue' )");
    $db.exec("insert into attributes values ( 3, 'hair', 'brown' )");
    $db.exec("insert into attributes values ( 3, 'eyes', 'grey')");
    $db.exec("insert into attributes values ( 4, 'hair', 'brown' )");
    $db.exec("insert into attributes values ( 4, 'eyes', 'brown' )");
    $db.exec("insert into attributes values ( 5, 'hair', 'red' )");
    $db.exec("insert into attributes values ( 5, 'eyes', 'green' )");
    $db.exec("insert into attributes values ( 6, 'hair', 'black' )");
    $db.exec("insert into attributes values ( 6, 'eyes', 'blue' )");
    $db.exec("insert into attributes values ( 7, 'hair', 'brown' )");
    $db.exec("insert into attributes values ( 7, 'eyes', 'brown' )");
    $db.commit();
}

sub drop_test_datamodel($db)
{
    # drop the tables and ignore any exceptions
    foreach my $table in (table_list)
	try { $db.exec("drop table " + $table); $db.commit(); } catch () { $db.commit(); }
}

sub getDS()
{
    my $ds = new Datasource($o.type, $o.user, $o.pass, $o.db);
    if (strlen($o.host))
	$ds.setHostName($o.host);
    return $ds;
}

sub tprintf($v, $msg)
{
    if ($v <= $o.verbose)
	vprintf($msg, $argv);
}

sub test_value($v1, $v2, $msg)
{
    ++$test_count;
    if ($v1 == $v2)
	tprintf(1, "OK: %s test\n", $msg);
    else
    {
        tprintf(0, "ERROR: %s test failed! (%n != %n)\n", $msg, $v1, $v2);
        $errors++;
    }
}

const family_hash = (
  "Jones" : (
      "people" : (
	  "John" : (
	      "dob" : 1995-03-23,
	      "eyes" : "brown",
	      "hair" : "brown" ),
	  "Alan" : (
	      "dob" : 1992-06-04,
	      "eyes" : "blue",
	      "hair" : "black" ) ) ),
    "Smith" : (
	"people" : (
	    "Arnie" : (
		"dob" : 1983-05-13,
		"eyes" : "hazel",
		"hair" : "blond" ),
	    "Carol" : ( 
		"dob" : 2003-07-23,
		"eyes" : "grey",
		"hair" : "brown" ),
	    "Isaac" : ( 
		"dob" : 2000-04-04,
		"eyes" : "green",
		"hair" : "red" ),
	    "Bernard" : ( 
		"dob" : 1979-02-27,
		"eyes" : "brown",
		"hair" : "brown" ),
	    "Sylvia" : (
		"dob" : 1994-11-10,
		"eyes" : "blue",
		"hair" : "blond" ) ) ) );

sub context_test($db)
{
    # first we select all the data from the tables and then use 
    # context statements to order the output hierarchically
    
    # context statements are most useful when a set of queries can be executed once
    # and the results processed many times by creating "views" with context statements

    my $people = $db.select("select * from people");
    my $attributes = $db.select("select * from attributes");

    my $today = format_date("YYYYMMDD", now());

    # in this test, we create a big hash structure out of the queries executed above
    # and compare it at the end to the expected result

    # display each family sorted by family name
    my $fl;
    context family ($db.select("select * from family")) sortBy (%name)
    {
	my $pl;
	tprintf(2, "Family %d: %s\n", %family_id, %name);

	# display people, sorted by eye color, descending
	context people ($people) 
	    sortDescendingBy (find %value in $attributes 
			      where (%attribute == "eyes" 
				     && %person_id == %people:person_id)) 
	    where (%family_id == %family:family_id)
	{
	    my $al;
	    tprintf(2, "  %s, born %s\n", %name, format_date("Month DD, YYYY", %dob));
	    context ($attributes) sortBy (%attribute) where (%person_id == %people:person_id)
	    {
		$al.%attribute = %value;
		tprintf(2, "    has %s %s\n", %value, %attribute);
	    }
	    # leave out the ID fields and name from hash under name; subtracting a 
	    # string from a hash removes that key from the result
	    # this is "doing it the hard way", there is only one key left, 
	    # "dob", then attributes are added directly into the person hash
	    $pl.%name = %% - "family_id" - "person_id" - "name" + $al;
	}
	# leave out family_id and name fields (leaving an empty hash)
	$fl.%name = %% - "family_id" - "name" + ( "people" : $pl );
    }

    # test context ordering
    test_value(keys $fl, ("Jones", "Smith"), "first context");
    test_value(keys $fl.Smith.people, ("Arnie", "Carol", "Isaac", "Bernard", "Sylvia"), "second context");
    # test entire context value
    test_value($fl, family_hash, "third context");
}
    

sub test_timeout($db, $c)
{
    $db.setTransactionLockTimeout(1ms);
    try {
	# this should cause a TRANSACTION-LOCK-TIMEOUT exception to be thrown
	$db.exec("insert into family values (3, 'Test')\n");
	test_value(True, False, "transaction timeout");
	$db.exec("delete from family where name = 'Test'");
    }
    catch ($ex)
    {
	test_value(True, True, "transaction timeout");
    }
    # signal parent thread to continue
    $c.dec();
}

sub transaction_test($db)
{
    my $ndb = getDS();
    my $r;
    tprintf(2, "db.autocommit=%N, ndb.autocommit=%N\n", $db.getAutoCommit(), $ndb.getAutoCommit());

    # first, we insert a new row into "family" but do not commit it
    my $rows = $db.exec("insert into family values (3, 'Test')\n");
    if ($rows !== 1)
	printf("FAILED INSERT, rows=%N\n", $rows);

    # now we verify that the new row is not visible to the other datasource
    # unless it's a sybase/ms sql server datasource, in which case this would deadlock :-(
    if ($o.type != "sybase" && $o.type != "mssql")
    {
	$r = $ndb.selectRow("select name from family where family_id = 3").name;
	test_value($r, NOTHING, "first transaction");
    }

    # now we verify that the new row is visible to the inserting datasource
    $r = $db.selectRow("select name from family where family_id = 3").name;
    test_value($r, "Test", "second transaction");

    # test datasource timeout
    # this Counter variable will allow the parent thread to sleep
    # until the child thread times out
    my $c = new Counter(1);
    background test_timeout($db, $c);

    # wait for child thread to time out
    $c.waitForZero();
    
    # now, we commit the transaction
    $db.commit();

    # now we verify that the new row is visible in the other datasource
    $r = $ndb.selectRow("select name from family where family_id = 3").name;
    test_value($r, "Test", "third transaction");
    
    # now we delete the row we inserted (so we can repeat the test)
    $r = $ndb.exec("delete from family where family_id = 3");
    test_value($r, 1, "delete row count");
    $ndb.commit();
}

sub oracle_test()
{
}

# here we use a little workaround for modules that provide functions, 
# namespace additions (constants, classes, etc) needed by test functions 
# at parse time.  To avoid parse errors (as database modules are loaded
# in this script at run-time when the Datasource class is instantiated)
# we use a Program object that we parse and run on demand to return the
# value required
sub get_val($code)
{
    my $p = new Program();

    my $str = sprintf("return %s;", $code);
    $p.parse($str, "code");
    return $p.run();
}

sub pgsql_test($db)
{
    my $args = ( "int2_f"          : 258,
		 "int4_f"          : 233932,
		 "int8_f"          : 239392939458,
		 "bool_f"          : True,
		 "float4_f"        : 21.3444,
		 "float8_f"        : 49394.23423491,
		 "number_f"        : get_val("pgsql_bind(PG_TYPE_NUMERIC, '7235634215.3250')"),
		 "money_f"         : get_val("pgsql_bind(PG_TYPE_CASH, \"400.56\")"),
		 "text_f"          : 'some text  ',
		 "varchar_f"       : 'varchar ',
		 "char_f"          : 'char text',
		 "name_f"          : 'name',
		 "date_f"          : 2004-01-05, 
		 "abstime_f"       : 2005-12-03,
		 "reltime_f"       : 5M + 71D + 19h + 245m + 51s,
		 "interval_f"      : 6M + 3D + 2h + 45m + 15s, 
		 "time_f"          : 11:35:00, 
		 "timetz_f"        : get_val("pgsql_bind(PG_TYPE_TIMETZ, \"11:38:21 CST\")"), 
		 "timestamp_f"     : 2005-04-01T11:35:26, 
		 "timestamptz_f"   : 2005-04-01T11:35:26.259,
		 "tinterval_f"     : get_val("pgsql_bind(PG_TYPE_TINTERVAL, '[\"May 10, 1947 23:59:12\" \"Jan 14, 1973 03:14:21\"]')"),
		 "bytea_f"         : <bead>
		 #bit_f             : 
		 #varbit_f          : 
    );

    $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", hash_values($args));

    my $q = $db.selectRow("select * from data_test");
    if ($o.verbose > 1)
	foreach my $k in (keys $q)
	    printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    # fix values where we know the return type is different
    $args.money_f = 400.56;
    $args.timetz_f = 11:38:21;
    $args.tinterval_f = '["1947-05-10 21:59:12" "1973-01-14 02:14:21"]';
    $args.number_f = "7235634215.3250";
    $args.reltime_f = 19177551s;
    $args.interval_f = 6M + 3D + 9915s;

    # rounding errors can happen in float4
    $q.float4_f = round($q.float4_f);
    $args.float4_f = round($args.float4_f);

    # remove values where we know they won't match
    # abstime and timestamptz are converted to GMT by the server
    delete $q.abstime_f;
    delete $q.timestamptz_f;
    
    # compare each value
    foreach my $k in (keys $q)
	test_value($q.$k, $args.$k, sprintf("%s bind and retrieve", $k));

    $db.commit();
}

sub mysql_test()
{
}

sub sybase_test($db)
{
    my $args = ( "null_f"          : NULL,
		 "varchar_f"       : "varchar", 
		 "char_f"          : "char", 
		 "unichar_f"       : "unichar",
		 "univarchar_f"    : "univarchar",
		 "text_f"          : "test",
		 "unitext_f"       : "test",
		 "bit_f"           : True,
		 "tinyint_f"       : 55, 
		 "smallint_f"      : 4285, 
		 "int_f"           : 405402,
		 "decimal_f"       : 500.1231, 
		 "float_f"         : 23443.234324234, 
		 "real_f"          : 213.123, 
		 "money_f"         : 3434234250.2034, 
		 "smallmoney_f"    : 211100.1012,
		 "date_f"          : 2007-05-01, 
	         "time_f"          : 10:30:01, 
		 "datetime_f"      : 3459-01-01T11:15:02.250, 
		 "smalldatetime_f" : 2007-12-01T12:01:00, 
		 "binary_f"        : <0badbeef>, 
		 "varbinary_f"     : <feedface>, 
		 "image_f"         : <cafebead> );

    # insert data
    my $rows = $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", hash_values($args));

    my $q = $db.selectRow("select * from data_test");
    if ($o.verbose > 1)
	foreach my $k in (keys $q)
	    printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    # remove values where we know they won't match
    # unitext_f is returned as IMAGE by the server
    delete $q.unitext_f;
    delete $args.unitext_f;
    # rounding errors can happen in real
    $q.real_f = round($q.real_f);
    $args.real_f = round($args.real_f);
    
    # compare each value
    foreach my $k in (keys $q)
	test_value($q.$k, $args.$k, sprintf("%s bind and retrieve", $k));

    $db.commit();
}

sub mssql_test($db)
{
    # the mssql driver does not with with the following sybase column types:
    # unichar, univarchar

    my $args = ( "null_f"          : NULL,
		 "varchar_f"       : "test", 
		 "char_f"          : "test", 
		 "text_f"          : "test",
		 "unitext_f"       : "test",
		 "bit_f"           : True,
		 "tinyint_f"       : 55, 
		 "smallint_f"      : 4285, 
		 "int_f"           : 405402,
		 "decimal_f"       : 500.1231, 
		 "float_f"         : 23443.234324234, 
		 "real_f"          : 213.123, 
		 "money_f"         : 3434234250.2034, 
		 "smallmoney_f"    : 211100.1012,
		 "date_f"          : 2007-05-01, 
	         "time_f"          : 10:30:01, 
		 "datetime_f"      : 3459-01-01T11:15:02.250, 
		 "smalldatetime_f" : 2007-12-01T12:01:00, 
		 "binary_f"        : <0badbeef>, 
		 "varbinary_f"     : <feedface>, 
		 "image_f"         : <cafebead> );

    # insert data, using the values from the hash above
    my $rows = $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", hash_values($args));

    my $q = $db.selectRow("select * from data_test");
    if ($o.verbose > 1)
	foreach my $k in (keys $q)
	     printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    # remove values where we know they won't match
    # unitext_f is returned as IMAGE by the server
    delete $q.unitext_f;
    delete $args.unitext_f;
    # rounding errors can happen in real
    $q.real_f = round($q.real_f);
    $args.real_f = round($args.real_f);
    
    # compare each value
    foreach my $k in (keys $q)
	test_value($q.$k, $args.$k, sprintf("%s bind and retrieve", $k));

    $db.commit();
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

    printf("testing %s driver\n", $db.getDriverName());
    create_datamodel($db);

    context_test($db);
    transaction_test($db);
    my $test = $test_map.($db.getDriverName());
    if (exists $test)
	$test($db);
    
    if (!$o.leave)
	drop_test_datamodel($db);
    printf("%d/%d tests OK\n", $test_count - $errors, $test_count);
}

main();
