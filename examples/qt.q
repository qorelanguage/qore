#!/usr/bin/env qore

%requires qt

%exec-class qt_example
%require-our
%enable-all-warnings

class LCDRange inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $lcd = new QLCDNumber(2);
	$lcd.setSegmentStyle(QLCDNumber::Filled);

	#  create dynamic signals
	$.createSignal("valueChanged(int)");

	$.slider = new QSlider(Qt::Horizontal);
	$.slider.setRange(0, 99);
	$.slider.setValue(0);
	QObject_connect($.slider, SIGNAL("valueChanged(int)"), $lcd, SLOT("display(int)"));
	QObject_connect($.slider, SIGNAL("valueChanged(int)"), $self, SIGNAL("valueChanged(int)"));

	#$lcd.connect($.slider, SIGNAL("valueChanged(int)"), SLOT("display(int)"));
	#$.connect($.slider, SIGNAL("valueChanged(int)"), SIGNAL("valueChanged(int)"));

	#$.connect($.slider, SIGNAL("valueChanged(int)"), SLOT("printValue(int)"));
	#$.connect($self, SIGNAL("valueChanged(int)"), SLOT("testValueChanged(int)"));

	my $layout = new QVBoxLayout();
	$layout.addWidget($lcd);
	$layout.addWidget($.slider);
	$.setLayout($layout);
    }

    value()
    {
	return $.slider.value();
    }

    setValue($val)
    {
	$.slider.setValue($val);
    }

    printValue($val)
    {
	printf("printValue(%n) called\n", $val); flush();
    }

    testValueChanged($val)
    {
	printf("testValueChanged(%n) called\n", $val); flush();
    }
}

class MyWidget inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $quit = new QPushButton(TR("Quit"));
	$quit.setFont(new QFont("Times", 18, QFont::Bold));

	QObject_connect($quit, SIGNAL("clicked()"), QAPP(), SLOT("quit()"));

	my $grid = new QGridLayout();
	my $previousRange;
	for (my $row = 0; $row < 3; ++$row) {
	    for (my $column = 0; $column < 3; ++$column) {
		my $lcdRange = new LCDRange();
		$grid.addWidget($lcdRange, $row, $column);
		if (exists $previousRange)
		    QObject_connect($lcdRange, SIGNAL("valueChanged(int)"), $previousRange, SLOT("setValue(int)"));
		    #$previousRange.connect($lcdRange, SIGNAL("valueChanged(int)"), SLOT("setValue(int)"));
		$previousRange = $lcdRange;
	    }
	}
	my $layout = new QVBoxLayout();
	$layout.addWidget($quit);
	$layout.addLayout($grid);
	$.setLayout($layout);
    }
}

class qt_example inherits QApplication 
{
    constructor() {

	my $widget = new MyWidget();
	$widget.show();

	$.exec();
    }
}
