#!/usr/bin/env qore

%requires ncurses

# ncurses example program ported to qore by David Nichols

#/*
# *	Name: Towers of Hanoi.
# *
# *	Desc:
# *		This is a playable copy of towers of hanoi.
# *		Its sole purpose is to demonstrate my Amiga Curses package.
# *		This program should compile on any system that has Curses.
# *		'hanoi'		will give a manual game with 7 playing pieces.
# *		'hanoi n'	will give a manual game with n playing pieces.
# *		'hanoi n a' will give an auto solved game with n playing pieces.
# *
# *	Author: Simon J Raybould	(sie@fulcrum.bt.co.uk).
# * 	(This version has been slightly modified by the ncurses maintainers.)
# *
# *	Date: 05.Nov.90
# *
# * $Id: hanoi.q,v 1.3 2006/04/27 06:04:00 david_nichols Exp $
# */

# half-second delay
const delay        = 500000;

const NPEGS        = 3;	/* This is not configurable !! */
const MINTILES     = 3;
const MAXTILES     = 9;
const DEFAULTTILES = 7;
const TOPLINE      = 6;
const BASELINE     = 16;
const LEFTPEG	   = 19;
const MIDPEG	   = 39;
const RIGHTPEG	   = 59;

sub LENTOIND($x)
{
    return ($x-1)/2;
}

sub OTHER($a, $b)
{
    return 3-($a+$b);
}

our $Pegs = ();

our $PegPos = (LEFTPEG, MIDPEG, RIGHTPEG);

our $TileColour = 
    (
     COLOR_WHITE,
     COLOR_GREEN,		/* Length 3 */
     COLOR_MAGENTA,		/* Length 5 */
     COLOR_RED,			/* Length 7 */
     COLOR_BLUE,			/* Length 9 */
     COLOR_CYAN,			/* Length 11 */
     COLOR_YELLOW,		/* Length 13 */
     COLOR_GREEN,		/* Length 15 */
     COLOR_MAGENTA,		/* Length 17 */
     COLOR_RED,			/* Length 19 */
     );

our $NMoves = 0;

sub main()
{
    my ($NTiles, $FromCol, $ToCol, $AutoFlag);
    $AutoFlag = False;

    if (elements $ARGV)
    {
	$NTiles = int(shift $ARGV);
	if (exists $ARGV[0])
	    if ($ARGV[0] != "a")
	        Usage();
	    else
		$AutoFlag = True;

	if ($NTiles > MAXTILES || $NTiles < MINTILES) {
	    printf("Range %d to %d\n", MINTILES, MAXTILES);
	    exit(1);
	}
    }
    else
	$NTiles = DEFAULTTILES;

    $w = new Window();
    if ($w.getLines() < 24) {
	endwin();
	printf("Min screen length 24 lines\n");
	exit(1);
    }
    if ($AutoFlag) {
	curs_set(0);
	#$w.leaveok(True);	/* Attempt to remove cursor */
    }
    InitTiles($NTiles);
    DisplayTiles();
    if ($AutoFlag) {
	do {
	    AutoMove(0, 2, $NTiles);
	} while (!Solved($NTiles));
	sleep(2);
    } else {
	echo();
	for (;;) {
	    if (GetMove(\$FromCol, \$ToCol))
		break;
	    if (InvalidMove($FromCol, $ToCol)) {
		$w.mvaddstr($w.getLines() - 3, 0, "Invalid Move !!");
		$w.refresh();
		beep();
		continue;
	    }
	    MakeMove($FromCol, $ToCol);
	    if (Solved($NTiles)) {
		$w.mvprintw($w.getLines() - 3, 0,
			    "Well Done !! You did it in %d moves", $NMoves);
		$w.refresh();
		sleep(5);
		break;
	    }
	}
    }
}

sub InvalidMove($From, $To)
{
    if ($From >= NPEGS)
	return True;
    if ($From < 0)
	return True;
    if ($To >= NPEGS)
	return True;
    if ($To < 0)
	return True;
    if ($From == $To)
	return True;
    if (!$Pegs[$From].Count)
	return True;
    if ($Pegs[$To].Count &&
	$Pegs[$From].Length[$Pegs[$From].Count - 1] >
	$Pegs[$To].Length[$Pegs[$To].Count - 1])
	return True;
    return False;
}

