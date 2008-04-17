#!/usr/bin/env qore

# This is basically a direct port of the QT tutorial to Qore 
# using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt
%requires opengl

# this is an object-oriented program, the application class is "hellogl"
%exec-class hellogl
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

const NumSectors = 200;

class GLWidget inherits QGLWidget
{
    private $.object, $.xRot, $.yRot, $.zRot, $.lastPos, $.trolltechGreen,
            $.trolltechPurple;

    constructor($parent) : QGLWidget($parent)
    {
	$.createSignal("xRotationChanged(int)");
	$.createSignal("yRotationChanged(int)");
	$.createSignal("zRotationChanged(int)");

	$.object = 0;
	$.xRot = 0;
	$.yRot = 0;
	$.zRot = 0;

	$.trolltechGreen = QColor_fromCmykF(0.40, 0.0, 1.0, 0.0);
	$.trolltechPurple = QColor_fromCmykF(0.39, 0.39, 0.0, 0.0);
    }

    destructor()
    {
	$.makeCurrent();
	glDeleteLists($.object, 1);
    }

    minimumSizeHint()
    {
	return new QSize(50, 50);
    }

    sizeHint()
    {
	return new QSize(400, 400);
    }

    setXRotation($angle)
    {
	$.normalizeAngle(\$angle);
	if ($angle != $.xRot) {
	    $.xRot = $angle;
	    $.emit("xRotationChanged(int)", $angle);
	    $.updateGL();
	}
    }
    
    setYRotation($angle)
    {
	$.normalizeAngle(\$angle);
	if ($angle != $.yRot) {
	    $.yRot = $angle;
	    $.emit("yRotationChanged(int)", $angle);
	    $.updateGL();
	}
    }

    setZRotation($angle)
    {
	$.normalizeAngle(\$angle);
	if ($angle != $.zRot) {
	    $.zRot = $angle;
	    $.emit("zRotationChanged(int)", $angle);
	    $.updateGL();
	}
    }

    initializeGL()
    {
	$.qglClearColor($.trolltechPurple.dark());
	$.object = $.makeObject();
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    }

