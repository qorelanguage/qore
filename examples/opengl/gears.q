#!/usr/bin/env qore

%require-our

# the glut module automatically loads the opengl module
%requires glut

our $view_rotx = 20.0;
our $view_roty = 30.0;
our $view_rotz = 0.0;
our ($gear1, $gear2, $gear3);
our $angle = 0.0;

our $limit;
our $count = 1;

const pos = ( 5.0, 5.0, 10.0, 0.0 );
const red = ( 0.8, 0.1, 0.0, 1.0 );
const green = ( 0.0, 0.8, 0.2, 1.0 );
const blue = ( 0.2, 0.2, 1.0, 1.0 );

#  Draw a gear wheel.  You'll probably want to call this function when
#  building a display list since we do a lot of trig here.
# 
#  Input:  inner_radius - radius of hole at center
#          outer_radius - radius at center of teeth
#          width - width of gear
#          teeth - number of teeth
#          tooth_depth - depth of tooth

sub gear($inner_radius, $outer_radius, $width, $teeth, $tooth_depth)
{
    my $i;
    my $angle;
    my ($u, $v, $len);

    my $r0 = $inner_radius;
    my $r1 = $outer_radius - $tooth_depth / 2.0;
    my $r2 = $outer_radius + $tooth_depth / 2.0;

    my $da = 2.0 * M_PI / $teeth / 4.0;

    glShadeModel(GL_FLAT);

    glNormal3f(0.0, 0.0, 1.0);

    # draw front face
    glBegin(GL_QUAD_STRIP);
    for ($i = 0; $i <= $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), $width * 0.5);
	glVertex3f($r1 * cos($angle), $r1 * sin($angle), $width * 0.5);
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), $width * 0.5);
	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), $width * 0.5);
    }
    glEnd();

    # draw front sides of teeth
    glBegin(GL_QUADS);
    $da = 2.0 * M_PI / $teeth / 4.0;
    for ($i = 0; $i < $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;

	glVertex3f($r1 * cos($angle), $r1 * sin($angle), $width * 0.5);
	glVertex3f($r2 * cos($angle + $da), $r2 * sin($angle + $da), $width * 0.5);
	glVertex3f($r2 * cos($angle + 2 * $da), $r2 * sin($angle + 2 * $da), $width * 0.5);
	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), $width * 0.5);
    }
    glEnd();

    glNormal3f(0.0, 0.0, -1.0);

    # draw back face
    glBegin(GL_QUAD_STRIP);
    for ($i = 0; $i <= $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;
	glVertex3f($r1 * cos($angle), $r1 * sin($angle), -$width * 0.5);
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), -$width * 0.5);
	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), -$width * 0.5);
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), -$width * 0.5);
    }
    glEnd();

    # draw back sides of teeth
    glBegin(GL_QUADS);
    $da = 2.0 * M_PI / $teeth / 4.0;
    for ($i = 0; $i < $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;

	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), -$width * 0.5);
	glVertex3f($r2 * cos($angle + 2 * $da), $r2 * sin($angle + 2 * $da), -$width * 0.5);
	glVertex3f($r2 * cos($angle + $da), $r2 * sin($angle + $da), -$width * 0.5);
	glVertex3f($r1 * cos($angle), $r1 * sin($angle), -$width * 0.5);
    }
    glEnd();

    # draw outward faces of teeth
    glBegin(GL_QUAD_STRIP);
    for ($i = 0; $i < $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;

	glVertex3f($r1 * cos($angle), $r1 * sin($angle), $width * 0.5);
	glVertex3f($r1 * cos($angle), $r1 * sin($angle), -$width * 0.5);
	$u = $r2 * cos($angle + $da) - $r1 * cos($angle);
	$v = $r2 * sin($angle + $da) - $r1 * sin($angle);
	$len = sqrt($u * $u + $v * $v);
	$u /= $len;
	$v /= $len;
	glNormal3f($v, -$u, 0.0);
	glVertex3f($r2 * cos($angle + $da), $r2 * sin($angle + $da), $width * 0.5);
	glVertex3f($r2 * cos($angle + $da), $r2 * sin($angle + $da), -$width * 0.5);
	glNormal3f(cos($angle), sin($angle), 0.0);
	glVertex3f($r2 * cos($angle + 2 * $da), $r2 * sin($angle + 2 * $da), $width * 0.5);
	glVertex3f($r2 * cos($angle + 2 * $da), $r2 * sin($angle + 2 * $da), -$width * 0.5);
	$u = $r1 * cos($angle + 3 * $da) - $r2 * cos($angle + 2 * $da);
	$v = $r1 * sin($angle + 3 * $da) - $r2 * sin($angle + 2 * $da);
	glNormal3f($v, -$u, 0.0);
	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), $width * 0.5);
	glVertex3f($r1 * cos($angle + 3 * $da), $r1 * sin($angle + 3 * $da), -$width * 0.5);
	glNormal3f(cos($angle), sin($angle), 0.0);
    }

    glVertex3f($r1 * cos(0), $r1 * sin(0), $width * 0.5);
    glVertex3f($r1 * cos(0), $r1 * sin(0), -$width * 0.5);

    glEnd();

    glShadeModel(GL_SMOOTH);

    # draw inside radius cylinder
    glBegin(GL_QUAD_STRIP);
    for ($i = 0; $i <= $teeth; $i++) {
	$angle = $i * 2.0 * M_PI / $teeth;

	glNormal3f(-cos($angle), -sin($angle), 0.0);
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), -$width * 0.5);
	glVertex3f($r0 * cos($angle), $r0 * sin($angle), $width * 0.5);
    }
    glEnd();
}

