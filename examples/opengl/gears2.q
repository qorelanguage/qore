#!/usr/bin/env qore

# this one is pretty slow compared to gears.q

%requires opengl
%requires glut

%require-our

const d_near = 1.0;
const d_far = 2000.0;

our $circle_subdiv;

our $gear_profile = (( "rad" : 0.000, "wid" : 0.0 ),
		     ( "rad" : 0.300, "wid" : 7.0 ),
		     ( "rad" : 0.340, "wid" : 0.4 ),
		     ( "rad" : 0.550, "wid" : 0.64 ),
		     ( "rad" : 0.600, "wid" : 0.4 ),
		     ( "rad" : 0.950, "wid" : 1.0 ));

our $a1 = 27.0;
our $a2 = 67.0;
our $a3 = 47.0;
our $a4 = 87.0;
our $i1 = 1.2;
our $i2 = 3.1;
our $i3 = 2.3;
our $i4 = 1.1;

sub gear($nt, $wd, $ir, $or, $tp, $tip, $ip)
{
    # * nt - number of teeth 
    # * wd - width of gear at teeth
    # * ir - inside radius absolute scale
    # * or - radius at outside of wheel (tip of tooth) ratio of ir
    # * tp - ratio of tooth in slice of circle (0..1] (1 = teeth are touching at base)
    # * tip - ratio of tip of tooth (0..tp] (cant be wider that base of tooth)
    # * ns - number of elements in wheel width profile
    # * *ip - list of float pairs {start radius, width, ...} (width is ratio to wd)
    
    # gear lying on xy plane, z for width. all normals calulated (normalized)

    my $ns = elements $ip;
	
    my $prev;
    my ($k, $t);
    
    # estimate # times to divide circle
    if ($nt <= 0)
	$circle_subdiv = 64;
    else {
	# lowest multiple of number of teeth
	$circle_subdiv = $nt;
	while ($circle_subdiv < 64)
	    $circle_subdiv += $nt;
    }

    # --- draw wheel face ---

    # draw horzontal, vertical faces for each section. if first
    # section radius not zero, use wd for 0.. first if ns == 0
    # use wd for whole face. last width used to edge.

    if ($ns <= 0) {
	flat_face(0.0, $ir, $wd);
    } else {
	# draw first flat_face, then continue in loop
	if ($ip[0].rad > 0.0) {
	    flat_face(0.0, $ip[0].rad * $ir, $wd);
	    $prev = $wd;
	    $t = 0;
	} else {
	    flat_face(0.0, $ip[1].rad * $ir, $ip[0].wid * $wd);
	    $prev = $ip[0].wid;
	    $t = 1;
	}
	for ($k = $t; $k < $ns; $k++) {
	    if ($prev < $ip[$k].wid) {
		draw_inside($prev * $wd, $ip[$k].wid * $wd, $ip[$k].rad * $ir);
	    } else {
		draw_outside($prev * $wd, $ip[$k].wid * $wd, $ip[$k].rad * $ir);
	    }
	    $prev = $ip[$k].wid;
	    # - draw to edge of wheel, add final face if needed
		if ($k == $ns - 1) {
		    flat_face($ip[$k].rad * $ir, $ir, $ip[$k].wid * $wd);
		    
		    # now draw side to match tooth rim
		    if ($ip[$k].wid < 1.0) {
			draw_inside($ip[$k].wid * $wd, $wd, $ir);
		    } else {
			draw_outside($ip[$k].wid * $wd, $wd, $ir);
		    }
		} else {
		    flat_face($ip[$k].rad * $ir, $ip[$k + 1].rad * $ir, $ip[$k].wid * $wd);
		}
	}
    }
    
    # --- tooth side faces ---
    tooth_side($nt, $ir, $or, $tp, $tip, $wd);

    # --- tooth hill surface ---
}