    paintGL()
    {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -10.0);
	glRotated($.xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated($.yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated($.zRot / 16.0, 0.0, 0.0, 1.0);
	glCallList($.object);
    }

    resizeGL($width, $height)
    {
	my $side = min($width, $height);
	glViewport(($width - $side) / 2, ($height - $side) / 2, $side, $side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
    }

    mousePressEvent($event)
    {
	$.lastPos = $event.pos();
    }

    mouseMoveEvent($event)
    {
	my $dx = $event.x() - $.lastPos.x();
	my $dy = $event.y
	    () - $.lastPos.y
	    ();
	
	if ($event.buttons() & Qt::LeftButton) {
	    $.setXRotation($.xRot + 8 * $dy);
	    $.setYRotation($.yRot + 8 * $dx);
	} else if ($event.buttons() & Qt::RightButton) {
	    $.setXRotation($.xRot + 8 * $dy);
	    $.setZRotation($.zRot + 8 * $dx);
	}
	$.lastPos = $event.pos();
    }

    makeObject()
    {
	my $list = glGenLists(1);
	glNewList($list, GL_COMPILE);

	glBegin(GL_QUADS);

	my $x1 =  0.06;
	my $y1 = -0.14;
	my $x2 =  0.14;
	my $y2 = -0.06;
	my $x3 =  0.08;
	my $y3 =  0.00;
	my $x4 =  0.30;
	my $y4 =  0.22;

	$.quad($x1, $y1, $x2, $y2, $y2, $x2, $y1, $x1);
	$.quad($x3, $y3, $x4, $y4, $y4, $x4, $y3, $x3);

	$.extrude($x1, $y1, $x2, $y2);
	$.extrude($x2, $y2, $y2, $x2);
	$.extrude($y2, $x2, $y1, $x1);
	$.extrude($y1, $x1, $x1, $y1);
	$.extrude($x3, $y3, $x4, $y4);
	$.extrude($x4, $y4, $y4, $x4);
	$.extrude($y4, $x4, $y3, $x3);

	for (my $i = 0; $i < NumSectors; ++$i) {
	    my $angle1 = ($i * 2 * M_PI) / NumSectors;
	    my $x5 = 0.30 * sin($angle1);
	    my $y5 = 0.30 * cos($angle1);
	    my $x6 = 0.20 * sin($angle1);
	    my $y6 = 0.20 * cos($angle1);

	    my $angle2 = (($i + 1) * 2 * M_PI) / NumSectors;
	    my $x7 = 0.20 * sin($angle2);
	    my $y7 = 0.20 * cos($angle2);
	    my $x8 = 0.30 * sin($angle2);
	    my $y8 = 0.30 * cos($angle2);

	    $.quad($x5, $y5, $x6, $y6, $x7, $y7, $x8, $y8);

	    $.extrude($x6, $y6, $x7, $y7);
	    $.extrude($x8, $y8, $x5, $y5);
	}

	glEnd();

	glEndList();
	return $list;
    }

    quad($x1, $y1, $x2, $y2, $x3, $y3, $x4, $y4)
    {
	$.qglColor($.trolltechGreen);

	glVertex3d($x1, $y1, -0.05);
	glVertex3d($x2, $y2, -0.05);
	glVertex3d($x3, $y3, -0.05);
	glVertex3d($x4, $y4, -0.05);

	glVertex3d($x4, $y4, +0.05);
	glVertex3d($x3, $y3, +0.05);
	glVertex3d($x2, $y2, +0.05);
	glVertex3d($x1, $y1, +0.05);
    }

    extrude($x1, $y1, $x2, $y2)
    {
	$.qglColor($.trolltechGreen.dark(250 + int(100 * $x1)));

	glVertex3d($x1, $y1,  0.05);
	glVertex3d($x2, $y2,  0.05);
	glVertex3d($x2, $y2, -0.05);
	glVertex3d($x1, $y1, -0.05);
    }

    normalizeAngle($angle)
    {
	while ($angle < 0)
	    $angle += 360 * 16;
	while ($angle > 360 * 16)
	    $angle -= 360 * 16;
    }
}

class Window inherits QWidget
{
    private $.glWidget, $.xSlider, $.ySlider, $.zSlider;

    constructor()
    {
	$.glWidget = new GLWidget();

	$.xSlider = $.createSlider();
	$.ySlider = $.createSlider();
	$.zSlider = $.createSlider();

	$.glWidget.connect($.xSlider, SIGNAL("valueChanged(int)"), SLOT("setXRotation(int)"));
	$.xSlider.connect($.glWidget, SIGNAL("xRotationChanged(int)"), SLOT("setValue(int)"));
	$.glWidget.connect($.ySlider, SIGNAL("valueChanged(int)"), SLOT("setYRotation(int)"));
	$.ySlider.connect($.glWidget, SIGNAL("yRotationChanged(int)"), SLOT("setValue(int)"));
	$.glWidget.connect($.zSlider, SIGNAL("valueChanged(int)"), SLOT("setZRotation(int)"));
	$.zSlider.connect($.glWidget, SIGNAL("zRotationChanged(int)"), SLOT("setValue(int)"));

	my $mainLayout = new QHBoxLayout();
	$mainLayout.addWidget($.glWidget);
	$mainLayout.addWidget($.xSlider);
	$mainLayout.addWidget($.ySlider);
	$mainLayout.addWidget($.zSlider);
	$.setLayout($mainLayout);

	$.xSlider.setValue(180 * 16);
	$.ySlider.setValue(0);
	$.zSlider.setValue(274 * 16);
	$.setWindowTitle(TR("Hello GL"));
    }

    createSlider()
    {
	my $slider = new QSlider(Qt::Vertical);
	$slider.setRange(0, 360 * 16);
	$slider.setSingleStep(16);
	$slider.setPageStep(15 * 16);
	$slider.setTickInterval(15 * 16);
	$slider.setTickPosition(QSlider::TicksRight);
	return $slider;
    }
}

class hellogl inherits QApplication 
{
    constructor() {
        my $window = new Window();
        $window.show();
        $.exec();
    }
}
