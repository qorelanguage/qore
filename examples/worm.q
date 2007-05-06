#!/usr/bin/env qore

%requires ncurses

%require-our
%enable-all-warnings

# ported to qore by David Nichols

/*

	 @@@        @@@    @@@@@@@@@@     @@@@@@@@@@@    @@@@@@@@@@@@
	 @@@        @@@   @@@@@@@@@@@@    @@@@@@@@@@@@   @@@@@@@@@@@@@
	 @@@        @@@  @@@@      @@@@   @@@@           @@@@ @@@  @@@@
	 @@@   @@   @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@  @@@@  @@@  @@@        @@@   @@@            @@@  @@@   @@@
	 @@@@ @@@@ @@@@  @@@        @@@   @@@            @@@  @@@   @@@
	  @@@@@@@@@@@@   @@@@      @@@@   @@@            @@@  @@@   @@@
	   @@@@  @@@@     @@@@@@@@@@@@    @@@            @@@  @@@   @@@
	    @@    @@       @@@@@@@@@@     @@@            @@@  @@@   @@@

				 Eric P. Scott
			  Caltech High Energy Physics
				 October, 1980

		Hacks to turn this into a test frame for cursor movement:
			Eric S. Raymond <esr@snark.thyrsus.com>
				January, 1995

		July 1995 (esr): worms is now in living color! :-)

Options:
	-f			fill screen with copies of 'WORM' at start.
	-l <n>			set worm length
	-n <n>			set number of worms
	-t			make worms leave droppings
	-T <start> <end>	set trace interval
	-S			set single-stepping during trace interval
	-N			suppress cursor-movement optimization

  This program makes a good torture-test for the ncurses cursor-optimization
  code.  You can use -T to set the worm move interval over which movement
  traces will be dumped.  The program stops and waits for one character of
  input at the beginning and end of the interval.

  $Id: worm.q,v 1.4 2006/04/27 06:04:00 david_nichols Exp $
*/

our $flavor = ( ord('O'), ord('*'), ord('#'), ord('$'), ord('%'), ord('0'), ord('@') );

our $colors = ( COLOR_GREEN, COLOR_RED, COLOR_CYAN, COLOR_WHITE, COLOR_MAGENTA, COLOR_BLUE, COLOR_YELLOW);

our $xinc = (  1, 1, 1, 0, -1, -1, -1, 0);
our $yinc = ( -1, 0, 1, 1, 1, 0, -1, -1);

our $field;
our $length = 16;
our $number = 3;
our $trail = ' ';

our $normal = (
    ( "nopts" : 3,
      "opts" : ( 7, 0, 1 ) ),
    ( "nopts" : 3,
      "opts" : ( 0, 1, 2 ) ),
    ( "nopts" : 3,
      "opts" : ( 1, 2, 3 ) ),
    ( "nopts" : 3,
      "opts" : ( 2, 3, 4 ) ),
    ( "nopts" : 3,
      "opts" : ( 3, 4, 5 ) ),
    ( "nopts" : 3,
      "opts" : ( 4, 5, 6 ) ),
    ( "nopts" : 3,
      "opts" : ( 5, 6, 7 ) ),
    ( "nopts" : 3,
      "opts" : ( 6, 7, 0 ) ));

our $upper = (
    ( "nopts" : 1,
      "opts" : ( 1, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 1, 2, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 4, 5, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 5, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 1, 5, 0 ) ));

our $left = (
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 2, 3, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 3, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 3, 7, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 7, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 7, 0, 0 ) ));

our $right = (
    ( "nopts" : 1,
      "opts" : ( 7, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 3, 7, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 3, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 3, 4, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 6, 7, 0 ) ));

our $lower = (
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 0, 1, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 1, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 1, 5, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 5, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 5, 6, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ));

our $upleft = (
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 3, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 1, 3, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 1, 0, 0 ) ));

our $upright = (
    ( "nopts" : 2,
      "opts" : ( 3, 5, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 3, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 5, 0, 0 ) ));

our $lowleft = (
    ( "nopts" : 3,
      "opts" : ( 7, 0, 1 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 1, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 1, 7, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 7, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ));

our $lowright = (
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 7, 0, 0 ) ),
    ( "nopts" : 2,
      "opts" : ( 5, 7, 0 ) ),
    ( "nopts" : 1,
      "opts" : ( 5, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ),
    ( "nopts" : 0,
      "opts" : ( 0, 0, 0 ) ));

sub cleanup()
{
    #standend();
    #refresh();
    curs_set(1);
    endwin();
}

sub ranf()
{
    my $r = (rand() & 077777);
    return (float($r) / 32768.0);
}

const worm_opts = 
    ( "field"  : "f,field",
      "length" : "l,length=i",
      "number" : "n,number=i",
      "trail"  : "t,trail",
      "help"   : "h,help" );

sub usage()
{
    printf(
"usage: %s [options]
  -f,--field
  -l,--length=arg
  -n,--number=arg
  -t,--trail
  -h,--help
",
	   basename($ENV."_"));
    exit(1);
}

sub doSomething1($w)
{
    $w.orientation = $w.head = 0;

    for (my $x = $length; --$x >= 0;)
	$w.xpos[$x] = -1;

    for (my $y = $length; --$y >= 0;)
	$w.ypos[$y] = -1;
}

