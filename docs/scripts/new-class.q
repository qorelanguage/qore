#!/usr/bin/env qore

%enable-all-warnings
%require-our

sub generate_toc($name, $class)
{
    my $rows = ();
    foreach my $m in (keys $class.methods)
    {
	my $l[0].para.link = ( "^attributes^" : ( "linkend" : $name + "_" + $m ),
			       "^value^" : $name + "::" + $m + "()" );
	$l[1].para = exists $class.methods.$m.exceptions ? "Y" : "N";
	$l[2].para = $class.methods.$m.desc;
	$rows += ( "entry" : $l );
    }

    return ( "^attributes^" : ( "id" : $name + "_Class" ),
	     "title" : $name + " Class",
	     "para" : $class.desc,
	     "table" : 
	     ( "title" : $name + " Class Method Overview",
	       "tgroup" :
	       ( "^attributes^" :
		 ( "cols" : 3,
		   "align" : "left",
		   "colsep" : "1",
		   "rowsep" : "1" ),
		 "thead" :
		 ( "row" : 
		   ( "entry" : (( "para" : "Method" ),
				( "para" : "Except?" ),
				( "para" : "Description" ) ))),
		 "tbody" : 
		 ( "row" : $rows ) ) ) );
}

sub doCap($args)
{
    my $l = ();
    foreach my $e in ($args)
	$l += toupper(substr($e, 0, 1)) + substr($e, 1);
    return $l;
}

sub do_arg($arg, $opt)
{
    if ($opt)
	return sprintf("[%s]", $arg);
    return $arg;
}

sub get_arg_list($mname, $args)
{
    if (!exists $args)
	return $mname + "()";
    my $l = ();
    foreach my $key in (keys $args)
	$l += do_arg($key, $args.$key.optional);

    return 
	( "^value^" : $mname + "(",
	  "replaceable" : join(", ", $l),
	  "^value1^" : ")" );
}

sub get_rv($name, $m, $rv)
{
    my $type = exists $rv.type ? $rv.type : ( $m == "constructor" ? "Object" : "n/a");
    my $desc = exists $rv.desc 
	? $rv.desc
	: ($m == "constructor"  
	   ? "The " + $name + " object is returned"
	   : "This method returns no value");
    return (( "para" : $type ),
	    ( "para" : $desc ) );	
}

sub get_arg_rows($args)
{
    if (!exists $args)
	return ( "entry" : (( "para" : "n/a" ), ( "para" : "n/a" ), ( "para" : "This method takes no arguments." )) );

    my $arg_rows = ();
    foreach my $arg in (keys $args)
    {
	my $entry[0].para.replaceable = do_arg($arg, $args.$arg.optional);
	$entry[1].para = exists $args.$arg.type ? $args.$arg.type : "String";
	$entry[2].para = $args.$arg.desc;
	$arg_rows += ( "entry" : $entry );
    }
    return $arg_rows;
}

sub generate_info($name, $class)
{
    my $rows = ();
    foreach my $m in (keys $class.methods)
    {
	my $meth = $class.methods.$m;
	
	my $sect = ( "^attributes^" : ( "id" : $name + "_" + $m ),
		     "title" : $name + "::" + $m + "()",
		     "variablelist" : 
		     ( "varlistentry" :
		       (( "term" : "Synopsis",
			  "listitem" : 
			  ( "para" : exists $meth.long ? $meth.long : $meth.desc ) ),
			( "term" : "Usage",
			  "listitem" : 
			  ( "programlisting" : get_arg_list($name + "::" + $m, $meth.args) ) ) ) ),
		     "table" : 
		     ( "title" : "Arguments for " + $name + "::" + $m + "()",
		       "tgroup" :
		       ( "^attributes^" :
			 ( "cols" : 3,
			   "align" : "left",
			   "colsep" : "1",
			   "rowsep" : "1" ),
			 "thead" :
			 ( "row" : 
			   ( "entry" : (( "para" : "Argument" ),
					( "para" : "Type" ),
					( "para" : "Description" ) ))),
			 "tbody" : 
			 ( "row" : get_arg_rows($meth.args) ) ) ),
		     "table^1" : 
		     ( "title" : "Return Values for " + $name + "::" + $m + "()",
		       "tgroup" :
		       ( "^attributes^" :
			 ( "cols" : 2,
			   "align" : "left",
			   "colsep" : "1",
			   "rowsep" : "1" ),
			 "thead" :
			 ( "row" : 
			   ( "entry" : (( "para" : "Return Type" ),
					( "para" : "Description" ) ))),
			 "tbody" : 
			 ( "row" : 
			   ( "entry" : get_rv($name, $m, $meth.rv) ) ) ) ) );
	
	if (exists $meth.exceptions)
	{
	    my $erows = ();
	    foreach my $e in (keys $meth.exceptions)
	    {
		my $l[0].para.code = $e;
		$l[1].para = $meth.exceptions.$e;
		$erows += ( "entry" : $l );
	    }
	    
	    $sect."table^2" = 
		( "title" : "Exceptions thrown by " + $name + "::" + $m + "()",
		  "tgroup" :
		  ( "^attributes^" :
		    ( "cols" : 2,
		      "align" : "left",
		      "colsep" : "1",
		      "rowsep" : "1" ),
		    "thead" :
		    ( "row" : 
		      ( "entry" : (( "para" : "err" ),
				   ( "para" : "desc" ) ))),
		    "tbody" : 
		    ( "row" : $erows ) ) );
	}
	$rows += $sect;
    }
    return $rows;
}

sub main()
{
    my $fn = shift $ARGV;
    if (!strlen($fn))
    {
	printf("no file name\n");
	exit(1);
    }
    my $start = int(shift $ARGV);
    if (!$start)
	$start = 1;
    my $f = new File();
    $f.open($fn);
    my $str = "";
    while (exists (my $line = $f.readLine()))
	$str += $line;

    my $p = new Program();
    $p.parse($str, "code");
    my $classes = $p.run();

    my ($toc, $methods);
    foreach my $name in (keys $classes)
    {
	$toc = generate_toc($name, $classes.$name);
	$methods = generate_info($name, $classes.$name);
    }
    my $classkey = sprintf("sect%d", $start);
    my $methodkey = sprintf("sect%d", $start + 1);

    my $data.$classkey = $toc;
    $data.$classkey.$methodkey = $methods;
    printf("%s\n", makeFormattedXMLString(("chapter" : $data)));
}

main();
