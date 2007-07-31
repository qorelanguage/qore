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
	$.setFocusProxy($.slider);
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

    setRange($minValue, $maxValue)
    {
	if ($minValue < 0 || $maxValue > 99 || $minValue > $maxValue) {
	    qWarning("LCDRange::setRange(%d, %d)
\tRange must be 0..99
\tand minValue must not be greater than maxValue",
		     $minValue, $maxValue);
	    return;
	}
	$.slider.setRange($minValue, $maxValue);
    }
}

class CannonField inherits QWidget
{
    private $.currentAngle;

    constructor($parent) : QWidget($parent)
    {
	$.createSignal("angleChanged(int)");
        $.currentAngle = 45;
	$.setPalette(new QPalette(new QColor(250, 250, 200)));
	$.setAutoFillBackground(True);
    }

    setAngle($angle)
    {
	#printf("CannonField::setAngle(%N) called\n", $angle);
	if ($angle < 5)
	    $angle = 5;
	else if ($angle > 70)
	    $angle = 70;
	if ($.currentAngle == $angle)
	    return;
	$.currentAngle = $angle;
	$.update();
	$.emit("angleChanged(int)", $.currentAngle);
    }

    paintEvent()
    {
	my $painter = new QPainter($self);
	$painter.drawText(20, 20, TR("Angle = ") + $.currentAngle);
    }
}

class MyWidget inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $quit = new QPushButton(TR("Quit"));
	$quit.setFont(new QFont("Times", 18, QFont::Bold));

	QObject_connect($quit, SIGNAL("clicked()"), QAPP(), SLOT("quit()"));

	my $angle = new LCDRange();
	$angle.setRange(5, 70);

	my $cannonField = new CannonField();

	QObject_connect($angle,       SIGNAL("valueChanged(int)"), $cannonField, SLOT("setAngle(int)"));
	QObject_connect($cannonField, SIGNAL("angleChanged(int)"), $angle,       SLOT("setValue(int)"));

	my $gridLayout = new QGridLayout();
	$gridLayout.addWidget($quit, 0, 0);
	$gridLayout.addWidget($angle, 1, 0);
	$gridLayout.addWidget($cannonField, 1, 1, 2, 1);
	$gridLayout.setColumnStretch(1, 10);
	$.setLayout($gridLayout);

	$angle.setValue(60);
	$angle.setFocus();
    }
}

class qt_example inherits QApplication 
{
    constructor() {

	my $widget = new MyWidget();
	$widget.setGeometry(100, 100, 500, 355);
	$widget.show();

	$.exec();
    }
}
