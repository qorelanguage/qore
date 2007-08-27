#!/usr/bin/env qore

%require-our
%enable-all-warnings

const opts = (
    "abstract" : "a,abstract",
    "indep"    : "i,independent",
    "help"     : "h.help"
    );

const ordinal = ( "first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth", "tenth" );

const class_list = ( "QFont*", "QFont",
		     "QPoint*", "QPoint",
		     "QMatrix*", "QMatrix",
		     "QSize*", "QSize",
		     "QRectF*", "QRectF",
		     "QRect*", "QRect",
		     "QRegion*", "QRegion",
		     "QBrush*", "QBrush",
		     "QColor*", "QColor",
		     "QPainter*", "QPainter",
		     "QBitmap*", "QBitmap",
		     "QImage*", "QImage",
		     "QMovie*", "QMovie",
		     "QPicture*", "QPicture",
		     "QDateTime", "QDate", "QTime", "QKeySequence", "QIcon",
		     "QAction", "QAction*", "QActionGroup", "QActionGroup*",
		     "QKeySequence"
 );

const qobject_list = ( "QWidget", "QMovie", "QAction", "QActionGroup" );

const const_class_list = ("QMovie", "QPixmap", "QPicture" ,"QImage", "QPoint", "QMatrix", "QSize", "QColor", "QDateTime", "QDate", "QTime", "QKeySequence", "QIcon", "QFont" );

const abstract_class_list = ( "QObject*", "QWidget*", "QLayout*" );

const dynamic_class_list = ( "QPaintDevice*", "QPaintDevice", 
			     "QPixmap*", "QPixmap", 
    );

const spaces = "                                                                        ";

our ($o, $if, $cn);

sub usage()
{
    printf(
"usage: %s input_file class_name
  -i,--independent  emit code for independent class
  -a,--abstract     emit abstract class name
  -h,--help         this help text
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

sub do_class($arg, $name, $cn, $i, $d)
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
	   ? sprintf("*(static_cast<%s *>(%s))", $type, $arg.name) 
	   : sprintf("static_cast<%s *>(%s)", $type, $arg.name)) 
	: ($ref
	   ? sprintf("*(%s->get%s())", $arg.name, $type)
	   : sprintf("%s->get%s()", $arg.name, $type));

    $lo += sprintf("   %s *%s = (p && p->type == NT_OBJECT) ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		   $tcn, $arg.name, $tcn, $utn);
    $lo += sprintf("   if (!%s) {", $arg.name);
    $lo += "      if (!xsink->isException())";
    $lo += sprintf("         xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a %s object as %s argument to %s::%s()\");", 
		   toupper($cn), toupper($name), $type, ordinal[$i], $cn, $name);
    $lo += "      return 0;";
    $lo += "   }";
    $lo += sprintf("   ReferenceHolder<%s> holder(%s, xsink);", $tcn, $arg.name);

    return $lo;
}

sub do_dynamic_class($arg, $name, $cn, $i)
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
    $lo += "      return 0;";
    $lo += "   }";
    $lo += sprintf("   ReferenceHolder<AbstractPrivateData> holder(apd_%s, xsink);", $arg.name);
    $lo += sprintf("   %s *%s = dynamic_cast<%s *>(apd_%s);", $tcn, $arg.name, $tcn, $arg.name);
    $lo += sprintf("   assert(%s);", $arg.name);

    return $lo;
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
	$t =~ s/  / /;
	trim $t;
	#printf("%s\n", $t);

	my ($rt, $name, $args) = $t =~ x/([a-zA-Z0-9\*<>:&]+) (\w+) \((.*)\)/;
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
	    if (inlist($type, class_list)) {
		$arg.is_class = True;
		$arg.classname = $type;
	    }
	    else if (inlist($type, abstract_class_list)) {
		$arg.is_abstract_class = True;
		$arg.classname = $type;
	    }
	    else if (inlist($type, dynamic_class_list)) {
		$arg.is_dynamic_class = True;
		$arg.classname = $type;
	    }
	    else if ($type == "float" || $type == "double")
		$arg.is_float = True;
	    else if (!$ref && $type !~ /\*/ && $type !~ /&/)
		$arg.is_int = True;

	    if (exists $arg.classname) {
		$arg.classname =~ s/\*//g;
		$arg.classname =~ s/&//g;
	    }
	    #printf("%N\n", $arg);
	}

	if (!exists $proto.$name)
	    $proto.$name = ( "funcname" : sprintf("%s_%s", $func_prefix, $name), "rt" : $rt, "inst" : () );

	$proto.$name.inst += ( "args" : $args, "orig" : $p );
    }

    foreach my $p in (keys $proto) {	
	# do function header
	$proto.$p.ok = True;
	my $lo = ();

	my $hdr;
	foreach my $i in ($proto.$p.inst)
	    $hdr += sprintf("//%s\n", $i.orig);
	$lo += sprintf("static QoreNode *%s(Object *self, Qore%s *%s, QoreNode *params, ExceptionSink *xsink)", 
		      $proto.$p.funcname, $qore_class, $cl);
	$lo += "{";

	my $callstr = $o.indep ? sprintf("%s->%s(", $cl, $p) : sprintf("%s->%s->%s(", $cl, $get_obj, $p);

	if (elements $proto.$p.inst == 1)
	    $lo += do_single_function($p, \$proto.$p, $proto.$p.inst[0], $callstr);
	else
	    $lo += do_multi_function($p, \$proto.$p, $proto.$p.inst, $callstr);

	$lo += "}";
    
	printf("%s", $hdr);
	foreach my $line in ($lo)
	    printf("%s%s\n", $proto.$p.ok ? "" : "//", $line);

	print("\n");
	#printf("%-15s %-20s (%s)\n", "(" + $rt + ")", $name, dlh($args));
    }

    foreach my $p in (keys $proto) {	
	printf("   %sQC_%s->addMethod(%-30s (q_method_t)%s);\n", $proto.$p.ok ? "" : "//", $cn, "\""+$p+"\",", $proto.$p.funcname);
    }
}

