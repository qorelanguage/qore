#!/usr/bin/env qore

%requires opengl
%requires glut

    const Scale = 0.3;

# Increasing this values produces better image quality, the price is speed.
# Very low values produces erroneous/incorrect plotting
const tetradivisions = 23;
const cubedivisions = 20;
const octadivisions = 21;
const dodecadivisions = 10;
const icodivisions = 15;

const tetraangle = 109.47122063449069174;
const cubeangle = 90.000000000000000000;
const octaangle = 109.47122063449069174;
const dodecaangle = 63.434948822922009981;
const icoangle = 41.810314895778596167;

const Pi = 3.1415926535897932385;
const SQRT2 = 1.4142135623730951455;
const SQRT3 = 1.7320508075688771932;
const SQRT5 = 2.2360679774997898051;
const SQRT6 = 2.4494897427831778813;
const SQRT15 = 3.8729833462074170214;
const cossec36_2 = 0.8506508083520399322;
const cos72 = 0.3090169943749474241;
const sin72 = 0.9510565162951535721;
const cos36 = 0.8090169943749474241;
const sin36 = 0.5877852522924731292;

const TAU = ((SQRT5 + 1) / 2);

# *************************************************************************

our $mono=0;
our $smooth=1;
our $anim=1;
our ($WindH, $WindW);
our $step=0.0;
our $seno;
our $object;
our $edgedivisions;

our $draw_object;

our $Magnitude;
our $MaterialColor;

our $t0 = -1.0;

const front_shininess = ( 60.0, );
const front_specular = ( 0.7, 0.7, 0.7, 1.0 );
const ambient = ( 0.0, 0.0, 0.0, 1.0 );
const diffuse = ( 1.0, 1.0, 1.0, 1.0 );
const position0 = ( 1.0, 1.0, 1.0, 0.0 );
const position1 = (-1.0,-1.0, 1.0, 0.0 );
const lmodel_ambient = ( 0.5, 0.5, 0.5, 1.0 );
const lmodel_twoside = ( GL_TRUE, );

const MaterialRed = ( 0.7, 0.0, 0.0, 1.0 );
const MaterialGreen = ( 0.1, 0.5, 0.2, 1.0 );
const MaterialBlue = ( 0.0, 0.0, 0.7, 1.0 );
const MaterialCyan = ( 0.2, 0.5, 0.7, 1.0 );
const MaterialYellow = ( 0.7, 0.7, 0.0, 1.0 );
const MaterialMagenta = ( 0.6, 0.2, 0.5, 1.0 );
const MaterialWhite = ( 0.7, 0.7, 0.7, 1.0 );
const MaterialGray = ( 0.2, 0.2, 0.2, 1.0 );

sub myVectMul($X1, $Y1, $Z1, $X2, $Y2, $Z2)
{
    return glNormal3f($Y1*$Z2-$Z1*$Y2,$Z1*$X2-$X1*$Z2,$X1*$Y2-$Y1*$X2);
}

sub sqr($A) { return $A*$A; }

sub TRIANGLE($Edge, $Amp, $Divisions, $Z)
{
    my ( $Xf,$Yf,$Xa,$Yb,$Xf2,$Yf2 );
    my ( $Factor,$Factor1,$Factor2 );
    my ( $VertX,$VertY,$VertZ,$NeiAX,$NeiAY,$NeiAZ,$NeiBX,$NeiBY,$NeiBZ );
    my ( $Ax,$Ay,$Bx );
    my ( $Ri,$Ti );
    
    my $Vr=($Edge)*SQRT3/3;
    my $AmpVr2=($Amp)/sqr($Vr);
    my $Zf=($Edge)*($Z);

    $Ax=($Edge)*( 0.5/($Divisions)); $Ay=($Edge)*(-SQRT3/(2*$Divisions)); 
    $Bx=($Edge)*(-0.5/($Divisions)); 
    
    for ($Ri=1; $Ri<=($Divisions); $Ri++) { 
	glBegin(GL_TRIANGLE_STRIP); 
	for ($Ti=0; $Ti<$Ri; $Ti++) { 
	    $Xf=float($Ri-$Ti)*$Ax + float($Ti)*$Bx; 
	    $Yf=$Vr+float($Ri-$Ti)*$Ay + float($Ti)*$Ay; 
	    $Xa=$Xf+0.001; $Yb=$Yf+0.001; 
	    $Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
	    $Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
	    $Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	    $VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf; 
	    $NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ; 
	    $NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ; 
	    myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	    glVertex3f($VertX, $VertY, $VertZ);

	    $Xf=float($Ri-$Ti-1)*$Ax + float($Ti*$Bx); 
	    $Yf=$Vr+float($Ri-$Ti-1)*$Ay + float($Ti*$Ay); 
	    $Xa=$Xf+0.001; $Yb=$Yf+0.001; 
	    $Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
	    $Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
	    $Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	    $VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf; 
	    $NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ; 
	    $NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ; 
	    myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	    glVertex3f($VertX, $VertY, $VertZ); 
	    
	} 
	$Xf=float($Ri*$Bx); 
	$Yf=$Vr+float($Ri*$Ay); 
	$Xa=$Xf+0.001; $Yb=$Yf+0.001; 
	$Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
	$Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
	$Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	$VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf; 
	$NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ; 
	$NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ; 
	myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	glVertex3f($VertX, $VertY, $VertZ); 
	glEnd(); 
    } 
}

