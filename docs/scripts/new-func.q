#!/usr/bin/env qore

sub generate_toc()
{
    my $rows = ();
    foreach my $k in (sort(keys $funcs))
    {
	my $entry[0].para.link = ( "^attributes^" : ( "linkend" : $k ), "^value^" : $k + "()" );
	$entry[1].para = exists $funcs.$k.ret ? $funcs.$k.ret : "N/A";
	$entry[2].para = exists $funcs.$k.exceptions ? "Y" : "N";
	$entry[3].para = $funcs.$k.desc;
	$rows += ( "entry" : $entry );
    }
    my $chapter = ( "chapter" : ( "table" : ( "tbody" : ( "row" : $rows ) ) ) );
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
    foreach my $k in (sort(keys $funcs))
    {
	my $sect2 = ( "^attributes^" : ( "id" : $k ),
		      "title" : $k + "()",
		      "variablelist" : 
		      ( "varlistentry" :
			(( "term" : "Synopsis",
			   "listitem" : 
			   ( "para" : exists $funcs.$k.long ? $funcs.$k.long : $funcs.$k.desc ) ),
			 ( "term" : "Usage",
			   "listitem" : 
			   ( "programlisting" : 
			     ( "^value^" : $k + "(",
			       "replaceable" : join(", ", $funcs.$k.args),
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
			    ( "entry" : (( "para" : join(", ", doCap($funcs.$k.args)) ),
					 ( "para" : $funcs.$k.ret ),
					 ( "para" : $funcs.$k.desc ) ) ) ) ) ) );

	if (exists $funcs.$k.exceptions)
	{
	    my $rows = ();
	    foreach my $e in (keys $funcs.$k.exceptions)
	    {
		my $l[0].para.code = $e;
		$l[1].para = $funcs.$k.exceptions.$e;
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
    our $funcs = $p.run();

    generate_toc();
    generate_info();
}

main();