sub do_multi_class_header($offset, $final, $arg, $name, $i)
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
	   ? sprintf("*(static_cast<%s *>(%s))", $type, $arg.name) 
	   : sprintf("static_cast<%s *>(%s)", $type, $arg.name)) 
	: sprintf("%s->get%s()", $arg.name, $type);

    $lo += sprintf("%s%s *%s = (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink);", 
		   $os, $tcn, $arg.name, $tcn, $utn);
    $lo += sprintf("%sif (!%s) {", $os, $arg.name);
    if ($final) {
	$lo += sprintf("%s   if (!xsink->isException())", $os);
	my $str = sprintf("%s      xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"%s::%s() does not know how to handle arguments of class '%s' as passed as the ", 
			  		       $os, toupper($cn), toupper($name), $cn, $name);
	$str += sprintf("%s argument\", p->val.object->getClass()->getName());", ordinal[$i]);
	$lo += $str;
	$lo += sprintf("%s   return 0;", $os);
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

sub do_multi_function($name, $func, $inst, $callstr, $param, $offset)
{
    my $lo = ();

    my $os = substr(spaces, 0, $offset + 3);

    # separate into 3 lists: classes, ints, everything else
    my $cl = ();
    my $il = ();
    my $fl = ();
    my $rl = ();

    foreach my $i in (\$inst) {
	$i.callstr = $callstr;

	if (elements $i.args <= $param)
	    continue;

	if ($i.args[$param].is_class || $i.args[$param].is_abstract_class || $i.args[$param].is_dynanic_class)
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
    if (elements $cl) {
	$lo += sprintf("if (p && p->type == NT_OBJECT) {");
	
	for (my $cc = $param; $cc < elements $cl; ++$cc) {
	    #printf("cl=%N\nrl=%N\nil=%N\n", $cl, $rl, $fl);
	    $lo += do_multi_class_header(($cc + 1) * 3, $cc == (elements $cl) - 1, \$cl[$cc].args[$param], $name, $param);

	    append_call(\$cl[$cc].callstr, $cl[$cc].args[$param]);
	}
	for (my $cc = (elements $cl) - 1; $cc >= 0; --$cc) {
	    $lo += do_multi_class_trailer(($cc + 1) * 3, \$cl[$cc].args[$param]);
	    
	    # if this is the last argument for this branch, do call
	    if ($param == (elements $cl[$cc].args - 1)) {
		splice $cl[$cc].callstr, -2, 2, ")";
		$lo += do_return_value(($cc + 1) * 3, $func.rt, $cl[$cc].callstr, \$func.ok);
	    }
	    else
		$lo += do_multi_function($name, \$func, $cl, $cl[$cc].callstr, $param + 1, $cc * 3);
	}
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
	    if (elements $opt.args)
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

sub do_single_arg($offset, $name, $arg, $i, $ok)
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
		    $lo += "   return 0;";
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
	    $lo += do_class(\$inst.args[$i], $name, $cn, $i, True);
	else if ($inst.args[$i].is_abstract_class)
	    $lo += do_class(\$inst.args[$i], $name, $cn, $i);
	else if ($inst.args[$i].is_dynamic_class)
	    $lo += do_dynamic_class(\$inst.args[$i], $name, $cn, $i);
	else
	    $lo += do_single_arg(3, $name, $inst.args[$i], $i, \$func.ok);

	append_call(\$callstr, $inst.args[$i]);
    }
    if (elements $inst.args)
	splice $callstr, -2, 2, ")";
    else
	$callstr += ")";

    $lo += do_return_value(3, $func.rt, $callstr, \$func.ok);
    
    return $lo;
}

sub do_return_value($offset, $rt, $callstr, $ok)
{
    my $lo = ();
    my $os = substr(spaces, 0, $offset);

    switch ($rt) {
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
	case /^QPoint/:
	case /^QSize/:
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
