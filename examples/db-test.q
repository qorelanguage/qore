#!/usr/bin/env qore

# database example script
# databases users must be able to create and destroy tables and procedures, etc
# in order to execute all tests

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
        int2_field smallint not null,
        int4_field integer not null,
        int8_field int8 not null,
        bool_field boolean not null,
        
        float4_field real not null,
        float8_field double precision not null,
        
        number_field numeric(14) not null,
        money_field money not null,

        text_field text not null,
        varchar_field varchar(40) not null,
        char_field char(40) not null,
        name_field name not null,

        date_field date not null,
        abstime_field abstime not null,
        reltime_field reltime not null,
        interval_field interval not null,
        time_field time not null,
        timetz_field time with time zone not null,
        timestamp_field timestamp not null,
        timestamptz_field timestamp with time zone not null,
        tinterval_field tinterval not null,
        
        bytea_field bytea not null
        --bit_field bit(11) not null,
        --varbit_field bit varying(11) not null
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
    $db.exec("insert into attributes values ( 1, 'eyes', 'brown' )");
    $db.exec("insert into attributes values ( 2, 'hair', 'blond' )");
    $db.exec("insert into attributes values ( 2, 'eyes', 'blue' )");
    $db.exec("insert into attributes values ( 3, 'hair', 'brown' )");
    $db.exec("insert into attributes values ( 3, 'eyes', 'green')");
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

sub context_test($db)
{
    # first we select all the data from the tables and then use 
    # context statements to order the output hierarchically
    
    # context statements are most useful when a set of queries can be executed once
    # and the results processed many times by creating "views" with context statements

    my $people = $db.select("select * from people");
    my $attributes = $db.select("select * from attributes");

    my $today = format_date("YYYYMMDD", now());

    # display each family sorted by family name
    context family ($db.select("select * from family")) sortBy (%name)
    {
	printf("Family %d: %s\n", %family_id, %name);

	# display people, sorted by eye color, descending
	context people ($people) 
	    sortDescendingBy (find %value in $attributes 
			      where (%attribute == "eyes" 
				     && %person_id == %people:person_id)) 
	    where (%family_id == %family:family_id)
	{
	    printf("  %s, born %s\n", %name, format_date("Month DD, YYYY", %dob));
	    context ($attributes) sortBy (%attribute) where (%person_id == %people:person_id)
		printf("    has %s %s\n", %value, %attribute);
	}
    }
}

sub test_timeout($db, $c)
{
    $db.setTransactionLockTimeout(1ms);
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
    # signal parent thread to continue
    $c.dec();
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

sub pgsql_test($db)
{
    # here we use a little workaround for the fact that the pgsql module provides
    # some functions and constants needed by this function, but it is loaded on
    # demand when the Datasource is first constructed (or not at all if another 
    # test is being run), therefore parse exceptions would occur unless either
    # the module is loaded at parse tine (in which case no tests could run if
    # the module is not present or loadable, for example, with a system where
    # postgresql is not installed), or this code is placed in a subprogram...x
    my $str = "
    return (
         258,            #-- int2
         233932,         #-- int4
         239392939458,   #-- int8
         True,           #-- bool

         21.3444,        #-- float4
         49394.23423491, #-- float8
         
         1232333200.304, #-- numeric
         pgsql_bind(PG_TYPE_CASH, \"400.56\"), #-- cash
              
         'some text  ',  #-- text
         'varchar ',     #-- varchar
         'char text ',   #-- char
         'name',         # --name

         2004-01-05,   #-- date
         2005-12-03,   #-- abstime
         5M + 71D + 19h + 245m + 51s, #-- reltime
         6M + 3D + 2h + 45m + 15s, #-- interval
         11:35:00,        #-- time
         pgsql_bind(PG_TYPE_TIMETZ, \"11:38:21 CST\"),    #-- time with tz
         2005-04-01T11:35:26,          #-- timestamp
         2005-04-01T11:35:26.259,      #-- timestamp with time zone
         pgsql_bind(PG_TYPE_TINTERVAL, '[\"May 10, 1947 23:59:12\" \"Jan 14, 1973 03:14:21\"]'),
         
         <bead>          #-- bytea
         #--B'10100010011',        #-- bit
         #--B'001010011'           #-- varbit
         );
";

    my $p = new Program();
    $p.parse($str, "code");
    my $args = $p.run();

    $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", $args);
    my $q = $db.selectRow("select * from data_test");
    foreach my $k in (keys $q)
	printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    $db.commit();
}

sub mysql_test()
{
}

sub sybase_test($db)
{
    my $args = ( NULL, "test", "test", "test", "test", "test", "test", True, 55, 4285, 405402,
		 500.1231, 23443.234324234, 213.123, 3434234250.2034, 211100.1012,
		 2007-05-01, 10:30:01, 3459-01-01T11:15:02.251, 2007-12-01T12:01:00, <0badbeef>, <feedface>, <cafebead> );

    # insert data
    my $rows = $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", $args);

    my $q = $db.selectRow("select * from data_test");
    foreach my $k in (keys $q)
	printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

    $db.commit();
}

sub mssql_test($db)
{
    # freetds doesn't support the following column types as far as I can tell:
    # unichar, univarchar

    my $args = ( NULL, "test", "test", "test", "test", True, 55, 4285, 405402,
		 500.1231, 23443.234324234, 213.123, 3434234250.2034, 211100.1012,
		 2007-05-01, 10:30:01, 3459-01-01T11:15:02.251, 2007-12-01T12:01:00, <0badbeef>, <feedface>, <cafebead> );

    # insert data
    my $rows = $db.vexec("insert into data_test values (%v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v, %v)", $args);

    my $q = $db.selectRow("select * from data_test");
    foreach my $k in (keys $q)
	printf(" %-16s= %-10s %N\n", $k, type($q.$k), $q.$k);

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

    create_datamodel($db);

    context_test($db);
    transaction_test($db);
    my $test = $test_map.($db.getDriverName());
    if (exists $test)
	$test($db);

    drop_test_datamodel($db);
}

main();