sub tooth_side($nt, $ir, $or, $tp, $tip, $wd)
{
    my $i;
    my $end = 2.0 * M_PI / $nt;
    my ($x, $y);
    my ($s, $c);


    my $bound = 2.0 * M_PI - $end / 4.0;
    $or = $or * $ir;         # or is really a ratio of ir
    for ($i = 0.0; $i < $bound; $i += $end) {

	#printf("tooth_side(%n, %n, %n, %n, %n, %n) i=%n, bound=%n, end=%n\n", $nt, $ir, $or, $tp, $tip, $wd, $i, $bound, $end);


	$c[0] = cos($i);
	$s[0] = sin($i);
	$c[1] = cos($i + $end * (0.5 - $tip / 2));
	$s[1] = sin($i + $end * (0.5 - $tip / 2));
	$c[2] = cos($i + $end * (0.5 + $tp / 2));
	$s[2] = sin($i + $end * (0.5 + $tp / 2));
	
	$x[0] = $ir * $c[0];
	$y[0] = $ir * $s[0];
	$x[5] = $ir * cos($i + $end);
	$y[5] = $ir * sin($i + $end);
	# ---treat veritices 1,4 special to match strait edge of face
	$x[1] = $x[0] + ($x[5] - $x[0]) * (0.5 - $tp / 2);
	$y[1] = $y[0] + ($y[5] - $y[0]) * (0.5 - $tp / 2);
	$x[4] = $x[0] + ($x[5] - $x[0]) * (0.5 + $tp / 2);
	$y[4] = $y[0] + ($y[5] - $y[0]) * (0.5 + $tp / 2);
	$x[2] = $or * cos($i + $end * (0.5 - $tip / 2));
	$y[2] = $or * sin($i + $end * (0.5 - $tip / 2));
	$x[3] = $or * cos($i + $end * (0.5 + $tip / 2));
	$y[3] = $or * sin($i + $end * (0.5 + $tip / 2));

	# draw face trapezoids as 2 tmesh
	glNormal3f(0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[2], $y[2], $wd / 2);
	glVertex3f($x[1], $y[1], $wd / 2);
	glVertex3f($x[3], $y[3], $wd / 2);
	glVertex3f($x[4], $y[4], $wd / 2);
	glEnd();

	glNormal3f(0.0, 0.0, -1.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[2], $y[2], -$wd / 2);
	glVertex3f($x[1], $y[1], -$wd / 2);
	glVertex3f($x[3], $y[3], -$wd / 2);
	glVertex3f($x[4], $y[4], -$wd / 2);
	glEnd();

	# draw inside rim pieces
	glNormal3f($c[0], $s[0], 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[0], $y[0], -$wd / 2);
	glVertex3f($x[1], $y[1], -$wd / 2);
	glVertex3f($x[0], $y[0], $wd / 2);
	glVertex3f($x[1], $y[1], $wd / 2);
	glEnd();

	# draw up hill side
	{
	    my ($a, $b, $n);
	    # calculate normal of face
	    $a = $x[2] - $x[1];
	    $b = $y[2] - $y[1];
	    $n = 1.0 / sqrt($a * $a + $b * $b);
	    $a = $a * $n;
	    $b = $b * $n;
	    glNormal3f($b, -$a, 0.0);
	}
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[1], $y[1], -$wd / 2);
	glVertex3f($x[2], $y[2], -$wd / 2);
	glVertex3f($x[1], $y[1], $wd / 2);
	glVertex3f($x[2], $y[2], $wd / 2);
	glEnd();
	# draw top of hill
	glNormal3f($c[1], $s[1], 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[2], $y[2], -$wd / 2);
	glVertex3f($x[3], $y[3], -$wd / 2);
	glVertex3f($x[2], $y[2], $wd / 2);
	glVertex3f($x[3], $y[3], $wd / 2);
	glEnd();

	# draw down hill side
	{
	    my ($a, $b, $c);
	    # calculate normal of face
	    $a = $x[4] - $x[3];
	    $b = $y[4] - $y[3];
	    $c = 1.0 / sqrt($a * $a + $b * $b);
	    $a = $a * $c;
	    $b = $b * $c;
	    glNormal3f($b, -$a, 0.0);
	}
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[3], $y[3], -$wd / 2);
	glVertex3f($x[4], $y[4], -$wd / 2);
	glVertex3f($x[3], $y[3], $wd / 2);
	glVertex3f($x[4], $y[4], $wd / 2);
	glEnd();
	# inside rim part 
	glNormal3f($c[2], $s[2], 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f($x[4], $y[4], -$wd / 2);
	glVertex3f($x[5], $y[5], -$wd / 2);
	glVertex3f($x[4], $y[4], $wd / 2);
	glVertex3f($x[5], $y[5], $wd / 2);
	glEnd();
    }
}

