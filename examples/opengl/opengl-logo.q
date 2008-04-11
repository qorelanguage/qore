#!/usr/bin/env qore

%require-our

%requires opengl
%requires glut

# Def_logo.c
# 
# This file is part of the openGL-logo demo.
# (c) Henk Kok (kok@wins.uva.nl)
# 
# Copying, redistributing, etc is permitted as long as this copyright
# notice and the Dutch variable names :) stay in tact.

# ported to Qore by David Nichols
# Dutch variable names left intact by David Nichols

const ACC   = 8;
const ACC2  = 16;
const ACC3  = 48;
const ACC4  = 24;
const THLD  = 0.6;
const THLD2 = 0.8;

our $TRANS;
our $ROTAXIS;
our $ROT;

our $char_El;
our $normal_El;

our $char_O;
our $normal_O;

our $char_P;
our $normal_P;

our $char_G;
our $normal_G;

our ($accSIN, $accCOS);

const difmat4 = ( 0.425, 0.570, 0.664, 1.0 );
const difamb4 = ( 0.425, 0.570, 0.664, 1.0 );
const matspec4 = ( 0.174, 0.174, 0.174, 1.0 );

const lightpos = ( 1.0, 1.0, 1.0, 0.0 );
const lightamb = ( 0.3, 0.3, 0.3, 1.0 );
const lightdif = ( 0.8, 0.8, 0.8, 1.0 );
our $speed=0.0;
our $progress = 1.0;

sub rnd($i)
{
    return rand() % $i;
}

# technically, I could rename this function since it's not a variable
# name, and groen is definitely dutch.  however I will resist this
# great temptation and leave the name in its original language... :-)
# david
sub groen_texture()
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difmat4);
    glMaterialfv(GL_FRONT, GL_AMBIENT, difamb4);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matspec4);
    glMaterialf(GL_FRONT, GL_SHININESS, 35.0);
}

sub def_O()
{
    my ($a, $s, $c, $ln);
    my ($i,$j,$k,$l,$m,$n);
    my ($dx, $dy);
    my ($dx1, $dx2, $dy1, $dy2, $dz1, $dz2);
    my $center_O;
    my $width_O;
    for ($i=0;$i<ACC4;$i++)
    {
	$a = 2.0*$i*M_PI/ACC4;
	$s = 1+sin($a);
	$c = cos($a);
	$center_O[$i][0] = $c*3.8;
	$center_O[$i][1] = $s*3.8+($s < -1.01 ? -0.8 : ($s > 1.01 ? 0.8 : 0)) + 0.8;
	$center_O[$i][2] = 0.0;
	$width_O[$i] = 0.6;
    }
    # /* I should be able to generalise this. oh well */
    for ($i=0; $i< ACC4;$i++)
    {
	$j = ($i+1)  %  ACC4;
	$k = ($i+ACC4-1) % ACC4;
	for ($a=0;$a<ACC;$a++)
	{
	    $c = cos($a*M_PI*2.0/ACC);
	    $s = sin($a*M_PI*2.0/ACC);
	    $dx = $center_O[$j][0] - $center_O[$k][0];
	    $dy = $center_O[$j][1] - $center_O[$k][1];
	    $ln = sqrt($dx*$dx+$dy*$dy);
	    $dx = $dx/$ln;
	    $dy = $dy/$ln;
	    $char_O[$i][$a][0] = $center_O[$i][0] + $width_O[$i] * $dy * $c;
	    $char_O[$i][$a][1] = $center_O[$i][1] - $width_O[$i] * $dx * $c;
	    $char_O[$i][$a][2] = ($s<-THLD?-THLD:($s>THLD?THLD:$s));
	}
    }
    for ($i=0;$i<ACC;$i++)
    {
	$j = ($i+1) % ACC;
	$k = ($i-1+ACC) % ACC;
	for ($l=0;$l<ACC4;$l++)
	{
	    $m = ($l+1) % ACC4;
	    $n = ($l+ACC4-1) % ACC4;
	    $dx1 = $char_O[$m][$i][0] - $char_O[$n][$i][0];
	    $dy1 = $char_O[$m][$i][1] - $char_O[$n][$i][1];
	    $dz1 = $char_O[$m][$i][2] - $char_O[$n][$i][2];
	    $dx2 = $char_O[$l][$k][0] - $char_O[$l][$j][0];
	    $dy2 = $char_O[$l][$k][1] - $char_O[$l][$j][1];
	    $dz2 = $char_O[$l][$k][2] - $char_O[$l][$j][2];
	    $normal_O[$l][$i][0] = $dy2*$dz1 - $dy1*$dz2;
	    $normal_O[$l][$i][1] = $dz2*$dx1 - $dz1*$dx2;
	    $normal_O[$l][$i][2] = $dx2*$dy1 - $dx1*$dy2;
	}
    }
}

