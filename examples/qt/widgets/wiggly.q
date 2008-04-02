#!/usr/bin/env qore

# this is basically a direct port of the QT widget example
# "wiggly" to Qore using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt

# this is an object-oriented program; the application class is "wiggly_example"
%exec-class wiggly_example
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

const sineTable = ( 0, 38, 71, 92, 100, 92, 71, 38, 0, -38, -71, -92, -100, -92, -71, -38 );

class Dialog inherits QDialog
{
    constructor($parent) : QDialog($parent)
    {
	my $wigglyWidget = new WigglyWidget();
	my $lineEdit = new QLineEdit();

	my $layout = new QVBoxLayout();
	$layout.addWidget($wigglyWidget);
	$layout.addWidget($lineEdit);
	$.setLayout($layout);

	$wigglyWidget.connect($lineEdit, SIGNAL("textChanged(QString)"), SLOT("setText(QString)"));

	$lineEdit.setText(TR("Hello world!"));

	$.setWindowTitle(TR("Wiggly"));
	$.resize(360, 145);	
    }
}

class WigglyWidget inherits QWidget
{
    private $.timer, $.text, $.step;

    constructor($parent) : QWidget($parent)
    {
	$.setBackgroundRole(QPalette::Midlight);
	$.setAutoFillBackground(True);

	my $newFont = new QFont();
	$newFont.setPointSize($newFont.pointSize() + 20);
	$.setFont($newFont);

	$.step = 0;
	$.timer = new QBasicTimer();
	$.timer.start(60, $self);
    }

    setText($newText) { $.text = $newText; }

    private paintEvent($event)
    {
	my $metrics = new QFontMetrics($.font());
	my $x = ($.width() - $metrics.width($.text)) / 2;
	my $y = ($.height() + $metrics.ascent() - $metrics.descent()) / 2;
	my $color = new QColor();

	my $painter = new QPainter($self);
	for (my $i = 0; $i < elements $.text; ++$i) {
	    my $index = ($.step + $i) % 16;
	    $color.setHsv((15 - $index) * 16, 255, 191);
	    $painter.setPen($color);
	    $painter.drawText($x, $y - ((sineTable[$index] * $metrics.height()) / 400), $.text[$i]);
	    $x += $metrics.width($.text[$i]);
	}
    }

    private timerEvent($event)
    {
	if ($event.timerId() == $.timer.timerId()) {
	    ++$.step;
	    $.update();
	} else {
	    QWidget::$.timerEvent($event);
	}
    }
}

class wiggly_example inherits QApplication
{
    constructor()
    {
	my $dialog = new Dialog();
	$dialog.show();
	$.exec();
    }
}