sub SQUARE($Edge, $Amp, $Divisions, $Z)
{
    my ($Xi,$Yi);
    my ($Xf,$Yf,$Y,$Xf2,$Yf2,$Y2,$Xa,$Yb);
    my ($Factor,$Factor1,$Factor2);
    my ($VertX,$VertY,$VertZ,$NeiAX,$NeiAY,$NeiAZ,$NeiBX,$NeiBY,$NeiBZ);
    my $Zf=($Edge)*($Z);
    my $AmpVr2=($Amp)/sqr(($Edge)*SQRT2/2);

    for ($Yi=0; $Yi<($Divisions); $Yi++) { 
	$Yf=-(($Edge)/2.0) + (float($Yi))/($Divisions)*($Edge); 
	$Yf2=sqr($Yf); 
	$Y=$Yf+1.0/($Divisions)*($Edge); 
	$Y2=sqr($Y); 
	glBegin(GL_QUAD_STRIP);
	for ($Xi=0; $Xi<=($Divisions); $Xi++) { 
	    $Xf=-(($Edge)/2.0) + (float($Xi))/($Divisions)*($Edge); 
	    $Xf2=sqr($Xf);

	    $Xa=$Xf+0.001; $Yb=$Y+0.001; 
	    $Factor=1-(($Xf2+$Y2)*$AmpVr2); 
	    $Factor1=1-((sqr($Xa)+$Y2)*$AmpVr2); 
	    $Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	    $VertX=$Factor*$Xf; $VertY=$Factor*$Y; $VertZ=$Factor*$Zf; 
	    $NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Y-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ;

	    $NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ;

	    myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	    glVertex3f($VertX, $VertY, $VertZ); 
	    
	    $Xa=$Xf+0.001; $Yb=$Yf+0.001; 
	    $Factor=1-(($Xf2+$Yf2)*$AmpVr2); 
	    $Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
	    $Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	    $VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf;

	    $NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ;

	    $NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ;

	    myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	    glVertex3f($VertX, $VertY, $VertZ);
	}
	glEnd();
    }
}