sub def_P()
{
    my ($a, $s, $c, $ln);
    my ($i,$j,$k,$l,$m,$n);
    my ($dx, $dy);
    my ($dx1, $dx2, $dy1, $dy2, $dz1, $dz2);
    my $center_P;
    my $width_P;
    for ($i=0;$i<ACC2;$i++)
    {
	$a = 2.0*$i*M_PI/ACC2;
	$s = 1+sin($a);
	$c = cos($a);
	$center_P[$i][0] = $c*2.15;
	$center_P[$i][1] = $s*2.1 + ($s<-1.01?-0.7:($s>1.01?0.7:0)) + 0.7;
	$center_P[$i][2] = 0.0;
	$width_P[$i] = 0.5;
    }

    for ($i=0;$i<ACC2;$i++)
    {
	$j = ($i+1) % ACC2;
	$k = ($i+ACC2-1) % ACC2;
	for ($a=0;$a<ACC;$a++)
	{
	    $accCOS[$a] = $c = cos($a*M_PI*2.0/ACC);
	    $accSIN[$a] = $s = sin($a*M_PI*2.0/ACC);
	    $dx = $center_P[$j][0] - $center_P[$k][0];
	    $dy = $center_P[$j][1] - $center_P[$k][1];
	    $ln = sqrt($dx*$dx+$dy*$dy);
	    $dx = $dx/$ln;
	    $dy = $dy/$ln;
	    $char_P[$i][$a][0] = $center_P[$i][0] + $width_P[$i] * $dy * $c;
	    $char_P[$i][$a][1] = $center_P[$i][1] - $width_P[$i] * $dx * $c;
	    $char_P[$i][$a][2] = ($s<-THLD?-THLD:($s>THLD?THLD:$s));
	}
    }
    for ($i=0;$i<ACC;$i++)
    {
	$j = ($i+1) % ACC;
	$k = ($i-1+ACC) % ACC;
	for ($l=0;$l<ACC2;$l++)
	{
	    $m = ($l+1) % ACC2;
	    $n = ($l+ACC2-1) % ACC2;
	    $dx1 = $char_P[$m][$i][0] - $char_P[$n][$i][0];
	    $dy1 = $char_P[$m][$i][1] - $char_P[$n][$i][1];
	    $dz1 = $char_P[$m][$i][2] - $char_P[$n][$i][2];
	    $dx2 = $char_P[$l][$k][0] - $char_P[$l][$j][0];
	    $dy2 = $char_P[$l][$k][1] - $char_P[$l][$j][1];
	    $dz2 = $char_P[$l][$k][2] - $char_P[$l][$j][2];
	    $normal_P[$l][$i][0] = $dy2*$dz1 - $dy1*$dz2;
	    $normal_P[$l][$i][1] = $dz2*$dx1 - $dz1*$dx2;
	    $normal_P[$l][$i][2] = $dx2*$dy1 - $dx1*$dy2;
	}
    }
}

