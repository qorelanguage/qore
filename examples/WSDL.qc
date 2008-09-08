# WSDL classes

# WSDL: Web Services Description Language: http://www.w3.org/TR/wsdl

# provides some minimal WSDL and XSD support for SOAP messaging
# used by the SoapClient class and the SoapHandler
# far from complete, needs proper namespace handling, better XSD support, complexType restriction handling, element groups, simpleType support, etc

namespace WSDL {
    const XET_ALL      = "ALL";
    const XET_CHOICE   = "CHOICE";
    const XET_SEQUENCE = "SEQUENCE";

    const SOAP_11_ENV  = "http://schemas.xmlsoap.org/soap/envelope/";
    const SOAP_12_ENV  = "http://www.w3.org/2003/05/soap-envelope";

    const SOAP_12_NS   = "http://schemas.xmlsoap.org/wsdl/soap12/";

    const XSD_NS       = "http://www.w3.org/2001/XMLSchema";
    const XSI_NS       = "http://www.w3.org/2001/XMLSchema-instance";

    # soap envelope namespaces
    const ENVELOPE_11_NS = 
	( "soapenv:Envelope" :
	  ( "^attributes^" : 
	    ( "xmlns:soapenv" : SOAP_11_ENV,
	      "xmlns:xsd"     : XSD_NS,
	      "xmlns:xsi"     : XSI_NS ) ) );

    const ENVELOPE_12_NS = 
	( "soapenv:Envelope" :
	  ( "^attributes^" : 
	    ( "xmlns:soapenv" : SOAP_12_ENV,
	      "xmlns:xsd"     : XSD_NS,
	      "xmlns:xsi"     : XSI_NS ) ) );

    const SOAP_ENCODING = "http://schemas.xmlsoap.org/soap/encoding/";
}

# abstract class providing helper methods to subclasses
class WSDL::XSDBase {
    static private doType($t, $nsinfo)
    {
	#printf("DEBUG: XSDBase::doType(%n, %n)\n", $t, $nsinfo);
	my ($ns, $type) = $t =~ x/(\w+):(\w+)/;
	if (!exists $type)
	    return ( "val" : $t );
	
	# if this is in the XML Schema namespace, then it's a base type
	if ($nsinfo.xml_schema.$ns)
	    return new XSDBaseType($type);
	
	return ( "ns"  : $ns,
		 "val" : $type );
    }

    static private removeNS($h)
    {
	foreach my $k in (keys $h) {
	    my ($ns, $name) = $k =~ x/(\w+):(\w+)/;
	    if (exists $ns) {
		if (type($h.$k) == Type::Hash)
		    $h.$k.ns = $ns;
		$h.$name = $h.$k;
		$h -= $k;
	    }
	}
    }
}

# abstract type common to all XSD classes
class WSDL::XSDData inherits XSDBase {
    getValue($mrh, $val)
    {
	if (exists $val."^attributes^".href) {
	    my $href = substr($val."^attributes^".href, 1);
	    
	    if (!exists $mrh.$href)
		throw "INVALID-REFERENCE", sprintf("multiRef id=%n does not exist", $href);
	    
	    return $mrh.$href;
	}
	return $val;	
    }
}

# abstract type common to all XSD classes with a "name" attribute
class WSDL::XSDNamedData inherits XSDData {
    constructor($e)
    {
	WSDL::XSDBase::removeNS(\$e);
	$.name = $e."^attributes^".name;
    }

    getName()
    {
	return exists $.name ? $.name : "<unnamed type>";
    }
}

class WSDL::XSDBaseType inherits XSDData {
    constructor($t)
    {
	$.type = $t;
	$.nstype = "xsd:" + $t;
    }