sub PENTAGON($Edge, $Amp, $Divisions, $Z)
{
    my ($Ri,$Ti,$Fi);
    my ($Xf,$Yf,$Xa,$Yb,$Xf2,$Yf2); 
    my ($x,$y);
    my ($Factor,$Factor1,$Factor2);
    my ($VertX,$VertY,$VertZ,$NeiAX,$NeiAY,$NeiAZ,$NeiBX,$NeiBY,$NeiBZ);
    my $Zf=($Edge)*($Z);
    my $AmpVr2=($Amp)/sqr(($Edge)*cossec36_2);
    
    for($Fi=0;$Fi<6;$Fi++) {
	$x[$Fi]=-cos( $Fi*2*Pi/5 + Pi/10 )/($Divisions)*cossec36_2*($Edge); 
	$y[$Fi]= sin( $Fi*2*Pi/5 + Pi/10 )/($Divisions)*cossec36_2*($Edge); 
    }

    for ($Ri=1; $Ri<=($Divisions); $Ri++) { 
	for ($Fi=0; $Fi<5; $Fi++) { 
	    glBegin(GL_TRIANGLE_STRIP); 
	    for ($Ti=0; $Ti<$Ri; $Ti++) { 
		$Xf=float($Ri-$Ti)*$x[$Fi] + float($Ti*$x[$Fi+1]); 
		$Yf=float($Ri-$Ti)*$y[$Fi] + float($Ti*$y[$Fi+1]); 
		$Xa=$Xf+0.001; $Yb=$Yf+0.001; 
		$Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
		$Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
		$Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
		$VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf; 
		$NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ; 
		$NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ; 
		myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
		glVertex3f($VertX, $VertY, $VertZ); 
		
		$Xf=float($Ri-$Ti-1)*$x[$Fi] + float($Ti*$x[$Fi+1]); 
		$Yf=float($Ri-$Ti-1)*$y[$Fi] + float($Ti*$y[$Fi+1]);
		$Xa=$Xf+0.001; $Yb=$Yf+0.001; 
		$Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
		$Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
		$Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
		$VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf;

		$NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ;

		$NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ;

		myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
		glVertex3f($VertX, $VertY, $VertZ); 

	    }
	    $Xf=float($Ri*$x[$Fi+1]); 
	    $Yf=float($Ri*$y[$Fi+1]); 
	    $Xa=$Xf+0.001; $Yb=$Yf+0.001; 
	    $Factor=1-((($Xf2=sqr($Xf))+($Yf2=sqr($Yf)))*$AmpVr2); 
	    $Factor1=1-((sqr($Xa)+$Yf2)*$AmpVr2); 
	    $Factor2=1-(($Xf2+sqr($Yb))*$AmpVr2); 
	    $VertX=$Factor*$Xf; $VertY=$Factor*$Yf; $VertZ=$Factor*$Zf; 
	    $NeiAX=$Factor1*$Xa-$VertX; $NeiAY=$Factor1*$Yf-$VertY; $NeiAZ=$Factor1*$Zf-$VertZ; 
	    $NeiBX=$Factor2*$Xf-$VertX; $NeiBY=$Factor2*$Yb-$VertY; $NeiBZ=$Factor2*$Zf-$VertZ; 
	    myVectMul($NeiAX, $NeiAY, $NeiAZ, $NeiBX, $NeiBY, $NeiBZ);
	    glVertex3f($VertX, $VertY, $VertZ); 
	    glEnd();
	} 
    }
}

sub draw_tetra()
{
    my $list = glGenLists( 1 );
    glNewList( $list, GL_COMPILE );
    TRIANGLE(2,$seno,$edgedivisions,0.5/SQRT6);
    glEndList();

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[0]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-tetraangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[1]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+tetraangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[2]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+tetraangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[3]);
    glCallList($list);

    glDeleteLists($list,1);
}

sub draw_cube()
{
    my $list;

    $list = glGenLists( 1 );
    glNewList( $list, GL_COMPILE );
    SQUARE(2, $seno, $edgedivisions, 0.5);
    glEndList();

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[0]);
    glCallList($list);
    glRotatef(cubeangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[1]);
    glCallList($list);
    glRotatef(cubeangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[2]);
    glCallList($list);
    glRotatef(cubeangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[3]);
    glCallList($list);
    glRotatef(cubeangle,0,1,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[4]);
    glCallList($list);
    glRotatef(2*cubeangle,0,1,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[5]);
    glCallList($list);
    
    glDeleteLists($list,1);
}

sub draw_octa()
{
    my $list;

    $list = glGenLists( 1 );
    glNewList( $list, GL_COMPILE );
    TRIANGLE(2,$seno,$edgedivisions,1/SQRT6);
    glEndList();

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[0]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-180+octaangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[1]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-octaangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[2]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-octaangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[3]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[4]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-180+octaangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[5]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-octaangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[6]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-octaangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[7]);
    glCallList($list);
    
    glDeleteLists($list,1);
}