sub def_El()
{
    my ($a, $s, $c, $ln);
    my ($i,$j,$k,$l,$m,$n);
    my ($dx, $dy);
    my ($dx1, $dx2, $dy1, $dy2, $dz1, $dz2);
    my $center_El;
    my $width_El;
    for ($i=0;$i<ACC3+1;$i++)
    {
	#/* $a = (ACC3/24 + i*11/12)*M_PI*2.0/ACC3; */
	$a = (ACC3/8 + $i * 3/4)*M_PI * 2.0/ACC3;
	$s = 1+sin($a);
	$c = cos($a);
	$center_El[$i][0] = $c*18.0;
	$center_El[$i][1] = $s*9.3;
	$center_El[$i][2] = 0.0;
	$width_El[$i] = pow(3.5, sin($i*M_PI/ACC3))-0.6;
    }

    for ($i=0;$i<ACC3+1;$i++)
    {
	$j = ($i+1) % ACC3;
	$k = ($i+ACC3-1) % ACC3;
	for ($a=0;$a<ACC;$a++)
	{
	    $c = cos($a*M_PI*2.0/ACC);
	    $s = sin($a*M_PI*2.0/ACC);
	    $dx = $center_El[$j][0] - $center_El[$k][0];
	    $dy = $center_El[$j][1] - $center_El[$k][1];
	    $ln = sqrt($dx*$dx+$dy*$dy);
	    $dx = $dx/$ln;
	    $dy = $dy/$ln;
	    $char_El[$i][$a][0] = $center_El[$i][0] + $width_El[$i] * $dy * $c;
	    $char_El[$i][$a][1] = $center_El[$i][1] - $width_El[$i] * $dx * $c;
	    $char_El[$i][$a][2] = ($s<-THLD2?-THLD2:($s>THLD2?THLD2:$s));
	}
    }
    for ($i=0;$i<ACC+1;$i++)
    {
	$j = ($i+1) % ACC;
	$k = ($i-1+ACC) % ACC;
	for ($l=0;$l<ACC3;$l++)
	{
	    $m = ($l+1) % ACC3;
	    $n = ($l+ACC3-1) % ACC3;
	    $dx1 = $char_El[$m][$i][0] - $char_El[$n][$i][0];
	    $dy1 = $char_El[$m][$i][1] - $char_El[$n][$i][1];
	    $dz1 = $char_El[$m][$i][2] - $char_El[$n][$i][2];
	    $dx2 = $char_El[$l][$k][0] - $char_El[$l][$j][0];
	    $dy2 = $char_El[$l][$k][1] - $char_El[$l][$j][1];
	    $dz2 = $char_El[$l][$k][2] - $char_El[$l][$j][2];
	    $normal_El[$l][$i][0] = $dy2*$dz1 - $dy1*$dz2;
	    $normal_El[$l][$i][1] = $dz2*$dx1 - $dz1*$dx2;
	    $normal_El[$l][$i][2] = $dx2*$dy1 - $dx1*$dy2;
	}
    }
}

sub def_G()
{
    my ($a, $s, $c, $ln);
    my ($i,$j,$k,$l,$m,$n);
    my ($dx, $dy);
    my ($dx1, $dx2, $dy1, $dy2, $dz1, $dz2);
    my $center_G;
    my $width_G;
    for ($i=0;$i<ACC4;$i++)
    {
	$a = 2.0*$i*M_PI/ACC4;
	$s = 1+sin($a);
	$c = cos($a);
	$center_G[$i][0] = $c*3.8;
	$center_G[$i][1] = $s*3.8+($s<-1.01?-0.8:($s>1.01?0.8:0)) + 0.8;
	$center_G[$i][2] = 0.0;
	$width_G[$i] = 0.9;
	if ($i>ACC4*3/4)
	    $width_G[$i] = 0.9 - (($i-ACC4*3/4)*0.9)/ACC;
    }
    for ($i=0;$i<ACC4;$i++)
    {
	$j = ($i+1) % ACC4;
	$k = ($i+ACC4-1) % ACC4;
	for ($a=0;$a<ACC;$a++)
	{
	    $c = cos($a*M_PI*2.0/ACC);
	    $s = sin($a*M_PI*2.0/ACC);
	    $dx = $center_G[$j][0] - $center_G[$k][0];
	    $dy = $center_G[$j][1] - $center_G[$k][1];
	    $ln = sqrt($dx*$dx+$dy*$dy);
	    $dx = $dx/$ln;
	    $dy = $dy/$ln;
	    $char_G[$i][$a][0] = $center_G[$i][0] + $width_G[$i] * $dy * $c;
	    $char_G[$i][$a][1] = $center_G[$i][1] - $width_G[$i] * $dx * $c;
	    $char_G[$i][$a][2] = ($s<-THLD?-THLD:($s>THLD?THLD:$s));
	}
    }
    for ($i=0;$i<ACC;$i++)
    {
	$j = ($i+1) % ACC;
	$k = ($i-1+ACC) % ACC;
	for ($l=0;$l<ACC4;$l++)
	{
	    $m = ($l+1) % ACC4;
	    $n = ($l+ACC4-1) % ACC4;
	    $dx1 = $char_G[$m][$i][0] - $char_G[$n][$i][0];
	    $dy1 = $char_G[$m][$i][1] - $char_G[$n][$i][1];
	    $dz1 = $char_G[$m][$i][2] - $char_G[$n][$i][2];
	    $dx2 = $char_G[$l][$k][0] - $char_G[$l][$j][0];
	    $dy2 = $char_G[$l][$k][1] - $char_G[$l][$j][1];
	    $dz2 = $char_G[$l][$k][2] - $char_G[$l][$j][2];
	    $normal_G[$l][$i][0] = $dy2*$dz1 - $dy1*$dz2;
	    $normal_G[$l][$i][1] = $dz2*$dx1 - $dz1*$dx2;
	    $normal_G[$l][$i][2] = $dx2*$dy1 - $dx1*$dy2;
	}
    }
}

