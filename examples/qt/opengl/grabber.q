#!/usr/bin/env qore

# This is basically a direct port of a QT example program to Qore
# using Qore's "qt-opengl" module.

# Note that Qore's "qt-opengl" module requires QT 4.3 or above with OpenGL support

# use the "qt-opengl" module (automatically loads the "qt" and "opengl" modules)
%requires qt-opengl

# this is an object-oriented program, the application class is "grabber"
%exec-class grabber
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

const lightPos = ( 5.0, 5.0, 10.0, 1.0 );
const reflectance1 = ( 0.8, 0.1, 0.0, 1.0 );
const reflectance2 = ( 0.0, 0.8, 0.2, 1.0 );
const reflectance3 = ( 0.2, 0.2, 1.0, 1.0 );

class GLWidget inherits QGLWidget
{
    private $.gear1, $.gear2, $.gear3, 
            $.xRot, $.yRot, $.zRot, $.gear1Rot, $.lastPos;

    constructor($parent) : QGLWidget($parent)
    {
	$.lastPos = new QPoint();

	# create signals
	$.createSignal("xRotationChanged(int)");
	$.createSignal("yRotationChanged(int)");
	$.createSignal("zRotationChanged(int)");

	$.gear1 = 0;
	$.gear2 = 0;
	$.gear3 = 0;
	$.xRot = 0;
	$.yRot = 0;
	$.zRot = 0;
	$.gear1Rot = 0;

	my $timer = new QTimer($self);
	$.connect($timer, SIGNAL("timeout()"), SLOT("advanceGears()"));
	$timer.start(20);
    }
    
    destructor()
    {
	$.makeCurrent();
	glDeleteLists($.gear1, 1);
	glDeleteLists($.gear2, 1);
	glDeleteLists($.gear3, 1);
    }
    
    xRotation() { return $.xRot; }
    yRotation() { return $.yRot; }
    zRotation() { return $.zRot; }

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
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	
	$.gear1 = $.makeGear(reflectance1, 1.0, 4.0, 1.0, 0.7, 20);
	$.gear2 = $.makeGear(reflectance2, 0.5, 2.0, 2.0, 0.7, 10);
	$.gear3 = $.makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 10);

	glEnable(GL_NORMALIZE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
    }

    paintGL()
    {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotated($.xRot / 16.0, 1.0, 0.0, 0.0);
	glRotated($.yRot / 16.0, 0.0, 1.0, 0.0);
	glRotated($.zRot / 16.0, 0.0, 0.0, 1.0);

	$.drawGear($.gear1, -3.0, -2.0, 0.0, $.gear1Rot / 16.0);
	$.drawGear($.gear2,  3.1, -2.0, 0.0, -2.0 * ($.gear1Rot / 16.0) - 9.0);

	glRotated(90.0, 1.0, 0.0, 0.0);
	$.drawGear($.gear3, -3.1, -1.8, -2.2,  2.0 * ($.gear1Rot / 16.0) - 2.0);

	glPopMatrix();
    }

    resizeGL($width, $height)
    {
	my $side = min($width, $height);
	glViewport(($width - $side) / 2, ($height - $side) / 2, $side, $side);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0,  1.0, -1.0, 1.0, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -40.0);
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

    advanceGears()
    {
	$.gear1Rot += 2 * 16;
	$.updateGL();
    }

    makeGear($reflectance, $innerRadius, $outerRadius, $thickness,
	     $toothSize, $toothCount)
    {
	my $list = glGenLists(1);
	glNewList($list, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, $reflectance);

	my $r0 = $innerRadius;
	my $r1 = $outerRadius - $toothSize / 2.0;
	my $r2 = $outerRadius + $toothSize / 2.0;
	my $delta = (2.0 * M_PI / $toothCount) / 4.0;
	my $z = $thickness / 2.0;
	my ($i, $j);

	glShadeModel(GL_FLAT);

	for ($i = 0; $i < 2; ++$i) {
	    my $sign = ($i == 0) ? 1.0 : -1.0;

	    glNormal3d(0.0, 0.0, $sign);

	    glBegin(GL_QUAD_STRIP);
	    for ($j = 0; $j <= $toothCount; ++$j) {
		my $angle = 2.0 * M_PI * $j / $toothCount;
		glVertex3d($r0 * cos($angle), $r0 * sin($angle), $sign * $z);
		glVertex3d($r1 * cos($angle), $r1 * sin($angle), $sign * $z);
		glVertex3d($r0 * cos($angle), $r0 * sin($angle), $sign * $z);
		glVertex3d($r1 * cos($angle + 3 * $delta), $r1 * sin($angle + 3 * $delta),
			   $sign * $z);
	    }
	    glEnd();

	    glBegin(GL_QUADS);
	    for ($j = 0; $j < $toothCount; ++$j) {
		my $angle = 2.0 * M_PI * $j / $toothCount;
		glVertex3d($r1 * cos($angle), $r1 * sin($angle), $sign * $z);
		glVertex3d($r2 * cos($angle + $delta), $r2 * sin($angle + $delta),
			   $sign * $z);
		glVertex3d($r2 * cos($angle + 2 * $delta), $r2 * sin($angle + 2 * $delta),
			   $sign * $z);
		glVertex3d($r1 * cos($angle + 3 * $delta), $r1 * sin($angle + 3 * $delta),
			   $sign * $z);
	    }
	    glEnd();
	}

	glBegin(GL_QUAD_STRIP);
	for ($i = 0; $i < $toothCount; ++$i) {
	    for ($j = 0; $j < 2; ++$j) {
		my $angle = 2.0 * M_PI * ($i + ($j / 2.0)) / $toothCount;
		my $s1 = $r1;
		my $s2 = $r2;
		if ($j == 1)
		    qSwap(\$s1, \$s2);

		glNormal3d(cos($angle), sin($angle), 0.0);
		glVertex3d($s1 * cos($angle), $s1 * sin($angle), $z);
		glVertex3d($s1 * cos($angle), $s1 * sin($angle), -$z);

		glNormal3d($s2 * sin($angle + $delta) - $s1 * sin($angle),
			   $s1 * cos($angle) - $s2 * cos($angle + $delta), 0.0);
		glVertex3d($s2 * cos($angle + $delta), $s2 * sin($angle + $delta), $z);
		glVertex3d($s2 * cos($angle + $delta), $s2 * sin($angle + $delta), -$z);
	    }
	}
	glVertex3d($r1, 0.0, $z);
	glVertex3d($r1, 0.0, -$z);
	glEnd();

	glShadeModel(GL_SMOOTH);

	glBegin(GL_QUAD_STRIP);
	for ($i = 0; $i <= $toothCount; ++$i) {
	    my $angle = $i * 2.0 * M_PI / $toothCount;
	    glNormal3d(-cos($angle), -sin($angle), 0.0);
	    glVertex3d($r0 * cos($angle), $r0 * sin($angle), $z);
	    glVertex3d($r0 * cos($angle), $r0 * sin($angle), -$z);
	}
	glEnd();

	glEndList();

	return $list;
    }

    drawGear($gear, $dx, $dy, $dz, $angle)
    {
	glPushMatrix();
	glTranslated($dx, $dy, $dz);
	glRotated($angle, 0.0, 0.0, 1.0);
	glCallList($gear);
	glPopMatrix();
    }

    normalizeAngle($angle)
    {
	while ($angle < 0)
	    $angle += 360 * 16;
	while ($angle > 360 * 16)
	    $angle -= 360 * 16;
    }
}