    serialize($val, $omit_type)
    {
	switch ($.type) {

	    case "byte": {
		my $v = int($val);
		if (($v & 0xff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
		break;
	    }

	    case "short": {
		my $v = int($val);
		if (($v & 0xffff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
		break;
	    }

	    case "int": {
		my $v = int($val);
		if (($v & 0xffffffff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
		break;
	    }

	    case "unsignedByte": {
		my $v = int($val);
		if (($v & 0xff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
	        if ($v < 0)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $v);
		break;
	    }

	    case "unsignedShort": {
		my $v = int($val);
		if (($v & 0xffff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
	        if ($v < 0)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $v);
		break;
	    }

	    case "unsignedInt": {
		my $v = int($val);
		if (($v & 0xffffffff) != $v)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $v, $.type);
	        if ($v < 0)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $v);
		break;
	    }

	    case "unsignedLong": {
		my $v = int($val);
	        if ($v < 0)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $v);
		break;
	    }

	    case "positiveInteger":
	        if ($val <= 0)
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("type %n only accepts positive values (value supplied: %d)", $.type, $val);
	        break;

	    case "date":
		$val = format_date("YYYYMMDD", date($val));
	        break;

	    case "dateTime":
		$val = format_date("YYYY-MM-DDTHH:mm:SS", date($val));
	        break;

	    case "boolean":
		$val = $val ? "true" : "false";
	        break;

	    case "time":
		$val = format_date("hh:mm:ss.ms", date($val));
	        break;

	    case "base64Binary":
		$val = makeBase64String($val);
	        break;

	    case "hexBinary":
		$val = makeHexString($val);
	        break;

	}
	return $omit_type ? $val : ( "^attributes^" : ( "xsi:type" : $.nstype ), "^value^" : $val );
    }

    deserialize($mrh, $val)
    {
	if (type($val) == Type::Hash) {
	    my $type = $val."^attributes^"."xsi:type";
	    my $t = ($type =~ x/\w+:(\w+)/)[0];
	    if (exists $t)
		$type = $t;
	    if ($type != $.type)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("expecting type %n, got %n", $.type, $val."^attributes^"."xsi:type");
	    $val = $val."^value^";
	}

	switch ($.type) {
	    case "string": 
	    case "anyURI":
	    # note that we do not convert xsd:integer to a qore integer to avoid losing precision
	    case "integer":
		return $val;
	    
	    case "byte":
		$val = int($val);
		if (($val & 0xff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "short":
		$val = int($val);
		if (($val & 0xffff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "int":
		$val = int($val);
		if (($val & 0xffffffff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "long":
		return int($val);

	    case "unsignedByte":
		$val = int($val);
		if (($val & 0xff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedShort":
		$val = int($val);
		if (($val & 0xffff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedInt":
		$val = int($val);
		if (($val & 0xffffffff) != $val)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedLong":
	        if ($val < 0)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
		return int($val);

	    case "positiveInteger":
	        if ($val <= 0)
		    throw "SOAP-DESERIALIZATION-ERROR", sprintf("type %n only accepts positive values (value supplied: %d)", $.type, $val);
		return int($val);

	    case "date":
		return date($val + "000000");
		
	    case "dateTime":
		return date(substr($val, 0, 4) + substr($val, 5, 2) + substr($val, 8, 2) + 
			    substr($val, 11, 2) + substr($val, 14, 2) + substr($val, 17, 2));
    
	    case "time":
		return date("19700101" + substr($val, 0, 2) + substr($val, 3, 2) + substr($val, 6, 2)) +
		       milliseconds(substr($val, 9, 3));

	    case "boolean":
		if ($val =~ /true/i)
		    return True;
	        if ($val =~ /false/i)
		    return False;
	        return boolean($val);
		
	    case "decimal":
		return float($val);

	    case "base64Binary":
		return parseBase64String($val);

	    case "hexBinary":
		return parseHexString($val);

	  default:
	    throw "SOAP-DECODING-ERROR", sprintf("don't know how to handle type %n", $.type);
	}
    }

    getName()
    {
	return $.type;
    }

    getNameWithNS()
    {
	return $.nstype;
    }
}

class WSDL::XSDElement inherits XSDNamedData {
    constructor($e, $nsinfo) : XSDNamedData(\$e)
    {
	my $a = $e."^attributes^";
	#printf("XSDElement(%n)\n", $a);
	
	if (exists $a.minOccurs)
	    $.minOccurs = int($a.minOccurs);
	else
	    $.minOccurs = 1;

	if (exists $a.maxOccurs)
	    if ($a.maxOccurs == "unbounded")
	        $.maxOccurs = -1;
	    else
		$.maxOccurs = int($a.maxOccurs);
	else
	    $.maxOccurs = 1;

	if ($.maxOccurs != -1 && $.minOccurs > $.maxOccurs)
	    throw "XSD-ELEMENT-ERROR", sprintf("minOccurs (%d) > maxOccurs (%d) for element %s", $.minOccurs, $.maxOccurs, $.name);

	if ($a.nillable == "true")
	    $.nillable = True;

	if (exists $a.type)
	    $.type = WSDL::XSDBase::doType($a.type, $nsinfo);

	if (exists $e.simpleType)
	    throw "XSD-ELEMENT-ERROR", "can't parse simpleTypes yet";
	else if (exists $e.complexType)
	    $.type = new XSDComplexType($e.complexType, $nsinfo);
    }

    serialize($h, $key, $typename, $omit_type)
    {
	if (!exists $h) {
	    if ($.minOccurs) {
		if ($.nillable) {
		    my $rh = ("xsi:nil" : "true");
		    if (!$omit_type)
			$rh += ("xsi:type" : $.type.getNameWithNS());
		    return ("^attributes^" : $rh);
		}
	        else
		    throw "SOAP-SERIALIZATION-ERROR", sprintf("missing value for %s.%s (minOccurs=%d, type %n)", $typename, $key, $.minOccurs, $.type.getName());
	    }
	    else
		return;
	}
	return $.type.serialize($h, $omit_type);
    }

    deserialize($mrh, $val)
    {
	if (!exists $val) {
	    if ($.nillable || !$.minOccurs)
		return;
	    throw "SOAP-DESERIALIZATION-ERROR", sprintf("NOTHING passed for element %n, but nillable=False and minOccurs=%d", $.name, $.minOccurs);
	}

	if (type($val) == Type::List) {
	    my $el = elements $val;
	    if ($.maxOccurs != -1 && $el > $.maxOccurs)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("error deserializing element %n, maxOccurs=%d but list is %d elements long", $.name, $.maxOccurs, $el);
	    if ($el < $.minOccurs)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("error deserializing element %n, minOccurs=%d but list is %d elements long", $.name, $.minOccurs, $el);

	    my $l;
	    foreach my $e in ($val)
		$l[elements $l] = $.type.deserialize($mrh, $.getValue($mrh, $e));
	    return $l;
	}

	if ($.minOccurs > 1)
	    throw "SOAP-DESERIALIZATION-ERROR", sprintf("single value passed for element %n, but minOccurs=%d", $.name, $.minOccurs);

	return $.type.deserialize($mrh, $.getValue($mrh, $val));
    }
}

class WSDL::XSDComplexType inherits XSDNamedData
{
    constructor($ct, $nsinfo) : XSDNamedData(\$ct)
    {
	$.nsinfo = $nsinfo;
	#my $a = $ct."^attributes^";

	my $d = $ct.complexContent;
	if (exists $d) {	
	    WSDL::XSDBase::removeNS(\$d);
	    if (exists $d.restriction) {
		$.parseData($d.restriction);
	    }
	    else
		throw "XSD-COMPLEXCONTENT-ERROR", sprintf("can't parse %n", $d);
	}
	else
	    $.parseData($ct);
    }

    private parseData($d)
    {
	if (exists $d.all) {
	    $.hash_type = XET_ALL;
	    WSDL::XSDBase::removeNS(\$d.all);
	    $.elements = $.parseElements($d.all.element);
	}
	else if (exists $d.choice) {
	    $.hash_type = XET_CHOICE;
	    WSDL::XSDBase::removeNS(\$d.choice);
	    $.elements = $.parseElements($d.choice.element);
	}
	else if (exists $d.sequence) {
	    $.hash_type = XET_SEQUENCE;
	    WSDL::XSDBase::removeNS(\$d.sequence);
	    $.elements = $.parseElements($d.sequence.element);
	}
	else
	    throw "XSD-COMPLEXTYPE-ERROR", sprintf("unknown keys in %n", $d);
    }

    private parseElements($el)
    {
	#printf("DEBUG: XSDComplexType::parseElements(%n)\n", $el);
	my $h;
	foreach my $e in ($el) {
	    my $elem = new XSDElement($e, $.nsinfo);
	    $h.($elem.name) = $elem;
	}
	return $h;
    }

    serialize($h, $omit_type)
    {
	my $rh;

	if (type($h) != Type::Hash)
	    throw "SOAP-SERIALIZATION-ERROR", sprintf("expecting hash argument to serialize from complexType %n (got %n, type %n)", $.getName(), $h, type($h));

	if ($.hash_type == XET_SEQUENCE || $.hash_type == XET_ALL) {
	    foreach my $p in (keys $.elements) {
		#printf("element=%s=%N\nvalue=%N\n", $p, $.elements.$p, $h.$p);
		my $e = $.elements.$p.serialize($h.$p, $p, $.name, $omit_type);
		if (exists $e)
		    $rh.$p = $e;
		delete $h.$p;
	    }
	    if (elements $h)
		throw "SOAP-SERIALIZATION-ERROR", sprintf("%n %s of type %n (valid elements: %n)", (my $kl = keys $h), elements $kl == 1 ? "is an invalid member" : "are invalid members", $.getName(), keys $.elements);
	}
	else { # "choice" - union
	    if (elements $h > 1)
		throw "SOAP-SERIALIAZION-ERROR", sprintf("cannot serialize type %s with more than 1 member (%n)", $.getName(), keys $h);
	    my $key = (keys $h)[0];
	    if (!exists $.elements.$key)
		throw "SOAP-SERIALIZATION-ERROR", sprintf("%n is an invalid member of type %n", $key, $.getName());
	    $rh.$key = $.elements.$key.serialize($h.$key, $key, $.name, $omit_type);
	}
	
	if (exists $.name)
	    $rh."^attributes^" = ( "xsi:type" : "ns1:" + $.name );
	#printf("complex type %s returning %n from %n\n", $.name, $rh, $h);
	return $rh;
    }

    deserialize($mrh, $val)
    {
	my $rh;

	if (type($val) != Type::Hash)
	    throw "SOAP-DESERIALIZATION-ERROR", sprintf("cannot deserialize type %n from qore type %n (expecting hash)", $.getName(), type($val));

	my $attr = $val."^attributes^";
	delete $val."^attributes^";

	# ensure types match
	my $tn = $attr."xsi:type";
	if (exists $tn) {
	    my ($ns, $name) = $tn =~ x/(.*):(.*)/;
	    if (exists $name)
		$tn = $name;
	    if ($tn != $.name)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("expecting type %n, got %n", $.getName(), $tn);
	}

	if ($.hash_type == XET_SEQUENCE || $.hash_type == XET_ALL) {
	    foreach my $p in (keys $.elements) {
		#printf("element %n\n", $p);
		$rh.$p = $.elements.$p.deserialize($mrh, $.getValue($mrh, $val.$p));
		delete $val.$p;
	    }
	    delete $val."^attributes^";
	    if (elements $val)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("invalid element %n passed in type %n", (keys $val)[0], $.getName());
	}
	else { # "choice" - union
	    my $kl = keys $val;
	    if (elements $kl > 1)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("too many elements supplied for union type %n (%n)", $.getName(), $kl);
	    $kl = $kl[0];
	    if (!exists $.elements.$kl)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("element %n is not a valid element for union type %n", $kl, $.getName());

	    $rh.$kl = $.elements.$kl.deserialize($mrh, $.getValue($mrh, $val.$kl));
	}
	return $rh;
    }

    getNameWithNS()
    {
	return "ns1:" + $.name;
    }
}

class WSDL::WSOperation inherits XSDNamedData
{
    constructor($p, $targetns, $messages, $soap12) : XSDNamedData(\$p)
    {
	$.targetns = $targetns;
	$.soap12 = $soap12;

	my $msghash = $.processNSValue($p.input."^attributes^");

	my $msg = $messages.($msghash.message.val);
	if (!exists $msg)
	    throw "WSDL-ERROR", sprintf("missing definition for input message %n required by operation %n", $msghash.name.val, $.name, $.messages);
	$.input = $msg;

	if (exists $p.output) {
	    $msghash = $.processNSValue($p.output."^attributes^");

	    $msg = $messages.($msghash.message.val);
	    if (!exists $msg)
		throw "WSDL-ERROR", sprintf("missing definition for output message %n required by operation %n", $msghash.name.val, $.name);

	    $.output = $msg;
	}
    }

    setDocStyle($idocmap)
    {
	$.docstyle = True;

	if (elements $.input.args > 1)
	    throw "WSDL-ERROR", sprintf("don't know how to handle document-style messages for operation %n that has more than one top-level part (%n)",
					$.name, keys $.input.args);

	my $arg = (keys $.input.args)[0];
	my $element = $.input.args.$arg.element.name;
	$.request_name = $element;
	$idocmap.$element = $.input.args.$arg.element;
    }		

    setTopLevelRequestElement($name)
    {
	$.request_name = $name;
    }

    getTopLevelRequestName()
    {
	return exists $.request_name ? $.request_name : $.name;
    }

    # returns an XML string in the SOAP request format
    serializeRequest($h)
    {
	# setup namespaces for SOAP envelope
	my $rh = $.soap12 ? WSDL::ENVELOPE_12_NS : WSDL::ENVELOPE_11_NS;

	if ($.docstyle)
	    $rh."soapenv:Envelope"."soapenv:Body" = $.input.serializeDocument($h, $.targetns);
	else
	    $rh."soapenv:Envelope"."soapenv:Body" = $.input.serialize($.name, $h, $.targetns);

	#return makeXMLString($rh);
	# DEBUG
	return makeFormattedXMLString($rh);
    }

    # returns an XML string in the SOAP response format
    serializeResponse($h)
    {
	# setup namespaces for SOAP envelope
	my $rh = $.soap12 ? WSDL::ENVELOPE_12_NS : WSDL::ENVELOPE_11_NS;

	if ($.docstyle)
	    $rh."soapenv:Envelope"."soapenv:Body" = $.output.serializeDocument($h, $.targetns);
	else
	    $rh."soapenv:Envelope"."soapenv:Body" = $.output.serialize($.name, $h, $.targetns);

	#return makeXMLString($rh);
	# DEBUG
	return makeFormattedXMLString($rh);
    }

    private processMultiRef($body)
    {
	# setup multiRef lookup hash, if any are present
	my $mrh;
	if (exists $body.multiRef) {
	    foreach my $mr in ($body.multiRef)
		$mrh.($mr."^attributes^".id) = $mr;

	    # resolve interior references to multiRefs
	    foreach my $id in (keys $mrh) {
		foreach my $key in (keys $mrh.$id) {
		    if ($key == "^attributes^")
			continue;

		    #printf("multiRef id=%n key=%n val=%n\n", $id, $key, $mrh.$id.$key);

		    if (type($mrh.$id.$key) == Type::List) {
			foreach my $e in (\$mrh.$id.$key) {
			    my $href = substr($e."^attributes^".href, 1);
			    if (exists $href) {
				if (!exists $mrh.$href)
				    throw "INVALID-MULTIREF", sprintf("multiRef id=%n does not exist", $href);
				$e = $mrh.$href;
			    }
			}
		    }
		    else if (type($mrh.$id.$key) == Type::Hash) {
			my $href = substr($mrh.$id.$key."^attributes^".href, 1);
			if (exists $href) {
			    if (!exists $mrh.$href)
				throw "INVALID-MULTIREF", sprintf("multiRef id=%n does not exist", $href);
			    $mrh.$id.$key = $mrh.$href;
			}
		    }
		}
	    }

	    delete $body.multiRef;
	}
	#printf("mrh=%N\n", $mrh);
	#printf("operation=%N\n", $self); 

	my $msg;
	foreach my $key in (keys $body) {
	    if ($key != "^attributes^") {
		$msg = $body.$key;
		break;
	    }
	}

	# remove namespace tags from element names
	WSDL::XSDBase::removeNS(\$msg);

	return ($mrh, $msg);
    }

    deserializeRequest($o)
    {
	WSDL::XSDBase::removeNS(\$o);
	WSDL::XSDBase::removeNS(\$o.Envelope);

	my $body = $o.Envelope.Body;

	my ($mrh, $msg) = $.processMultiRef($body);

	return $.docstyle ? $.input.deserializeDocument($mrh, $msg) : $.input.deserialize($mrh, $msg);
    }

    deserializeResponse($o)
    {
	WSDL::XSDBase::removeNS(\$o);
	WSDL::XSDBase::removeNS(\$o.Envelope);

	my $body = $o.Envelope.Body;

	my ($mrh, $msg) = $.processMultiRef($body);

	# check for Soap Fault, if so raise an exception immediately with the fault info
	WSDL::XSDBase::removeNS(\$body);
	if (exists $body.Fault) {
	    WSDL::XSDBase::removeNS(\$body.Fault);
	    my $desc = sprintf("The following fault response was received from the server: code=%n", $body.Fault.faultcode);
	    if (exists $body.Fault.faultstring)
		$desc += sprintf(", faultstring=%n", $body.Fault.faultstring);
	    if (exists $body.Fault.desc)
		$desc += sprintf(", desc=%n", $body.Fault.desc);
	    
	    throw "SOAP-SERVER-FAULT-RESPONSE", $desc, $body.Fault;
	}

	return $.docstyle ? $.output.deserializeDocument($mrh, $msg) : $.output.deserialize($mrh, $msg);
    }

    private processNSValue($h)
    {
	foreach my $k in (keys $h) {
	    my ($ns, $name) = $h.$k =~ x/(\w+):(\w+)/;
	    if (!exists $name)
		$h.$k.val = $h.$k;
	    else {
		$h.$k.ns = $ns;
		$h.$k.val = $name;
	    }
	}
	return $h;
    }
}

class WSDL::WSMessage inherits XSDNamedData
{
    constructor($m, $element_map) : XSDNamedData(\$m)
    {
	#printf("DEBUG: WSMessage::constructor() element_map: %n\n", $element_map);

	$.name = $m."^attributes^".name;
	foreach my $p in ($m.part) {
	    my $arg = $p."^attributes^";
	    if (exists $arg.element) {
		my ($ns, $name) = $arg.element =~ x/(\w+):(\w+)/;
		if (!exists $name)
		    $name = $arg.element;

		#printf("DEBUG: WSMessage::constructor() message %n element %n = %n\n", $.name, $arg.element, $element_map.$name);

		if (!exists $element_map.$name)
		    throw "WSDL-ERROR", sprintf("message %n references unknown element %n", $.name, $name);
		$.args.($arg.name).element = $element_map.$name;
	    }
	    else {
		$.args.($arg.name) = $arg;
		$.args.($arg.name).type = WSDL::XSDBase::doType($p."^attributes^".type);
	    }
	}
    }

    serialize($name, $h, $targns)
    {
	my $rh;
	#printf("DEBUG: message %s: h=%n\n", $.name ,$h);
    
	foreach my $k in (keys $.args) {
	    if (!exists $h.$k)
		throw "SOAP-SERIALIZATION-ERROR", sprintf("missing message argument %n", $k);
	    $rh.$k = $.args.$k.type.serialize($h.$k);
	    #printf("DEBUG: arg %s got %n from %n\n", $k, $rh.$k, $h.$k);
	}
	
	$rh."^attributes^" = ( "xmlns:ns1" : $targns );
	
	if ($.encoded)
	    $rh."^attributes^" += 
	    ( "soapenv:encodingStyle" : SOAP_ENCODING,
	      "xmlns:soapenc"         : SOAP_ENCODING );
	
	my $rvh.("ns1:" + $name) = $rh;
	
	return $rvh;
    }

    serializeDocument($h, $targns)
    {
	my $k = (keys $.args)[0];

	#printf("DEBUG: message %s: args=%n h=%n\n", $.name, keys $.args, $h);
    
	my $th = $.args.$k.element.serialize($h, $k, "<unknown>", True);

	my $rh."^attributes^" = ( "xmlns:ns1" : $targns );

	if ($.encoded)
	    $rh."^attributes^" += 
	    ( "soapenv:encodingStyle" : SOAP_ENCODING,
	      "xmlns:soapenc"         : SOAP_ENCODING );
	
	$rh.("ns1:" + $.args.$k.element.name) = $th;

	return $rh;
    }

    deserialize($mrh, $val)
    {
	my $ro;

	foreach my $key in (keys $.args) {
	    if (!exists $val.$key)
		throw "SOAP-DESERIALIZATION-ERROR", sprintf("message %n is missing argument %n", $.name, $key);

	    $ro.$key = $.args.$key.type.deserialize($mrh, $.getValue($mrh, $val.$key));
	}

	# if there is only one argument, return it directly
	if (elements $ro == 1)
	    return hash_values($ro)[0];
	return $ro;
    }

    deserializeDocument($mrh, $val)
    {
	my $k = (keys $.args)[0];

	return $.args.$k.element.deserialize($mrh, $val);
    }
}

class WSDL::WebService inherits XSDBase {
    constructor($str)
    {
	my $h = parseXML($str);

	# remove namespace (normally "wsdl:") prefix from keys if present
	WSDL::XSDBase::removeNS(\$h);
	WSDL::XSDBase::removeNS(\$h.definitions);
	$.getNSPrefixes($h.definitions."^attributes^");

	if (exists $h.definitions.types)
	    $.parseTypes($h.definitions.types);
	
	if (exists $h.definitions.message)
	    $.parseMessage($h.definitions.message);
	
	if (exists $h.definitions.portType)
	    $.parsePortType($h.definitions.portType);

	if (exists $h.definitions.binding)
	    $.parseBinding($h.definitions.binding);
	
	if (exists $h.definitions.service)
	    $.parseService($h.definitions.service);
    }

    # return a map of top-level element names to WSOperation objects
    getOperationMap($name)
    {
	if (!exists $name) {
	    if (elements $.services > 1)
		throw "WSDL-GET-INPUT-OPERATION-MAP-ERROR", sprintf("no service argument passed to WSDL::getOperationMap() but the WSDL defines more than one service (%n)", $.services);
	    $name = $.services[0];
	}
	else if (!inlist($name, $.services))
	    throw "WSDL-GET-INPUT-OPERATION-MAP-ERROR", sprintf("service argument %n passed to WSDL::getOperationMap() is not defined by this WSDL (vaild services: %n)", $name, $.services);

	return $.idocmap + $.imap;
    }

    private getNSPrefixes($a)
    {
	foreach my $k in (keys $a) {
	    my $ns = ($k =~ x/xmlns:(\w+)/)[0];
	    
	    if (!exists $ns)
		continue;
	    $.ns.map.$ns = $a.$k;

	    if ($a.$k == "http://www.w3.org/2001/XMLSchema")
		$.ns.xml_schema.$ns = True;

	    if ($a.$k == SOAP_12_NS)
		$.soap12 = True;
	}
	# default has to be quoted because it's a reserved word
	if (exists $a.xmlns)
	    $.ns."default" = $a.xmlns;
	$.ns.target = $a.targetNamespace;
    }

    private getBaseType($t)
    {
	if (exists $.base_type.$t)
	    return $.base_type.$t;

	return $.base_type.$t = new XSDBaseType($t);
    }

    private resolveType($v)
    {
	if (exists $v.ns && $.ns.xml_schema.($v.ns))
	    return $.getBaseType($v.val);

	# find type
	if (!exists $.types.($v.val))
	    throw "WSDL-ERROR", sprintf("cannot resolve type %n", $v.val);
	
	return $.types.($v.val);
    }

    # parse types
    private parseTypes($data)
    {
	WSDL::XSDBase::removeNS(\$data);
	foreach my $schema in ($data.schema) {
	    WSDL::XSDBase::removeNS(\$schema);
	    #printf("DEBUG: WebService::parseTypes() schema=%N\n", $schema);

	    foreach my $ct in ($schema.complexType) {
		my $t = new XSDComplexType($ct, $.ns);
		$.types.($t.name) = $t;
	    }
	    # make element map
	    foreach my $el in ($schema.element) {
		my $attr = $el."^attributes^";
		if (exists $attr.type)
		    $.element_map.($attr.name) = WSDL::XSDBase::doType($attr.type, $.ns);
		else
		    $.element_map.($attr.name) = new WSDL::XSDElement($el, $.ns);

	    }
	    # resolve types
	    foreach my $t in (keys $.types)
		if ($.types.$t instanceof XSDComplexType)
		    foreach my $e in (keys $.types.$t."elements")
		        if (!($.types.$t."elements".$e.type instanceof XSDData))
			    $.types.$t."elements".$e.type = $.resolveType($.types.$t."elements".$e.type);
	}
    }
	
    private parseMessage($message)
    {
	# parse messages
	foreach my $m in ($message) {
	    my $msg = new WSMessage($m, $.element_map);
	    foreach my $arg in (keys $msg.args) {
		#printf("DEBUG: WebService::parseMessage(): %n: %n\n", $arg, $msg);
		#printf("DEBUG: WebService::parseMessage(): %n: %n\n", $arg, $msg.args.$arg.type);
		if (exists $msg.args.$arg.type && !($msg.args.$arg.type instanceof XSDData))
		    $msg.args.$arg.type = $.resolveType($msg.args.$arg.type); 
	    }
	    $.messages.($msg.name) = $msg;
	}
    }

    private parseService($svc)
    {
	WSDL::XSDBase::removeNS(\$svc);
	$.services.name = $svc."^attributes^".name;
	foreach my $port in ($svc.port) {
	    WSDL::XSDBase::removeNS(\$port);
	    my $name = $port."^attributes^".name;
	    $.services.port.$name = $port."^attributes^";
	    $.services.port.$name.address = $port.address."^attributes^".location;
	    $.services.port.$name.binding = WSDL::XSDBase::doType($.services.port.$name.binding);
	}
    }

    private parsePortType($data)
    {
	# setup list of services defined in this WSDL
	$.services = ();

        foreach my $port in ($data) {
	    WSDL::XSDBase::removeNS(\$port);
	    #printf("DEBUG: portType=%N\n", $port);
	    my $name = $port."^attributes^".name;
	    $.services += $name;
	    foreach my $p in ($port.operation) {
		my $op = new WSOperation($p, $.ns.target, $.messages, $.soap12);
		$.portType.$name.operations.($op.name) = $op;
		#printf("DEBUG: %n registered operation %n\n", $name, $op.name);
		$.opmap.($op.name) = $op;
	    }
	}
    }

    private parseBinding($data)
    {
	WSDL::XSDBase::removeNS(\$data);
	$data += $data."^attributes^";
	delete $data."^attributes^";

	foreach my $ophash in ($data.operation) {
	    my $name = $ophash."^attributes^".name;
	    if (!exists $name)
		throw "WSDL-ERROR", sprintf("binding for %n references unnamed operation (%n)", $data.name, $ophash);

	    my $op = $.opmap.$name;
	    if (!exists $op)
		throw "WSDL-ERROR", sprintf("binding for %n references unknown operation %n", $data.name, $name);

	    WSDL::XSDBase::removeNS(\$ophash);

	    if ($ophash.operation."^attributes^".style == "document") {
		my $element = $op.setDocStyle(\$.idocmap);
	    }

	    WSDL::XSDBase::removeNS(\$ophash.input);
	    if ($ophash.input.body."^attributes^".use == "encoded") {
		$op.input.encoded = True;
		#printf("DEBUG: setting encoding = True for %n.%n input\n", $data.name, $name);
	    }

	    WSDL::XSDBase::removeNS(\$ophash.output);
	    if ($ophash.output.body."^attributes^".use == "encoded") {
		$op.output.encoded = True;
		#printf("DEBUG: setting encoding = True for %n.%n output\n", $data.name, $name);
	    }
	}

	$.binding = $data;
    }
}