sub randomize()
{
    for (my $i=0;$i<7;$i++)
    {
	$TRANS[$i][0] = rnd(100)-rnd(100);
	$TRANS[$i][1] = rnd(100)-rnd(100);
	$TRANS[$i][1] = rnd(100)-rnd(100);
	$ROTAXIS[$i][0] = rnd(100)-rnd(100);
	$ROTAXIS[$i][1] = rnd(100)-rnd(100);
	$ROTAXIS[$i][1] = rnd(100)-rnd(100);
	$ROT[$i]=rnd(3600)-rnd(3600);
    }
}

sub def_logo()
{
    def_O();
    def_P();
    def_El();
    def_G();
}

sub draw_O()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	for ($j=0;$j<ACC4;$j++)
	{
	    glNormal3f($normal_O[$j][$k][0], $normal_O[$j][$k][1], $normal_O[$j][$k][2]);
	    glVertex3f($char_O[$j][$k][0], $char_O[$j][$k][1], $char_O[$j][$k][2]);
	    glNormal3f($normal_O[$j][$i][0], $normal_O[$j][$i][1], $normal_O[$j][$i][2]);
	    glVertex3f($char_O[$j][$i][0], $char_O[$j][$i][1], $char_O[$j][$i][2]);
	}
	glNormal3f($normal_O[0][$k][0], $normal_O[0][$k][1], $normal_O[0][$k][2]);
	glVertex3f($char_O[0][$k][0], $char_O[0][$k][1], $char_O[0][$k][2]);
	glNormal3f($normal_O[0][$i][0], $normal_O[0][$i][1], $normal_O[0][$i][2]);
	glVertex3f($char_O[0][$i][0], $char_O[0][$i][1], $char_O[0][$i][2]);
	glEnd();
    }
}

sub draw_P()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	for ($j=0;$j<ACC2;$j++)
	{
	    glNormal3f($normal_P[$j][$k][0], $normal_P[$j][$k][1], $normal_P[$j][$k][2]);
	    glVertex3f($char_P[$j][$k][0], $char_P[$j][$k][1], $char_P[$j][$k][2]);
	    glNormal3f($normal_P[$j][$i][0], $normal_P[$j][$i][1], $normal_P[$j][$i][2]);
	    glVertex3f($char_P[$j][$i][0], $char_P[$j][$i][1], $char_P[$j][$i][2]);
	}
	glNormal3f($normal_P[0][$k][0], $normal_P[0][$k][1], $normal_P[0][$k][2]);
	glVertex3f($char_P[0][$k][0], $char_P[0][$k][1], $char_P[0][$k][2]);
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0], $char_P[0][$i][1], $char_P[0][$i][2]);
	glEnd();
    }
    $j = 0;
    glBegin(GL_QUAD_STRIP);
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0]-4.3, -1.6, 1.0*$char_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0]-4.3, 6.0, 1.0*$char_P[0][$i][2]);
    }
    glNormal3f($normal_P[0][0][0], $normal_P[0][0][1], $normal_P[0][0][2]);
    glVertex3f($char_P[0][0][0]-4.3, -1.6, 1.0*$char_P[0][0][2]);
    glVertex3f($char_P[0][0][0]-4.3, 6.0, 1.0*$char_P[0][0][2]);
    glEnd();
}

