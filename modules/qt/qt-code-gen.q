#!/usr/bin/env qore

%require-our
%enable-all-warnings

const opts = (
    "abstract"        : "a,abstract",
    "abstract_class"  : "n,name=s",
    "abstract_list"   : "A,abstract-list=s@",
    "indep"           : "i,independent",
    "file"            : "f,file",
    "parent"          : "p,parent=s",
    "widget"          : "w,widget",
    "style"           : "s,style",
    "validator"       : "v,validator",
    "qt"              : "q,qt-class",
    "static"          : "S,static",
    "test"            : "t,test",
    "help"            : "h,help"
    );

const ordinal = ( "first", "second", "third", "fourth", "fifth", "sixth", 
		  "seventh", "eighth", "ninth", "tenth" );

our $special_hash = 
    ( "QDate"      : \do_qdate(),
      "QDateTime"  : \do_qdatetime(),
      "QTime"      : \do_qtime(),
    );

const special_arg_list =
    ( "QVariant", "QTime", "QDate", "QDateTime", "QString", "QByteArray", "QKeySequence" );

const qobject_list = 
    ( "QWidget", "QMovie", "QAction", "QActionGroup", "QMenu", 
      "QAbstractItemDelegate", "QItemDelegate", "QItemModel", "QLayout", 
      "QAbstractButton", "QLineEdit", "QScrollBar", "QMimeData",
      "QApplication", "QStyle", "QAbstractItemModel", "ToolButton", "QMessageBox",
      "QCheckBox", "QRadioButton", "QPushButton", "QMenuBar",
      "QPrintDialog", "QValidator"
 );

const abstract_class_list = 
    ( "QObject", "QWidget", "QAbstractItemDelegate", "QItemDelegate", 
      "QItemModel", "QLayout", "QStyle", "QHeaderView", "QMenuBar",
      "QAction", "QValidator");

const const_class_list = 
    ( "QPalette", "QMovie", "QPixmap", "QPicture", "QImage", "QPoint", "QPointF", 
      "QPolygon", "QPolygonF", "QLine", "QLineF", "QMatrix", "QSize", "QColor", 
      "QDateTime", "QDate", "QTime", "QKeySequence", "QIcon", "QFont", "QBrush", 
      "QTextFormat", "QTextBlockFormat", "QTextCharFormat", "QTextFrameFormat", 
      "QTextImageFormat", "QTextListFormat", "QTextTableFormat", "QTextLength", 
      "QPen", "QModelIndex", "QStyleOptionViewItem", 
      "QStyleOptionViewItemV2", "QLocale", "QUrl", "QByteArray", "QVariant", 
      "QRect", "QRectF", "QFontInfo", "QFontMetrics", "QDir", "QRegExp",
      "QFileInfo"
    );

const class_list = ( "QRegion",
		     "QPainter",
		     "QBitmap",
		     "QMenu",
		     "QAction", 
		     "QActionGroup",
		     "QEvent",
		     "QActionEvent",
		     "QCloseEvent",
		     "QContextMenuEvent",
		     "QDropEvent",
		     "QDragMoveEvent",
		     "QDragEnterEvent",
		     "QDragLeaveEvent",
		     "QFocusEvent",
		     "QHideEvent",
		     "QInputMethodEvent",
		     "QShowEvent",
		     "QTabletEvent",
		     "QWheelEvent",
		     "QInputEvent",
		     "QKeyEvent",
		     "QMouseEvent",
		     "QMoveEvent",
		     "QPaintEvent",
		     "QResizeEvent",
		     "QStyleOption",
		     "QStyleOptionComboBox",
		     "QStyleOptionComplex",
		     "QStyleOptionGroupBox",
		     "QStyleOptionMenuItem",
		     "QStyleOptionSizeGrip",
		     "QStyleOptionSlider",
		     "QStyleOptionSpinBox",
		     "QStyleOptionTitleBar",
		     "QStyleOptionToolButton",
		     "QStyleOptionButton",
		     "QTableWidgetItem",
		     "QHeaderView",
		     "QMetaObject",
		     "QPrinter",

 ) + const_class_list + qobject_list;

const dynamic_class_list = ( "QPaintDevice", "QPixmap", 
    );

const spaces = "                                                                        ";

our ($o, $if, $cn);

