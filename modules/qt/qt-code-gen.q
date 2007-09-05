#!/usr/bin/env qore

%require-our
%enable-all-warnings

const opts = (
    "abstract"        : "a,abstract",
    "abstract_class"  : "n,name=s",
    "indep"           : "i,independent",
    "file"            : "f,file",
    "parent"          : "p,parent=s",
    "widget"          : "w,widget",
    "help"            : "h,help"
    );

const ordinal = ( "first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth", "tenth" );

const class_list = ( "QFont",
		     "QPoint",
		     "QPointF",
		     "QLine",
		     "QLineF",
		     "QPolygon",
		     "QPolygonF",
		     "QMatrix",
		     "QSize",
		     "QPalette",
		     "QRectF",
		     "QRect",
		     "QRegion",
		     "QBrush",
		     "QColor",
		     "QPainter",
		     "QBitmap",
		     "QImage",
		     "QMovie",
		     "QMenu",
		     "QPicture",
		     "QDateTime", "QDate", "QTime", "QKeySequence", "QIcon",
		     "QAction", "QActionGroup", "QActionGroup*",
		     "QKeySequence"
 );

const qobject_list = ( "QWidget", "QMovie", "QAction", "QActionGroup", "QMenu" );

const const_class_list = ("QPalette", "QMovie", "QPixmap", "QPicture" ,"QImage", "QPoint", "QPointF", "QPolygon", "QPolygonF", "QLine", "QLineF", "QMatrix", "QSize", "QColor", "QDateTime", "QDate", "QTime", "QKeySequence", "QIcon", "QFont", "QBrush" );

const abstract_class_list = ( "QObject", "QWidget", "QLayout", "QAbstractButton" );

const dynamic_class_list = ( "QPaintDevice", "QPixmap", 
    );

const spaces = "                                                                        ";

our ($o, $if, $cn);