sub draw_E()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	glNormal3f($normal_P[0][$k][0], $normal_P[0][$k][1], $normal_P[0][$k][2]);
	glVertex3f($char_P[0][$k][0], $char_P[0][$k][1]+0.0, $char_P[0][$k][2]);
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0], $char_P[0][$i][1]+0.0, $char_P[0][$i][2]);
	for ($j=1;$j<ACC2;$j++)
	{
	    glNormal3f($normal_P[$j][$k][0], $normal_P[$j][$k][1], $normal_P[$j][$k][2]);
	    glVertex3f($char_P[$j][$k][0], $char_P[$j][$k][1], $char_P[$j][$k][2]);
	    glNormal3f($normal_P[$j][$i][0], $normal_P[$j][$i][1], $normal_P[$j][$i][2]);
	    glVertex3f($char_P[$j][$i][0], $char_P[$j][$i][1], $char_P[$j][$i][2]);
	}
	glNormal3f($normal_P[0][$k][0], $normal_P[0][$k][1], $normal_P[0][$k][2]);
	glVertex3f($char_P[0][$k][0], $char_P[0][$k][1]-0.4, $char_P[0][$k][2]);
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0], $char_P[0][$i][1]-0.4, $char_P[0][$i][2]);
	glEnd();
    }

    glBegin(GL_QUAD_STRIP);
    $j = ACC2*3/4;
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($normal_P[$j][$i][0], $normal_P[$j][$i][1], $normal_P[$j][$i][2]);
	glVertex3f(-2.0, $char_P[$j][$i][1]+2.55, 1.0*$char_P[$j][$i][2]);
	glVertex3f(2.0,  $char_P[$j][$i][1]+2.55, 1.0*$char_P[$j][$i][2]);
    }
    glNormal3f($normal_P[$j][0][0], $normal_P[$j][0][1], $normal_P[$j][0][2]);
    glVertex3f(-2.0, $char_P[$j][0][1]+2.55, 1.0*$char_P[$j][0][2]);
    glVertex3f(2.0,  $char_P[$j][0][1]+2.55, 1.0*$char_P[$j][0][2]);
    glEnd();
}

sub draw_El()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	for ($j=0;$j<=ACC3;$j++)
	{
	    glNormal3f($normal_El[$j][$k][0], $normal_El[$j][$k][1], $normal_El[$j][$k][2]);
	    glVertex3f($char_El[$j][$k][0], $char_El[$j][$k][1], $char_El[$j][$k][2]);
	    glNormal3f($normal_El[$j][$i][0], $normal_El[$j][$i][1], $normal_El[$j][$i][2]);
	    glVertex3f($char_El[$j][$i][0], $char_El[$j][$i][1], $char_El[$j][$i][2]);
	}
	glEnd();
    }
}

sub draw_N()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	for ($j=0;$j<=ACC2/2;$j++)
	{
	    glNormal3f($normal_P[$j][$k][0], $normal_P[$j][$k][1], $normal_P[$j][$k][2]);
	    glVertex3f($char_P[$j][$k][0], $char_P[$j][$k][1], $char_P[$j][$k][2]);
	    glNormal3f($normal_P[$j][$i][0], $normal_P[$j][$i][1], $normal_P[$j][$i][2]);
	    glVertex3f($char_P[$j][$i][0], $char_P[$j][$i][1], $char_P[$j][$i][2]);
	}
	glEnd();
    }

    $j = 0;
    glBegin(GL_QUAD_STRIP);
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0]-4.3, 0.2, 1.0*$char_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0]-4.3, 6.0, 1.0*$char_P[0][$i][2]);
    }
    glNormal3f($normal_P[0][0][0], $normal_P[0][0][1], $normal_P[0][0][2]);
    glVertex3f($char_P[0][0][0]-4.3, 0.2, 1.0*$char_P[0][0][2]);
    glVertex3f($char_P[0][0][0]-4.3, 6.0, 1.0*$char_P[0][0][2]);
    glEnd();
    $j = 0;

    glBegin(GL_QUAD_STRIP);
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($normal_P[0][$i][0], $normal_P[0][$i][1], $normal_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0], 0.2, 1.0*$char_P[0][$i][2]);
	glVertex3f($char_P[0][$i][0], 3.4, 1.0*$char_P[0][$i][2]);
    }
    glNormal3f($normal_P[0][0][0], $normal_P[0][0][1], $normal_P[0][0][2]);
    glVertex3f($char_P[0][0][0], 0.2, 1.0*$char_P[0][0][2]);
    glVertex3f($char_P[0][0][0], 3.4, 1.0*$char_P[0][0][2]);
    glEnd();
}

