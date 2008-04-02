#!/usr/bin/env qore

# This is basically a direct port of the QT tutorial to Qore 
# using Qore's "qt" module.  

# Note that Qore's "qt" module requires QT 4.3 or above 

# use the "qt" module
%requires qt
# use the "opengl" module
#%requires opengl

# this is an object-oriented program, the application class is "framebufferobject"
%exec-class framebufferobject
# require all variables to be explicitly  declared
%require-our
# enable all parse warnings
%enable-all-warnings

const PI = 3.1415927;
const AMP = 5;

/*
class GLWidget inherits QGLWidget
{
    private $.anchor, $.scale, $.rot_x, $.rot_y, $.rot_z,
    $.tile_list, $.wave, $.logo, $.anim, $.svg_renderer;
    
    constructor($parent) : QGLWidget(new QGLFormat(QGL::SampleBuffers|QGL::AlphaChannel), $parent)
    {
	$.anchor = new QPoint();

	$.setWindowTitle(TR("OpenGL framebuffer objects"));
	$.makeCurrent();
	$.fbo = new QGLFramebufferObject(512, 512);
	$.rot_x = $.rot_y = $.rot_z = 0.0;
	$.scale = 0.1;
	$.anim = new QTimeLine(750, $self);
	$.anim.setUpdateInterval(20);
	$.connect($.anim, SIGNAL("valueChanged(qreal)"), SLOT("animate(qreal)"));
	$.connect($.anim, SIGNAL("finished()"), SLOT("animFinished()"));

	$.svg_renderer = new QSvgRenderer(QLatin1String(":/res/bubbles.svg"), $self);
	$.connect($.svg_renderer, SIGNAL("repaintNeeded()"), SLOT("draw()"));

	$.logo = QImage(":/res/qt4-logo.png");
	$.logo = $.logo.convertToFormat(QImage::Format_ARGB32);

	$.tile_list = glGenLists(1);
	glNewList($.tile_list, GL_COMPILE);
	glBegin(GL_QUADS);
	{
	    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
	    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
	    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
	    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);

	    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
	    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
	    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
	    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);

	    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
	    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0,  1.0);
	    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0,  1.0);
	    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);

	    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, -1.0, -1.0);
	    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0, -1.0, -1.0);
	    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
	    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);

	    glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
	    glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
	    glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
	    glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);

	    glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
	    glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
	    glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
	    glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
	}
	glEnd();
	glEndList();

	$.wave = new GLfloat[$.logo.width()*$.logo.height()];
	memset($.wave, 0, $.logo.width()*$.logo.height());
	startTimer(30); # $.wave timer
    }

    destructor()
    {
	glDeleteLists($.tile_list, 1);
	delete $.fbo;
    }

    paintEvent()
    {
	draw();
    }

    draw()
    {
	my $p = new QPainter($self); # used for text overlay
	
	# save the GL state set for QPainter
	$.saveGLState();

	# render the 'bubbles.svg' file into our framebuffer object
	QPainter $.fbo_painter($.fbo);
	$.svg_renderer.render(&$.fbo_painter);
	$.fbo_painter.end();

	# draw into the GL widget
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 10, 100);
	glTranslatef(0.0, 0.0, -15.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, width(), height());
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, $.fbo.texture());
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);

	# draw background
	glPushMatrix();
	glScalef(1.7, 1.7, 1.7);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glCallList($.tile_list);
	glPopMatrix();

	const int w = $.logo.width();
	const int h = $.logo.height();

	glRotatef($.rot_x, 1.0, 0.0, 0.0);
	glRotatef($.rot_y, 0.0, 1.0, 0.0);
	glRotatef($.rot_z, 0.0, 0.0, 1.0);
	glScalef($.scale/w, $.scale/w, $.scale/w);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	# draw the Qt icon
	glTranslatef(-w+1, -h+1, 0.0);
	for (my $y=h-1; $y>=0; --$y) {
	    uint *p = (uint*) $.logo.scanLine(y);
	    uint *end = p + w;
	    int  x = 0;
	    while (p < end) {
		glColor4ub(qRed(*p), qGreen(*p), qBlue(*p), uchar(qAlpha(*p)*.9));
		glTranslatef(0.0, 0.0, $.wave[y*w+x]);
             if (qAlpha(*p) > 128)
                 glCallList($.tile_list);
             glTranslatef(0.0, 0.0, -$.wave[y*w+x]);
		glTranslatef(2.0, 0.0, 0.0);
		++x;
		++p;
	    }
	    glTranslatef(-w*2.0, 2.0, 0.0);
	}

		# restore the GL state that QPainter expects
	    restoreGLState();

	# draw the overlayed text using QPainter
	    p.setPen(QColor(197, 197, 197, 157));
	p.setBrush(QColor(197, 197, 197, 127));
	p.drawRect(QRect(0, 0, width(), 50));
	p.setPen(Qt::black);
	p.setBrush(Qt::NoBrush);
	const QString str1(TR("A simple OpenGL framebuffer object example."));
	const QString str2(TR("Use the mouse wheel to zoom, press buttons and move mouse to rotate, double-click to flip."));
	QFontMetrics fm(p.font());
	p.drawText(width()/2 - fm.width(str1)/2, 20, str1);
	p.drawText(width()/2 - fm.width(str2)/2, 20 + fm.lineSpacing(), str2);
    }

    mousePressEvent(QMouseEvent *e)
    {
	$.anchor = e.pos();
    }

    mouseMoveEvent(QMouseEvent *e)
    {
	QPoint diff = e.pos() - $.anchor;
	if (e.buttons() & Qt::LeftButton) {
	    $.rot_x += diff.y()/5.0;
         $.rot_y += diff.x()/5.0;
	} else if (e.buttons() & Qt::RightButton) {
	    $.rot_z += diff.x()/5.0;
	}

	$.anchor = e.pos();
	draw();
    }

    wheelEvent(QWheelEvent *e)
    {
	e.delta() > 0 ? $.scale += $.scale*0.1 : $.scale -= $.scale*0.1;
	draw();
    }

    mouseDoubleClickEvent(QMouseEvent *)
    {
	$.anim.start();
    }

    animate(qreal val)
    {
	$.rot_y = val * 180;
	draw();
    }

    animFinished()
    {
	if ($.anim.direction() == QTimeLine::Forward)
	    $.anim.setDirection(QTimeLine::Backward);
	else
	    $.anim.setDirection(QTimeLine::Forward);
    }

    saveGLState()
    {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    }

    restoreGLState()
    {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
    }

    timerEvent()
    {
	if (QApplication::mouseButtons() != 0)
	    return;

	static bool scale_in = true;

	if (scale_in && $.scale > 35.0)
	    scale_in = false;
	else if (!scale_in && $.scale < .5)
	    scale_in = true;

	$.scale = scale_in ? $.scale + $.scale*0.01 : $.scale-$.scale*0.01;
	$.rot_z += 0.3;
	$.rot_x += 0.1;

	my $dx, $dy; # disturbance point
	my $s, $v, $W, $t;
	our $wt;
	my $width = $.logo.width();

	$dx = $dy = $width >> 1;

	$W = .3;
	$v = -4; # wave speed

	for (my $i = 0; $i < $width; ++$i) {
	    for (my $j = 0; $j < $width; ++$j) {
		$s = sqrt((($j - $dx) * ($j - $dx) + ($i - $dy) * ($i - $dy)));
		$wt[$i][$j] += 0.1;
		$t = $s / $v;
		if ($s != 0)
		    $.wave[$i*$width + $j] = AMP * sin(2 * PI * $W * ($wt[$i][$j] + $t)) / (0.2*($s + 2));
		else
		    $.wave[$i*$width + $j] = AMP * sin(2 * PI * $W * ($wt[$i][$j] + $t));
         }
     }
 }
}

class framebufferobject inherits QApplication 
{
    constructor() {
	if (!QGLFormat_hasOpenGL() || !QGLFramebufferObject_hasOpenGLFramebufferObjects()) {
	    QMessageBox_information(0, "OpenGL framebuffer objects",
				    "this system does not support OpenGL/framebuffer objects.");
	    return -1;
	}

	my $widget = new GLWidget();
	$widget.resize(640, 480);
	$widget.show();
        $.exec();
    }
}
*/

class framebufferobject inherits QApplication 
{
    constructor() {
	QMessageBox_information(0, "OpenGL framebuffer objects",
				    "this example is not yet ready because it depends on the as-yet-unimplemented opengl module");
	return -1;
    }
}