sub usage()
{
    printf(
"usage: %s input_file class_name
  -i,--independent         emit code for independent class
  -a,--abstract            emit abstract class name
  -n,--abstract_class=ARG  abstract class name
  -w,--widget              is a QWidget
  -p,--parent=ARG          parent class name
  -h,--help                this help text
", basename($ENV."_"));
    exit(1);
}

sub command_line()
{
    my $g = new GetOpt(opts);
    $o = $g.parse(\$ARGV);
    if (exists $o."_ERRORS_")
    {
        printf("%s\n", $o."_ERRORS_"[0]);
        exit(1);
    }
    if ($o.help)
        usage();

    $if = shift $ARGV;
    $cn = shift $ARGV;

    if (!exists $cn)
	usage();

    if (!$o.indep && !exists $o.abstract_class)
	$o.abstract_class = $o.widget ? "QWidget" : "QObject";

    if ($o.indep && $o.widget) {
	stderr.printf("cannot be independent and a QWidget at the same time\n");
	exit(1);
    }

    if (!$o.indep && !exists $o.parent)
	$o.parent = "parent";
}

sub dl($l)
{
    my $str;
    foreach my $e in ($l)
	$str += $e + ", ";
    return substr($str, 0, -2);
}

sub dlh($l)
{
    my $str;
    foreach my $e in ($l)
	$str += sprintf("(%s) %s, ", $e.type, $e.name);
    return substr($str, 0, -2);
}

sub inlist($val, $list)
{
    foreach my $v in ($list)
        if ($val == $v)
            return True;
    return False;
}

sub do_return_class($type, $callstr)
{
    $type =~ s/&//g;
    $type =~ s/\*//g;
    my $cl = $type;
    $cl =~ s/[a-z]//g;
    $cl = tolower($cl);

    if (inlist($type, const_class_list) && $type =~ /\*/)
	$callstr = "*(" + $callstr + ")";

    my $lo = ();

    my $utn = toupper($type);

    if ($type == $cn)
	$lo += sprintf("Object *o_%s = new Object(self->getClass(CID_%s), getProgram());", $cl, $utn);
    else {
	$lo += sprintf("Object *o_%s = new Object(QC_%s, getProgram());", $cl, $type);
    }
    if (inlist($type, qobject_list))
	$lo += sprintf("Qore%s *q_%s = new Qore%s(o_%s, %s);", $type, $cl, $type, $cl, $callstr);
    else
	$lo += sprintf("Qore%s *q_%s = new Qore%s(%s);", $type, $cl, $type, $callstr);
	
    $lo += sprintf("o_%s->setPrivate(CID_%s, q_%s);", $cl, $utn, $cl);
    $lo += sprintf("return new QoreNode(o_%s);", $cl);

    return $lo;
}

sub do_class($arg, $name, $cn, $i, $d, $const)
{
    my $lo = ();
    
    my $type = $arg.type;
    $type =~ s/\*//;
    $type =~ s/\&//;
    my $utn = toupper($type);

    my $ref = inlist($type, const_class_list); # && ($arg.type =~ /\*/);

    my $tcn = sprintf("Qore%s%s", $d ? "" : "Abstract", $type);

    $arg.get = 
	$d 
	? ($ref
	   ? sprintf("*(static_cast<%s *>(%s))", $type, get_class($arg.name, $type))
	   : sprintf("static_cast<%s *>(%s)", $type, get_class($arg.name, $type)))
	: ($ref
	   ? sprintf("*(%s->get%s())", $arg.name, $type)
	   : sprintf("%s->get%s()", $arg.name, $type));

    $lo += sprintf("   %s *%s = (p && p->type == NT_OBJECT) ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		   $tcn, $arg.name, $tcn, $utn);
    $lo += sprintf("   if (!%s) {", $arg.name);
    $lo += "      if (!xsink->isException())";
    $lo += sprintf("         xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a %s object as %s argument to %s::%s()\");", 
		   toupper($cn), toupper($name), $type, ordinal[$i], $cn, $name);
    $lo += $const ? "      return;" : "      return 0;";
    $lo += "   }";
    $lo += sprintf("   ReferenceHolder<%s> %sHolder(%s, xsink);", $tcn, $arg.name, $arg.name);

    return $lo;
}

sub do_dynamic_class($arg, $name, $cn, $i, $const)
{
    my $lo = ();
    
    my $type = $arg.type;
    $type =~ s/\*//;
    $type =~ s/\&//;
    my $utn = toupper($type);

    my $tcn = sprintf("QoreAbstract%s", $type);

    my $ref = inlist($type, const_class_list); # && ($arg.type =~ /\*/);

    $arg.get = 
	($ref
	 ? sprintf("*(%s->get%s())", $arg.name, $type)
	 : sprintf("%s->get%s()", $arg.name, $type));

    $lo += sprintf("   AbstractPrivateData *apd_%s = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		   $arg.name, $utn);
    $lo += sprintf("   if (!apd_%s) {", $arg.name);
    $lo += "      if (!xsink->isException())";
    $lo += sprintf("         xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a %s object as %s argument to %s::%s()\");", 
		   toupper($cn), toupper($name), $type, ordinal[$i], $cn, $name);
    $lo += $const ? "      return;" : "      return 0;";
    $lo += "   }";
    $lo += sprintf("   ReferenceHolder<AbstractPrivateData> holder(apd_%s, xsink);", $arg.name);
    $lo += sprintf("   %s *%s = dynamic_cast<%s *>(apd_%s);", $tcn, $arg.name, $tcn, $arg.name);
    $lo += sprintf("   assert(%s);", $arg.name);

    return $lo;
}

sub get_file($fn)
{
    #return stdout;

    if (is_file($fn)) {
	my $nfn = $fn + ".orig";
	if (is_file($nfn)) {
	    my $suf = 1;
	    while (is_file(sprintf("%s.%d", $nfn, $suf)))
		++$suf;
	    $nfn = sprintf("%s.%d", $nfn, $suf);
	}
	printf("backing up %s as %s\n", $fn, $nfn);
	system(sprintf("mv %s %s", $fn, $nfn));
    }
    else
	printf("creating new file %s\n", $fn);
    my $of = new File();
    $of.open($fn, O_CREAT | O_TRUNC | O_WRONLY);
    return $of;
}

sub add_new_build_files($fp)
{
    my $cc = $fp + ".cc";
    my $hh = $fp + ".h";
    # see if the file is already present in single-compilation-unit.cc
    my $str = "grep " + $cc + " single-compilation-unit.cc";
    if (!strlen(trim(backquote($str))))
	system("printf '#include \\\"" + $cc + "\\\"\\n'>> single-compilation-unit.cc");

    # add to qt/Makefile.am
    if (!strlen(trim(backquote("grep " + $cc + " Makefile.am")))) {
	my $lines = split("\n", `cat Makefile.am`);
	my $of = get_file("Makefile.am");
	my $done;
	for (my $i = 0; $i < elements $lines; ++$i) {
	    
	    if (!$done && $lines[$i + 1] =~ /endif/) {
	        trim $lines[$i];
		$of.printf("\t%s \\\n", $lines[$i]);
		$of.printf("\t%s\n", $cc);
		$done = True;
		continue;
	    }
	    $of.printf("%s\n", $lines[$i]);
	}
    }    

    # add to root Makefile.am
    if (!strlen(trim(backquote("grep " + $hh + " ../../Makefile.am")))) {
	my $lines = split("\n", `cat ../../Makefile.am`);
	my $of = get_file("../../Makefile.am");
	my ($found, $done);
	for (my $i = 0; $i < elements $lines; ++$i) {
	    if (!$done) {
		if (!$found && $lines[$i] =~ /modules\/qt/) {
		    $found = True;
		}
		else if ($found && $lines[$i] !~ /modules\/qt/) {
		    $of.printf("\tmodules/qt/%s \\\n", $hh);
		    $done = True;
		}
	    }
	    $of.printf("%s\n", $lines[$i]);
	}
    }

    # add to qt.cc
    if (!strlen(trim(backquote("grep " + $hh + " qt.cc")))) {
	my $lines = split("\n", `cat qt.cc`);
	my $of = get_file("qt.cc");
	my ($found, $done);
	for (my $i = 0; $i < elements $lines; ++$i) {
	    if (!$done) {
		if (!$found && $lines[$i] =~ /#include "QC_/) {
		    $found = True;
		}
		else if ($found && $lines[$i] !~ /#include "QC_/) {
		    $of.printf("#include \"%s\"\n", $hh);
		    $done = True;
		}
	    }
	    else if ($lines[$i + 1] =~ /ColorRole enum/)
		$of.printf("   qt->addSystemClass(init%sClass(%s));\n", $cn, tolower($o.parent));

	    $of.printf("%s\n", $lines[$i]);
	}	
    }
}

sub main()
{
    command_line();

    my $func_prefix = toupper($cn);
    my $qore_class = $o.abstract ? "Abstract" + $cn : $cn;

    my $get_obj = $o.abstract ? sprintf("get%s()", $cn) : "qobj";

    my $cl = $cn;
    $cl =~ s/[a-z]//g;
    $cl = tolower($cl);
    #printf("%s=%n\n", $cn, $cl);

    # get prototype list
    if (!is_readable($if))
	throw "MISSING-FILE", sprintf("can't read input file '%s'", $if);

    my $l = backquote(sprintf("cat %s | grep \\(", $if));
    $l = split("\n", $l);
    trim $l;

    my $proto = ();

    foreach my $p in ($l) {
	my $t = $p;
	$t =~ s/ \* /* /g;
	$t =~ s/ & /& /g;
	$t =~ s/const / /g;
	$t =~ s/virtual / /g;
	$t =~ s/\) const.*/)/; 
	$t =~ s/ \*>/*>/;
	$t =~ s/  / /g;
	trim $t;
	#printf("%s\n", $t);

	my ($rt, $name, $args) = $t =~ x/([a-zA-Z0-9\*<>:&]+) (\w+) \((.*)\)/;
	if (!exists $rt) { # must be a constructor or destructor
	    if ($t =~ /~/) # skip the destructor
		continue;
	    ($name, $args) = $t =~ x/(\w+) \((.*)\)/;
	}
	my $orig_args = trim($args);
	$args = split(",", $args);
	trim $args;
	foreach my $arg in (\$args)
	{
	    #printf("arg=%n\n", $arg);
	    $arg =~ s/const //;
	    my ($ra, $def) = $arg =~ x/(.*)=(.*)/;
	    if (exists $def)
		$arg = trim($ra);
	    my ($type, $pname) = $arg =~ x/([a-zA-Z0-9\*<>:&]+) (\w+)/;
	    if (!exists $type) {
		$type = $arg;
		if ($type == "int" || $type == "float")
		    $pname = "x";
		else if ($type == "bool")
		    $pname = "b";
		else {
		    $pname = tolower($arg);
		    $pname =~ s/\*//g;
		    $pname =~ s/&//g;
		    $pname =~ s/.*:://;
		}
		$arg =~ s/&//;
	    }
	    my $ref;
	    if ($type =~ /&/) {
		$type =~ s/&//;
		$ref = True;
	    }
	    $arg = ( "type" : $type, "name" : $pname, "def" : $def, "ref" : $ref );
	    my $typename = $type;
	    $typename =~ s/\*//g;
	    $typename =~ s/&//g;
	    if (inlist($typename, class_list)) {
		$arg.is_class = True;
		$arg.classname = $typename;
	    }
	    else if (inlist($typename, abstract_class_list)) {
		$arg.is_abstract_class = True;
		$arg.classname = $typename;
	    }
	    else if (inlist($typename, dynamic_class_list)) {
		$arg.is_dynamic_class = True;
		$arg.classname = $typename;
	    }
	    else if ($type == "float" || $type == "double")
		$arg.is_float = True;
	    else if (!$ref && $type !~ /\*/ && $type !~ /&/)
		$arg.is_int = True;

	    #printf("%N\n", $arg);
	}

	if (!exists $proto.$name)
	    $proto.$name = ( "funcname" : sprintf("%s_%s", $func_prefix, $cn == $name ? "constructor" : $name), 
			     "rt" : $rt, "inst" : () );

	$proto.$name.inst += ( "args" : $args, "orig" : $p, "orig_args" : $orig_args );
    }

    foreach my $p in (keys $proto) {	
	# do function header
	$proto.$p.ok = True;
	my $lo = ();

	my $hdr = ();
	foreach my $i in (\$proto.$p.inst) {
	    $hdr += sprintf("//%s", $i.orig);
            # delete constant constructors
            #printf("%n %n i=%n\n", elements $i.args, $i.args[0].type, $i);
            if ($p == $cn && elements $i.args == 1 && $i.args[0].type == $cn) { 
                delete $i;
                continue;
            }
        }
	if ($p == $cn)
	    $lo += sprintf("static void %s(Object *self, QoreNode *params, ExceptionSink *xsink)", 
			   $proto.$p.funcname, $cl);
	else
	    $lo += sprintf("static QoreNode *%s(Object *self, Qore%s *%s, QoreNode *params, ExceptionSink *xsink)", 
			   $proto.$p.funcname, $qore_class, $cl);
	$lo += "{";

	my $callstr;
	if (exists $proto.$p.rt)
	    $callstr = $o.indep ? sprintf("%s->%s(", $cl, $p) : sprintf("%s->%s->%s(", $cl, $get_obj, $p);
	else { # for constructor calls
	    $callstr = sprintf("new Qore%s(", $p);
	    if (!$o.indep)
		$callstr += "self, ";
	}
	if (elements $proto.$p.inst == 1)
	    $lo += do_single_function($p, \$proto.$p, $proto.$p.inst[0], $callstr);
	else
	    $lo += do_multi_function($p, \$proto.$p, $proto.$p.inst, $callstr);

	$lo += "}";

	$proto.$p.code = $hdr + $lo; 
    
	#printf("%-15s %-20s (%s)\n", "(" + $rt + ")", $name, dlh($args));
    }

    my $of;
    if (exists $o.file) {
	add_new_build_files("QC_" + $cn);

	$of = get_file("QC_" + $cn + ".h");
	$of.printf(
"/*
 QC_%s.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _QORE_QT_QC_%s_H

#define _QORE_QT_QC_%s_H

#include <%s>
", $cn, $func_prefix, $func_prefix, $cn);

	if (exists $o.abstract_class) {
	    $of.printf("#include \"QoreAbstract%s.h\"\n", $o.abstract_class);
	    $of.printf("#include \"qore-qt-events.h\"\n");
	}

	$of.printf("
DLLLOCAL extern int CID_%s;
DLLLOCAL extern class QoreClass *QC_%s;

DLLLOCAL class QoreClass *init%sClass(%s);

", $func_prefix, $cn, $cn, exists $o.parent ? "QoreClass *" : "");

	if ($o.indep) {
	    $of.printf("class Qore%s : public AbstractPrivateData, public %s\n", $cn, $cn);
	    $of.printf("{
   public:
", $cn, $cn);
	    
	    if (exists $proto.$cn) {
		foreach my $i in ($proto.$cn.inst) {
		    my $arg_names;
		    foreach my $a in ($i.args)
			$arg_names += $a.name + ", ";
		    if (exists $arg_names)
			splice $arg_names, -2;
		    $of.printf("      DLLLOCAL Qore%s(%s) : %s(%s)\n      {\n      }\n", $cn, $i.orig_args, $cn, $arg_names);
		}
	    }
	    $of.printf("};\n");
	}
	else # abstract class
	{
	    $of.printf("class my%s : public %s\n", $cn, $cn);
	    $of.printf("{
#define QOREQTYPE %s
#include \"qore-qt-metacode.h\"
#include \"qore-qt-widget-events.h\"
#undef QOREQTYPE

   public:
", $cn, $cn, $cn);
	    
	    if (exists $proto.$cn) {
		foreach my $i in ($proto.$cn.inst) {
		    my $arg_names;
		    foreach my $a in ($i.args)
			$arg_names += $a.name + ", ";
		    if (exists $arg_names)
			splice $arg_names, -2;
		    my $str = "Object *obj";
		    if (exists $i.orig_args)
			$str += ", " + $i.orig_args;
		    $of.printf("      DLLLOCAL my%s(%s) : %s(%s)\n      {\n", $cn, $str, $cn, $arg_names);
		    $of.printf("         init(obj);\n");
		    if ($o.widget)
			$of.printf("         init_widget_events();\n");
		    $of.printf("      }\n"); 
		}
	    }
	    $of.printf("};\n\n");

	    $of.printf("class Qore%s : public QoreAbstract%s\n", $cn, $o.abstract_class);
	    $of.printf("{
   public:
      QPointer<my%s> qobj;

", $cn, $cn, $cn);
	    
	    if (exists $proto.$cn) {
		foreach my $i in ($proto.$cn.inst) {
		    my $arg_names;
		    foreach my $a in ($i.args)
			$arg_names += $a.name + ", ";
		    if (exists $arg_names)
			splice $arg_names, -2;
		    my $str = "Object *obj";
		    if (exists $i.orig_args)
			$str += ", " + $i.orig_args;
		    $of.printf("      DLLLOCAL Qore%s(%s) : qobj(new my%s(obj, %s))\n      {\n      }\n", $cn, $str, $cn, $arg_names);
		}
	    }
	    $of.printf("      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
");
	    if ($o.widget)
		$of.printf("      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
");
	    if ($o.abstract_class != "QObject" && $o.abstract_class != "QWidget")
		$of.printf("      DLLLOCAL virtual class %s *get%s() const
      {
         return static_cast<%s *>(&(*qobj));
      }
", $o.abstract_class, $o.abstract_class, $o.abstract_class);

	    $of.printf("      QORE_VIRTUAL_QOBJECT_METHODS
};\n");
	}

	$of.printf("\n#endif // _QORE_QT_QC_%s_H\n", $func_prefix);

	$of = get_file("QC_" + $cn + ".cc");
    }
    else
	$of = stdout;

    # do file prefix
    if (exists $o.file) {
	$of.printf("/*
 QC_%s.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

#include \"QC_%s.h\"

int CID_%s;
class QoreClass *QC_%s = 0;

", $cn, $cn, $func_prefix, $cn);
    }

    foreach my $p in (keys $proto) {
	foreach my $line in ($proto.$p.code)
	    $of.printf("%s%s\n", $proto.$p.ok ? "" : "//", $line);
	
	$of.printf("\n");

	if ($p == $cn) {
	    $of.printf("static void %s_copy(class Object *self, class Object *old, class Qore%s *%s, ExceptionSink *xsink)
{
   xsink->raiseException(\"%s-COPY-ERROR\", \"objects of this class cannot be copied\");
}

", $func_prefix, $cn, $cl, $func_prefix);
	}
    }

    # do init function
    if (exists $o.file) {
	$of.printf("QoreClass *init%sClass(%s)\n{\n", $cn, exists $o.parent ? "QoreClass *" + tolower($o.parent) : "");
	$of.printf("   QC_%s = new QoreClass(\"%s\", QDOM_GUI);\n", $cn, $cn);
	$of.printf("   CID_%s = QC_%s->getID();\n\n", $func_prefix, $cn);

	if (exists $o.parent)
	    $of.printf("   QC_%s->addBuiltinVirtualBaseClass(%s);\n\n", $cn, tolower($o.parent));

	$of.printf("   QC_%s->setConstructor(%s_constructor);\n", $cn, $func_prefix);
        $of.printf("   QC_%s->setCopy((q_copy_t)%s_copy);\n\n", $cn, $func_prefix);
    }

    foreach my $p in (keys $proto) {	
	if ($p != $cn)
	    $of.printf("   %sQC_%s->addMethod(%-30s (q_method_t)%s);\n", $proto.$p.ok ? "" : "//", $cn, "\""+$p+"\",", $proto.$p.funcname);
    }

    if (exists $o.file) {
	$of.printf("\n   return QC_%s;\n}\n", $cn);
    }
}

sub get_class($name, $type)
{
    if (inlist($type, qobject_list))
	return $name + "->qobj";
    return $name;
}

sub do_multi_class_header($offset, $final, $arg, $name, $i, $const, $last)
{
    my $lo = ();
    
    my $os;
    while ($offset--) $os += " ";

    my $type = $arg.type;
    $type =~ s/\*//;
    my $utn = toupper($type);

    my $tcn = sprintf("Qore%s%s", $arg.is_class ? "" : "Abstract", $type);

    $arg.get = 
	$arg.is_class 
	? ($arg.ref 
	   ? sprintf("*(static_cast<%s *>(%s))", $type, get_class($arg.name, $type)) 
	   : sprintf("static_cast<%s *>(%s)", $type, get_class($arg.name, $type))) 
	: sprintf("%s->get%s()", $arg.name, $type);

    if (!$last)
	$lo += sprintf("%s%s *%s = (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink);", 
		       $os, $tcn, $arg.name, $tcn, $utn);
    else
	$lo += sprintf("%s%s *%s = p ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		       $os, $tcn, $arg.name, $tcn, $utn);
    $lo += sprintf("%sif (!%s) {", $os, $arg.name);
    if ($final) {
	$lo += sprintf("%s   if (!xsink->isException())", $os);
	my $str;
	if (!$last) 
	    $str = sprintf("%s      xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"%s::%s() does not know how to handle arguments of class '%s' as passed as the ", 
			  		       $os, toupper($cn), toupper($name), $cn, $name);
	else
	    $str = sprintf("%s      xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"this version of %s::%s() expects an object derived from %s as the ", 
			  		       $os, toupper($cn), toupper($name), $cn, $name, $type);

	$str += sprintf("%s argument\", p->val.object->getClass()->getName());", ordinal[$i]);
	$lo += $str;
	$lo += sprintf("%s   return%s;", $os, $const ? "" : " 0");
    }

    #$lo += sprintf("%sReferenceHolder<%s> holder(%s, xsink);", $os, $tcn, $arg.name);

    return $lo;
}

sub do_multi_class_trailer($offset, $arg)
{
    my $lo = ();
    
    my $os = substr(spaces, 0, $offset);

    my $type = $arg.type;
    $type =~ s/\*//;
    my $tcn = sprintf("Qore%s%s", $arg.is_class ? "" : "Abstract", $type);

    $lo += sprintf("%s}", $os);
    $lo += sprintf("%sReferenceHolder<%s> %sHolder(%s, xsink);", $os, $tcn, $arg.name, $arg.name);

    return $lo;
}

sub cl_order($l, $r) 
{
    return elements $l.args < $r.args ? 1 : -1;
}

sub do_multi_function($name, $func, $inst, $callstr, $param, $offset)
{
    my $lo = ();

    my $os = substr(spaces, 0, $offset + 3);

    # separate into 4 lists: classes, ints, everything else
    my $cl = ();
    my $il = ();
    my $fl = ();
    my $rl = ();
    my $none;

    foreach my $i in (\$inst) {
	$i.callstr = $callstr;

	if (!$param && !elements $i.args)
	    $none = $i;
	else if (elements $i.args <= $param)
	    continue;
	else if ($i.args[$param].is_class || $i.args[$param].is_abstract_class || $i.args[$param].is_dynamic_class)
	    $cl += $i;	
	else if ($i.args[$param].is_int)
	    $il += $i;
	else if ($i.args[$param].is_float)
	    $fl += $i;
	else
	    $rl += $i;
    }

    if (!elements $fl)
	$fl = $il;
    else
	$rl += $il;

    if (!elements $fl) {
	$fl = $rl;
	delete $rl;
    }

    $lo += sprintf("%sp = get_param(params, %d);", $param ? "" : "QoreNode *", $param);
    if (exists $none) {
	$lo += sprintf("if (is_nothing(p)) {");
	$lo += do_return_value(3, $func.rt, $callstr + ")", \$func.ok);
	$lo += "}";
    }
    if (elements $cl) {
	if (!$param)
	    $cl = sort($cl, \cl_order());

	my $last = $param ? elements $cl == 1 : elements $cl == 1 && !elements $rl && !elements $fl;

	if (!$last)
	    $lo += sprintf("if (p && p->type == NT_OBJECT) {");
	#$lo += sprintf("++ param=%n, elements $cl=%n", $param, elements $cl);

	for (my $cc = 0; $cc < elements $cl; ++$cc) {
	    my $off = $last ? 0 : ($cc + 1) * 3;

	    #printf("cl=%N\nrl=%N\nil=%N\n", $cl, $rl, $fl);
	    $lo += do_multi_class_header($off, $cc == (elements $cl) - 1, \$cl[$cc].args[$param], $name, $param, $name == $cn, $last);
	    
	    append_call(\$cl[$cc].callstr, $cl[$cc].args[$param]);
	}
	for (my $cc = (elements $cl) - 1; $cc >= 0; --$cc) {
	    my $off = $last ? 0 : ($cc + 1) * 3;

	    $lo += do_multi_class_trailer($off, \$cl[$cc].args[$param]);
	    
	    # if this is the last argument for this branch, do call
	    if ($param == (elements $cl[$cc].args - 1)) {
		splice $cl[$cc].callstr, -2, 2, ")";
		$lo += do_return_value($off, $func.rt, $cl[$cc].callstr, \$func.ok);
	    }
	    else
		$lo += do_multi_function($name, \$func, $cl, $cl[$cc].callstr, $param + 1, $cc * 3);
	}
	if (!$last)
	    $lo += sprintf("}");
    }

    foreach my $opt in ($rl) {
	my $rcallstr = $callstr;

	my $qt;

	if ($opt.args[$param].is_int)
	    $qt = "INT";
	else if ($opt.args[$param].is_float)
	    $qt = "FLOAT";
	else
	    switch ($opt.args[$param].type) {
		case "QString":
		case /char/:
		    $qt = "STRING";
		    break;
	    
	        default:
		    $qt = "???";
		    $func.ok = False;
		    break;
	    }
	
	$lo += sprintf("if (p && p->type == NT_%s) {", $qt);
	$lo += do_single_arg(3, $name, $opt.args[$param], $param, \$func.ok);

	append_call(\$rcallstr, $opt.args[$param]);

	if ($param == elements $opt.args - 1) {
	    if (elements $opt.args || !$o.indep)
		splice $rcallstr, -2, 2, ")";
	    else
		$rcallstr += ")";
	    
	    $lo += do_return_value(3, $func.rt, $rcallstr, \$func.ok);
	}
	else
	    $lo += do_multi_function($name, \$func, $fl, $rcallstr, $param + 1, -3);

	$lo += "}";
    }

    if (elements $fl) {
	# find longest
	my $longest;
	for (my $i = $param + 1; $i < elements $fl; ++$i)
	    if (elements $fl[$i].args > elements $fl[$longest].args)
	        $longest = $i;

	$lo += do_single_arg(0, $name, $fl[$longest].args[$param], $param, \$func.ok);

	append_call(\$callstr, $fl[$longest].args[$param]);

	if ($param == elements $fl[$longest].args - 1) {
	    if (elements $fl[$longest].args)
		splice $callstr, -2, 2, ")";
	    else
		$callstr += ")";
	    
	    $lo += do_return_value(0, $func.rt, $callstr, \$func.ok);
	}
	else
	    $lo += do_multi_function($name, \$func, $fl, $callstr, $param + 1, -3);

/*
	#printf("il=%n\n", $fl);
	for (my $i = $param; $i < elements $fl[$longest].args; ++$i) {
	    $lo += do_single_arg(0, $name, $fl[$longest].args[$i], $i + $param);
	    $callstr += sprintf("%s, ", exists $fl[$longest].args[$i].get ? $fl[$longest].args[$i].get : $fl[$longest].args[$i].name);
	}
	if (elements $fl[$longest].args)
	    splice $callstr, -2, 2, ")";
	else
	    $callstr += ")";

	$lo += do_return_value(0, $func.rt, $callstr, \$func.ok);
*/
    }

    foreach my $str in (\$lo)
	$str = $os + $str;

    return $lo;
}

sub do_single_arg($offset, $name, $arg, $i, $ok, $const)
{
    my $lo = ();
    my $os = substr(spaces, 0, $offset);

    switch ($arg.type) {
	case "QRgb":
	    case "quint64":
	    case "qint64":
		if ($arg.def)
		    $lo += sprintf("int64 %s = !is_nothing(p) ? p->getAsBigInt() : %d;", $arg.name, $arg.def);
		else
		    $lo += sprintf("int64 %s = p ? p->getAsBigInt() : 0;", $arg.name);
		break;

	    case "quint32":
	    case "uint":
		if ($arg.def)
		    $lo += sprintf("unsigned %s = !is_nothing(p) ? p->getAsBigInt() : %d;", $arg.name, $arg.def);
		else
		    $lo += sprintf("unsigned %s = p ? p->getAsBigInt() : 0;", $arg.name);
		break;

	    case "int": {
		if ($arg.def)
		    $lo += sprintf("int %s = !is_nothing(p) ? p->getAsInt() : %d;", $arg.name, $arg.def);
		else
		    $lo += sprintf("int %s = p ? p->getAsInt() : 0;", $arg.name);
		break;
	}
	case "bool": {
	    if ($arg.def =~ /true/)
		$lo += sprintf("bool %s = !is_nothing(p) ? p->getAsBool() : true;", $arg.name);
	    else
		$lo += sprintf("bool %s = p ? p->getAsBool() : false;", $arg.name);
	    break;
	}
	case "qreal":
	{
	    $lo += sprintf("qreal %s = p ? p->getAsFloat() : 0.0;", $arg.name);
	    break;
	}
	    case "double":
	{
	    $lo += sprintf("double %s = p ? p->getAsFloat() : 0.0;", $arg.name);
	    break;
	}
	    case "float": {
		$lo += sprintf("float %s = p ? p->getAsFloat() : 0.0;", $arg.name);
		break;
	}
	case "QString":
	    case "char*": {
		if (exists $arg.def)
		    $lo = sprintf("const char *%s = p ? p->val.String->getBuffer() : %s;", $arg.name, trim($arg.def));
		else {
		    $lo += "if (!p || p->type != NT_STRING) {";
		    $lo += sprintf("   xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a string as %s argument to %s::%s()\");", 
				   toupper($cn), toupper($name), ordinal[$i], $cn, $name);
	
		    $lo += $const ? "   return;" : "   return 0;";
		    $lo += "}";
		    $lo += sprintf("const char *%s = p->val.String->getBuffer();", $arg.name);
		}
		break;
	}
	
      default: {
	  if ($arg.is_int) {
	      if ($arg.def) {
		  $lo += sprintf("%s %s = (%s)(!is_nothing(p) ? p->getAsInt() : %d);", $arg.type, $arg.name, $arg.type, $arg.def);
	      }
	      else
	      {
		  # add class prefix to enum if not already present
		  my $tn = $arg.type;
		  if ($tn !~ /::/) 
		      $tn = $cn + "::" + $tn;
		  $lo += sprintf("%s %s = (%s)(p ? p->getAsInt() : 0);", $tn, $arg.name, $tn);
	      }
	  }
	  else {
	      #printf("DEBUG err arg type=%n (%n)\n", $inst.args[$i].type, $inst.args[$i]);
	      $lo += sprintf("??? %s %s = p;", $arg.type, $arg.name);
	      $ok = False;
	  }
	  break;
	}
    }

    foreach my $str in (\$lo)
	$str = $os + $str;

    return $lo;
}

sub append_call($cs, $arg)
{
    if (exists $arg.classname && $arg.type =~ /\*/ && inlist($arg.classname, const_class_list))
	$cs += sprintf("*(%s), ", exists $arg.get ? $arg.get : $arg.name);
    else
	$cs += sprintf("%s, ", exists $arg.get ? $arg.get : $arg.name);
}

sub do_single_function($name, $func, $inst, $callstr)
{
    my $lo = ();

    # do arguments
    for (my $i = 0; $i < elements $inst.args; ++$i) {
	$lo += sprintf("   %sp = get_param(params, %d);", $i ? "" : "QoreNode *", $i);
	
	if ($inst.args[$i].is_class)
	    $lo += do_class(\$inst.args[$i], $name, $cn, $i, True, !exists $func.rt);
	else if ($inst.args[$i].is_abstract_class)
	    $lo += do_class(\$inst.args[$i], $name, $cn, $i, False, !exists $func.rt);
	else if ($inst.args[$i].is_dynamic_class)
	    $lo += do_dynamic_class(\$inst.args[$i], $name, $cn, $i, !exists $func.rt);
	else
	    $lo += do_single_arg(3, $name, $inst.args[$i], $i, \$func.ok, !exists $func.rt);

	append_call(\$callstr, $inst.args[$i]);
    }
    if (elements $inst.args)
	splice $callstr, -2, 2, ")";
    else
	$callstr += ")";

    $lo += do_return_value(3, $func.rt, $callstr, \$func.ok, !exists $func.rt);
    
    return $lo;
}

sub do_constructor_return($callstr)
{
    my $lo = ();

    $lo += sprintf("self->setPrivate(CID_%s, %s);", toupper($cn), $callstr);
    $lo += "return;";
    return $lo;
}

sub do_return_value($offset, $rt, $callstr, $ok)
{
    my $lo = ();
    my $os = substr(spaces, 0, $offset);

    switch ($rt) {
	case NOTHING: {
	    $lo = do_constructor_return($callstr);
	    break;
	}

	case "void": {
	    $lo += sprintf("%s;", $callstr);
	    $lo += "return 0;"; 
	    break;
	}
	case "bool": {
	    $lo += sprintf("return new QoreNode(%s);", $callstr); 
	    break;
	}
	case =~ m/::/:
	    case "quint32":
	    case "quint64":
	    case "qint64":
	    case "uint":
	    case "QRgb":
	    case "int": {
		$lo += sprintf("return new QoreNode((int64)%s);", $callstr); 
		break;
	}
	case "qreal" :
	    case "double":
	    case "float" : {
		$lo += sprintf("return new QoreNode((double)%s);", $callstr); 
		break;
	}
	#case "QWidget*":
	case /^QPalette/:
	case /^QPoint/:
	case /^QPointF/:
	case /^QMenu/:
	case /^QSize/:
	case /^QBrush/:
	case /^QLine/:
	case /^QLineF/:
	case /^QPolygon/:
	case /^QPolygonF/:
	case /^QMatrix/:
	case /^QKeySequence/:
	    case /^QRegion/:
	    case /^QRectF/:
	    case /^QRect/:
	    case /^QMovie/:
	    case /^QPicture/:
	    case /^QPixmap/:
	    case /^QBitmap/:
	    case /^QImage/:
	    case /^QDateTime/:
	    case /^QDate/:
	    case /^QTime/:
	    case /^QFont/:
	    case /^QIcon/:
	    case /^QAction/:
	    case /^QColor/: {
		# add blank line if not the first line
		if (elements $lo > 2) $lo += "";
		$lo += do_return_class($rt, $callstr);
		break;
	}		
	case "QString": {
	    $lo += sprintf("return new QoreNode(new QoreString(%s.toUtf8().data(), QCS_UTF8));", $callstr); 
	    break;
	}
      default: {
	  if ($rt !~ /\*/)
	      $lo += sprintf("??? return new QoreNode((int64)%s);", $callstr);
	  else
	      $lo += sprintf("??? return %s;", $callstr); 
	  $ok = False; 
	  break;
	}
    }

    foreach my $str in (\$lo)
	$str = $os + $str;

    return $lo;
}

main();
