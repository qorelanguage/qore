#!/usr/bin/env qore

%requires qt

%exec-class qt_example
%require-our
%enable-all-warnings

class LCDRange inherits QWidget
{
    constructor($text, $parent) : QWidget($parent)
    {
	my $lcd = new QLCDNumber(2);
	$lcd.setSegmentStyle(QLCDNumber::Filled);

	#  create dynamic signals
	$.createSignal("valueChanged(int)");

	$.slider = new QSlider(Qt::Horizontal);
	$.slider.setRange(0, 99);
	$.slider.setValue(0);
	
	$.label = new QLabel();
	$.label.setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	QObject_connect($.slider, SIGNAL("valueChanged(int)"), $lcd, SLOT("display(int)"));
	QObject_connect($.slider, SIGNAL("valueChanged(int)"), $self, SIGNAL("valueChanged(int)"));

	#$lcd.connect($.slider, SIGNAL("valueChanged(int)"), SLOT("display(int)"));
	#$.connect($.slider, SIGNAL("valueChanged(int)"), SIGNAL("valueChanged(int)"));

	#$.connect($.slider, SIGNAL("valueChanged(int)"), SLOT("printValue(int)"));
	#$.connect($self, SIGNAL("valueChanged(int)"), SLOT("testValueChanged(int)"));

	my $layout = new QVBoxLayout();
	$layout.addWidget($lcd);
	$layout.addWidget($.slider);
	$layout.addWidget($.label);
	$.setLayout($layout);
	$.setFocusProxy($.slider);
	$.setText($text);
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

    text()
    {
	return $.label.text();
    }

    setText($text)
    {
	$.label.setText($text);
    }
}

class CannonField inherits QWidget
{
    private $.currentAngle, $.currentForce, $.timerCount, $.autoShootTimer, $.shootAngle, $.shootForce, $.target, $.gameEnded;

    constructor($parent) : QWidget($parent)
    {
	$.createSignal("angleChanged(int)");
	$.createSignal("forceChanged(int)");
	$.createSignal("hit()");
	$.createSignal("missed()");
	$.createSignal("canShoot(bool)");

        $.currentAngle = 45;
	$.currentForce = 0;

	$.timerCount = 0;
	$.autoShootTimer = new QTimer($self);
	$.connect($.autoShootTimer, SIGNAL("timeout()"), SLOT("moveShot()"));
	$.shootAngle = 0;
	$.shootForce = 0;

	$.target = new QPoint(0, 0);
	$.gameEnded = False;
	$.setPalette(new QPalette(new QColor(250, 250, 200)));
	$.setAutoFillBackground(True);
	$.newTarget();
    }

    angle()
    {
	return $.currentAngle;
    }

    force()
    {
	return $.currentForce;
    }

    gameOver()
    {
	return $.gameEnded;
    }

    newTarget()
    {
	if ($firstTime) {
	    $firstTime = False;
	    my $now = now();
	    qsrand($now - get_midnight($now));
	}
	$.target = new QPoint(200 + qrand() % 190, 10 + qrand() % 255);
	$.update();
    }

    setGameOver()
    {
	if ($.gameEnded)
	    return;
	if ($.isShooting())
	    $.autoShootTimer.stop();
	$.gameEnded = True;
	$.update();
    }

    restartGame()
    {
	if ($.isShooting())
	    $.autoShootTimer.stop();
	$.gameEnded = False;
	$.update();
	$.emit("canShoot(bool)", True);
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

	if ($.gameEnded) {
	    $painter.setPen(Qt::black);
	    $painter.setFont(new QFont("Countier", 48, QFont::Bold));
	    $painter.drawText($.rect(), Qt::AlignCenter, TR("Game Over"));
	}

	$.paintCannon($painter);
	if ($.isShooting())
	    $.paintShot($painter);
	if (!$.gameEnded)
	    $.paintTarget($painter);
    }

    paintTarget($painter)
    {
	$painter.setPen(Qt::black);
	$painter.setBrush(Qt::red);
	$painter.drawRect($.targetRect());
    }

    paintCannon($painter)
    {
	$painter.setPen(Qt::NoPen);
	$painter.setBrush(Qt::blue);

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
	$painter.setBrush(Qt::black);
	$painter.drawRect($.shotRect());
    }

    cannonRect()
    {
	my $result = new QRect(0, 0, 50, 50);
	$result.moveBottomLeft($.rect().bottomLeft());
	return $result;
    }

    isShooting() 
    {
	return $.autoShootTimer.isActive();
    }

    shoot()
    {
	if ($.isShooting())
	    return;
	$.timerCount = 0;
	$.shootAngle = $.currentAngle;
	$.shootForce = $.currentForce;
	$.autoShootTimer.start(5);
	$.emit("canShoot(bool)", False);
    }