sub InitTiles($NTiles)
{
    my ($Size, $SlotNo);

    $Size = $NTiles * 2 + 1;
    $SlotNo = 0;
    for (; $Size >= 3; $Size -= 2)
	$Pegs[0].Length[$SlotNo++] = $Size;

    $Pegs[0].Count = $NTiles;
    $Pegs[1].Count = 0;
    $Pegs[2].Count = 0;
}

sub DisplayTiles()
{
    my ($Line, $peg, $SlotNo, $TileBuf);

    $w.erase();
    $w.mvaddstr(1, 24, "T O W E R S   O F   H A N O I");
    $w.mvaddstr(3, 34, "SJR 1990");
    $w.mvprintw(19, 5, "Moves : %d", $NMoves);
    $w.attrset(A_REVERSE);
    $w.mvaddstr(BASELINE, 8,
	     "                                                               ");

    for ($Line = TOPLINE; $Line < BASELINE; $Line++) {
	$w.mvaddch($Line, LEFTPEG, ' ');
	$w.mvaddch($Line, MIDPEG, ' ');
	$w.mvaddch($Line, RIGHTPEG, ' ');
    }
    $w.mvaddch(BASELINE, LEFTPEG, '1');
    $w.mvaddch(BASELINE, MIDPEG, '2');
    $w.mvaddch(BASELINE, RIGHTPEG, '3');
    $w.attrset(A_NORMAL);

    /* Draw tiles */
    for ($peg = 0; $peg < NPEGS; $peg++) {
	for ($SlotNo = 0; $SlotNo < $Pegs[$peg].Count; $SlotNo++) {

	    $TileBuf = "";

	    for (my $i = 0; $i < $Pegs[$peg].Length[$SlotNo]; $i++)
		$TileBuf += " ";

	    if (has_colors())
	    {
		#my $c = $TileColour[LENTOIND($Pegs[$peg].Length[$SlotNo])];
		#$w.printw("color=%5d", $c); 
		$w.setBackgroundColor($TileColour[LENTOIND($Pegs[$peg].Length[$SlotNo])]);
		#$w.printw(", color=%5d", $c); 
	    }
	    else
		$w.attrset(A_REVERSE);

	    $w.mvaddstr(BASELINE - ($SlotNo + 1),
		     ($PegPos[$peg] - $Pegs[$peg].Length[$SlotNo] / 2),
		     $TileBuf);
	}
    }
    $w.attrset(A_NORMAL);
    $w.refresh();
}

sub GetMove($From, $To)
{
    $w.mvaddstr($w.getLines() - 3, 0, "Next move ('q' to quit) from ");
    $w.clrtoeol();
    $w.refresh();

    if (($From = $w.getch()) == ord('q'))
	return True;
    $From -= (ord('0') + 1);
    $w.addstr(" to ");
    $w.clrtoeol();
    $w.refresh();

    if (($To = $w.getch()) == ord('q'))
	return True;
    $To -= (ord('0') + 1);

    $w.refresh();
    usleep(delay);

    $w.move($w.getLines() - 3, 0);
    $w.clrtoeol();
    $w.refresh();
    return False;
}

sub MakeMove($From, $To)
{
    $Pegs[$From].Count--;
    $Pegs[$To].Length[$Pegs[$To].Count] = $Pegs[$From].Length[$Pegs[$From].Count];
    $Pegs[$To].Count++;
    $NMoves++;
    DisplayTiles();
}

sub AutoMove($From, $To, $Num)
{
    if ($Num == 1) {
	MakeMove($From, $To);
	usleep(delay);
	return;
    }
    AutoMove($From, OTHER($From, $To), $Num - 1);
    MakeMove($From, $To);
    usleep(delay);
    AutoMove(OTHER($From, $To), $To, $Num - 1);
}

sub Solved($NumTiles)
{
    for (my $i = 1; $i < NPEGS; $i++)
	if ($Pegs[$i].Count == $NumTiles)
	    return True;
    return False;
}

sub Usage()
{
    printf("Usage: hanoi [<No Of Tiles>] [a]\n");
    printf("The 'a' option causes the tower to be solved automatically\n");
    exit();
}

main();