sub draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef($view_rotx, 1.0, 0.0, 0.0);
    glRotatef($view_roty, 0.0, 1.0, 0.0);
    glRotatef($view_rotz, 0.0, 0.0, 1.0);

    glPushMatrix();
    glTranslatef(-3.0, -2.0, 0.0);
    glRotatef($angle, 0.0, 0.0, 1.0);
    glCallList($gear1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.1, -2.0, 0.0);
    glRotatef(-2.0 * $angle - 9.0, 0.0, 0.0, 1.0);
    glCallList($gear2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3.1, 4.2, 0.0);
    glRotatef(-2.0 * $angle - 25.0, 0.0, 0.0, 1.0);
    glCallList($gear3);
    glPopMatrix();

    glPopMatrix();

    glutSwapBuffers();

    $count++;
    if ($count == $limit) {
	exit(0);
    }
}

sub idle()
{
    $angle += 2.0;
    glutPostRedisplay();
}

# change view angle, exit upon ESC
sub key($k)
{
    switch ($k) {
    case 122: # 'z'
        $view_rotz += 5.0;
	break;
    case 90: # 'Z'
	$view_rotz -= 5.0;
	break;
    case 27:  /* Escape */
        exit(0);
	break;
    default:
	return;
    }
    glutPostRedisplay();
}

# change view angle
sub special($k, $x, $y)
{
    switch ($k) {
    case GLUT_KEY_UP:
	$view_rotx += 5.0;
	break;
    case GLUT_KEY_DOWN:
	$view_rotx -= 5.0;
	break;
    case GLUT_KEY_LEFT:
	$view_roty += 5.0;
	break;
    case GLUT_KEY_RIGHT:
	$view_roty -= 5.0;
	break;
    default:
	return;
    }
    glutPostRedisplay();
}

#/* new window size or exposure */
sub reshape($width, $height)
{
    my $h = float($height) / $width;

    glViewport(0, 0, $width, $height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -$h, $h, 5.0, 60.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -40.0);
}

sub init()
{
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    # make the gears
    $gear1 = glGenLists(1);
    glNewList($gear1, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    gear(1.0, 4.0, 1.0, 20, 0.7);
    glEndList();

    $gear2 = glGenLists(1);
    glNewList($gear2, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    gear(0.5, 2.0, 2.0, 10, 0.7);
    glEndList();

    $gear3 = glGenLists(1);
    glNewList($gear3, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    gear(1.3, 2.0, 0.5, 10, 0.7);
    glEndList();

    glEnable(GL_NORMALIZE);
}

sub visible($vis)
{
    if ($vis == GLUT_VISIBLE)
	glutIdleFunc(\idle());
    else
	glutIdleFunc();
}

sub main()
{
    glutInit();
    $limit = int(shift $ARGV);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutCreateWindow("Gears");
    init();

    glutDisplayFunc(\draw());
    glutReshapeFunc(\reshape());
    glutKeyboardFunc(\key());
    glutSpecialFunc(\special());
    glutVisibilityFunc(\visible());
    
    glutMainLoop();
}

main();
