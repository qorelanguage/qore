#!/usr/bin/env qore

%requires ncurses

%no-global-vars
%exec-class ncurses_test

Panel::myBox($y, $x, $lines, $cols)
{
    if ($y + $lines >= $.getLines())
	$lines = $.getLines() - $y - 1;

    if ($x + $cols >= $.getColumns())
	$cols = $.getColumns() - $x - 1;

    # draw top line
    $.mvhline($y, $x + 1, ACS_HLINE(), $cols - 1);
    # draw corners
    $.mvaddch($y, $x, ACS_ULCORNER());
    $.mvaddch($y, $x + $cols, ACS_URCORNER());
    $.mvaddch($y + $lines, $x, ACS_LLCORNER());
    $.mvaddch($y + $lines, $x + $cols, ACS_LRCORNER());
    # draw sides
    $.mvvline($y + 1, $x, ACS_VLINE(), $lines - 1);
    $.mvvline($y + 1, $x + $cols, ACS_VLINE(), $lines - 1);
    # draw bottom line
    $.mvhline($y + $lines, $x + 1, ACS_HLINE(), $cols - 1);
}

Panel::decorate($title, $color1, $color2)
{
    # set inverse colors for title
    $.setBackgroundColor($color1);
    $.setForegroundColor($color2);
    $.mvaddstr(0, 1, $title);
    $.setBackgroundColor(COLOR_BLACK);
    $.setForegroundColor($color1);
    $.myBox(1, 0, $.getLines() - 1, $.getColumns());
}

class ncurses_test {
    private thread_test()
    {
	my $y = rand() % (getLines() - 8);
	my $x = rand() % (getColumns() - 40);
	my $w = new Panel(8, 40, $y, $x);
	$w.decorate(sprintf("Thread Test (TID %d)", gettid()), COLOR_CYAN, COLOR_WHITE);
	# create subpanel for scrolling
	my $s = new Panel(5, 38, $y+2, $x+1);
	$s.scrollok(True);
	$s.setscrreg(0, 5);
	$s.setForegroundColor(COLOR_WHITE);
	# put the subpanel on the bottom
	$s.bottom();
	# put the parent panel below
	$w.bottom();
	# put the application panel back on the bottom
	$.w.bottom();
	# refresh everything
	$w.refresh();

	my $i = 0;
	while (!$.stop_test)
	{
	    if ($i)
		$s.addch("\n");
	    $s.printw("iteration %d", $i++);
	    $s.refresh();
	    $.tm.lock();
	    $.tc.wait($.tm, 1);
	    $.tm.unlock();
	}
	$.t.dec();
    }

    private menu()
    {
	$.tm = new Mutex();
	$.t  = new Counter();
	$.tc = new Condition();

	my $x = new Panel(9, 40, 5, 10);
	$x.decorate("Main Menu", COLOR_BLUE, COLOR_WHITE);
	$x.setForegroundColor(COLOR_WHITE);
	$x.mvaddstr(2, 1, "M: Move Window Test");
	$x.mvaddstr(3, 1, "T: Start Thread Test Window");
	$x.mvaddstr(4, 1, "K: Kill All Test Threads");
	$x.mvaddstr(5, 1, "Q: Quit");
	$x.mvaddstr(7, 1, "Enter your choice: ");
	$x.refresh();
	while ((my $c = $x.getch()) != ord('q') && $c != ord('Q'))
	{
	    $c = tolower(chr($c));
	    #$x.printw("c=%d c=%n", $c, chr($c));
	    switch ($c)
	    {
		case 'm':
		$.move_test();
		break;

		case 't':
		$.t.inc();
		background $.thread_test();
	        break;

 		case 'k':
		$.stop_test = True;
		$.tc.broadcast();
		$.t.waitForZero();
		$.stop_test = False;
	    }
	}

	$.stop_test = True;
	$.tc.broadcast();
	$.t.waitForZero();
    }
    
    private move_test()
    {
	my $y = new Panel(8, 50, 7, 15);
	$y.decorate("Move Test", COLOR_YELLOW, COLOR_BLACK);
	$y.setForegroundColor(COLOR_WHITE);
	$y.mvaddstr(2, 1, "move=arrow keys or mouse, q=quit: ");
	$y.refresh();
    
	while ((my $c = $y.getch()) != ord("q"))
	{
	    #$y.printw("y=%d, x=%d\n", $y.getBegY(), $y.getBegX());
	    my $rc;
	    switch ($c)
	    {
		case KEY_LEFT:
		$rc = $y.movePanel($y.getBegY(), $y.getBegX() - 1);
		break;
		
		case KEY_RIGHT:
		$rc = $y.movePanel($y.getBegY(), $y.getBegX() + 1);
		break;
		
		case KEY_UP:
		$rc = $y.movePanel($y.getBegY() - 1, $y.getBegX());
		break;
		
		case KEY_DOWN:
		$rc = $y.movePanel($y.getBegY() + 1, $y.getBegX());
		break;
		
		case KEY_MOUSE:
		my $event = getmouse();
		$rc = $y.movePanel($event.y, $event.x);
		break;
		
	      default:
		continue;
	    }
	    #$y.printw("move() = %d\n", $rc);
	    $y.refresh();
	}
    }
    
    status()
    {
	my $w = new Panel(1, getColumns(), getLines() - 1, 0);
	my $i;
	$w.setForegroundColor(COLOR_CYAN);
	while (!$.stop)
	{
	    my $str = sprintf("status thread TID: %d, PID: %d, Qore version: %s, running threads: %2d ..... Qore NCurses Test Script by David Nichols", gettid(), getpid(), Qore::VersionString, num_threads());
	    my $len = strlen($str);
	    $w.mvaddstr(0, 0, substr($str, $i));
	    while ($w.getX() < ($w.getColumns() - 1))
		$w.printw(" ..... %s", $str);
	    $w.addch(ord(" "));
	    #$w.mvaddstr(0, 0, "hello");
	    $w.refresh();
	    $i++;
	    if ($i == $len)
		$i = 0;
	    usleep(200000);
	}
	$.c.dec();
    }
    
    constructor()
    {
	srand(now());

	initscr();
	# enable all mouse events
	mousemask(ALL_MOUSE_EVENTS);
	
	# create a panel covering the entire screen except the bottom line
	$.w = new Panel(getLines() - 1, getColumns(), 0, 0);
	$.w.decorate("Qore NCurses Test Program", COLOR_RED, COLOR_WHITE);

	$.c = new Counter();
	$.c.inc();
	background $.status();

	$.menu();
	
	$.w.setColor(COLOR_WHITE, COLOR_BLUE);
	my $l = $.w.getLines();
	$.w.mvaddstr($l - 3, 1, "push any key to exit: ");
	$.w.refresh();
	$.w.getch();
	$.stop = 1;
	$.c.waitForZero();
    }
}
