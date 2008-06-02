#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "digitalclock" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program, the application class is "digital_clock_example"
%exec-class digital_clock_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

class DigitalClock inherits QLCDNumber
{
    constructor($parent) : QLCDNumber($parent)
    {
	$.setSegmentStyle(Filled);
	
	my $timer = new QTimer($self);
	$.connect($timer, SIGNAL("timeout()"), SLOT("showTime()"));
	$timer.start(1000);
	
	$.showTime();
	
	$.setWindowTitle(TR("Digital Clock"));
	$.resize(150, 60);
    }
    
    showTime()
    {
	my $time = now();
	my $text = format_date("HH:mm", $time);
	if ((get_seconds($time) % 2) == 0)
	    splice $text, 2, 1, ' ';
	$.display($text);
    }
}

class digital_clock_example inherits QApplication
{
    constructor()
    {
	my $clock = new DigitalClock();
	$clock.show();
	$.exec();
    }
}
