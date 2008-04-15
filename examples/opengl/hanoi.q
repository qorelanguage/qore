#!/usr/bin/env qore

# hanoi.c - written by Greg Humphreys while an intern at SGI

%require-our

%requires opengl
%requires glut

const WIDTH = 800.0;
const HEIGHT = 400.0;

our $motion = GL_TRUE;
our $back_wall = GL_FALSE;
our $xangle = 0;
our $yangle = 0;
our $xlangle = 0;
our $ylangle = 0;

const wallz = -(WIDTH/2);
const DISK_HEIGHT = 20;

our $NUM_DISKS = 11;

const HANOI_SOLVE = 0;
const HANOI_QUIT = 1;
const HANOI_LIGHTING = 2;
const HANOI_WALL = 3;
const HANOI_FOG = 4;

const lightOneDirection = (0, 0, -1);
const lightOnePosition  = (200, 100, 300, 1);
const lightOneColor     = (1.0, 1.0, 0.5, 1.0);

const lightTwoDirection = (0, 0, -1);
const lightTwoPosition  = (600, 100, 300, 1);
const lightTwoColor     = (1.0, 0.0, 0.3, 1.0);

const lightZeroPosition = (400, 200, 300, 1);
const lightZeroColor    = (0.3, 0.3, 0.3, 0.3);

const diskColor         = (1.0, 1.0, 1.0, 0.8);
const poleColor         = (1.0, 0.2, 0.2, 0.8);

our $poles = ((), (), (), ());

sub push($which, $size)
{
    push $poles[$which], $size;
}

sub pop($which)
{
    return pop $poles[$which];
}

our $moves = ();

sub init()
{
    for (my $i = 1; $i <= $NUM_DISKS; $i++) {
	glNewList($i, GL_COMPILE);
	{
	    glutSolidTorus(DISK_HEIGHT / 2, 5 * $i, 15, 15);
	}
	glEndList();
    }
    glNewList($NUM_DISKS + 1, GL_COMPILE);
    {
	glutSolidCone(10, ($NUM_DISKS + 1) * DISK_HEIGHT, 20, 20);
    }
    glEndList();
}

sub mpop()
{
    shift $moves;
}

sub mpush(my $t, my $f)
{
    push $moves, ( "t" : $t, "f" : $f );
}

sub keyboard($key, $x, $y)
{
    switch ($key) {
	case 27:             /* ESC */
	case 113: #'q':
	case 81:  #'Q':
	    exit(0);
    }
}

sub update()
{
    glutPostRedisplay();
}

sub DrawPost($xcenter)
{
    glPushMatrix();
    {
	glTranslatef($xcenter, 0, 0);
	glRotatef(90, -1, 0, 0);
	glCallList($NUM_DISKS + 1);
    } glPopMatrix();
}

sub DrawPosts()
{
    glColor3fv(poleColor);
    glLineWidth(10);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, poleColor);
    DrawPost(WIDTH / 4);
    DrawPost(2 * WIDTH / 4);
    DrawPost(3 * WIDTH / 4);
}

sub DrawDisk($xcenter, $ycenter, $size)
{
    glPushMatrix();
    {
	glTranslatef($xcenter, $ycenter, 0);
	glRotatef(90, 1, 0, 0);
	glCallList($size);
    }
    glPopMatrix();
}

sub DrawDooDads()
{
    my ($xcenter, $ycenter);
    glColor3fv(diskColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diskColor);
    for (my $i = 1; $i <= 3; $i++) {
	$xcenter = $i * WIDTH / 4;

	my $rl = reverse($poles[$i]);
	$ycenter = DISK_HEIGHT * elements $rl - DISK_HEIGHT / 2;
	for (my $j = 0; $j < elements $rl; ++$j) {
	    my $size = $rl[$j];
	    DrawDisk($xcenter, $ycenter, $size);
	    $ycenter -= DISK_HEIGHT;
	}
    }
}

sub mov($n, $f, $t)
{
    my $o;

    if ($n == 1) {
	mpush($t, $f);
	return;
    }
    $o = 6 - ($f + $t);
    mov($n - 1, $f, $o);
    mov(1, $f, $t);
    mov($n - 1, $o, $t);
}

const wallcolor = (0, 0.3, 1, 1);

sub DrawWall()
{
    glColor3fv(wallcolor);
    for (my $i = 0; $i < WIDTH; $i += 10) {
	for (my $j = 0; $j < HEIGHT; $j += 10) {
	    glBegin(GL_POLYGON);
	    {
		glNormal3f(0, 0, 1);
		glVertex3f($i + 10, $j, wallz);
		glVertex3f($i + 10, $j + 10, wallz);
		glVertex3f($i, $j + 10, wallz);
		glVertex3f($i, $j, wallz);
	    }
	    glEnd();
	}
    }
}