sub usage()
{
    printf(
"usage: %s input_file class_name
  -f,--file                create cc and header file(s)
  -i,--independent         emit code for independent class
  -a,--abstract            emit abstract class name
  -n,--name=ARG            abstract parent's class name
  -A,--abstract-list=ARG   list of abstract classes
  -w,--widget              is a QWidget
  -s,--style               is a QStyle
  -q,--qt-class            add Qt class to abstract class
  -p,--parent=ARG          parent class name
  -S,--static              assume prototypes are static functions
  -t,--test                do not create files (use with -f)
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
	$o.abstract_class = $o.abstract ? $cn : $o.widget ? "QWidget" : "QObject";
    else if ($o.indep && $o.abstract && !exists $o.abstract_class)
	$o.abstract_class = $cn;

    if ($o.indep && $o.widget) {
	stderr.printf("cannot be independent and a QWidget at the same time\n");
	exit(1);
    }

    if ($o.indep && $o.style) {
	stderr.printf("cannot be independent and a QStyle at the same time\n");
	exit(1);
    }

    if ($o.indep && $o.validator) {
	stderr.printf("cannot be independent and a QValidator at the same time\n");
	exit(1);
    }

    if (!$o.indep && !exists $o.parent)
	$o.parent = $o.widget ? "QWidget" : "QObject";

    if ($o.qt && !$o.abstract) {
	stderr.printf("cannot add Qt class to non-abstract class\n");
	exit(1);
    }
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

sub do_return_qobject($type, $callstr)
{
    if ($type == "QObject")
	return sprintf("return return_qobject(%s);", $callstr);
    
    my $lo = ();
    
    $lo += sprintf("%s *qt_qobj = %s;", $type, $callstr);
    $lo += "if (!qt_qobj)";
    $lo += "   return 0;";
    $lo += "QVariant qv_ptr = qt_qobj->property(\"qobject\");";
    $lo += "Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());";
    $lo += "if (rv_obj)";
    $lo += "   rv_obj->ref();";
    $lo += "else {";
    $lo += sprintf("   rv_obj = new Object(QC_%s, getProgram());", $type);
    $lo += sprintf("   QoreQt%s *t_qobj = new QoreQt%s(rv_obj, qt_qobj);", $type, $type);
    $lo += sprintf("   rv_obj->setPrivate(CID_%s, t_qobj);", toupper($type));
    $lo += "}";
    $lo += "return new QoreNode(rv_obj);";
    return $lo;
}

sub do_return_class($type, $callstr)
{
    $type =~ s/&//g;
    $type =~ s/\*//g;
    my $cl = $type;
    $cl =~ s/[a-z]//g;
    $cl = tolower($cl);

    #if (inlist($type, const_class_list) && $type =~ /\*/)
    #    $callstr = "*(" + $callstr + ")";

    my $lo = ();

    my $utn = toupper($type);
    if ($type == $cn)
	$lo += sprintf("Object *o_%s = new Object(self->getClass(CID_%s), getProgram());", $cl, $utn);
    else {
	$lo += sprintf("Object *o_%s = new Object(QC_%s, getProgram());", $cl, $type);
    }
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

    #my $ref = inlist($type, const_class_list); # && ($arg.type =~ /\*/);
    my $ref = $arg.is_const;

    my $tcn = sprintf("Qore%s%s", $d ? "" : "Abstract", $type);

    $arg.get = $d 
	? sprintf("static_cast<%s *>(%s)", $type, get_class($arg.name, $type))
	: sprintf("%s->get%s()", $arg.name, $type);

    if ($ref)
	$arg.get = "*(" + $arg.get + ")";

    $lo += sprintf("   %s *%s = (p && p->type == NT_OBJECT) ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		   $tcn, $arg.name, $tcn, $utn);
    if (!exists $arg.def) {
	$lo += sprintf("   if (!%s) {", $arg.name);
	$lo += "      if (!xsink->isException())";
	$lo += sprintf("         xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a %s object as %s argument to %s::%s()\");", 
		       toupper($cn), toupper($name), $type, ordinal[$i], $cn, $name);
	$lo += $const ? "      return;" : "      return 0;";
	$lo += "   }";
    }
    else {
	$lo += "   if (*xsink)";
	$lo += $const ? "      return;" : "      return 0;";
    }
    $lo += sprintf("   ReferenceHolder<AbstractPrivateData> %sHolder(static_cast<AbstractPrivateData *>(%s), xsink);", $arg.name, $arg.name);

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

    #my $ref = inlist($type, const_class_list); # && ($arg.type =~ /\*/);
    my $ref = $arg.is_const;

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
    if ($o.test)
	return stdout;

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
    if (!$o.test && !strlen(trim(backquote("grep " + $cc + " Makefile.am")))) {
	my $lines = split("\n", `cat Makefile.am`);
	my $of = get_file("Makefile.am");
	my $done;
	for (my $i = 0; $i < elements $lines; ++$i) {
	    
	    if (!$done && ($lines[$i + 1] =~ /endif/ || $lines[$i + 1] =~ /if COND_MACOSX/)) {
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
    if (!$o.test && !strlen(trim(backquote("grep " + $hh + " ../../Makefile.am")))) {
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
		    if ($o.abstract) 
			$of.printf("\tmodules/qt/QoreAbstract%s.h \\\n", $cn);
		    $done = True;
		}
	    }
	    $of.printf("%s\n", $lines[$i]);
	}
    }

    # add to qt.cc
    if (!$o.test && !strlen(trim(backquote("grep " + $hh + " qt.cc")))) {
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
	    else if ($lines[$i + 1] =~ /add QBoxLayout namespace/)
		$of.printf("   qt->addSystemClass(init%sClass(%s));\n", $cn, tolower($o.parent));

	    $of.printf("%s\n", $lines[$i]);
	}	
    }
}

sub do_abstract($of, $proto, $qt)
{
    my $class_name = "Qore" + ($qt ? "Qt" : "") + $cn;
    my $lcn;
    if ($qt)
	$lcn = tolower($cn);

    if ($qt)
	$of.printf("\n");
    $of.printf("class %s : public QoreAbstract%s\n", $class_name, $o.abstract_class);
    $of.printf("{\n   public:\n");
    if ($qt)
	$of.printf("      Object *qore_obj;\n");
    $of.printf("      QPointer<%s%s> qobj;

", $qt ? "" : "my", $cn);
    
    if ($qt)
	$of.printf("      DLLLOCAL %s(Object *obj, %s *%s) : qore_obj(obj), qobj(%s)\n      {\n      }\n", $class_name, $cn, $lcn, $lcn);
    else {
	if (exists $proto.$cn) {
	    foreach my $i in ($proto.$cn.inst) {
		my $arg_names;
		foreach my $a in ($i.args)
		    $arg_names += $a.name + ", ";
		if (exists $arg_names) {
		    splice $arg_names, -2;
		    $arg_names = ", " + $arg_names;
		}
		my $str = "Object *obj";
		if (strlen($i.orig_args))
		    $str += ", " + $i.orig_args;
		$of.printf("      DLLLOCAL %s(%s) : qobj(new my%s(obj%s))\n      {\n      }\n", $class_name, $str, $cn, $arg_names);
	    }
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
    
    if ($o.abstract && $o.abstract_class != $cn)
	$of.printf("      DLLLOCAL virtual class %s *get%s() const
      {
         return static_cast<%s *>(&(*qobj));
      }
", $cn, $cn, $cn);
    
    foreach my $ac in ($o.abstract_list)
	$of.printf("      DLLLOCAL virtual class %s *get%s() const
      {
         return static_cast<%s *>(&(*qobj));
      }
", $ac, $ac, $ac);

    if (!$qt)
	$of.printf("      QORE_VIRTUAL_Q%s_METHODS\n", $o.widget ? "WIDGET" : ($o.style ? "STYLE" : ($o.validator ? "VALIDATOR" : "OBJECT")));
    else
	$of.printf("#include \"qore-qt-static-q%s-methods.h\"\n", $o.widget ? "widget" : ($o.style ? "style" : "object"));
    $of.printf("};\n");
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
	$t =~ s/const /const^/g;
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
	    $args =~ s/const\^/const /g;
	}
	else {
	    $rt =~ s/const\^//;
	    $rt =~ s/\*//;
	    $rt =~ s/&//;
	}
	#printf("args=%n\n", $args);

	my $orig_args = trim($args);
	$args = split(",", $args);
	trim $args;
	foreach my $arg in (\$args)
	{
	    my $is_const = ($arg =~ /const[ ^]/ && $arg !~ /\*/);
	    $arg =~ s/const[ ^]//;

	    my ($ra, $def) = $arg =~ x/(.*)=(.*)/;
	    if (exists $def) {
		$arg = trim($ra);
		trim $def;
		if ($def =~ /\|/)
		    delete $def;
	    }
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
	    $arg = ( "type" : $type, "name" : $pname, "def" : $def, "ref" : $ref, "is_const" : $is_const );

	    my $typename = $type;
	    $typename =~ s/\*//g;
	    $typename =~ s/&//g;
	    if (exists $special_hash.$typename)
		$arg.special = $special_hash.$typename;
	    else if (inlist($typename, class_list)) {
		$arg.is_class = True;
		$arg.classname = $typename;
		if (inlist($typename, special_arg_list)) {
		    $arg.special_arg = True;
		    #printf("special: %n\n", $typename);
		}
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

	if (!exists $proto.$name) {
	    my $funcname;
	    if ($o.static)
		$funcname = "f_" + $cn + "_" + $name;
	    else
		$funcname = sprintf("%s_%s", $func_prefix, $cn == $name ? "constructor" : $name);
	    $proto.$name = ( "funcname" : $funcname, "rt" : $rt, "inst" : () );
	}

	$proto.$name.inst += ( "args" : $args, "orig" : $p, "orig_args" : $orig_args );
    }

    my $of;
    if (exists $o.file) {
	add_new_build_files("QC_" + $cn);

	# add new abstract header
	if ($o.abstract) {
	    $of = get_file("QoreAbstract" + $cn + ".h");
	    $of.printf(
"/*
 QoreAbstract%s.h
 
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

#ifndef _QORE_QT_QOREABSTRACT%s_H

#define _QORE_QT_QOREABSTRACT%s_H

", $cn, $func_prefix, $func_prefix);

	    if (exists $o.parent)
		$of.printf("#include \"QoreAbstract%s.h\"\n\n", $o.parent); 


	    $of.printf(
"class QoreAbstract%s%s
{
   public:
      DLLLOCAL virtual class %s *get%s() const = 0;
};

#endif  // _QORE_QT_QOREABSTRACT%s_H
", $cn, exists $o.parent ? " : public QoreAbstract" + $o.parent : "", $cn, $cn, $func_prefix);
	}

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
	    if (!$o.indep)
		$of.printf("#include \"qore-qt-events.h\"\n");
	}

	$of.printf("
DLLLOCAL extern int CID_%s;
DLLLOCAL extern class QoreClass *QC_%s;

DLLLOCAL class QoreClass *init%sClass(%s);

", $func_prefix, $cn, $cn, exists $o.parent ? "QoreClass *" : "");

	if ($o.indep) {
	    if ($o.abstract)
		$of.printf("class Qore%s : public QoreAbstract%s, public %s\n", $cn, $o.abstract_class, $cn);
	    else
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
	    if ($o.abstract)
		$of.printf("      DLLLOCAL virtual %s *get%s() const\n      {\n      return static_cast<%s *>(this);\n      }\n",
		$o.abstract_class, $o.abstract_class, $o.abstract_class);
	    $of.printf("};\n");
	}
	else # abstract/dependent class
	{
	    $of.printf("class my%s : public %s%s\n", $cn, $cn, $o.widget ? ", public QoreQWidgetExtension" 
		       : ($o.validator ? ", public QoreQValidatorExtension" : ""));
            $of.printf("{\n");
            if ($o.style)
                $of.printf("      friend class Qore%s;\n\n", $cn);
	    $of.printf("#define QOREQTYPE %s
#include \"qore-qt-metacode.h\"
%s#undef QOREQTYPE

   public:
", $cn, $o.widget ? "#include \"qore-qt-widget-events.h\"\n" : 
		       $o.validator ? "#include \"qore-qt-qvalidator-methods.h\"\n" : "");
	    
	    if (exists $proto.$cn) {
		foreach my $i in ($proto.$cn.inst) {
		    my $arg_names;
		    foreach my $a in ($i.args)
			$arg_names += $a.name + ", ";
		    if (exists $arg_names)
			splice $arg_names, -2;
		    my $str = "Object *obj";
		    if (strlen($i.orig_args))
			$str += ", " + $i.orig_args;
		    $of.printf("      DLLLOCAL my%s(%s) : %s(%s)%s\n      {\n", $cn, $str, $cn, $arg_names,
			$o.widget ? ", QoreQWidgetExtension(obj->getClass())" 
			       : $o.validator ? ", QoreQValidatorExtension(obj->getClass())" : "");
		    $of.printf("         init(obj);\n");
		    $of.printf("      }\n"); 
		}
	    }
	    $of.printf("};\n\n");

            do_abstract($of, $proto);
            if ($o.qt)
                do_abstract($of, $proto, $o.qt);
	}

	$of.printf("\n#endif // _QORE_QT_QC_%s_H\n", $func_prefix);

	$of = get_file("QC_" + $cn + ".cc");
    }
    else
	$of = stdout;


    foreach my $p in (keys $proto) {	
	# do function header
	$proto.$p.ok = True;
	my $lo = ();

	my $hdr = ();

	my $name = $p == $cn ? "constructor" : $p;

	for (my $i; $i < elements $proto.$p.inst; ++$i) {
	    my $inst = $proto.$p.inst[$i];
	    $hdr += sprintf("//%s", $inst.orig);

	    if ($p == $cn && elements $inst.args == 1 && $inst.args[0].type == $cn) {
		# remove this element from the list if it's a constant constructor
		splice $proto.$p.inst, $i, 1;
		--$i;
	    }
        }
	if ($p == $cn)
	    $lo += sprintf("static void %s(Object *self, QoreNode *params, ExceptionSink *xsink)", 
			   $proto.$p.funcname, $cl);
        else {
            if ($o.static)
                $lo += sprintf("static QoreNode *%s(QoreNode *params, ExceptionSink *xsink)", 
			       $proto.$p.funcname);
            else
                $lo += sprintf("static QoreNode *%s(Object *self, Qore%s *%s, QoreNode *params, ExceptionSink *xsink)", 
			       $proto.$p.funcname, $qore_class, $cl);
        }
	$lo += "{";

        my $callstr;
        if ($o.static)
            $callstr = $cn + "::" + $p + "(";
        else {
            if (exists $proto.$p.rt)
                $callstr = $o.indep && !$o.abstract 
                           ? sprintf("%s->%s(", $cl, $p) 
                           : sprintf("%s->%s->%s(", $cl, $get_obj, $p);
	    else { # for constructor calls
	        $callstr = sprintf("new Qore%s(", $p);
	        if (!$o.indep)
		    $callstr += "self, ";
	    }
        }
	if (elements $proto.$p.inst == 1)
	    $lo += do_single_function($name, \$proto.$p, $proto.$p.inst[0], $callstr);
	else
	    $lo += do_multi_function($name, \$proto.$p, $proto.$p.inst, $callstr);

	$lo += "}";

	$proto.$p.code = $hdr + $lo; 
    
	#printf("%-15s %-20s (%s)\n", "(" + $rt + ")", $name, dlh($args));
    }


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
	if (!$proto.$p.ok)
            $of.printf("/*\n");
	foreach my $line in ($proto.$p.code)
	    $of.printf("%s\n", $line);
	if (!$proto.$p.ok)
            $of.printf("*/\n");
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
            if ($o.static)
	        $of.printf("   %sbuiltinFunctions.add(\"%s_%-30s %s);\n", $proto.$p.ok ? "" : "//", $cn, $p+"\",", $proto.$p.funcname);
            else
	        $of.printf("   %sQC_%s->addMethod(%-30s (q_method_t)%s);\n", $proto.$p.ok ? "" : "//", $cn, "\""+$p+"\",", $proto.$p.funcname);
    }

    if (exists $o.file) {
	$of.printf("\n   return QC_%s;\n}\n", $cn);
    }
}

sub get_class($name, $type)
{
    if (inlist($type, abstract_class_list))
	return sprintf("%s->get%s()", $name, $type);
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
	$lo += sprintf("%s%s *%s = (p && p->type == NT_OBJECT) ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		       $os, $tcn, $arg.name, $tcn, $utn);
    if (!exists $arg.def) {
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
    }
    else {
	$lo += sprintf("%sif (*xsink)", $os);
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

    if (!exists $arg.def)
	$lo += sprintf("%s}", $os);
    $lo += sprintf("%sReferenceHolder<AbstractPrivateData> %sHolder(static_cast<AbstractPrivateData *>(%s), xsink);", $os, $arg.name, $arg.name);

    return $lo;
}

sub cl_order($l, $r) 
{
    return elements $l.args < $r.args ? 1 : -1;
}

sub do_qdate($arg, $const)
{
    my $lo = ();
    
    $lo += sprintf("QDate %s;", $arg.name);
    $lo += sprintf("if (get_qdate(p, %s, xsink))", $arg.name);
    $lo += sprintf("   return%s;", $const ? "" : " 0");
    return $lo;
}

sub do_qtime($arg, $const)
{
    my $lo = ();
    
    $lo += sprintf("QTime %s;", $arg.name);
    $lo += sprintf("if (get_qtime(p, %s, xsink))", $arg.name);
    $lo += sprintf("   return%s;", $const ? "" : " 0");
    return $lo;
}

sub do_qdatetime($arg, $const)
{
    my $lo = ();
    
    $lo += sprintf("QDateTime %s;", $arg.name);
    $lo += sprintf("if (get_qdatetime(p, %s, xsink))", $arg.name);
    $lo += sprintf("   return%s;", $const ? "" : " 0");
    return $lo;
}

sub all_default($l)
{
    foreach my $arg in ($l)
	if (!exists $arg.def)
	    return False;

    return True;
}

sub do_multi_function($name, $func, $inst, $callstr, $param, $offset)
{
    my $lo = ();

    my $os = substr(spaces, 0, $offset + 3);

    #printf("elements=%n $inst[0].args[%n].special=%N\n", elements $inst, $param, $inst[0].args[$param].special);
    if (elements $inst == 1 && exists $inst[0].args[$param].special) {
	my $i = $inst[0];
	$lo += $i.args[$param].special($i.args[$param], !exists $func.rt);
	append_call(\$callstr, $i.args[$param]);

	# if this is the last argument for this branch, do call
	if ($param == (elements $i.args - 1)) {
	    splice $callstr, -2, 2, ")";
	    $lo += do_return_value(0, $func.rt, $callstr, \$func.ok);
	}
	else
	    $lo += do_multi_function($name, \$func, $i, \$callstr, $param + 1, $offset + 3);

	foreach my $str in (\$lo)
	    $str = $os + $str;
	
	return $lo;
    }

    # separate into 4 lists: classes, floats, ints, everything else
    my $cl = ();
    my $il = ();
    my $fl = ();
    my $rl = ();
    my $none;

    foreach my $i in (\$inst) {
	$i.callstr = $callstr;

	if (!$param && (!elements $i.args || all_default($i.args))) {
	    $none = $i;
	    if (!elements $i.args)
		continue;
	}

	if (elements $i.args <= $param)
	    continue;
	else if (($i.args[$param].is_class || $i.args[$param].is_abstract_class || $i.args[$param].is_dynamic_class) 
		 && !$i.args[$param].special_arg)
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
	    $lo += do_multi_class_header($off, $cc == (elements $cl) - 1, \$cl[$cc].args[$param], $name, $param, !exists $cl[$cc].rt, $last);
	    
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
		case "QChar":
		case /char/:
		    $qt = "STRING";
		    break;
	    
	        default:
		    $qt = "???";
		    $func.ok = False;
		    break;
	    }
	
	$lo += sprintf("if (p && p->type == NT_%s) {", $qt);
	$lo += do_single_arg(3, $name, $opt.args[$param], $param, \$func.ok, !exists $func.rt);

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

	$lo += do_single_arg(0, $name, $fl[$longest].args[$param], $param, \$func.ok, !exists $func.rt);

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

    if (exists $arg.special) {
	my $c = $arg.special;
	$lo += $c($arg, $const);
    } 
    else
	switch ($arg.type) {
	    case "QRgb":
		case "quint64":
		case "qint64": {
		    if (exists $arg.def)
			$lo += sprintf("int64 %s = !is_nothing(p) ? p->getAsBigInt() : %s;", $arg.name, $arg.def);
		    else
			$lo += sprintf("int64 %s = p ? p->getAsBigInt() : 0;", $arg.name);
	    }
	    break;
	    
	    case "quint32":
		case "uint": {
		    if (exists $arg.def)
			$lo += sprintf("unsigned %s = !is_nothing(p) ? p->getAsBigInt() : %s;", $arg.name, $arg.def);
		    else
			$lo += sprintf("unsigned %s = p ? p->getAsBigInt() : 0;", $arg.name);
	    }
	    break;
	    
	    case "int": {
		if (exists $arg.def)
		    $lo += sprintf("int %s = !is_nothing(p) ? p->getAsInt() : %s;", $arg.name, $arg.def);
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
	    case "char*": {
		if (exists $arg.def)
		    $lo = sprintf("const char *%s = (p && p->type == NT_STRING) ? p->val.String->getBuffer() : %s;", $arg.name, trim($arg.def));
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
	    case "QString": {
		$lo += sprintf("QString %s;", $arg.name);
		if (exists $arg.def) {
		    $lo += sprintf("if (get_qstring(p, %s, xsink, true))", $arg.name);
		    $lo += sprintf("   %s = %s;", $arg.name, trim($arg.def));
		}
		else {
		    $lo += sprintf("if (get_qstring(p, %s, xsink))", $arg.name);
		    $lo += $const ? "   return;" : "   return 0;";
		}
	    }
	    break;
	    case "QKeySequence": {
		$lo += sprintf("QKeySequence %s;", $arg.name);
		if (exists $arg.def) {
		    $lo += sprintf("if (get_qkeysequence(p, %s, xsink, true))", $arg.name);
		    $lo += sprintf("   %s = %s;", $arg.name, trim($arg.def));
		}
		else {
		    $lo += sprintf("if (get_qkeysequence(p, %s, xsink))", $arg.name);
		    $lo += $const ? "   return;" : "   return 0;";
		}
	    }
	    break;
	    case "QStringList" : {
		$lo += "if (!p || p->type != NT_LIST) {";
		$lo += sprintf("   xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a list as %s argument to %s::%s()\");", 
			       toupper($cn), toupper($name), ordinal[$i], $cn, $name);
		$lo += $const ? "   return;" : "   return 0;";
		$lo += "}";
		$lo += sprintf("QStringList %s;", $arg.name);
		$lo += sprintf("ListIterator li_%s(p->val.list);", $arg.name);
		$lo += sprintf("while (li_%s.next())", $arg.name);
		$lo += "{";
		$lo += sprintf("   QoreNodeTypeHelper str(li_%s.getValue(), NT_STRING, xsink);", $arg.name);
		$lo += "   if (*xsink)";
		$lo += $const ? "      return;" : "      return 0;";
		$lo += "   QString tmp;";
		$lo += "   if (get_qstring(*str, tmp, xsink))";
		$lo += $const ? "      return;" : "      return 0;";
		$lo += sprintf("   %s.push_back(tmp);", $arg.name);
		$lo += "}";
	    }
	    break;
	    case "QVariant": {
		$lo += sprintf("QVariant %s;", $arg.name);
		if (exists $arg.def) {
		    $lo += sprintf("if (get_qvariant(p, %s, xsink, true))", $arg.name);
		    $lo += sprintf("   %s = %s;", $arg.name, trim($arg.def));
		}
		else {
		    $lo += sprintf("if (get_qvariant(p, %s, xsink))", $arg.name);
		    $lo += $const ? "   return;" : "   return 0;";
		}
	    }
	    break;
	    case "QByteArray": {
		$lo += sprintf("QByteArray %s;", $arg.name);
		if (exists $arg.def) {
		    $lo += sprintf("if (get_qbytearray(p, %s, xsink, true))", $arg.name);
		    $lo += sprintf("   %s = %s;", $arg.name, trim($arg.def));
		}
		else {
		    $lo += sprintf("if (get_qbytearray(p, %s, xsink))", $arg.name);
		    $lo += $const ? "   return;" : "   return 0;";
		}
	    }
	    break;
	    case "QChar": {
		$lo += sprintf("QChar %s;", $arg.name);
		if (exists $arg.def) {
		    $lo += sprintf("if (get_qchar(p, %s, xsink, true))", $arg.name);
		    $lo += sprintf("   %s = %s;", $arg.name, trim($arg.def));
		}
		else {
		    $lo += sprintf("if (get_qchar(p, %s, xsink))", $arg.name);
		    $lo += $const ? "   return;" : "   return 0;";
		}
	    }
	    case "char": {
		if (exists $arg.def)
		    $lo = sprintf("const char %s = p ? p->val.String->getBuffer()[0] : %s;", $arg.name, trim($arg.def));
		else {
		    $lo += "if (!p || p->type != NT_STRING) {";
		    $lo += sprintf("   xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a string as %s argument to %s::%s()\");", 
				   toupper($cn), toupper($name), ordinal[$i], $cn, $name);
		    
		    $lo += $const ? "   return;" : "   return 0;";
		    $lo += "}";
		    $lo += sprintf("const char %s = p->val.String->getBuffer()[0];", $arg.name);
		}
		break;
	    }
	    
	  default: {
	      if ($arg.is_int) {
		  # add class prefix to enum if not already present
		  my $tn = $arg.type;
		  my $def = $arg.def;
		  if ($tn !~ /::/)  {
		      $tn = $cn + "::" + $tn;
		      $def = $cn + "::" + $def;
		  }
		  if (exists $arg.def)
		      $lo += sprintf("%s %s = !is_nothing(p) ? (%s)p->getAsInt() : %s;", $tn, $arg.name, $tn, $def);
		  else
		      $lo += sprintf("%s %s = (%s)(p ? p->getAsInt() : 0);", $tn, $arg.name, $tn);
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
    #if (exists $arg.classname && $arg.type =~ /\*/ && inlist($arg.classname, const_class_list))
	#$cs += sprintf("*(%s), ", exists $arg.get ? $arg.get : $arg.name);
    #else 
    #printf("arg=%n, call=%n\n", $arg, $cs);
    if (($arg.is_class || $arg.is_abstract_class) && !$arg.special_arg && $arg.def == "0")
	$cs += sprintf("%s ? %s : 0, ", $arg.name, exists $arg.get ? $arg.get : $arg.name);
    else
	$cs += sprintf("%s, ", exists $arg.get ? $arg.get : $arg.name);
}

sub do_single_function($name, $func, $inst, $callstr)
{
    my $lo = ();

    # do arguments
    for (my $i = 0; $i < elements $inst.args; ++$i) {
	$lo += sprintf("   %sp = get_param(params, %d);", $i ? "" : "QoreNode *", $i);
	
	if ($inst.args[$i].is_class && !$inst.args[$i].special_arg)
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
	    case "ushort":
	    case "short":
	    case "ulong":
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

	case /^QDateTime/: {
	    $lo += sprintf("QDateTime rv_dt = %s;", $callstr);
	    $lo += "QDate rv_d = rv_dt.date();";
	    $lo += "QTime rv_t = rv_dt.time();";
	    $lo += "return new QoreNode(new DateTime(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec()));";
	    break;
	}

	case /^QDate/: {
	    $lo += sprintf("QDate rv_date = %s;", $callstr);
	    $lo += "return new QoreNode(new DateTime(rv_date.year(), rv_date.month(), rv_date.day()));";
	    break;
	}

	case /^QTime/: {
	    $lo += sprintf("QTime rv_t = %s;", $callstr);
	    $lo += "return new QoreNode(new DateTime(1970, 1, 1, rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec()));";
	    break;
	}

	case "QString": {
	    $lo += sprintf("return new QoreNode(new QoreString(%s.toUtf8().data(), QCS_UTF8));", $callstr); 
	    break;
	}

	case "QChar": {
	    $lo += "QoreString *rv_str = new QoreString(QCS_UTF8);";
	    $lo += sprintf("QChar rv_qc = %s;", $callstr);
	    $lo += "rv_str->concatUTF8FromUnicode(rv_qc.unicode());";
	    $lo += "return new QoreNode(rv_str);";
	    break;
	}

	case "char": {
	    $lo += sprintf("const char c_rv = %s;", $callstr);
	    $lo += "QoreString *rv_str = new QoreString();";
	    $lo += "rv_str->concat(c_rv);";
	    $lo += "return new QoreNode(rv_str);";
	    break;
	}

	case "QList<int>": {
	    $lo += sprintf("QList<int> ilist_rv = %s;", $callstr);
	    $lo += "List *l = new List();";
	    $lo += "for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)";
	    $lo += "   l->push(new QoreNode((int64)(*i)));";
	    $lo += "return new QoreNode(l);";
	    break;
	}

	case "QStringList": {
	    $lo += sprintf("QStringList strlist_rv = %s;", $callstr);
	    $lo += "List *l = new List();";
	    $lo += "for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)";
	    $lo += "   l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));";
	    $lo += "return new QoreNode(l);";
	    break;
	}

	case "QVariant" : {
	    $lo += sprintf("return return_qvariant(%s);", $callstr);
	    break;
	}

      default: {
	  #printf("rt=%s il=%n cs=%n\n", $rt, inlist($rt, qobject_list), $callstr);
	  if (inlist($rt, qobject_list)) {
	      # add blank line if not the first line
	      if (elements $lo > 2) $lo += "";
	      $lo += do_return_qobject($rt, $callstr);	      
	  }
	  else if (inlist($rt, class_list)) {
	      # add blank line if not the first line
	      if (elements $lo > 2) $lo += "";
	      $lo += do_return_class($rt, $callstr);
	  }
	  else {
	      if ($rt !~ /\*/)
		  $lo += sprintf("??? return new QoreNode((int64)%s);", $callstr);
	      else
		  $lo += sprintf("??? return %s;", $callstr); 
	      $ok = False;
	  } 
	  break;
	}
    }

    foreach my $str in (\$lo)
	$str = $os + $str;

    return $lo;
}

main();