sub draw_G()
{
    my ($i,$j,$k);
    for ($i=0;$i<ACC;$i++)
    {
	$k = $i+1;
	if ($k>=ACC)
	    $k = 0;
	glBegin(GL_QUAD_STRIP);
	glNormal3f($normal_G[0][$k][0], $normal_G[0][$k][1], $normal_G[0][$k][2]);
	glVertex3f($char_G[0][$k][0], $char_G[0][$k][1]+1.2, $char_G[0][$k][2]);
	glNormal3f($normal_G[0][$i][0], $normal_G[0][$i][1], $normal_G[0][$i][2]);
	glVertex3f($char_G[0][$i][0], $char_G[0][$i][1]+1.2, $char_G[0][$i][2]);
	for ($j=1;$j<ACC4;$j++)
	{
	    glNormal3f($normal_G[$j][$k][0], $normal_G[$j][$k][1], $normal_G[$j][$k][2]);
	    glVertex3f($char_G[$j][$k][0], $char_G[$j][$k][1], $char_G[$j][$k][2]);
	    glNormal3f($normal_G[$j][$i][0], $normal_G[$j][$i][1], $normal_G[$j][$i][2]);
	    glVertex3f($char_G[$j][$i][0], $char_G[$j][$i][1], $char_G[$j][$i][2]);
	}
	glEnd();
    }

    glBegin(GL_QUAD_STRIP);
    $j = ACC4*3/4;
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($accSIN[$i], 0.0, $accCOS[$i] );
	glVertex3f(4.0+0.9*$accSIN[$i], 4.0+0.9*$accSIN[$i], 0.9*$accCOS[$i] );
	glVertex3f(4.0+0.9*$accSIN[$i], 0.0, 0.9*$accCOS[$i]);
    }
    glNormal3f($accSIN[0], 0.0, $accCOS[0] );
    glVertex3f(4.0+0.9*$accSIN[0], 4.0+0.9*$accSIN[0], 0.9*$accCOS[0] );
    glVertex3f(4.0+0.9*$accSIN[0], 0.0, 0.9*$accCOS[0]);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    $j = ACC4*3/4;
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f(0.0, $accSIN[$i], $accCOS[$i] );
	glVertex3f(4.0-0.9*$accSIN[$i], 4.0-0.9*$accSIN[$i], 0.9*$accCOS[$i] );
	glVertex3f(0.0, 4.0-0.9*$accSIN[$i], 0.9*$accCOS[$i]);
    }
    glNormal3f(0.0, $accSIN[0], $accCOS[0] );
    glVertex3f(4.0-0.9*$accSIN[0], 4.0-0.9*$accSIN[0], 0.9*$accCOS[0] );
    glVertex3f(0.0, 4.0-0.9*$accSIN[0], 0.9*$accCOS[0]);
    glEnd();

    $j = ACC4*3/4;
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(0.0, 4.0, 0.0);
    for ($i=0;$i<ACC;$i++)
	glVertex3f(0.0, 4.0+0.9*$accSIN[$i], 0.9*$accCOS[$i]);
    glVertex3f(0.0, 4.0+0.9*$accSIN[0], 0.9*$accCOS[0]);
    glEnd();
}

