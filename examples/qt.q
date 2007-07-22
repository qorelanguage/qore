#!/usr/bin/env qore

%requires qt

%exec-class qt_example
%require-our
%enable-all-warnings

class MyWidget inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $quit = new QPushButton(TR("Quit"));
	$quit.setFont(new QFont("Times", 18, QFont::Bold));

	my $lcd = new QLCDNumber(2);
	$lcd.setSegmentStyle(QLCDNumber::Filled);

	my $slider = new QSlider(Qt::Horizontal);
	$slider.setRange(0, 99);
	$slider.setValue(0);

	QObject_connect($quit, SIGNAL("clicked()"), QAPP(), SLOT("quit()"));
	QObject_connect($slider, SIGNAL("valueChanged(int)"), $lcd, SLOT("display(int)"));

	my $layout = new QVBoxLayout();
	$layout.addWidget($quit);
	$layout.addWidget($lcd);
	$layout.addWidget($slider);
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
