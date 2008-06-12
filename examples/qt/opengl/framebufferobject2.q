#!/usr/bin/env qore

# This is basically a direct port of a QT example program to Qore
# using Qore's "qt-opengl" module.

# Note that Qore's "qt-opengl" module requires QT 4.3 or above with OpenGL support

# use the "qt-opengl" module (automatically loads the "qt-gui" and "opengl" modules)
%requires qt-opengl

# this is an object-oriented program, the application class is "framebufferobject2"
%exec-class framebufferobject2
# require all variables to be explicitly declared
%require-our
# enable all parse warnings
%enable-all-warnings

const cubeArray = (0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0,
		   0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1,
		   0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1,
		   0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
		   0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1,
		   1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1);

const cubeTextureArray = (0, 0, 1, 0, 1, 1, 0, 1,
			  0, 0, 0, 1, 1, 1, 1, 0,
			  0, 0, 1, 0, 1, 1, 0, 1,
			  1, 0, 0, 0, 0, 1, 1, 1,
			  0, 0, 1, 0, 1, 1, 0, 1,
			  1, 0, 0, 0, 0, 1, 1, 1);

const faceArray = (1, -1, 1, 1, -1, 1, -1, -1);

const colorArray = (170, 202, 0, 255,
		    120, 143, 0, 255,
		    83, 102, 0, 255,
		    120, 143, 0, 255);

class GLWidget inherits QGLWidget
{
    private $.rot, $.xOffs, $.yOffs, $.xInc, $.pbufferList, 
    $.cubeTexture, $.timerId, $.fbo;
    
    constructor($parent) : QGLWidget(new QGLFormat(QGL::SampleBuffers), $parent)
    {
	# create the framebuffer object - make sure to have a current
	# context before creating it
	$.makeCurrent();
	$.fbo = new QGLFramebufferObject(512, 512);
	$.timerId = $.startTimer(20);
	$.setWindowTitle(TR("OpenGL framebuffer objects 2"));
    }

    destructor()
    {
        glDeleteLists($.pbufferList, 1);
    }

    initializeGL()
    {
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_INT, cubeArray);
	glTexCoordPointer(2, GL_INT, cubeTextureArray);
	glColorPointer(4, GL_UNSIGNED_BYTE, colorArray);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	$.pbufferList = glGenLists(1);
	glNewList($.pbufferList, GL_COMPILE);
	{
	    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	    # draw cube background
	    glPushMatrix();
	    glLoadIdentity();
	    glTranslatef(0.5, 0.5, -2.0);
	    glDisable(GL_TEXTURE_2D);
	    glEnableClientState(GL_COLOR_ARRAY);
	    glVertexPointer(2, GL_INT, faceArray);
	    glDrawArrays(GL_QUADS, 0, 4);
	    glVertexPointer(3, GL_INT, cubeArray);
	    glDisableClientState(GL_COLOR_ARRAY);
	    glEnable(GL_TEXTURE_2D);
	    glPopMatrix();

	    # draw cube
	    glTranslatef(0.5, 0.5, 0.5);
	    glRotatef(3.0, 1.0, 1.0, 1.0);
	    glTranslatef(-0.5, -0.5, -0.5);
	    glColor4f(0.9, 0.9, 0.9, 1.0);
	    glDrawArrays(GL_QUADS, 0, 24);

	    glPushMatrix(); # this state is popped back in the paintGL() function
	}
	glEndList();

	for (my $i = 0; $i < 3; ++$i) {
	    $.yOffs[$i] = 0.0;
	    $.xInc[$i] = 0.005;
	    $.rot[$i] = 0.0;
	}
	$.xOffs[0]= 0.0;
	$.xOffs[1]= 0.5;
	$.xOffs[2]= 1.0;

	$.cubeTexture = $.bindTexture(new QImage("images/cubelogo.png"));

	glPushMatrix(); # push to avoid stack underflow in the first paintGL() call
    }

    resizeGL($w, $h)
    {
	glViewport(0, 0, $w, $h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	my $aspect = $w/float($h ? $h : 1);
	glFrustum(-$aspect, $aspect, -1, 1, 10, 100);
	glTranslatef(-0.5, -0.5, -0.5);
	glTranslatef(0.0, 0.0, -15.0);
    }

    paintGL()
    {
	glPopMatrix(); # pop the matrix pushed in the $.pbuffer list

	# push the projection matrix and the entire GL state before
	# doing any rendering into our framebuffer object
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	glViewport(0, 0, $.fbo.size().width(), $.fbo.size().height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -99, 99);
	glTranslatef(-0.5, -0.5, 0.0);
	glMatrixMode(GL_MODELVIEW);

	# render to the framebuffer object
	$.fbo.bind();
	glBindTexture(GL_TEXTURE_2D, $.cubeTexture);
	glCallList($.pbufferList);
	$.fbo.release();

	# pop the projection matrix and GL state back for rendering
	# to the actual widget
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, $.fbo.texture());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	# draw the background
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glVertexPointer(2, GL_INT, faceArray);
	glTranslatef(-1.2, -0.8, 0.0);
	glScalef(0.2, 0.2, 0.2);
	for (my $y = 0; $y < 5; ++$y) {
	    for (my $x = 0; $x < 5; ++$x) {
		glTranslatef(2.0, 0, 0);
		glColor4f(0.5, 0.5, 0.5, 1.0);
		glDrawArrays(GL_QUADS, 0, 4);
	    }
	    glTranslatef(-10.0, 2.0, 0);
	}
	glVertexPointer(3, GL_INT, cubeArray);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	# draw the bouncing cubes
	$.drawCube(0, 0.0, 1.5, 2.5, 1.5);
	$.drawCube(1, 1.0, 2.0, 2.5, 2.0);
	$.drawCube(2, 2.0, 3.5, 2.5, 2.5);
	glPopMatrix();
    }

    drawCube($i, $z, $rotation, $jmp, $amp)
    {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef($.xOffs[$i], $.yOffs[$i], $z);
	glTranslatef(0.5, 0.5, 0.5);
	my $scale = 0.75 + $i*(0.25/2);
	glScalef($scale, $scale, $scale);
	glRotatef($.rot[$i], 1.0, 1.0, 1.0);
	glTranslatef(-0.5, -0.5, -0.5);

	glColor4f(1.0, 1.0, 1.0, 0.8);
	glDrawArrays(GL_QUADS, 0, 24);

	if ($.xOffs[$i] > 1.0 || $.xOffs[$i] < -1.0) {
	    $.xInc[$i] = -$.xInc[$i];
	    $.xOffs[$i] = $.xOffs[$i] > 1.0 ? 1.0 : -1.0;
	}
	$.xOffs[$i] += $.xInc[$i];
	$.yOffs[$i] = abs(cos((-M_PI * $jmp) * $.xOffs[$i]) * $amp) - 1;
	$.rot[$i] += $rotation;
    }
    
}

class framebufferobject2 inherits QApplication 
{
    constructor() {
        if (!QGLFormat::hasOpenGL()) {
            QMessageBox::information(0, "OpenGL framebuffer objects",
                                    "this system does not support OpenGL");
            return -1;
        }
        if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
            QMessageBox::information(0, "OpenGL framebuffer objects",
                                    "this system does not support framebuffer objects.");
            return -1;
        }

        my $widget = new GLWidget();
        $widget.resize(640, 480);
        $widget.show();
        $.exec();
    }
}
