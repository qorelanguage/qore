#!/usr/bin/env qore

%require-our

%requires qore >= 0.7.2

my $opt; # global options

const opts = ( "verb" : "v,verbose",
	       "help" : "h,help" );

const infix = ( "+", "-", "*", "/", "&", "|", "&&", "||", "<", "<=", "==",
		"===", ">=", ">", "<=>", "%", "!=", "!==", ">>", "<<",
		".", "^" );

const assign = ( "=", "+=", "-=", "*=", "/=", "&=", "|=", "%=", "^=", 
		 ">>=", "<<=" );

const lop = ( "!", "~", "-" );

const lv_pre_post = ( "++", "--" );

const lv = (
    "0" : ( "trim", "chomp", "pop", "shift" ),
    "1" : ( "unshift", "push", "splice" ),
    "2" : ( "splice" ),
    "3" : ( "splice" )
    );

const pre = (
    "1" : ( "elements", "exists", "keys" ),
    "2" : ( "map", "foldl", "foldr", "select" ),
    "3" : ( "map" )
    );

# strings that will be parsed to be all basic Qore types
my $types = ("NOTHING" : 'NOTHING',
	     "integer" : '1',
	     "float" : '2.3',
	     "string" : '"string"',
	     "date" : '2008-12-03T10:09:14.123',
	     "boolean" : 'True',
	     "NULL" : 'NULL',
	     "binary" : 'binary("hello")',
	     "list" : '(1, "two", 3.0)',
	     "hash" : '( "a" : "value", "another" : 123 )',
	     "object" : '(new Mutex())',
	     "NOTHING0" : 'my $nothing'
    );

my $kt = keys $types;

sub eval($desc, $str) {
    if ($opt.verb)
	stdout.printf($desc);

    my $p = new Program();
    try {
	$p.parse($str, "test");
	$p.run();
    }
    catch ($ex) {
	if ($opt.verb)
	    stdout.printf("INVALID (%s: %s)\n", $ex.err, $ex.desc);
	else
	    stdout.printf("X");

	return;
    }
    if ($opt.verb)
	stdout.printf("OK\n"); 
    else
	stdout.printf(".");
}

sub infix_test() {
    if (!$opt.verb)
	stdout.printf("infix tests: ");
    foreach my $op in (infix) {
	foreach my $t in ($kt) {
	    foreach my $t1 in ($kt) {
		my $desc = sprintf("infix: evaluating %s %s %s: ", $t, $op, $t1);
		my $str = sprintf("my $l=%s;my $r=%s; return $l %s $r;", $types.$t, $types.$t1, $op);
		eval($desc, $str);
	    }
	}
    }
    if (!$opt.verb)
	stdout.printf("\n");
}

sub assign_test() {
    if (!$opt.verb)
	stdout.printf("assignment tests: ");
    foreach my $op in (assign) {
	foreach my $t in ($kt) {
	    my $desc = sprintf("assign: evaluating %s %s: ", $op, $t);
	    my $str = sprintf("my $v=%s;my $x %s $v;", $types.$t, $op);
	    eval($desc, $str);
	}
    }
    if (!$opt.verb)
	stdout.printf("\n");
}

sub lop_tests() {
    if (!$opt.verb)
	stdout.printf("unary prefix operator tests: ");
    foreach my $op in (lop) {
	foreach my $t in ($kt) {
	    my $desc = sprintf("unary prefix: evaluating %s%s: ", $op, $t);
	    my $str = sprintf("my $v=%s; return %s$v;", $types.$t, $op);
	    eval($desc, $str);
	}
    }
    if (!$opt.verb)
	stdout.printf("\n");
}

sub lv_pre_tests() {
    if (!$opt.verb)
	stdout.printf("lvalue prefix operator tests: ");
    foreach my $op in (lv_pre_post) {
	foreach my $t in ($kt) {
	    my $desc = sprintf("lvalue prefix: evaluating %s%s: ", $op, $t);
	    my $str = sprintf("my $v=%s; return %s$v;", $types.$t, $op);
	    eval($desc, $str);
	}
    }
    if (!$opt.verb)
	stdout.printf("\n");
}