sub draw_dodeca()
{
    my $list = glGenLists( 1 );
    glNewList( $list, GL_COMPILE );
    PENTAGON(1,$seno,$edgedivisions,sqr(TAU) * sqrt((TAU+2)/5) / 2);
    glEndList();

    glPushMatrix();
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[0]);
    glCallList($list);
    glRotatef(180,0,0,1);
    glPushMatrix();
    glRotatef(-dodecaangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[1]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-dodecaangle,cos72,sin72,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[2]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-dodecaangle,cos72,-sin72,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[3]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(dodecaangle,cos36,-sin36,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[4]);
    glCallList($list);
    glPopMatrix();
    glRotatef(dodecaangle,cos36,sin36,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[5]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[6]);
    glCallList($list);
    glRotatef(180,0,0,1);
    glPushMatrix();
    glRotatef(-dodecaangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[7]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-dodecaangle,cos72,sin72,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[8]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-dodecaangle,cos72,-sin72,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[9]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(dodecaangle,cos36,-sin36,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[10]);
    glCallList($list);
    glPopMatrix();
    glRotatef(dodecaangle,cos36,sin36,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[11]);
    glCallList($list);
    
    glDeleteLists($list,1);
}

sub draw_ico()
{
    my $list;

    $list = glGenLists( 1 );
    glNewList( $list, GL_COMPILE );
    TRIANGLE(1.5,$seno,$edgedivisions,(3*SQRT3+SQRT15)/12);
    glEndList();
    
    glPushMatrix();

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[0]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[1]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[2]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[3]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[4]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[5]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[6]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[7]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[8]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[9]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[10]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[11]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[12]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[13]);
    glCallList($list);
    glPopMatrix();
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[14]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[15]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[16]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[17]);
    glCallList($list);
    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-180+icoangle,0.5,-SQRT3/2,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[18]);
    glCallList($list);
    glPopMatrix();
    glRotatef(180,0,0,1);
    glRotatef(-icoangle,1,0,0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, $MaterialColor[19]);
    glCallList($list);

    glDeleteLists($list,1);
}

sub draw ()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();

    glTranslatef( 0.0, 0.0, -10.0 );
    glScalef( Scale*$WindH/$WindW, Scale, Scale );
    glTranslatef(2.5*$WindW/$WindH*sin($step*1.11),2.5*cos($step*1.25*1.11),0);
    glRotatef($step*100,1,0,0);
    glRotatef($step*95,0,1,0);
    glRotatef($step*90,0,0,1);

    $seno=(sin($step)+1.0/3.0)*(4.0/5.0)*$Magnitude;

    $draw_object();
    
    glPopMatrix();

    glFlush();

    glutSwapBuffers();

}

sub idle_()
{
    my $dt;
    my $t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if ($t0 < 0.0)
	$t0 = $t;
    $dt = $t - $t0;
    $t0 = $t;

    $step += $dt;

    glutPostRedisplay();
}

sub reshape($width, $height)
{
    glViewport(0, 0, ($WindW=$width), ($WindH=$height));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode(GL_MODELVIEW);
}

sub key($k, $x, $y)
{
    switch ($k) {
	case 49: $object=1; break; # '1'
	case 50: $object=2; break; # '2'
	case 51: $object=3; break; # '3'
	case 52: $object=4; break; # '4'
	case 53: $object=5; break; # '5'
	case 32: $mono^=1; break; # ' '
	case 115: $smooth^=1; break; # 's'
	case 97: { # 'a'
	    $anim^=1;
	    if ($anim)
		glutIdleFunc( \idle_() );
	    else
		glutIdleFunc();
	    break;
	}
	case 27:
	exit(0);
    }
    pinit();
    glutPostRedisplay();
}