sub flat_face($ir, $or, $wd)
{
    my $i;
    my $w;

    # draw each face (top & bottom )
    #printf("Face   : %n..%n wid=%n\n", $ir, $or, $wd);
    if ($wd == 0.0)
	return;
    for ($w = $wd / 2; $w > -$wd; $w -= $wd) {
	if ($w > 0.0)
	    glNormal3f(0.0, 0.0, 1.0);
	else
	    glNormal3f(0.0, 0.0, -1.0);

	if ($ir == 0.0) {
	    # draw as t-fan
	    glBegin(GL_TRIANGLE_FAN);
	    glVertex3f(0.0, 0.0, $w);  #/* center */
	    glVertex3f($or, 0.0, $w);
	    for ($i = 1; $i < $circle_subdiv; $i++) {
		glVertex3f(cos(2.0 * M_PI * $i / $circle_subdiv) * $or,
			   sin(2.0 * M_PI * $i / $circle_subdiv) * $or, $w);
	    }
	    glVertex3f($or, 0.0, $w);
	    glEnd();
	} else {
	    #/* draw as tmesh */
	    glBegin(GL_TRIANGLE_STRIP);
	    glVertex3f($or, 0.0, $w);
	    glVertex3f($ir, 0.0, $w);
	    for ($i = 1; $i < $circle_subdiv; $i++) {
		glVertex3f(cos(2.0 * M_PI * $i / $circle_subdiv) * $or,
			   sin(2.0 * M_PI * $i / $circle_subdiv) * $or, $w);
		glVertex3f(cos(2.0 * M_PI * $i / $circle_subdiv) * $ir,
			   sin(2.0 * M_PI * $i / $circle_subdiv) * $ir, $w);
	    }
	    glVertex3f($or, 0.0, $w);
	    glVertex3f($ir, 0.0, $w);
	    glEnd();
	}
    }
}

sub draw_inside($w1, $w2, $rad)
{
    my ($i, $j);
    my ($c, $s);
    #printf("Inside : wid=%n..%n rad=%n\n", $w1, $w2, $rad);
    if ($w1 == $w2)
	return;

    $w1 = $w1 / 2;
    $w2 = $w2 / 2;
    for ($j = 0; $j < 2; $j++) {
	if ($j == 1) {
	    $w1 = -$w1;
	    $w2 = -$w2;
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f($rad, 0.0, $w1);
	glVertex3f($rad, 0.0, $w2);
	for ($i = 1; $i < $circle_subdiv; $i++) {
	    $c = cos(2.0 * M_PI * $i / $circle_subdiv);
	    $s = sin(2.0 * M_PI * $i / $circle_subdiv);
	    glNormal3f(-$c, -$s, 0.0);
	    glVertex3f($c * $rad,
		       $s * $rad,
		       $w1);
	    glVertex3f($c * $rad,
		       $s * $rad,
		       $w2);
	}
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f($rad, 0.0, $w1);
	glVertex3f($rad, 0.0, $w2);
	glEnd();
    }
}

sub draw_outside($w1, $w2, $rad)
{
    my ($i, $j);
    my ($c, $s);
    #printf("Outside: wid=%n..%n rad=%n\n", $w1, $w2, $rad);

    if ($w1 == $w2)
	return;

    $w1 = $w1 / 2;
    $w2 = $w2 / 2;
    for ($j = 0; $j < 2; $j++) {
	if ($j == 1) {
	    $w1 = -$w1;
	    $w2 = -$w2;
	}
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f($rad, 0.0, $w1);
	glVertex3f($rad, 0.0, $w2);
	for ($i = 1; $i < $circle_subdiv; $i++) {
	    $c = cos(2.0 * M_PI * $i / $circle_subdiv);
	    $s = sin(2.0 * M_PI * $i / $circle_subdiv);
	    glNormal3f($c, $s, 0.0);
	    glVertex3f($c * $rad,
		       $s * $rad,
		       $w1);
	    glVertex3f($c * $rad,
		       $s * $rad,
		       $w2);
	}
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f($rad, 0.0, $w1);
	glVertex3f($rad, 0.0, $w2);
	glEnd();
    }
}

sub oneFrame()
{
    #printf("oneFrame()\n");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -4.0);
    glRotatef($a3, 1.0, 1.0, 1.0);
    glRotatef($a4, 0.0, 0.0, -1.0);
    glTranslatef(0.14, 0.2, 0.0);
    gear(76,
	 0.4, 2.0, 1.1,
	 0.4, 0.04,
	 $gear_profile);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.1, 0.2, -3.8);
    glRotatef($a2, -4.0, 2.0, -1.0);
    glRotatef($a1, 1.0, -3.0, 1.0);
    glTranslatef(0.0, -0.2, 0.0);
    gear(36,
	 0.4, 2.0, 1.1,
	 0.7, 0.2,
	 $gear_profile);
    glPopMatrix();
    
    $a1 += $i1;
    if ($a1 > 360.0)
	$a1 -= 360.0;
    if ($a1 < 0.0)
	$a1 -= 360.0;
    $a2 += $i2;
    if ($a2 > 360.0)
	$a2 -= 360.0;
    if ($a2 < 0.0)
	$a2 -= 360.0;
    $a3 += $i3;
    if ($a3 > 360.0)
	$a3 -= 360.0;
    if ($a3 < 0.0)
	$a3 -= 360.0;
    $a4 += $i4;
    if ($a4 > 360.0)
	$a4 -= 360.0;
    if ($a4 < 0.0)
	$a4 -= 360.0;
    glutSwapBuffers();
}