    moveShot()
    {
	my $region = $.shotRect();
	++$.timerCount;
	
	my $shotR = $.shotRect();
	
	if ($shotR.intersects($.targetRect())) {
	    $.autoShootTimer.stop();
	    $.emit("hit()");
	    $.emit("canShoot(bool)", True);
	} else if ($shotR.x() > $.width() || $shotR.y
		   () > $.height()) {
	    $.autoShootTimer.stop();
	    $.emit("missed()");
	    $.emit("canShoot(bool)", True);
	} else
	    $region = $region.united($shotR);

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

    targetRect()
    {
	my $result = new QRect(0, 0, 20, 10);
	$result.moveCenter(new QPoint($.target.x(), $.height() - 1 - $.target.y
				      ()));
	return $result;
    }
}

class GameBoard inherits QWidget
{
    constructor($parent) : QWidget($parent)
    {
	my $quit = new QPushButton(TR("&Quit"));
	$quit.setFont(new QFont("Times", 18, QFont::Bold));

	QObject_connect($quit, SIGNAL("clicked()"), QAPP(), SLOT("quit()"));

	my $angle = new LCDRange(TR("ANGLE"));
	$angle.setRange(5, 70);

	my $force = new LCDRange(TR("FORCE"));
	$force.setRange(10, 50);

	$.cannonField = new CannonField();

	QObject_connect($angle,       SIGNAL("valueChanged(int)"), $.cannonField, SLOT("setAngle(int)"));
	QObject_connect($.cannonField, SIGNAL("angleChanged(int)"), $angle,       SLOT("setValue(int)"));

	$.cannonField.connect($force, SIGNAL("valueChanged(int)"), SLOT("setForce(int)"));
	$force.connect($.cannonField, SIGNAL("forceChanged(int)"), SLOT("setValue(int)"));
	
	$.connect($.cannonField, SIGNAL("hit()"),    SLOT("hit()"));
	$.connect($.cannonField, SIGNAL("missed()"), SLOT("missed()"));

	my $shoot = new QPushButton(TR("&Shoot"));
	$shoot.setFont(new QFont("Times", 18, QFont::Bold));

	$.connect($shoot, SIGNAL("clicked()"), SLOT("fire()"));
	$shoot.connect($.cannonField, SIGNAL("canShoot(bool)"), SLOT("setEnabled(bool)"));

	my $restart = new QPushButton(TR("&New Game"));
	$restart.setFont(new QFont("Times", 18, QFont::Bold));

	$.connect($restart, SIGNAL("clicked()"), SLOT("newGame()"));

	$.hits = new QLCDNumber(2);
	$.hits.setSegmentStyle(QLCDNumber::Filled);

	$.shotsLeft = new QLCDNumber(2);
	$.shotsLeft.setSegmentStyle(QLCDNumber::Filled);

	my $hitsLabel = new QLabel(TR("HITS"));
	my $shotsLeftLabel = new QLabel(TR("SHOTS LEFT"));

	my $topLayout = new QHBoxLayout();
	$topLayout.addWidget($shoot);
	$topLayout.addWidget($.hits);
	$topLayout.addWidget($hitsLabel);
	$topLayout.addWidget($.shotsLeft);
	$topLayout.addWidget($shotsLeftLabel);
	$topLayout.addStretch(1);
	$topLayout.addWidget($restart);

	my $leftLayout = new QVBoxLayout();
	$leftLayout.addWidget($angle);
	$leftLayout.addWidget($force);

	my $gridLayout = new QGridLayout();
	$gridLayout.addWidget($quit, 0, 0);
	$gridLayout.addLayout($topLayout, 0, 1);
	$gridLayout.addLayout($leftLayout, 1, 0);
	$gridLayout.addWidget($.cannonField, 1, 1, 2, 1);
	$gridLayout.setColumnStretch(1, 10);
	$.setLayout($gridLayout);

	$angle.setValue(60);
	$force.setValue(25);
	$angle.setFocus();

	$.newGame();
    }

    fire()
    {
	if ($.cannonField.gameOver() || $.cannonField.isShooting())
	    return;
	$.shotsLeft.display($.shotsLeft.intValue() - 1);
	$.cannonField.shoot();
    }

    hit()
    {
	$.hits.display($.hits.intValue() + 1);
	if ($.shotsLeft.intValue() == 0)
	    $.cannonField.setGameOver();
	else
	    $.cannonField.newTarget();
    }

    missed()
    {
	if ($.shotsLeft.intValue() == 0)
	    $.cannonField.setGameOver();
    }

    newGame()
    {
	$.shotsLeft.display(15);
	$.hits.display(0);
	$.cannonField.restartGame();
	$.cannonField.newTarget();
    }
}

class qt_example inherits QApplication 
{
    constructor() {

	our $firstTime = True;
	
	our $barrelRect = new QRect(30, -5, 20, 10);

	my $board = new GameBoard();
	$board.setGeometry(100, 100, 500, 355);
	$board.show();

	$.exec();
    }
}