sub lv_post_tests() {
    if (!$opt.verb)
	stdout.printf("lvalue postfix operator tests: ");
    foreach my $op in (lv_pre_post) {
	foreach my $t in ($kt) {
	    my $desc = sprintf("lvalue postfix: evaluating %s%s: ", $t, $op);
	    my $str = sprintf("my $v=%s; return $v%s;", $types.$t, $op);
	    eval($desc, $str);
	}
    }
    if (!$opt.verb)
	stdout.printf("\n");
}

sub lv_tests() {
    if (!$opt.verb)
	stdout.printf("lvalue operator tests: ");
   
    # get list of values in hash
    my $vt = hash_values($types);

    foreach my $num in (keys lv) {
	my $limit = int($num) + 1;

	foreach my $op in (lv.$num) {
	    my $max = pow(elements $kt, $limit);

	    my $p;
	    # calculate offsets
	    for (my $i = 0; $i < $limit; ++$i)
		$p[$i] = pow(elements $kt, $i);

	    #printf("DEBUG: offsets: %N\n", $p);

	    for (my $i = 0; $i < $max; ++$i) {
		my $str;
		my $args;
		my $dargs;
		for (my $j = $limit - 1; $j >= 0; --$j) {
		    # get each argument's position in the type hash
		    #printf("DEBUG: i=%n,j=%n: %n\n", $i, $j, $p[$j]);
		    my $x = ($i / $p[$j]) % (elements $kt);
		    $str += sprintf("my $l[%d]=%s;", $j, $vt[$x]);
		    $args += sprintf("$l[%d], ", $j);
		    $dargs += sprintf("%s, ", $kt[$x]);
		}
		splice $args, -2;
		splice $dargs, -2;
		my $desc = sprintf("lvalue: evaluating %s, %s: ", $op, $dargs);
		$str += sprintf("%s %s;", $op, $args);
		eval($desc, $str);
	    }
	}
    }

    if (!$opt.verb)
	stdout.printf("\n");
}

sub pre_tests() {
    if (!$opt.verb)
	stdout.printf("pre operator tests: ");
   
    # get list of values in hash
    my $vt = hash_values($types);

    foreach my $num in (keys pre) {
	my $limit = int($num);

	foreach my $op in (pre.$num) {
	    my $max = pow(elements $kt, $limit);

	    my $p;
	    # calculate offsets
	    for (my $i = 0; $i < $limit; ++$i)
		$p[$i] = pow(elements $kt, $i);

	    for (my $i = 0; $i < $max; ++$i) {
		my $str;
		my $args;
		my $dargs;
		for (my $j = $limit - 1; $j >= 0; --$j) {
		    # get each argument's position in the type hash
		    #printf("DEBUG: i=%n,j=%n: %n\n", $i, $j, $p[$j]);
		    my $x = ($i / $p[$j]) % (elements $kt);
		    $str += sprintf("my $l[%d]=%s;", $j, $vt[$x]);
		    $args += sprintf("$l[%d], ", $j);
		    $dargs += sprintf("%s, ", $kt[$x]);
		}
		splice $args, -2;
		splice $dargs, -2;
		my $desc = sprintf("lvalue: evaluating %s, %s: ", $op, $dargs);
		$str += sprintf("return %s %s;", $op, $args);
		eval($desc, $str);
	    }
	}
    }

    if (!$opt.verb)
	stdout.printf("\n");
}

sub main() {
    my $g = new GetOpt(opts);
    try
	$opt = $g.parse2(\$ARGV);
    catch ($ex) {
	printf("option error: %s\n", $ex.desc);
        exit(1);
    }

    infix_test();
    assign_test();
    lop_tests();
    lv_pre_tests();
    lv_post_tests();
    lv_tests();
    pre_tests();
}

main();

