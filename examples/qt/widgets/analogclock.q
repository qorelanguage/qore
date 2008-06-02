#!/usr/bin/env qore

# This is basically a direct port of the QT widget example
# "analogclock" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt-gui" module
%requires qt-gui

# this is an object-oriented program, the application class is "analog_clock_example"
%exec-class analog_clock_example
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

class AnalogClock inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $timer = new QTimer($self);
	$.connect($timer, SIGNAL("timeout()"), SLOT("update()"));
	$timer.start(1000);
	
	$.setWindowTitle(TR("Analog Clock"));
	$.resize(200, 200);
    }

    paintEvent($event)
    {	
	my $side = min($.width(), $.height());
	my $time = now();

	my $painter = new QPainter($self);
	$painter.setRenderHint(QPainter::Antialiasing);
	$painter.translate($.width() / 2, $.height() / 2);
	$painter.scale($side / 200.0, $side / 200.0);
	
	$painter.setPen(Qt::NoPen);
	$painter.setBrush($hourColor);

	$painter.save();
	$painter.rotate(30.0 * ((get_hours($time) + get_minutes($time) / 60.0)));
	$painter.drawConvexPolygon($hourHand, 3);
	$painter.restore();
	
	$painter.setPen($hourColor);
	
	for (my $i = 0; $i < 12; ++$i) {
	    $painter.drawLine(88, 0, 96, 0);
	    $painter.rotate(30.0);
	}
	
	$painter.setPen(Qt::NoPen);
	$painter.setBrush($minuteColor);
	
	$painter.save();
	$painter.rotate(6.0 * (get_minutes($time) + get_seconds($time) / 60.0));
	$painter.drawConvexPolygon($minuteHand, 3);
	$painter.restore();
	
	$painter.setPen($minuteColor);
	
	for (my $j = 0; $j < 60; ++$j) {
	    if (($j % 5) != 0)
		$painter.drawLine(92, 0, 96, 0);
	    $painter.rotate(6.0);
	}
    }
}

class analog_clock_example inherits QApplication 
{
    constructor()
    {
	# declare global variables
	our $hourHand   = new QPolygon((new QPoint(7, 8), 
					new QPoint(-7, 8), 
					new QPoint(0, -40)));
	our $minuteHand = new QPolygon((new QPoint(7, 8),
					new QPoint(-7, 8),
					new QPoint(0, -70)));

	our $hourColor   = new QColor(127, 0, 127);
	our $minuteColor = new QColor(0, 127, 127, 191);

	my $clock = new AnalogClock();
	$clock.show();
	$.exec();
    }
}