sub doSomething2($w, $ref, $n)
{
    my ($x, $y, $h);

    if (($x = $w.xpos[$h = $w.head]) < 0) {
	$win.move(($y = $w.ypos[$h] = $bottom), ($x = $w.xpos[$h] = 0));
	$win.addch($flavor[$n % (elements $flavor)]);
	$ref[$y][$x]++;
    } else {
	$y = $w.ypos[$h];
    }
    if ($x > $last)
	$x = $last;
    if ($y > $bottom)
	$y = $bottom;
    if (++$h == $length)
	$h = 0;
    if ($w.xpos[$w.head = $h] >= 0) {
	my ($x1, $y1);
	$x1 = $w.xpos[$h];
	$y1 = $w.ypos[$h];
	if ($y1 < $win.getLines()
	    && $x1 < $win.getColumns()
	    && --$ref[$y1][$x1] == 0) {
	    $win.move($y1, $x1);
	    $win.addch($trail);
	}
    }
    
    my $op = ($x == 0 ? ($y == 0 ? $upleft : ($y == $bottom ? $lowleft :	$left)) :
	      ($x == $last ? ($y == 0 ? $upright : ($y == $bottom ? $lowright : $right)) :
	       ($y == 0 ? $upper : ($y == $bottom ? $lower : $normal))))[$w.orientation];
    switch ($op.nopts) {
	case 0:
	cleanup();
	exit(0);

	case 1:
	$w.orientation = $op.opts[0];
	break;

      default:
	$w.orientation = $op.opts[ranf() * float($op.nopts)];
    }
    $win.move(($y += $yinc[$w.orientation]), ($x += $xinc[$w.orientation]));
    
    if ($y < 0)
	$y = 0;
    $win.addch($flavor[$n % (elements $flavor)]);
    $ref[$w.ypos[$h] = $y][$w.xpos[$h] = $x]++;
}

sub main()
{
    my ($ref, $x, $y, $n, $w, $h, $worm);

    my $g = new GetOpt(worm_opts);
    my $o = $g.parse(\$ARGV);
    if (exists $o."_ERRORS_")
    {
	printf("%s\n", $o."_ERRORS_"[0]);
	exit(1);
    }
    if ($o.help)
	usage();

    if ($o.field)
	$field = "WORM";
    if ($o.length)
    {
	$length = $o.length;
	if ($length < 2 || $length > 1024) {
	    printf("%s: Invalid length\n", basename($ENV."_"));
	    exit(1);
	}
    }
    if ($o.number)
    {
	$number = $o.number;
	if ($number < 1 || $number > 40) {
	    printf("%s: Invalid number of worms\n", basename($ENV."_"));
	    exit(1);
	}
    }
    if ($o.trail)
	$trail = ".";

    initscr();
    nonl();
    curs_set(0);
    our $win = new Window();
    our $bottom = $win.getLines() - 1;
    our $last = $win.getColumns() - 1;

    if (has_colors()) {
    	for (my $i = 0; $i < num_colors(); $i++)
    	   $flavor[$i] |= (A_BOLD | get_color_pair($i + 1, COLOR_BLACK));
    }

    for ($y = 0; $y < $win.getLines(); $y++) {
	for ($x = 0; $x < $win.getColumns(); $x++) {
	    $ref[$y][$x] = 0;
	}
    }

    $w = 0;
    for ($n = $number; --$n >= 0; $w++)
	doSomething1(\$worm[$w]);

    my $p;
    if (strlen($field)) {
	for ($y = $bottom; --$y >= 0;) {
	    for ($x = $win.getColumns(); --$x >= 0;) {
		$win.addch(substr($field, $p++, 1));
		if ($p == strlen($field))
		    $p = 0;
	    }
	}
    }
    usleep(10000);
    $win.refresh();

    for (;;) {
	my $ch;

	if (($ch = $win.getch()) > 0) {
	    if ($ch == KEY_RESIZE) {
		if ($last != $win.getColumns() - 1) {
		    for ($y = 0; $y <= $bottom; $y++) {
			for ($x = $last + 1; $x < $win.getColumns(); $x++)
			    $ref[$y][$x] = 0;
		    }
		    $last = $win.getColumns() - 1;
		}
		if ($bottom != $win.getLines() - 1) {
		    for ($y = $bottom + 1; $y < $win.getLines(); $y++) {
			for ($x = 0; $x < $win.getColumns(); $x++)
			    $ref[$y][$x] = 0;
		    }
		    $bottom = $win.getLines() - 1;
		}
	    }
	    /*
		* Make it simple to put this into single-step mode, or resume
		* normal operation -T.Dickey
	    */
	    if ($ch == ord('q')) {
		cleanup();
		exit(0);
	    } else if ($ch == ord('s')) {
		$win.nodelay(False);
	    } else if ($ch == ord(' ')) {
		$win.nodelay(True);
	    }
	}

	$w = 0;
	for ($n = 0; $n < $number; $n++)
	{
	    doSomething2(\$worm[$w], \$ref, $n);
	    $w++;
	}
	usleep(10000);
	$win.refresh();
    }
}

main();