sub display()
{
    #printf("display()");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

sub myReshape($w, $h)
{
    #printf("myReshape(%n, %n)\n", $w, $h);
    glViewport(0, 0, $w, $h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, 1.0, -1.0, 1.0, d_near, d_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

sub visibility($status)
{
    #printf("visibility(%n)\n", $status);
    if ($status == GLUT_VISIBLE) {
	glutIdleFunc(\oneFrame());
    } else {
	glutIdleFunc();
    }
}

sub myinit()
{
    my $f;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    myReshape(640, 480);
    # glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);

    glLightf(GL_LIGHT0, GL_SHININESS, 1.0);
    $f[0] = 1.3;
    $f[1] = 1.3;
    $f[2] = -3.3;
    $f[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_POSITION, $f);
    $f[0] = 0.8;
    $f[1] = 1.0;
    $f[2] = 0.83;
    $f[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_SPECULAR, $f);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, $f);
    glEnable(GL_LIGHT0);

    glLightf(GL_LIGHT1, GL_SHININESS, 1.0);
    $f[0] = -2.3;
    $f[1] = 0.3;
    $f[2] = -7.3;
    $f[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_POSITION, $f);
    $f[0] = 1.0;
    $f[1] = 0.8;
    $f[2] = 0.93;
    $f[3] = 1.0;
    glLightfv(GL_LIGHT1, GL_SPECULAR, $f);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, $f);
    glEnable(GL_LIGHT1);
    
    # gear material
    $f[0] = 0.1;
    $f[1] = 0.15;
    $f[2] = 0.2;
    $f[3] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, $f);
    
    $f[0] = 0.9;
    $f[1] = 0.3;
    $f[2] = 0.3;
    $f[3] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $f);
    
    $f[0] = 0.4;
    $f[1] = 0.9;
    $f[2] = 0.6;
    $f[3] = 1.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, $f);
    
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 4);
}

sub keys($c, $x, $y)
{
    #printf("keys: c=%s x=%d y=%d\n", $c, $x, $y);
    if ($c == 0x1b)
	exit(0);
}

sub main()
{
    my $mode = GLUT_DOUBLE;

    #glutInit(&argc, argv);
    glutInit($ARGV);

    if (elements $ARGV > 1)
	$mode = GLUT_SINGLE;
    glutInitDisplayMode($mode | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Gears");
    
    myinit();
    glutReshapeFunc(\myReshape());
    glutDisplayFunc(\display());
    glutKeyboardFunc(\keys());
    glutVisibilityFunc(\visibility());
    glutPostRedisplay();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutMainLoop();
}

main();