sub draw_L()
{
    my $i;

    glBegin(GL_QUAD_STRIP);
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f($accSIN[$i], 0.0, $accCOS[$i] );
	glVertex3f(0.9*$accSIN[$i], 9.6, 0.9*$accCOS[$i]);
	glVertex3f(0.9*$accSIN[$i], 0.9+0.9*$accSIN[$i], 0.9*$accCOS[$i] );
    }
    glNormal3f($accSIN[0], 0.0, $accCOS[0] );
    glVertex3f(0.9*$accSIN[0], 9.6, 0.9*$accCOS[0]);
    glVertex3f(0.9*$accSIN[0], 0.9+0.9*$accSIN[0], 0.9*$accCOS[0] );
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for ($i=0;$i<ACC;$i++)
    {
	glNormal3f(0.0, $accSIN[$i], $accCOS[$i] );
	glVertex3f(0.9*$accSIN[$i], 0.9+0.9*$accSIN[$i], 0.9*$accCOS[$i] );
	glVertex3f(5.6, 0.9+0.9*$accSIN[$i], 0.9*$accCOS[$i]);
    }
    glNormal3f(0.0, $accSIN[0], $accCOS[0] );
    glVertex3f(0.9*$accSIN[0], 0.9+0.9*$accSIN[0], 0.9*$accCOS[0] );
    glVertex3f(5.6, 0.9+0.9*$accSIN[0], 0.9*$accCOS[0]);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(5.6, 0.9, 0.0);
    for ($i=ACC-1;$i>=0;$i--)
	glVertex3f(5.6, 0.9+0.9*$accSIN[$i], 0.9*$accCOS[$i]);
    glVertex3f(5.6, 0.9+0.9*$accSIN[ACC-1], 0.9*$accCOS[ACC-1]);
    glEnd();
}

sub draw_part($i)
{
    glPushMatrix();
    glTranslatef($TRANS[$i][0]*$progress, $TRANS[$i][1]*$progress, $TRANS[$i][2]*$progress);
    glRotatef($ROT[$i]*$progress, $ROTAXIS[$i][0], $ROTAXIS[$i][1], $ROTAXIS[$i][2]);
    switch($i)
    {
	case 0: draw_El(); break;
	case 1: draw_O(); break;
	case 2: draw_P(); break;
	case 3: draw_E(); break;
	case 4: draw_N(); break;
	case 5: draw_G(); break;
	case 6: draw_L(); break;
    }
    glPopMatrix();
}

sub draw_logo()
{
    groen_texture();
    glEnable(GL_CULL_FACE);
    glTranslatef(-2.8, 0.0, 0.0);

    draw_part(0);
    glTranslatef(-12.0, 4.3, 0.0);
    draw_part(1);
    glTranslatef(7.3, 0.0, 0.0);
    draw_part(2);
    glTranslatef(5.4, 0.0, 0.0);
    draw_part(3);
    glTranslatef(5.4, 0.0, 0.0);
    draw_part(4);
    glTranslatef(7.4, 0.0, 0.0);
    draw_part(5);
    glTranslatef(6.8, 0.0, 0.0);
    draw_part(6);
}

sub do_display ()
{
    SetCamera();
    draw_logo();
    glFlush();
    glutSwapBuffers();
}

sub display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    do_display();
}

sub myinit ()
{
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColor3f(1.0, 1.0, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_NORMALIZE);
    def_logo();
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

sub parsekey($key, $x, $y)
{
    switch ($key)
    {
	case 27: exit(0);
	case 13: break;
	case 32: $progress = 1; randomize(); break; # ' '
    }
}

sub parsekey_special($key, $x, $y)
{
    switch ($key)
    {
	case GLUT_KEY_UP:		break;
	case GLUT_KEY_DOWN:		break;
	case GLUT_KEY_RIGHT:	break;
	case GLUT_KEY_LEFT:		break;
    }
}

sub Animate()
{
    $speed = -0.95*$speed + $progress*0.05;
    if ($progress > 0.0 && $speed < 0.0003)
	$speed = 0.0003;
    if ($speed > 0.01)
	$speed = 0.01;
    $progress = $progress - $speed;
    if ($progress < 0.0)
    {
	$progress = 0.0;
	$speed = 0;
    }
    glutPostRedisplay();
}

sub myReshape($w, $h)
{
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, $w, $h);
    glLoadIdentity();
    SetCamera();
}

sub SetCamera()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-0.1333, 0.1333, -0.1, 0.1, 0.2, 150.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,1.5,2, 0,1.5,0, 0,1,0);
    glTranslatef(0.0, -8.0, -45.0);
    glRotatef(-$progress*720, 0.0, 1.0, 0.0);
}

sub main()
{
    glutInit();
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowPosition(200, 0);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Rotating OpenGL Logo");
    glutDisplayFunc(\display());
    glutKeyboardFunc(\parsekey());
    glutSpecialFunc(\parsekey_special());
    glutReshapeFunc(\myReshape());
    glutIdleFunc(\Animate());
    randomize();
    myinit();
    glutSwapBuffers();
    glutMainLoop();
}

main();
