#!/usr/bin/env qore

sub generate_toc()
{
    my $rows = ();
    foreach my $k in (keys $classes)
    {
	my $mrows = ();
	foreach my $m in (keys $classes.$k.methods)
	{
	    my $l[0].para.link = ( "^attributes^" : ( "linkend" : $k + "_" + $m ),
				   "^value^" : $k + "::" + $m + "()" );
	    $l[1].para = exists $classes.$k.methods.$m.exceptions ? "Y" : "N";
	    $l[2].para = $classes.$k.methods.$m.desc;
	    $mrows += ( "entry" : $l );
	}

	my $sect = ( "^attributes^" : ( "id" : $k + "_Class" ),
		     "title" : $k + " Class",
		     "para" : "",
		     "table" : 
		     ( "title" : $k + " Class Method Overview",
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
			 ( "row" : $mrows ) ) ) );

	$rows += $sect;
    }
    my $chapter = ( "chapter" : ( "sect1" : $rows ) );
    printf("%s\n", makeFormattedXMLString($chapter));
}

sub doCap($args)
{
    my $l = ();
    foreach my $e in ($args)
	$l += toupper(substr($e, 0, 1)) + substr($e, 1);
    return $l;
}

sub generate_info()
{
    my $rows = ();
    foreach my $k in (keys $classes)
    {
	my $sect2 = ( "^attributes^" : ( "id" : $k ),
		      "title" : $k + "()",
		      "variablelist" : 
		      ( "varlistentry" :
			(( "term" : "Synopsis",
			   "listitem" : 
			   ( "para" : exists $classes.$k.long ? $classes.$k.long : $classes.$k.desc ) ),
			 ( "term" : "Usage",
			   "listitem" : 
			   ( "programlisting" : 
			     ( "^value^" : $k + "(",
			       "replaceable" : join(", ", $classes.$k.args),
			       "^value1^" : ")" ) ) ) ) ),
		      "table" : 
		      ( "title" : "Arguments and Return Values for " + $k + "()",
			"tgroup" :
			( "^attributes^" :
			  ( "cols" : 3,
			    "align" : "left",
			    "colsep" : "1",
			    "rowsep" : "1" ),
			  "thead" :
			  ( "row" : 
			    ( "entry" : (( "para" : "Argument Type" ),
					 ( "para" : "Return Type" ),
					 ( "para" : "Description" ) ))),
			  "tbody" : 
			  ( "row" : 
			    ( "entry" : (( "para" : join(", ", doCap($classes.$k.args)) ),
					 ( "para" : $classes.$k.ret ),
					 ( "para" : $classes.$k.desc ) ) ) ) ) ) );

	if (exists $classes.$k.exceptions)
	{
	    my $rows = ();
	    foreach my $e in (keys $classes.$k.exceptions)
	    {
		my $l[0].para.code = $e;
		$l[1].para = $classes.$k.exceptions.$e;
		$rows += ( "entry" : $l );
	    }

	    $sect2."table^1" = 
		( "title" : "Exceptions thrown by " + $k + "()",
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
		    ( "row" : $rows ) ) );
	}

	$rows += $sect2;
    }

    my $chapter = ( "chapter" : ( "sect2" : $rows ) );
    printf("%s\n", makeFormattedXMLString($chapter));

}

sub main()
{
    my $fn = shift $ARGV;
    if (!strlen($fn))
    {
	printf("no file name\n");
	exit(1);
    }
    my $f = new File();
    $f.open($fn);
    my $str = "";
    while (exists ($line = $f.readLine()))
	$str += $line;

    my $p = new Program();
    $p.parse($str, "code");
    our $classes = $p.run();

    generate_toc();
    #generate_info();
}

main();