sub draw()
{
    my ($t, $f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if ($back_wall) {
	glMaterialfv(GL_FRONT, GL_DIFFUSE, wallcolor);
	DrawWall();
    }
    glPushMatrix();
    {
	glTranslatef(WIDTH / 2, HEIGHT / 2, 0);
	glRotatef($xlangle, 0, 1, 0);
	glRotatef($ylangle, 1, 0, 0);
	glTranslatef(-WIDTH / 2, -HEIGHT / 2, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightZeroPosition);
    }
    glPopMatrix();
    
    glPushMatrix();
    {
	glTranslatef(WIDTH / 2, HEIGHT / 2, 0);
	glRotatef($xangle, 0, 1, 0);
	glRotatef($yangle, 1, 0, 0);
	glTranslatef(-WIDTH / 2, -HEIGHT / 2, 0);
	DrawPosts();
	DrawDooDads();
    }
    glPopMatrix();
    if ($motion && elements $moves) {
	$t = $moves[0].t;
	$f = $moves[0].f;
	push($t, pop($f));
	mpop();
    }
    glutSwapBuffers();
}

sub hanoi_menu($value)
{
    switch ($value) {
    case HANOI_SOLVE:
	$motion = !$motion;
	if($motion) {
	    glutIdleFunc(\update());
	} else {
	    glutIdleFunc();
	}
	break;
    case HANOI_LIGHTING:
	if (glIsEnabled(GL_LIGHTING))
	    glDisable(GL_LIGHTING);
	else
	    glEnable(GL_LIGHTING);
	break;
    case HANOI_WALL:
	$back_wall = !$back_wall;
	break;
    case HANOI_FOG:
	if (glIsEnabled(GL_FOG))
	    glDisable(GL_FOG);
	else {
	    glEnable(GL_FOG);
	    glFogi(GL_FOG_MODE, GL_EXP);
	    glFogf(GL_FOG_DENSITY, .01);
	}
	break;
    case HANOI_QUIT:
	exit(0);
	break;
    }
    glutPostRedisplay();
}

our ($oldx, $oldy);

our $leftb = GL_FALSE;
our $middleb = GL_FALSE;

sub hanoi_mouse($button, $state, $x, $y)
{
    if ($button == GLUT_LEFT_BUTTON) {
	$oldx = $x;
	$oldy = $y;
	if ($state == GLUT_DOWN)
	    $leftb = GL_TRUE;
	else
	    $leftb = GL_FALSE;
    }
    if ($button == GLUT_MIDDLE_BUTTON) {
	$oldx = $x;
	$oldy = $y;
	if ($state == GLUT_DOWN)
	    $middleb = GL_TRUE;
	else
	    $middleb = GL_FALSE;
    }
}

sub hanoi_visibility($state)
{
    if ($state == GLUT_VISIBLE && $motion) {
	glutIdleFunc(\update());
    } else {
	glutIdleFunc();
    }
}

sub hanoi_motion($x, $y)
{
    if ($leftb) {
      $xangle -= ($x - $oldx);
      $yangle -= ($y - $oldy);
    }
    if ($middleb) {
	$xlangle -= ($x - $oldx);
	$ylangle -= ($y - $oldy);
    }
    $oldx = $x;
    $oldy = $y;
    glutPostRedisplay();
}

const opts = ( "disks" : "n:i" );

sub main()
{
    my $p = new GetOpt(opts);
    my $o = $p.parse(\$ARGV);

    glutInit();

    if ($o.disks)
	$NUM_DISKS = $o.disks;

    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Hanoi");

    glutDisplayFunc(\draw());
    glutKeyboardFunc(\keyboard());

    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -10000, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    #glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

    glLightfv(GL_LIGHT1, GL_POSITION, lightOnePosition);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightOneColor);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 10);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightOneDirection);
    glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT2, GL_POSITION, lightTwoPosition);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightTwoColor);
    #glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION,.005);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightTwoDirection);
    glEnable(GL_LIGHT2);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glEnable(GL_LIGHT0);

    glEnable(GL_LIGHTING);

    glutMouseFunc(\hanoi_mouse());
    glutMotionFunc(\hanoi_motion());
    glutVisibilityFunc(\hanoi_visibility());

    glutCreateMenu(\hanoi_menu());
    glutAddMenuEntry("Solve", HANOI_SOLVE);
    glutAddMenuEntry("Lighting", HANOI_LIGHTING);
    glutAddMenuEntry("Back Wall", HANOI_WALL);
    glutAddMenuEntry("Fog", HANOI_FOG);
    glutAddMenuEntry("Quit", HANOI_QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    init();

    for (my $i = 0; $i < $NUM_DISKS; $i++)
	push(1, $NUM_DISKS - $i);
    mov($NUM_DISKS, 1, 3);

    glutMainLoop();
}

main();
