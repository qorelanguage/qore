#!/usr/bin/env qore

%require-our
%enable-all-warnings

const opts = (
    "abstract" : "a,abstract",
    "indep"    : "i,independent",
    "help"     : "h.help"
    );

const ordinal = ( "first", "second", "third", "fourth", "fifth", "sixth", "seventh", "eighth", "ninth", "tenth" );
		  

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

sub do_return_class($type, $callstr)
{
    $type =~ s/&//g;
    my $cl = $type;
    $cl =~ s/[a-z]//g;
    $cl = tolower($cl);

    my $lo = ();

    my $utn = toupper($type);

    $lo += "";
    $lo += sprintf("   Qore%s *q_%s = new Qore%s(%s);", $type, $cl, $type, $callstr);
    $lo += sprintf("   Object *o_%s = new Object(self->getClass(CID_%s), getProgram());", $cl, $utn);
    $lo += sprintf("   o_%s->setPrivate(CID_%s, q_%s);", $cl, $utn, $cl);
    $lo += sprintf("   return new QoreNode(o_%s);", $cl);

    return $lo;
}

sub do_class($arg, $name, $cn, $i, $d)
{
    my $lo = ();
    
    my $type = $arg.type;
    $type =~ s/\*//;
    my $utn = toupper($type);

    my $tcn = sprintf("Qore%s%s", $d ? "" : "Abstract", $type);

    $arg.get = 
	$d 
	? ($arg.ref 
	   ? sprintf("*((%s *)%s)", $type, $arg.name) 
	   : sprintf("(%s *)%s", $type, $arg.name)) 
	: sprintf("%s->get%s()", $arg.name, $type);

    $lo += sprintf("   %s *%s = (p && p->type == NT_OBJECT) ? (%s *)p->val.object->getReferencedPrivateData(CID_%s, xsink) : 0;", 
		   $tcn, $arg.name, $tcn, $utn);
    $lo += sprintf("   if (!p || !%s)", $arg.name);
    $lo += "   {";
    $lo += "      if (!xsink->isException())";
    $lo += sprintf("         xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a %s object as %s argument to %s::%s()\");", 
		   toupper($cn), toupper($name), $type, ordinal[$i], $cn, $name);
    $lo += "      return 0;";
    $lo += "   }";
    $lo += sprintf("   ReferenceHolder<%s> holder(%s, xsink);", $tcn, $arg.name);

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
		else
		    $pname = tolower($arg);
		$arg =~ s/&//;
	    }
	    my $ref;
	    if ($type =~ /&/) {
		$type =~ s/&//;
		$ref = True;
	    }
	    $arg = ( "type" : $type, "name" : $pname, "def" : $def, "ref" : $ref );
	    #printf("%N\n", $arg);
	}

	$proto += ( "name" : $name, "rt" : $rt, "args" : $args, "orig" : $p, "funcname" : sprintf("%s_%s", $func_prefix, $name) );
    }

    foreach my $p in (\$proto) {	
	# do function header
	$p.ok = True;
	my $lo = ();

	my $hdr = sprintf("//%s", $p.orig);
	$lo += sprintf("static QoreNode *%s(Object *self, Qore%s *%s, QoreNode *params, ExceptionSink *xsink)", 
		      $p.funcname, $qore_class, $cl);
	$lo += "{";

	my $callstr = $o.indep ? sprintf("%s->%s(", $cl, $p.name) : sprintf("%s->%s->%s(", $cl, $get_obj, $p.name);

	# do arguments
	for (my $i = 0; $i < elements $p.args; ++$i) {
	    $lo += sprintf("   %sp = get_param(params, %d);", $i ? "" : "QoreNode *", $i);
	    switch ($p.args[$i].type) {
		case "QRgb":
		case "quint32":
		case "int": {
		    if ($p.args[$i].def)
			$lo += sprintf("   int %s = !is_nothing(p) ? p->getAsInt() : %d;", $p.args[$i].name, $p.args[$i].def);
		    else
			$lo += sprintf("   int %s = p ? p->getAsInt() : 0;", $p.args[$i].name);
		    break;
		}
		case "bool": {
		    if ($p.args[$i].def =~ /true/)
			$lo += sprintf("   bool %s = !is_nothing(p) ? p->getAsBool() : true;", $p.args[$i].name);
		    else
			$lo += sprintf("   bool %s = p ? p->getAsBool() : 0;", $p.args[$i].name);
		    break;
		}
		case "qreal":
		case "float": {
		    $lo += sprintf("   float %s = p ? p->getAsFloat() : 0;", $p.args[$i].name);
		    break;
		}
		case "QString":
		case "char*": {
		    $lo += "   if (!p || p->type != NT_STRING) {";
		    $lo += sprintf("      xsink->raiseException(\"%s-%s-PARAM-ERROR\", \"expecting a string as %s argument to %s::%s()\");", 
				   toupper($cn), toupper($p.name), ordinal[$i], $cn, $p.name);
		    $lo += "      return 0;";
		    $lo += "   }";
		    $lo += sprintf("   const char *%s = p->val.String->getBuffer();", $p.args[$i].name);
		    break;
		}
		case "QFont*":
		case "QFont":
		case "QRectF*":
		case "QRectF":
		case "QRect*":
		case "QRect":
		case "QRegion*":
		case "QRegion":
		case "QBrush*":
		case "QBrush":
		case "QColor*":
		case "QColor": {
		    $lo += do_class(\$p.args[$i], $p.name, $cn, $i, True);
		    break;
		}

		case "QPaintDevice*":
		case "QObject*":
		case "QWidget*":
		case "QLayout*": {
		    $lo += do_class(\$p.args[$i], $p.name, $cn, $i);
		    break;
		}
		
	        default: {
		    if (!$p.args[$i].ref && $p.args[$i].type !~ /\*/ && $p.args[$i].type !~ /&/) {
			if ($p.args[$i].def) {
			    $lo += sprintf("   %s %s = (%s)(!is_nothing(p) ? p->getAsInt() : %d);" ,$p.args[$i].type, $p.args[$i].name,
, $p.args[$i].type, $p.args[$i].def);
			}
			else
			{
			    # add class prefix to enum if not already present
			    my $tn = $p.args[$i].type;
			    if ($tn !~ /::/) 
				$tn = $cn + "::" + $tn;
			    $lo += sprintf("   %s %s = (%s)(p ? p->getAsInt() : 0);", $tn, $p.args[$i].name, $tn);
			}
		    }
		    else {
			#printf("DEBUG err arg type=%n (%n)\n", $p.args[$i].type, $p.args[$i]);
			$lo += sprintf("   ??? %s %s = p;", $p.args[$i].type, $p.args[$i].name);
			$p.ok = False;
		    }
		    break;
		}
	    }
	    $callstr += sprintf("%s, ", exists $p.args[$i].get ? $p.args[$i].get : $p.args[$i].name);
	}
	if (elements $p.args)
	    splice $callstr, -2, 2, ")";
	else
	    $callstr += ")";

	switch ($p.rt) {
	    case "void": {
		$lo += sprintf("   %s;", $callstr);
		$lo += "   return 0;"; 
		break;
	    }
	    case "bool": {
		$lo += sprintf("   return new QoreNode(%s);", $callstr); 
		break;
	    }
	    case =~ m/::/:
	    case "quint32" :
	    case "QRgb":
	    case "int" : {
		$lo += sprintf("   return new QoreNode((int64)%s);", $callstr); 
		break;
	    }
	    case "qreal" :
	    case "float" : {
		$lo += sprintf("   return new QoreNode(%s);", $callstr); 
		break;
	    }
	    case "QRegion":
	    case "QRegion&":
	    case "QRectF":
	    case "QRectF&":
	    case "QRect":
	    case "QRect&":
	    case "QColor":
	    case "QColor&": {
		$lo += do_return_class($p.rt, $callstr);
		break;
	    }		
	    case "QString": {
		$lo += sprintf("   return new QoreNode(new QoreString(%s.toUtf8().data(), QCS_UTF8));", $callstr); 
		break;
	    }
	    default: {
		if ($p.rt !~ /\*/)
		    $lo += sprintf("   ??? return new QoreNode((int64)%s);", $callstr);
		else
		    $lo += sprintf("   ??? return %s;", $callstr); 
		$p.ok = False; 
		break;
	    }
	}
	$lo += "}";
    
	printf("%s\n", $hdr);
	foreach my $line in ($lo)
	    printf("%s%s\n", $p.ok ? "" : "//", $line);

	print("\n");
	#printf("%-15s %-20s (%s)\n", "(" + $rt + ")", $name, dlh($args));
    }

    foreach my $p in ($proto) {	
	printf("   %sQC_%s->addMethod(%-30s (q_method_t)%s);\n", $p.ok ? "" : "//", $cn, "\""+$p.name+"\",", $p.funcname);
    }
}

main();