class MainWindow inherits QMainWindow
{
    private $.centralWidget, $.glWidgetArea, $.pixmapLabelArea,
    $.glWidget, $.pixmapLabel, $.xSlider, $.ySlider, 
    $.zSlider, $.fileMenu, $.helpMenu, $.grabFrameBufferAct, 
    $.renderIntoPixmapAct, $.clearPixmapAct, $.exitAct, 
    $.aboutAct, $.aboutQtAct;

    constructor()
    {
	$.centralWidget = new QWidget();
	$.setCentralWidget($.centralWidget);

	$.glWidget = new GLWidget();
	$.pixmapLabel = new QLabel();
	
	$.glWidgetArea = new QScrollArea();
	$.glWidgetArea.setWidget($.glWidget);
	$.glWidgetArea.setWidgetResizable(True);
	$.glWidgetArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	$.glWidgetArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	$.glWidgetArea.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	$.glWidgetArea.setMinimumSize(50, 50);

	$.pixmapLabelArea = new QScrollArea();
	$.pixmapLabelArea.setWidget($.pixmapLabel);
	$.pixmapLabelArea.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	$.pixmapLabelArea.setMinimumSize(50, 50);

	$.xSlider = $.createSlider(SIGNAL("xRotationChanged(int)"), SLOT("setXRotation(int)"));
	$.ySlider = $.createSlider(SIGNAL("yRotationChanged(int)"), SLOT("setYRotation(int)"));
	$.zSlider = $.createSlider(SIGNAL("zRotationChanged(int)"), SLOT("setZRotation(int)"));
        
	$.createActions();
	$.createMenus();

	my $centralLayout = new QGridLayout();
	$centralLayout.addWidget($.glWidgetArea, 0, 0);
	$centralLayout.addWidget($.pixmapLabelArea, 0, 1);
	$centralLayout.addWidget($.xSlider, 1, 0, 1, 2);
	$centralLayout.addWidget($.ySlider, 2, 0, 1, 2);
	$centralLayout.addWidget($.zSlider, 3, 0, 1, 2);
	$.centralWidget.setLayout($centralLayout);

	$.xSlider.setValue(15 * 16);
	$.ySlider.setValue(345 * 16);
	$.zSlider.setValue(0 * 16);

	$.setWindowTitle(TR("Grabber"));
	$.resize(400, 300);
    }

