#!/usr/bin/env qore

# This is basically a direct port of a QT example program to Qore 
# using Qore's "qt-opengl" module.  

# Note that Qore's "qt-opengl" module requires QT 4.3 or above with OpenGL support 

# use the "qt-opengl" module (automatically loads the "qt-gui" and "opengl" modules)
%requires qt-opengl

# this is an object-oriented program, the application class is "two_d_painting"
%exec-class two_d_painting
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

class GLWidget inherits QGLWidget
{
    private $.helper, $.elapsed;

    constructor($helper, $parent) : QGLWidget(new QGLFormat(QGL::SampleBuffers), $parent)
    {
	$.helper = $helper;
	$.elapsed = 0;
	$.setFixedSize(200, 200);
    }

    animate()
    {
	$.elapsed = ($.elapsed + $.sender().interval()) % 1000;
	$.repaint();
    }

    paintEvent($event)
    {
	my $painter = new QPainter();
	$painter.begin($self);
	$painter.setRenderHint(QPainter::Antialiasing);
	$.helper.paint($painter, $event, $.elapsed);
	$painter.end();
    }
}

class Helper
{
    private $.background, $.circleBrush, $.textFont,
            $.circlePen, $.textPen;

    constructor()
    {
	$.textFont = new QFont();

	my $gradient = new QLinearGradient(new QPointF(50, -20), new QPointF(80, 20));
	$gradient.setColorAt(0.0, Qt::white);
	$gradient.setColorAt(1.0, new QColor(0xa6, 0xce, 0x39));

	$.background = new QBrush(new QColor(64, 32, 64));
	$.circleBrush = new QBrush($gradient);
	$.circlePen = new QPen(Qt::black);
	$.circlePen.setWidth(1);
	$.textPen = new QPen(Qt::white);
	$.textFont.setPixelSize(50);
    }

    paint($painter, $event, $elapsed)
    {	
	$painter.fillRect($event.rect(), $.background);
	$painter.translate(100, 100);
	
	$painter.save();
	$painter.setBrush($.circleBrush);
	$painter.setPen($.circlePen);
	$painter.rotate($elapsed * 0.030);

	my $r = $elapsed/1000.0;
	my $n = 30;

	for (my $i = 0; $i < $n; ++$i) {
	    $painter.rotate(30);
	    my $radius = 0 + 120.0*(($i+$r)/$n);
	    my $circleRadius = 1 + (($i+$r)/$n)*20;
	    $painter.drawEllipse(new QRectF($radius, -$circleRadius,
					    $circleRadius*2, $circleRadius*2));
	}
	$painter.restore();
	
	$painter.setPen($.textPen);
	$painter.setFont($.textFont);
	$painter.drawText(new QRect(-50, -50, 100, 100), Qt::AlignCenter, "Qt");
    }
}

class Widget inherits QWidget
{
    private $.helper, $.elapsed;

    constructor($helper, $parent) : QWidget($parent)
    {
	$.helper = $helper;
	$.elapsed = 0;
	$.setFixedSize(200, 200);
    }

    animate()
    {
	$.elapsed = ($.elapsed + $.sender().interval()) % 1000;
	$.repaint();
    }

    paintEvent($event)
    {
	my $painter = new QPainter();
	$painter.begin($self);
	$painter.setRenderHint(QPainter::Antialiasing);
	$.helper.paint($painter, $event, $.elapsed);
	$painter.end();
    }
}

class Window inherits QWidget
{
    private $.helper;

    constructor()
    {
	$.helper = new Helper();

	my $native = new Widget($.helper, $self);
	my $openGL = new GLWidget($.helper, $self);
	my $nativeLabel = new QLabel(TR("Native"));
	$nativeLabel.setAlignment(Qt::AlignHCenter);
	my $openGLLabel = new QLabel(TR("OpenGL"));
	$openGLLabel.setAlignment(Qt::AlignHCenter);

	my $layout = new QGridLayout();
	$layout.addWidget($native, 0, 0);
	$layout.addWidget($openGL, 0, 1);
	$layout.addWidget($nativeLabel, 1, 0);
	$layout.addWidget($openGLLabel, 1, 1);
	$.setLayout($layout);

	my $timer = new QTimer($self);
	$native.connect($timer, SIGNAL("timeout()"), SLOT("animate()"));
	$openGL.connect($timer, SIGNAL("timeout()"), SLOT("animate()"));
	$timer.start(50);

	$.setWindowTitle(TR("2D Painting on Native and OpenGL Widgets"));
    }
}

class two_d_painting inherits QApplication 
{
    constructor() {
	my $window = new Window();
	$window.show();
        $.exec();
    }
}
