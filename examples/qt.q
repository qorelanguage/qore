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
    private $.currentAngle, $.currentForce, $.timerCount, $.autoShootTimer, $.shootAngle, $.shootForce;

    constructor($parent) : QWidget($parent)
    {
	$.createSignal("angleChanged(int)");
	$.createSignal("forceChanged(int)");
        $.currentAngle = 45;
	$.currentForce = 0;

	$.timerCount = 0;
	$.autoShootTimer = new QTimer($self);
	$.connect($.autoShootTimer, SIGNAL("timeout()"), SLOT("moveShot()"));
	$.shootAngle = 0;
	$.shootForce = 0;

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
	$.update($.cannonRect());
	$.emit("angleChanged(int)", $.currentAngle);
    }

    setForce($force)
    {
	if ($force < 0)
	    $force = 0;
	if ($.currentForce == $force)
	    return;
	$.currentForce = $force;
	$.emit("forceChanged(int)", $.currentForce);
    }

    paintEvent()
    {
	my $painter = new QPainter($self);
	$.paintCannon($painter);
	if ($.autoShootTimer.isActive())
	    $.paintShot($painter);
    }

    paintCannon($painter)
    {
	$painter.setPen(Qt::NoPen);
	my $brush = new QBrush(Qt::SolidPattern);
	$brush.setColor(Qt::blue);
	$painter.setBrush($brush);

	$painter.save();
	$painter.translate(0, $.height());
	$painter.drawPie(new QRect(-35, -35, 70, 70), 0, 90 * 16);
	$painter.rotate(-$.currentAngle);
	$painter.drawRect($barrelRect);
	$painter.restore();
    }

    paintShot($painter)
    {
	$painter.setPen(Qt::NoPen);
	my $brush = new QBrush(Qt::SolidPattern);
	$brush.setColor(Qt::black);
	$painter.setBrush($brush);
	$painter.drawRect($.shotRect());
    }

    cannonRect()
    {
	my $result = new QRect(0, 0, 50, 50);
	$result.moveBottomLeft($.rect().bottomLeft());
	return $result;
    }

    shoot()
    {
	if ($.autoShootTimer.isActive())
	    return;
	$.timerCount = 0;
	$.shootAngle = $.currentAngle;
	$.shootForce = $.currentForce;
	$.autoShootTimer.start(5);
    }

    moveShot()
    {
	my $region = $.shotRect();
	++$.timerCount;
	
	my $shotR = $.shotRect();
	
	if ($shotR.x() > $.width() || $shotR.y
	    () > $.height()) {
	    $.autoShootTimer.stop();
	} else {
	    $region = $region.united($shotR);
	}
	$.update($region);
    }

    shotRect()
    {
	my $gravity = 4.0;

	my $time = $.timerCount / 20.0;
	my $velocity = $.shootForce;
	my $radians = $.shootAngle * 3.14159265 / 180;

	my $velx = $velocity * cos($radians);
	my $vely = $velocity * sin($radians);
	my $x0 = ($barrelRect.right() + 5) * cos($radians);
	my $y0 = ($barrelRect.right() + 5) * sin($radians);
	my $x = $x0 + $velx * $time;
	my $y = $y0 + $vely * $time - 0.5 * $gravity * $time * $time;

	my $result = new QRect(0, 0, 6, 6);
	$result.moveCenter(new QPoint(qRound($x), $.height() - 1 - qRound($y)));
	return $result;
    }
}

class MyWidget inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $quit = new QPushButton(TR("&Quit"));
	$quit.setFont(new QFont("Times", 18, QFont::Bold));

	QObject_connect($quit, SIGNAL("clicked()"), QAPP(), SLOT("quit()"));

	my $angle = new LCDRange();
	$angle.setRange(5, 70);

	my $cannonField = new CannonField();

	QObject_connect($angle,       SIGNAL("valueChanged(int)"), $cannonField, SLOT("setAngle(int)"));
	QObject_connect($cannonField, SIGNAL("angleChanged(int)"), $angle,       SLOT("setValue(int)"));

	my $force = new LCDRange();
	$force.setRange(10, 50);

	$cannonField.connect($force, SIGNAL("valueChanged(int)"), SLOT("setForce(int)"));
	$force.connect($cannonField, SIGNAL("forceChanged(int)"), SLOT("setValue(int)"));
	
	my $shoot = new QPushButton(TR("&Shoot"));
	$shoot.setFont(new QFont("Times", 18, QFont::Bold));

	$cannonField.connect($shoot, SIGNAL("clicked()"), SLOT("shoot()"));
	my $topLayout = new QHBoxLayout();
	$topLayout.addWidget($shoot);
	$topLayout.addStretch(1);

	my $leftLayout = new QVBoxLayout();
	$leftLayout.addWidget($angle);
	$leftLayout.addWidget($force);

	my $gridLayout = new QGridLayout();
	$gridLayout.addWidget($quit, 0, 0);
	$gridLayout.addLayout($topLayout, 0, 1);
	$gridLayout.addLayout($leftLayout, 1, 0);
	$gridLayout.addWidget($cannonField, 1, 1, 2, 1);
	$gridLayout.setColumnStretch(1, 10);
	$.setLayout($gridLayout);

	$angle.setValue(60);
	$force.setValue(25);
	$angle.setFocus();
    }
}

class qt_example inherits QApplication 
{
    constructor() {

	our $barrelRect = new QRect(30, -5, 20, 10);

	my $widget = new MyWidget();
	$widget.setGeometry(100, 100, 500, 355);
	$widget.show();

	$.exec();
    }
}