sub pinit()
{
    switch($object) {
	case 1:
	$draw_object=\draw_tetra();
	$MaterialColor[0]=MaterialRed;
	$MaterialColor[1]=MaterialGreen;
	$MaterialColor[2]=MaterialBlue;
	$MaterialColor[3]=MaterialWhite;
	$edgedivisions=tetradivisions;
	$Magnitude=2.5;
	break;
	case 2:
	$draw_object=\draw_cube();
	$MaterialColor[0]=MaterialRed;
	$MaterialColor[1]=MaterialGreen;
	$MaterialColor[2]=MaterialCyan;
	$MaterialColor[3]=MaterialMagenta;
	$MaterialColor[4]=MaterialYellow;
	$MaterialColor[5]=MaterialBlue;
	$edgedivisions=cubedivisions;
	$Magnitude=2.0;
	break;
	case 3:
	$draw_object=\draw_octa();
	$MaterialColor[0]=MaterialRed;
	$MaterialColor[1]=MaterialGreen;
	$MaterialColor[2]=MaterialBlue;
	$MaterialColor[3]=MaterialWhite;
	$MaterialColor[4]=MaterialCyan;
	$MaterialColor[5]=MaterialMagenta;
	$MaterialColor[6]=MaterialGray;
	$MaterialColor[7]=MaterialYellow;
	$edgedivisions=octadivisions;
	$Magnitude=2.5;
	break;
	case 4:
	$draw_object=\draw_dodeca();
	$MaterialColor[ 0]=MaterialRed;
	$MaterialColor[ 1]=MaterialGreen;
	$MaterialColor[ 2]=MaterialCyan;
	$MaterialColor[ 3]=MaterialBlue;
	$MaterialColor[ 4]=MaterialMagenta;
	$MaterialColor[ 5]=MaterialYellow;
	$MaterialColor[ 6]=MaterialGreen;
	$MaterialColor[ 7]=MaterialCyan;
	$MaterialColor[ 8]=MaterialRed;
	$MaterialColor[ 9]=MaterialMagenta;
	$MaterialColor[10]=MaterialBlue;
	$MaterialColor[11]=MaterialYellow;
	$edgedivisions=dodecadivisions;
	$Magnitude=2.0;
	break;
	case 5:
	$draw_object=\draw_ico();
	$MaterialColor[ 0]=MaterialRed;
	$MaterialColor[ 1]=MaterialGreen;
	$MaterialColor[ 2]=MaterialBlue;
	$MaterialColor[ 3]=MaterialCyan;
	$MaterialColor[ 4]=MaterialYellow;
	$MaterialColor[ 5]=MaterialMagenta;
	$MaterialColor[ 6]=MaterialRed;
	$MaterialColor[ 7]=MaterialGreen;
	$MaterialColor[ 8]=MaterialBlue;
	$MaterialColor[ 9]=MaterialWhite;
	$MaterialColor[10]=MaterialCyan;
	$MaterialColor[11]=MaterialYellow;
	$MaterialColor[12]=MaterialMagenta;
	$MaterialColor[13]=MaterialRed;
	$MaterialColor[14]=MaterialGreen;
	$MaterialColor[15]=MaterialBlue;
	$MaterialColor[16]=MaterialCyan;
	$MaterialColor[17]=MaterialYellow;
	$MaterialColor[18]=MaterialMagenta;
	$MaterialColor[19]=MaterialGray;
	$edgedivisions=icodivisions;
	$Magnitude=2.5;
	break;
    }
    if ($mono) {
	my $loop;
	for ($loop=0; $loop<20; $loop++) $MaterialColor[$loop]=MaterialGray;
    }
    if ($smooth) {
	glShadeModel( GL_SMOOTH );
    } else {
	glShadeModel( GL_FLAT );
    }

}

sub main()
{
    printf("Morph 3D - Shows morphing platonic polyhedra\n");
    printf("Author: Marcelo Fernandes Vianna (vianna@cat.cbpf.br)\n\n");
    printf(" [1] - Tetrahedron\n");
    printf(" [2] - Hexahedron (Cube)\n");
    printf(" [3] - Octahedron\n");
    printf(" [4] - Dodecahedron\n");
    printf(" [5] - Icosahedron\n");
    printf("[SPACE] - Toggle colored faces\n");
    printf("[RETURN] - Toggle smooth/flat shading\n");
    printf(" [ESC] - Quit\n");

    $object=1;

    glutInit();
    glutInitWindowPosition(0,0);
    glutInitWindowSize(640,480);

    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB );

    if (glutCreateWindow("Morph 3D - Shows morphing platonic polyhedra") <= 0) {
	exit(0);
    }

    glClearDepth(1.0);
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glColor3f( 1.0, 1.0, 1.0 );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glFlush();
    glutSwapBuffers();

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);

    glHint(GL_FOG_HINT, GL_FASTEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

    pinit();

    glutReshapeFunc( \reshape() );
    glutKeyboardFunc( \key() );
    glutIdleFunc( \idle_() );
    glutDisplayFunc( \draw() );
    glutMainLoop();    
}

main();