    renderIntoPixmap()
    {
	my $size = $.getSize();
	if ($size.isValid()) {
	    my $pixmap = $.glWidget.renderPixmap($size.width(), $size.height());
	    $.setPixmap($pixmap);
	}
    }

    grabFrameBuffer()
    {
	my $image = $.glWidget.grabFrameBuffer();
	$.setPixmap(QPixmap_fromImage($image));
    }

    clearPixmap()
    {
	$.setPixmap(new QPixmap());
    }

    about()
    {
	QMessageBox_about($self, TR("About Grabber"),
			  TR("The <b>Grabber</b> example demonstrates two approaches for "
			     "rendering OpenGL into a Qt pixmap."));
    }

    createActions()
    {
	$.renderIntoPixmapAct = new QAction(TR("&Render into Pixmap..."), $self);
	$.renderIntoPixmapAct.setShortcut(TR("Ctrl+R"));
	$.connect($.renderIntoPixmapAct, SIGNAL("triggered()"), SLOT("renderIntoPixmap()"));

	$.grabFrameBufferAct = new QAction(TR("&Grab Frame Buffer"), $self);
	$.grabFrameBufferAct.setShortcut(TR("Ctrl+G"));
	$.connect($.grabFrameBufferAct, SIGNAL("triggered()"), SLOT("grabFrameBuffer()"));

	$.clearPixmapAct = new QAction(TR("&Clear Pixmap"), $self);
	$.clearPixmapAct.setShortcut(TR("Ctrl+L"));
	$.connect($.clearPixmapAct, SIGNAL("triggered()"), SLOT("clearPixmap()"));

	$.exitAct = new QAction(TR("E&xit"), $self);
	$.exitAct.setShortcut(TR("Ctrl+Q"));
	$.connect($.exitAct, SIGNAL("triggered()"), SLOT("close()"));

	$.aboutAct = new QAction(TR("&About"), $self);
	$.connect($.aboutAct, SIGNAL("triggered()"), SLOT("about()"));

	$.aboutQtAct = new QAction(TR("About &Qt"), $self);
	QAPP().connect($.aboutQtAct, SIGNAL("triggered()"), SLOT("aboutQt()"));
    }

    createMenus()
    {
	$.fileMenu = $.menuBar().addMenu(TR("&File"));
	$.fileMenu.addAction($.renderIntoPixmapAct);
	$.fileMenu.addAction($.grabFrameBufferAct);
	$.fileMenu.addAction($.clearPixmapAct);
	$.fileMenu.addSeparator();
	$.fileMenu.addAction($.exitAct);

	$.helpMenu = $.menuBar().addMenu(TR("&Help"));
	$.helpMenu.addAction($.aboutAct);
	$.helpMenu.addAction($.aboutQtAct);
    }

    createSlider($changedSignal, $setterSlot)
    {
 	my $slider = new QSlider(Qt::Horizontal);
	$slider.setRange(0, 360 * 16);
	$slider.setSingleStep(16);
	$slider.setPageStep(15 * 16);
	$slider.setTickInterval(15 * 16);
	$slider.setTickPosition(QSlider::TicksRight);
	$.glWidget.connect($slider, SIGNAL("valueChanged(int)"), $setterSlot);
	$slider.connect($.glWidget, $changedSignal, SLOT("setValue(int)"));
	return $slider;
    }

    setPixmap($pixmap)
    {
	$.pixmapLabel.setPixmap($pixmap);
	my $size = $pixmap.size();
	if ($size.subtract(new QSize(1, 0)) == $.pixmapLabelArea.maximumViewportSize())
	    $size.subtractEquals(new QSize(1, 0));
	$.pixmapLabel.resize($size);
    }

    getSize()
    {
	my $ok;
	my $text = QInputDialog_getText($self, TR("Grabber"),
					TR("Enter pixmap size:"),
					QLineEdit::Normal,
					sprintf(TR("%d x %d"), $.glWidget.width(), $.glWidget.height()),
					\$ok);
	if (!$ok)
	    return new QSize();

	my $regExp = new QRegExp(TR("([0-9]+) *x *([0-9]+)"));
	if ($regExp.exactMatch($text)) {
	    my $width = int($regExp.cap(1));
	    my $height = int($regExp.cap(2));
	    if ($width > 0 && $width < 2048 && $height > 0 && $height < 2048)
		return new QSize($width, $height);
	}

	return $.glWidget.size();
    }
}

class grabber inherits QApplication 
{
    constructor() {
        my $mainWin = new MainWindow();
        $mainWin.show();
        $.exec();
    }
}
