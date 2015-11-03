# WSDL classes

# WSDL: Web Services Description Language: http://www.w3.org/TR/wsdl

# provides some minimal WSDL and XSD support for SOAP messaging used by the
# SoapClient class and the SoapHandler
# not complete, needs namespace verification, improved XSD support, element
# groups, etc

# 0.3.2: fixed bugs de/serializing negative values for "int" and "short"
# 0.3.1: improved XSD imports and namespace handling

# 0.3: implemented WSDLLib class of helper functions
#      implemented support for xsd import statements in WSDLs

# 0.2: use parseXMLAsData() instead of parseXML()
#      implemented initial simpleType support
#      fixed xsd:date serialization and deserialization

# make sure we have the required qore version
# 0.7.3 has parseXMLAsData()
# 0.7.6 has object -= list and object -= string handling
%requires qore >= 0.7.6

namespace WSDL {
    const version     = "0.3.2";

    const XET_ALL      = "ALL";
    const XET_CHOICE   = "CHOICE";
    const XET_SEQUENCE = "SEQUENCE";

    const SOAP_11_ENV  = "http://schemas.xmlsoap.org/soap/envelope/";
    const SOAP_12_ENV  = "http://www.w3.org/2003/05/soap-envelope";

    const SOAP_12_NS   = "http://schemas.xmlsoap.org/wsdl/soap12/";

    const XSD_NS       = "http://www.w3.org/2001/XMLSchema";
    const XSI_NS       = "http://www.w3.org/2001/XMLSchema-instance";

    const HTTP_NS      = "http://schemas.xmlsoap.org/wsdl/http/";
    const MIME_NS      = "http://schemas.xmlsoap.org/wsdl/mime/";

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

    # mapping from Qore types to xsd types for xsd type "anyType"
    const any_type_map = 
	( Type::String      : "string",
	  Type::Int         : "long",
	  Type::Boolean     : "boolean",
	  Type::Date        : "dateTime",
	  Type::Float       : "decimal",
	  Type::NothingType : "string",
	  Type::NullType    : "string",
	  Type::Binary      : "base64Binary" );

    # error codes, to save memory, only defined once
    const SOAP_SERIALIZATION_ERROR = "SOAP-SERIALIZATION-ERROR";
    const SOAP_DESERIALIZATION_ERROR = "SOAP-DESERIALIZATION-ERROR";
    const WSDL_ERROR = "WSDL-ERROR";

    const RANGE_SHORT = (-32768, 32767);
    const RANGE_INT = (-2147483648, 2147483647);

    # currently there are no options
    #const Options = ();
}

class WSDLLib {
    static getFile($url, $u) {
	my $fn = $u.host + $u.path;
	if (!exists $fn)
	    throw "WSDL-LIB-ERROR", sprintf("missing file name in URL %n", $url);
	my $f = new File();
	$f.open2($fn);
	return $f.read(-1);
    }

    static getHTTP($url, $path, $hc, $headers) {
	if (exists $hc) {
	    if (!($hc instanceof HTTPClient))
		throw "GET-FILE-FROM-URL-ERROR", sprintf("object is not an HTTPClient: %n", type($hc));
	    $hc.setURL($url);
	}
	else
	    $hc = new HTTPClient(("url" : $url));
	return $hc.get(exists $path ? $path : "/", $headers);
    }

    static getFTP($url, $path) {
	my $file = basename($path);
	if (!exists $file)
	    throw "WSDL-LIB-ERROR", sprintf("missing file name in URL %n", $url);
	
	my $f = new FtpClient($url);
	$f.connect();
	
	my $dir = dirname($path);
	if (exists $dir)
	    $f.cwd($dir);
	
	return $f.getAsBinary($file);
    }

    static getFileFromURL($url, $def_protocol, $http_client, $http_headers) {
	my $u = parseURL($url);

	if (!exists $u.protocol)
	    $u.protocol = exists $def_protocol ? $def_protocol : "file";
	
	switch ($u.protocol) {
	    case "file":
		return WSDLLib::getFile($url, $u);

	    case /^http(s)?$/:
		return WSDLLib::getHTTP($url, $u.path, $http_client, $http_headers);

	    case /^ftp(s)?$/:
		return WSDLLib::getFTP($url, $u.path);

	    default:
	        throw "WSDL-LIB-ERROR", sprintf("do not know how to retrieve data with protocol %n given in URL %n", $u.protocol, $url);
	}
    }

    static getWSDL($wsdl, $http_client, $http_headers) {
	if ($wsdl instanceof WebService || (strlen($wsdl) > 256 && $wsdl !~ /^\w+:\/\/.*$/))
	    return $wsdl;
	return WSDLLib::getFileFromURL($wsdl, "file", $http_client, $http_headers);
    }

    static parseSOAPMessage($msg) {
	if (exists $msg."_qore_multipart") {
	    if ($msg."_qore_multipart" != "related")
		throw "SOAP-MESSAGE-ERROR", sprintf("don't understand multipart/%s messages, expected multipart/related", $msg."_qore_multipart");

	    my $bdry = $msg."_qore_multipart_boundary";
	    if (!strlen($bdry))
		throw "SOAP-MESSAGE-ERROR", sprintf("multipart message received without multipart boundary; headers=%n", $msg - "body");

	    my $mpmsg;
	    my $l = split("\r\n--" + $bdry, $msg.body);

	    #printf("l=%N\n", $l);
	    #my $f = new File(); $f.open2("t.bin", O_CREAT|O_WRONLY|O_TRUNC); $f.write(binary($l[3])); exit();
	    #my $f = new File(); $f.open2("t.bin", O_CREAT|O_WRONLY|O_TRUNC); $f.write(binary($msg.body)); exit();

	    for (my $i = 1; $i < (elements $l - 1); ++$i) {
		my $m = $l[$i];
		
		my $ie = index($m, "\r\n\r\n");
		if ($ie == -1) {
		    throw "SOAP-MESSAGE-ERROR", sprintf("part %d has no headers: %n", $i, $m);
		}
		my $hh;
		foreach my $hl in (split("\r\n", substr($m, 2, $ie))) {
		    trim $hl;
		    my ($hi, $ignore, $ha) = $hl =~ x/^(.*):([ \t])*(.*)$/;
		    $hi = tolower($hi);
		    #printf("hl=%n hi=%n ha=%n\n", $hl, $hi, $ha);exit();
		    $hh.$hi = $ha;		    
		}
		if (!exists $hh."content-id")
		    throw "SOAP-MESSAGE-ERROR", sprintf("expecting part header Content-ID in part %d; headers: %n", $i, $hh);

		my $b;
		if ($hh."content-transfer-encoding" == "binary") {
		    # unfortunately we have to do some tricks to get the binary data out here
		    $m = force_encoding($m, "ascii");
		    # recalculate byte offset
		    $ie = index($m, "\r\n\r\n");
		    #printf("ie=%d m=%d\n", $ie, strlen($m));exit();
		    $b = binary(substr($m, $ie + 4));

		    #my $f = new File(); $f.open2("t.bin", O_CREAT|O_WRONLY|O_TRUNC); $f.write($b); exit();
		}
		else {
		    $b = substr($m, $ie + 4);

		    if ($hh."content-type" =~ /charset=/) {
			my $c = ($hh."content-type" =~ x/charset=([^;]+)/)[0];
			$b = force_encoding($b, $c);
		    }
		}

		if ($hh."content-id" !~ /^\<.*\>$/)
		    throw "SOAP-MESSAGE-ERROR", sprintf("expected part ID to have the following format: <id>, instead got %s", $hh."content-id");

		my $p = ( "hdr" : $hh,
			  "body" : $b );

		if ((!exists $msg."_qore_multipart_start" && $i == 1)
		    || (exists $msg."_qore_multipart_start" && $msg."_qore_multipart_start" == $hh."content-id")) {
		    $mpmsg.body = $p;
		}
		else {
		    my $id = substr($hh."content-id", 1, -1);
		    $mpmsg.part.$id = $p;
		}
	    }
	    # check content-type
	    WSDLLib::checkSOAPContentType($mpmsg.body.hdr."content-type");

	    #printf("part %d hh=%N\nbody=%s (%d)\n", $i, $hh, type($b), elements($b)); #exit();	
	    my $xmldata = parseXMLAsData($mpmsg.body.body);
	    
	    # parse entire data structure to find "href"s or href attributes
	    WSDLLib::substHref(\$xmldata, $mpmsg.part);
	    return $xmldata;
	}

	WSDLLib::checkSOAPContentType($msg."content-type");

	return parseXMLAsData($msg.body);
    }

    private static checkSOAPContentType($ct) {
	foreach my $sct in ("application/soap+xml", "text/xml") {
	    if (bindex($ct, $sct) != -1)
		return;
	}
	
	throw "SOAP-MESSAGE-ERROR", sprintf("don't know how to handle content-type %n (expecting 'application/soap+xml' or 'text/xml')", $ct);
    }

    private static processHref($xmldata, $hr, $parts) {
	if ($hr !~ /^cid:/)
	    throw "SOAP-MESSAGE-ERROR", sprintf("messages references non-local part %n; cannot handle non-local parts", $hr);
	$hr = substr($hr, 4);
	if (!exists $parts.$hr)
	    throw "SOAP-MESSAGE-ERROR", sprintf("message references non-existent part %n", $hr);
	$xmldata = $parts.$hr.body;
    }
    
    private static substHref($xmldata, $parts) {
	foreach my $k in (keys $xmldata) {
	    if (exists $xmldata.$k."^attributes^".href)
		WSDLLib::processHref(\$xmldata.$k, $xmldata.$k."^attributes^".href, $parts);
	    else if (exists $xmldata.$k.href)
		WSDLLib::processHref(\$xmldata.$k, $xmldata.$k.href, $parts);
	    else if (type($xmldata.$k) == Type::List) {
		foreach my $e in (\$xmldata.$k)
		    WSDLLib::substHref(\$e, $parts);
	    }
	    else if (type($xmldata.$k) == Type::Hash)
		WSDLLib::substHref(\$xmldata.$k, $parts);
	}
    }
}

# abstract class providing helper methods to subclasses
class WSDL::XSDBase {
    static private doType($t, $nsinfo) {
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

    static removeNS($h) {
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

    static removeNS2($h) {
	foreach my $k in (keys $h) {
	    my ($ns, $name) = $k =~ x/(\w+):(\w+)/;
	    if (exists $ns) {
		if (type($h.$k) == Type::Hash)
		    $h.$k.".ns" = $ns;
		$h.$name = $h.$k;
		$h -= $k;
	    }
	}
    }
}

# abstract type common to all XSD classes
class WSDL::XSDData inherits XSDBase {
    getValue($mrh, $val) {
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
    constructor($e) {
	WSDL::XSDBase::removeNS(\$e);

	$.name = $e."^attributes^".name;
    }

    getName() {
	return exists $.name ? $.name : "<unnamed type>";
    }

    getNS() {
	return $.ns;
    }
}

class WSDL::XSDBaseType inherits XSDData {
    constructor($t) {
	$.type = $t;
	$.nstype = "xsd:" + $t;
    }

    serialize($val, $omit_type) {
	my ($type, $nstype);
	# set type according to Qore type if xsd type is anyType
	if ($.type == "anyType") {
	    # we have to specify the type in this case
	    $omit_type = False;
	    $type = any_type_map{type($val)};
	    $nstype = "xsd:" + $type;
	    if (!exists $type)
		throw SOAP_SERIALIZATION_ERROR, sprintf("cannot serialize xsd type anyType from Qore type %n", type($val));
	}
	else {
	    $type = $.type;
	    $nstype = $.nstype;
	}

	switch ($type) {
	    case "byte": {
		my $v = int($val);
		if (($v & 0xff) != $v)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
		break;
	    }

	    case "short": {
		my $v = int($val);
		if ($v < RANGE_SHORT[0] || $v > RANGE_SHORT[1])
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
		break;
	    }

	    case "int": {
		my $v = int($val);
		if ($v < RANGE_INT[0] || $v > RANGE_INT[1])
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
		break;
	    }

	    case "unsignedByte": {
		my $v = int($val);
		if (($v & 0xff) != $v)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
	        if ($v < 0)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $type, $v);
		break;
	    }

	    case "unsignedShort": {
		my $v = int($val);
		if ($v < 0 || $v > RANGE_SHORT[1])
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
		break;
	    }

	    case "unsignedInt": {
		my $v = int($val);
		if ($v < 0 || $v > RANGE_INT[1])
		    throw SOAP_SERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $v, $type);
		break;
	    }

	    case "unsignedLong": {
		my $v = int($val);
	        if ($v < 0)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $type, $v);
		break;
	    }

	    case "positiveInteger":
	        if ($val <= 0)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("type %n only accepts positive values (value supplied: %d)", $type, $val);
	        break;

	    case "date":
		$val = format_date("YYYY-MM-DD", date($val));
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

	#printf("DEBUG: FORCE: type=%n, nstype=%n, val=%n\n", $type, $.nstype, $val);
	if ($omit_type == "ns")
	    return ( "^attributes^" : ( "xmlns:xsi" : XSI_NS, "xsi:type" : $nstype ), "^value^" : $val );
	return $omit_type ? $val : ( "^attributes^" : ( "xsi:type" : $nstype ), "^value^" : $val );
    }

    deserialize($types, $mrh, $val) {
	my $type;
	if (type($val) == Type::Hash) {
	    $type = $val."^attributes^"."xsi:type";
	    my $t = ($type =~ x/\w+:(\w+)/)[0];
	    if (exists $t)
		$type = $t;
		
	    if ($.type != "anyType" && $type != $.type)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("expecting base type %n, got %n", $.type, $val."^attributes^"."xsi:type");
	    if (exists $val."^value^")
		$val = $val."^value^";
	}
	else
	    $type = $.type;

	switch ($type) {
	    case "string": 
	    case "anyURI":
	    # note that we do not convert xsd:integer to a qore integer to avoid losing precision
	    case "integer":
		return $val;
	    
	    case "byte":
		$val = int($val);
		if (($val & 0xff) != $val)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "short":
		$val = int($val);
		if ($val < RANGE_SHORT[0] || $val > RANGE_SHORT[1])
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "int":
		$val = int($val);
		if ($val < RANGE_INT[0] || $val > RANGE_INT[1])
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        return $val;

	    case "long":
		return int($val);

	    case "unsignedByte":
		$val = int($val);
		if (($val & 0xff) != $val)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedShort":
		$val = int($val);
		if (($val & 0xffff) != $val)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedInt":
		$val = int($val);
		if (($val & 0xffffffff) != $val)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("value %d is out of range for type %n", $val, $.type);
	        if ($val < 0)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
	        return $val;

	    case "unsignedLong":
	        if ($val < 0)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("type %n does not accept negative values (value supplied: %d)", $.type, $val);
		return int($val);

	    case "positiveInteger":
	        if ($val <= 0)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("type %n only accepts positive values (value supplied: %d)", $.type, $val);
		return int($val);

	    case "date":
	        # remove dashes from date
		$val =~ s/-//g;
		return date($val);
		
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

	    default: {
		if ($.type == "anyType")  {
		    if (exists $types.$type)
			return $types.$type.deserialize($types, $mrh, $val);
		}
		throw SOAP_DESERIALIZATION_ERROR, sprintf("don't know how to handle type %n", $.type);
	    }
	}
    }

    getName() {
	return $.type;
    }

    getNameWithNS() {
	return $.nstype;
    }
}

class WSDL::XSDArrayType inherits XSDData {
    constructor($t) {
	$.type = $t;

	if ($t != "binary")
	    throw "XSD-ARRAYTYPE-ERROR", sprintf("don't know how to handle arrays of type %n", $t);
    }
    serialize($val, $omit_type) {
	switch ($.type) {
	    case "binary": {
		my $t = type($val);
		if ($t == Type::String)
		    $val = binary($val);
		else if ($t != Type::Binary)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("cannot serialize type %n from type %n; requires string or binary", $.type, $t);
		return $val;
	    }

	    default: {
		throw SOAP_SERIALIZATION_ERROR, sprintf("don't know how to handle type %n", $.type);
	    }
	}
    }
    deserialize($types, $mrh, $val) {
	switch ($.type) {
	    case "binary": {
		if (type($val) != Type::Binary)
		    throw SOAP_DESERIALIZATION_ERROR, sprintf("cannot deserialize type %n from type %n; requires binary", $.type, type($val));
		return $val;
	    }

	    default: {
		throw SOAP_DESERIALIZATION_ERROR, sprintf("don't know how to handle type %n", $.type);
	    }
	}
    }
}

class WSDL::XSDElement inherits XSDNamedData {
    constructor($e, $nsinfo, $unresolved) : XSDNamedData(\$e) {
	if ($nsinfo instanceof XSDData) {
	    $.type = $nsinfo;
	    return;
	}

	my $a = $e."^attributes^";

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

	if (exists $a.type) {
	    $.type = WSDL::XSDBase::doType($a.type, $nsinfo);

	    # add self to unresolved list if element type cannot be resolved
	    if (!($.type instanceof XSDData)) {
		#printf("DEBUG: self=%n\n", $self);
		$unresolved.add($self);
	    }
	}
	else if (exists $e.simpleType)
	    $.type = new XSDSimpleType($e.simpleType, $nsinfo, $unresolved);
	else if (exists $e.complexType)
	    $.type = new XSDComplexType($e.complexType, $nsinfo, $unresolved);

	#printf("DEBUG: XSDElement self=%N\n", $self);
    }

    serialize($h, $omit_type, $omit_ns, $key, $typename) {
	if (!exists $h) {
	    if ($.minOccurs) {
		if ($.nillable) {
		    my $rh = ("xsi:nil" : "true");
		    if (!$omit_type)
			$rh += ("xsi:type" : $.type.getNameWithNS());
		    return ("^attributes^" : $rh);
		}
	        else {
		    if (exists $key && exists $typename)
			throw SOAP_SERIALIZATION_ERROR, sprintf("missing value for %s.%s (minOccurs=%d, type %n)", $typename, $key, $.minOccurs, $.type.getName());
		    else
			throw SOAP_SERIALIZATION_ERROR, sprintf("missing element value (minOccurs=%d, type %n)", $.minOccurs, $.type.getName());
		}
	    }
	    else
		return;
	}
	if (type($h) == Type::List) {
	    if (elements $h == 1)
		$h = $h[0];
	    else {
		if ($.maxOccurs == 1)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("cannot serialize element %n of type %n from a list because maxOccurs = 1", $.name, $.type.getName());
		if (elements $h > $.maxOccurs && $.maxOccurs > 0)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("list for element %n of type %n has %d element%s, but maxOccurs = %d", $.name, $.type.getName(), elements $h, elements $h == 1 ? "" : "s", $.maxOccurs);
		if (elements $h < $.minOccurs)
		    throw SOAP_SERIALIZATION_ERROR, sprintf("list for element %n of type %n has %d element%s, but minOccurs = %d", $.name, $.type.getName(), elements $h, elements $h == 1 ? "" : "s", $.minOccurs);
		
		my $l = ();
		foreach my $e in ($h) {
		    $l += $.type.serialize($e, $omit_type, $omit_ns);
		}
		return $l;
	    }
	}
	if ($.minOccurs > 1)
	    throw SOAP_SERIALIZATION_ERROR, sprintf("only one element passed to element %n of type $n, but minOccurs = %d", $.name, $.type.getName(), $.minOccurs);
	#printf("DEBUG: element %n omit_type=%n omit_ns=%n\n", $.name, $omit_type, $omit_ns);
	#printf("DEBUG: type=%N\n", $.type);
	return $.type.serialize($h, $omit_type, $omit_ns);
    }

    deserialize($types, $mrh, $val) {
	my $a = $val."^attributes^";
	WSDL::XSDBase::removeNS(\$a);
	
	if (!exists $val || $a.nil == "true") {
	    if ($.nillable || !$.minOccurs)
		return;
	    throw SOAP_DESERIALIZATION_ERROR, sprintf("NOTHING passed for element %n, but nillable=False and minOccurs=%d", $.name, $.minOccurs);
	}

	if (type($val) == Type::List) {
	    my $el = elements $val;
	    if ($.maxOccurs != -1 && $el > $.maxOccurs)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("error deserializing element %n, maxOccurs=%d but list is %d elements long", $.name, $.maxOccurs, $el);
	    if ($el < $.minOccurs)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("error deserializing element %n, minOccurs=%d but list is %d elements long", $.name, $.minOccurs, $el);

	    my $l;
	    foreach my $e in ($val)
		$l[elements $l] = $.type.deserialize($types, $mrh, $.getValue($mrh, $e));
	    return $l;
	}

	if ($.minOccurs > 1)
	    throw SOAP_DESERIALIZATION_ERROR, sprintf("single value passed for element %n, but minOccurs=%d", $.name, $.minOccurs);

	return $.type.deserialize($types, $mrh, $.getValue($mrh, $val));
    }
}

class WSDL::XSDSimpleType inherits XSDNamedData {
    constructor($st, $nsinfo, $unresolved) : XSDNamedData(\$st) {
	$.nsinfo = $nsinfo;
	my $a = $st."^attributes^";
	delete $st."^attributes^";

	WSDL::XSDBase::removeNS(\$st);

	if (exists $st.restriction) {
	    my $r = $st.restriction;

	    my $base = $r."^attributes^".base;
	    if (!exists $base)
		throw "XSD-SIMPLETYPE-ERROR", sprintf("missing 'base' attribute in simpleType %n restriction", $.name);

	    $.type = WSDL::XSDBase::doType($base, $nsinfo);

	    # add base type to unresolved list if type cannot be resolved
	    if (!($.type instanceof XSDData))
		$unresolved.add($self);

	    WSDL::XSDBase::removeNS(\$r);
	    
	    if (exists $r.enumeration)
		$.enum = map $1."^attributes^".value, $r.enumeration;
	    else
		throw "XSD-SIMPLETYPE-ERROR", sprintf("missing enumeration element in simpleType %n restriction", $.name);
	}
	else
	    throw "XSD-SIMPLETYPE-ERROR", sprintf("missing restriction element in simpleType %n", $.name);

	#printf("DEBUG: st=%N\n", $self); exit();
    }

    serialize($val, $omit_type, $omit_ns) {
	if (!inlist($val, $.enum))
	    throw "SOAP-SERIALIZATION-ERROR", sprintf("value %n passed to simpleType %n is not in the enumeration list (%n)", $val, $.name, $.enum);

	return $.type.serialize($val, $omit_type, $omit_ns);
    }

    deserialize($types, $mrh, $val) {
	my $v = $.type.deserialize($types, $mrh, $val);

	if (!inlist($v, $.enum))
	    throw "SOAP-DESERIALIZATION-ERROR", sprintf("value %n passed to simpleType %n is not in the enumeration list (%n)", $v, $.name, $.enum);

	return $v;
    }

    getNameWithNS() {
	return "ns1:" + $.name;
    }
}

class WSDL::XSDComplexType inherits XSDNamedData {
    constructor($ct, $nsinfo, $unresolved) : XSDNamedData(\$ct) {
	$.nsinfo = $nsinfo;
	my $a = $ct."^attributes^";
	delete $ct."^attributes^";

	my $d = $ct.complexContent;
	if (exists $d) {
	    WSDL::XSDBase::removeNS(\$d);
	    if (exists $d.restriction) {
		WSDL::XSDBase::removeNS(\$d.restriction);

		my $base = $d.restriction."^attributes^".base;

		# FIXME: handle namespace
		my ($ns, $tn) = $base =~ x/(\w+):(\w+)/;
		if (exists $tn) {
		    if ($tn == "Array") {
			# FIXME check that namespace is SOAP encoding
			my $aa = $d.restriction.attribute."^attributes^";
			WSDL::XSDBase::removeNS(\$aa);
			if (!exists $aa.arrayType)
			    throw WSDL_ERROR, sprintf("cannot parse complexType restriction: %n", $d.restriction);

			# FIXME: handle multiple dimensions?
			my ($ans, $atn) = $aa.arrayType =~ x/(\w+):(\w+)\[\]$/;
			if (exists $atn) {
			    $.array.val = $atn;
			    $.array.ns  = $ans;
			}
			else
			    $.array.val = $aa.arrayType;
			
			delete $d.restriction.attribute;
			#printf("DEBUG: ans=%n atn=%n aa=%N\n", $ans, $atn, $aa);
			return;
		    }
		    else {
			$.restriction = $tn;
		    }
		}
		
		delete $d.restriction."^attributes^";

		$.parseData($d.restriction, $unresolved);
	    }
	    else if (exists $d.extension) {
		$.extension = $d.extension."^attributes^".base;

		# FIXME: check for soap encoding namespace
		$.extension =~ s/(.*:)(.*)/$2/;
		delete $d.extension."^attributes^";
		WSDL::XSDBase::removeNS(\$d.extension);

		$.parseData($d.extension, $unresolved);
	    }
	    else
		throw "XSD-COMPLEXCONTENT-ERROR", sprintf("can't parse complexContent %n information", (keys $d)[0]);
	}
	else
	    $.parseData($ct, $unresolved);
    }

    private parseData($d, $unresolved) {
	delete $d.ns;
	if (elements $d > 1)
	    throw WSDL_ERROR, sprintf("expecting a single element in the complexType hash, got: %n", keys $d);

	my $k = (keys $d)[0];
	if ($k == "all") {
	    $.hash_type = XET_ALL;
	    WSDL::XSDBase::removeNS(\$d.all);
	    $.elements = $.parseElements($d.all.element, $unresolved);
	}
	else if ($k == "choice") {
	    $.hash_type = XET_CHOICE;
	    WSDL::XSDBase::removeNS(\$d.choice);
	    $.elements = $.parseElements($d.choice.element, $unresolved);
	}
	else if ($k == "sequence") {
	    $.hash_type = XET_SEQUENCE;
	    WSDL::XSDBase::removeNS(\$d.sequence);
	    if (exists $d.sequence.element)
		$.elements = $.parseElements($d.sequence.element, $unresolved);
	    else
		$.elements = hash();
	}
	else
	    throw "XSD-COMPLEXTYPE-ERROR", sprintf("unknown keys in %n", $d);
    }

    private parseElements($el, $unresolved) {
	#printf("DEBUG: XSDComplexType::parseElements(%n)\n", $el);
	my $h;
	foreach my $e in ($el) {
	    my $elem = new XSDElement($e, $.nsinfo, $unresolved);
	    $h.($elem.name) = $elem;
	}
	return $h;
    }

    serialize($h, $omit_type, $omit_ns) {
	if (exists $.array)
	    return $.array.serialize($h, $omit_type, $omit_ns);

	if (type($h) != Type::Hash)
	    throw SOAP_SERIALIZATION_ERROR, sprintf("expecting hash argument to serialize from complexType %n (got %n, type %n)", $.getName(), $h, type($h));

	my $rh;

	if ($.hash_type == XET_SEQUENCE || $.hash_type == XET_ALL) {
	    foreach my $p in (keys $.elements) {
		#printf("element=%s=%N\nvalue=%N\n", $p, $.elements.$p, $h.$p);
		my $e = $.elements.$p.serialize($h.$p, $omit_type, $omit_ns, $p, $.name);
		if (exists $e) {
		    my $key = $omit_ns ? $p : ("ns1:" + $p);
		    $rh.$key = $e;
		}
		delete $h.$p;
	    }
	    if (elements $h)
		throw SOAP_SERIALIZATION_ERROR, sprintf("%n %s of type %n (valid elements: %n)", (my $kl = keys $h), elements $kl == 1 ? "is an invalid member" : "are invalid members", $.getName(), keys $.elements);
	}
	else { # "choice" - union
	    if (elements $h > 1)
		throw SOAP_SERIALIZATION_ERROR, sprintf("cannot serialize type %s with more than 1 member (%n)", $.getName(), keys $h);
	    my $key = (keys $h)[0];
	    if (!exists $.elements.$key)
		throw SOAP_SERIALIZATION_ERROR, sprintf("%n is an invalid member of type %n", $key, $.getName());

	    # add namespace if necessary
	    my $nskey = $omit_ns ? $key : ("ns1:" + $key);
	    $rh.$nskey = $.elements.$key.serialize($h.$key, $omit_type, $omit_ns, $key, $.name);
	}
	
	if (exists $.name && !$omit_type)
	    $rh."^attributes^" = ( "xsi:type" : "ns1:" + $.name );
	#printf("complex type %s returning %n from %n\n", $.name, $rh, $h);
	return $rh;
    }
    
    deserialize($types, $mrh, $val) {
	if (exists $.array)
	    return $.array.deserialize($types, $mrh, $val);

	if (type($val) != Type::Hash)
	    throw SOAP_DESERIALIZATION_ERROR, sprintf("cannot deserialize type %n from qore type %n (expecting hash)", $.getName(), type($val));

	my $rh;

	my $attr = $val."^attributes^";
	delete $val."^attributes^";

	# ensure types match
	my $tn = $attr."xsi:type";
	if (exists $tn) {
	    my ($ns, $name) = $tn =~ x/(.*):(.*)/;
	    if (exists $name)
		$tn = $name;
	    if ($tn != $.name)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("expecting ComplexType type %n, got %n", $.getName(), $tn);
	}

	WSDL::XSDBase::removeNS2(\$val);

	my $ns = $val.".ns"; $val -= ".ns";

	if ($.hash_type == XET_SEQUENCE || $.hash_type == XET_ALL) {
	    foreach my $p in (keys $.elements) {
		#printf("element %n\n", $p);
		$rh.$p = $.elements.$p.deserialize($types, $mrh, $.getValue($mrh, $val.$p));
		delete $val.$p;
	    }
	    delete $val."^attributes^";
	    if (elements $val)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("invalid element %n passed in type %n (expecting %n)", (keys $val)[0], $.getName(), keys $.elements);
	}
	else { # "choice" - union
	    my $kl = keys $val;
	    if (elements $kl > 1)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("too many elements supplied for union type %n (%n)", $.getName(), $kl);
	    $kl = $kl[0];
	    if (!exists $.elements.$kl)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("element %n is not a valid element for union type %n", $kl, $.getName());

	    $rh.$kl = $.elements.$kl.deserialize($types, $mrh, $.getValue($mrh, $val.$kl));
	}
	return $rh;
    }

    getNameWithNS() {
	return "ns1:" + $.name;
    }
}

class WSDL::WSOperation inherits XSDNamedData {
    constructor($p, $types, $targetns, $messages, $soap12, $usedocns) : XSDNamedData(\$p) {
	$.types = $types;
	$.targetns = $targetns;
	$.soap12 = $soap12;
	$.usedocns = $usedocns;

	my $msghash = $.processNSValue($p.input."^attributes^");

	my $msg = $messages.($msghash.message.val);
	if (!exists $msg)
	    throw WSDL_ERROR, sprintf("missing definition for input message %n required by operation %n", $msghash.name.val, $.name, $.messages);
	$.input = $msg;

	if (exists $p.output) {
	    $msghash = $.processNSValue($p.output."^attributes^");

	    $msg = $messages.($msghash.message.val);
	    if (!exists $msg)
		throw WSDL_ERROR, sprintf("missing definition for output message %n required by operation %n", $msghash.name.val, $.name);

	    $.output = $msg;
	}
	
	my $op_ns = $.targetns;
	if (exists $op_ns && $op_ns !~ /\/$/)
	    $op_ns += "/";
	$.soapAction = exists $op_ns ? $op_ns + $.name : $.name;
    }

    setDocStyle($idocmap) {
	$.docstyle = True;

	if (elements $.input.args > 1)
	    throw WSDL_ERROR, sprintf("don't know how to handle document-style messages for operation %n that has more than one top-level part (%n)",
					$.name, keys $.input.args);

	my $arg = (keys $.input.args)[0];
	my $element = $.input.args.$arg.element.name;
	$.request_name = $element;
	$idocmap.$element = $.input.args.$arg.element;
    }		

    setTopLevelRequestElement($name) {
	$.request_name = $name;
    }

    getTopLevelRequestName() {
	return exists $.request_name ? $.request_name : $.name;
    }

    # returns a hash: keys: body: XML string in the SOAP request format, hdr: hash of HTTP headers
    serializeRequest($h, $hdr) {
	# setup namespaces for SOAP envelope
	my $rh = $.soap12 ? WSDL::ENVELOPE_12_NS : WSDL::ENVELOPE_11_NS;

	my $mpm;
	# do we have mime/multipart input format?
	if (exists $.in.multipart)
	    $mpm = new MultiPartRelatedMessage();

	#printf("DEBUG: docstyle=%n\n", $.docstyle);
	if ($.docstyle)
	    $rh."soapenv:Envelope"."soapenv:Body" = $.input.serializeDocument($.in, $mpm, $h, $.targetns, $.usedocns);
	else
	    $rh."soapenv:Envelope"."soapenv:Body" = $.input.serialize($.in, $mpm, $.name, $h, $.targetns);

	my $body = HAVE_LIBRARY_DEBUGGING ? makeFormattedXMLString($rh) : makeXMLString($rh);

	if (exists $mpm) {
	    $mpm.splicePart($body, sprintf("<%s>", $.in.body.parts), $.soap12 ? "application/soap+xml" : "text/xml");
	    
	    my $rv = $mpm.getMsgAndHeaders();
	    if (strlen($.soapAction)) {
		if ($.soap12)
		    $rv.hdr."Content-Type" += sprintf(";action=%s", $.soapAction);
		$rv.hdr += ("SoapAction" : $.soapAction);
	    }

	    return $rv;
	}

	my $ct;
	if ($.soap12) {
	    $ct = "application/soap+xml";
	    if (strlen($.soapAction))
		$ct += sprintf(";action=%s", $.soapAction);
	}
	else
	    $ct = "text/xml";

	my $rv = ( "hdr" : ( "Content-Type" : $ct ),
		   "body" : $body );

	if (strlen($.soapAction))
	    $rv.hdr += ( "SoapAction" : $.soapAction );

	return $rv;
    }

    # returns an XML string in the SOAP response format
    serializeResponse($h) {
	# setup namespaces for SOAP envelope
	my $rh = $.soap12 ? WSDL::ENVELOPE_12_NS : WSDL::ENVELOPE_11_NS;

	my $mpm;
	# do we have mime/multipart output format?
	if (exists $.out.multipart)
	    $mpm = new MultiPartRelatedMessage();

	#printf("DEBUG: docstyle=%n\n", $.docstyle);
	if ($.docstyle)
	    $rh."soapenv:Envelope"."soapenv:Body" = $.output.serializeDocument($.out, $mpm, $h, $.targetns, $.usedocns);
	else
	    $rh."soapenv:Envelope"."soapenv:Body" = $.output.serialize($.out, $mpm, $.name, $h, $.targetns);

	my $body = HAVE_LIBRARY_DEBUGGING ? makeFormattedXMLString($rh) : makeXMLString($rh);

	my $ct = $.soap12 ? "application/soap+xml" : "text/xml";
	if (exists $mpm) {
	    $mpm.splicePart($body, sprintf("<%s>", $.out.body.parts), $ct);
	    return $mpm.getMsgAndHeaders();
	}

	return ( "hdr"  : ( "Content-Type" : $ct ),
		 "body" : $body );
    }

    private processMultiRef($body) {
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
	WSDL::XSDBase::removeNS2(\$msg);

	return ($mrh, $msg);
    }

    deserializeRequest($o) {
	WSDL::XSDBase::removeNS(\$o);
	WSDL::XSDBase::removeNS(\$o.Envelope);

	my $body = $o.Envelope.Body;

	my ($mrh, $msg) = $.processMultiRef($body);

	#my $ns = $msg.".ns";
	$msg -= ".ns";

	return $.docstyle ? $.input.deserializeDocument($.types, $mrh, $msg) : $.input.deserialize($.types, $mrh, $msg);
    }

    deserializeResponse($o) {
	WSDL::XSDBase::removeNS2(\$o);
	WSDL::XSDBase::removeNS2(\$o.Envelope);

	my $body = $o.Envelope.Body;

	my ($mrh, $msg) = $.processMultiRef($body);

	#my $ns = $msg.".ns";
	$msg -= ".ns";

	# check for Soap Fault, if so raise an exception immediately with the fault info
	WSDL::XSDBase::removeNS2(\$body);

	if (exists $body.Fault) {
	    WSDL::XSDBase::removeNS(\$body.Fault);
	    if ($.soap12) {
		WSDL::XSDBase::removeNS(\$body.Fault.Code);
		WSDL::XSDBase::removeNS(\$body.Fault.Reason);
		my $desc = sprintf("The following fault response was received from the server: code=%n", $body.Fault.Code.Value);
		my $sc = $body.Fault.Code.Subcode;
		while (exists $sc) {
		    WSDL::XSDBase::removeNS(\$sc);
		    $desc += sprintf(", subcode=%n", $sc.Value);
		    $sc = $sc.Subcode;
		}
		foreach my $rn in ($body.Fault.Reason.Text) {
		    $desc += sprintf(", text=%n", $rn);
		}
		
		throw "SOAP-SERVER-FAULT-RESPONSE", $desc, $body.Fault;
	    }
	    else {
		my $desc = sprintf("The following fault response was received from the server: code=%n", $body.Fault.faultcode);
		if (exists $body.Fault.faultstring)
		    $desc += sprintf(", faultstring=%n", $body.Fault.faultstring);
		if (exists $body.Fault.desc)
		    $desc += sprintf(", desc=%n", $body.Fault.desc);
		
		throw "SOAP-SERVER-FAULT-RESPONSE", $desc, $body.Fault;
	    }
	}

	return $.docstyle ? $.output.deserializeDocument($.types, $mrh, $msg) : $.output.deserialize($.types, $mrh, $msg);
    }

    private processNSValue($h) {
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
    
    isSoap12() {
	return $.soap12;
    }

    getTargetNS() {
	return $.targetns;
    }

    setOutputMultipart($v) {
	$.out.multipart = $v;
	$.out.parts = ();
    }

    private parsePart($msg, $part) {
	WSDL::XSDBase::removeNS(\$part);
	my $a = $part."^attributes^";
	$part -= "^attributes^";

	if (exists $part.body) {
	    my $pa = $part.body."^attributes^";
	    if ($pa.use != "literal")
		throw WSDL_ERROR, sprintf("unsupported body part without use=\"literal\": %n", $part.body);
	    $msg.body = $pa;
	}
	else if (exists $part.content) {
	    foreach my $c in ($part.content) {
		my $pa = $c."^attributes^";
		my $name = $pa.part;
		if (!exists $name)
		    throw WSDL_ERROR, sprintf("unsupported content part without part attribute: %n", $c);
		my $type = $pa.type;
		if (!exists $type)
		    throw WSDL_ERROR, sprintf("unsupported content part without type attribute: %n", $c);

		if (!exists $msg.parts.$name)
		    $msg.parts.$name = $type;
		else {
		    if (type($msg.parts.$name) != Type::List)
			$msg.parts.$name = list($msg.parts.$name);
		    $msg.parts.$name += $type;
		}
	    }
	}
	else
	    throw WSDL_ERROR, sprintf("cannot parse part: %n", $part);

	#printf("DEBUG: part: %N\nmsg=%N\n", $part, $msg);#exit();
    }

    addOutputPart($part) {
	if (!exists $.out.multipart)
	    throw WSDL_ERROR, sprintf("WSOperation::addOutputPart(): internal error: cannot add a part to a non-multipart message; part=%n", $part);

	$.parsePart(\$.out, $part);
    }

    setInputMultipart($v) {
	$.in.multipart = $v;
	$.in.parts = ();
    }

    addInputPart($part) {
	if (!exists $.in.multipart)
	    throw WSDL_ERROR, sprintf("WSOperation::addInputPart(): internal error: cannot add a part to a non-multipart message; part=%n", $part);

	$.parsePart(\$.in, $part);
    }
}

class WSDL::WSMessage inherits XSDNamedData {
    constructor($m, $element_map) : XSDNamedData(\$m) {
	#printf("DEBUG: WSMessage::constructor() m=%n element_map: %n\n", $m, $element_map);

	$.name = $m."^attributes^".name;
	foreach my $p in ($m.part) {
	    my $arg = $p."^attributes^";
	    if (exists $arg.element) {
		my ($ns, $name) = $arg.element =~ x/(\w+):(\w+)/;
		if (!exists $name)
		    $name = $arg.element;

		if (!exists $element_map.$name) {
		    #printf("DEBUG: WSMessage::constructor() message %n element %n (%n)\n", $.name, $arg.element, keys $element_map);
		    throw WSDL_ERROR, sprintf("message %n references unknown element %n", $.name, $name);
		}
		$.args.($arg.name).element = $element_map.$name;
	    }
	    else {
		$.args.($arg.name) = $arg;
		$.args.($arg.name).type = WSDL::XSDBase::doType($p."^attributes^".type);
	    }
	}
    }

    serialize($msginfo, $mpm, $name, $h, $targns) {
	my $rh;
	#printf("DEBUG: message %s: h=%n\n", $.name ,$h);
    
	foreach my $k in (keys $.args) {
	    if (!exists $h.$k)
		throw SOAP_SERIALIZATION_ERROR, sprintf("missing message argument %n (got %n instead)", $k, keys $h);

	    my $hv;
	    #printf("DEBUG: arg %n with %n\n", $k, $.args.$k);
	    if (exists $.args.$k.element)
		$hv = $.args.$k.element.serialize($h.$k, !$.encoded, True);
	    else
		$hv = $.args.$k.type.serialize($h.$k, !$.encoded, True);
	    #printf("DEBUG: arg %s got %n from %n\n", $k, $rh.$k, $h.$k);

	    #printf("DEBUG: WSMessage::serialize() k=%n args=%n, parts=%n\n", $k, keys $.args, $msginfo.parts);
	    if (exists $msginfo.parts.$k) {
		my $ct = $msginfo.parts.$k;
		if (type($ct) == Type::List)
		    $ct = shift $ct;
		$mpm.addPart($hv, sprintf("<%s>", $k), $ct);
		$hv."^attributes^".href = "cid:" + $k;
	    }
	    
	    $rh.$k = $hv;
	}
	
	$rh."^attributes^" = ( "xmlns:ns1" : $targns );
	
	if ($.encoded)
	    $rh."^attributes^" += 
	    ( "soapenv:encodingStyle" : SOAP_ENCODING,
	      "xmlns:soapenc"         : SOAP_ENCODING );
	
	my $rvh.("ns1:" + $name) = $rh;
	
	return $rvh;
    }

    serializeDocument($msginfo, $mpm, $h, $targns, $force_ns) {
	my $k = (keys $.args)[0];
	#printf("DEBUG: message %s: force_ns=%n args=%n h=%n\n", $.name, $force_ns, keys $.args, $h);
    
	my $th = $.args.$k.element.serialize($h, !$.encoded, !$force_ns, $k, "<unknown>");

	my $rh."^attributes^" = ( "xmlns:ns1" : $targns );

	if ($.encoded)
	    $rh."^attributes^" += 
	    ( "soapenv:encodingStyle" : SOAP_ENCODING,
	      "xmlns:soapenc"         : SOAP_ENCODING );
	
	$rh.("ns1:" + $.args.$k.element.name) = $th;
	return $rh;
    }

    deserialize($types, $mrh, $val) {
	my $ro;

	#my $ns = $val.".ns";
	$val -= ".ns";

	foreach my $key in (keys $.args) {
	    if (!exists $val.$key)
		throw SOAP_DESERIALIZATION_ERROR, sprintf("message %n is missing argument %n (%n)", $.name, $key, $val);
	    
	    $ro.$key = exists $.args.$key.element 
		? $.args.$key.element.deserialize($types, $mrh, $.getValue($mrh, $val.$key)) 
		: $.args.$key.type.deserialize($types, $mrh, $.getValue($mrh, $val.$key));
	}

	# if there is only one argument, return it directly
	if (elements $ro == 1)
	    return hash_values($ro)[0];
	return $ro;
    }

    deserializeDocument($types, $mrh, $val) {
	my $k = (keys $.args)[0];

	return $.args.$k.element.deserialize($types, $mrh, $val);
    }
}

class WSDL::LateResolverHelper {
    private $.l;

    constructor() { 
	$.l = (); 
    }

    add($v) { 
	$.l += $v; 
    }

    getList() {
	return $.l;
    }
}

# class saves namespaces in the WebService class, parses the imported definitions, 
# and restores the namespace info on exit
class XSDImportHelper {
    constructor($ws, $xsd) {
	# save WebService object
	$.ws = $ws;

	# get old values of namespace attributes
	$.sp = $ws.("ns", "soap12");
	$ws -= ("ns", "soap12");
    }
    destructor() {
	$.ws -= ("ns", "soap12");
	$.ws += $.sp;
    }
}

class WSDL::WebService inherits XSDBase {
    constructor($str, $opts) {
        if (type($str) != Type::String)
 	    throw WSDL_ERROR, sprintf("expecting an XML string as the first argument to WebService::constructor(), got %n instead", type($str));

	my $h = parseXMLAsData($str);

	# save WSDL text
	$.wsdl = $str;

	# remove namespace (normally "wsdl:") prefix from keys if present
	WSDL::XSDBase::removeNS(\$h);
	WSDL::XSDBase::removeNS(\$h.definitions);
	$.getNSPrefixes($h.definitions."^attributes^");

	#printf("%N\n", $h.definitions.types);

	if (exists $h.definitions.types)
	    $.parseTypes($h.definitions.types, $opts.http_client instanceof HTTPClient ? $opts.http_client : NOTHING, $opts.http_headers);
	
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
    getOperationMap($name) {
	if (!exists $name) {
	    if (elements $.services > 2)
		throw "WSDL-GET-INPUT-OPERATION-MAP-ERROR", sprintf("no service argument passed to WSDL::getOperationMap() but the WSDL defines more than one service (%n)", keys ($.services - "port"));
	    $name = $.services[0];
	}
	else if (!inlist($name, ($.services - "port")))
	    throw "WSDL-GET-INPUT-OPERATION-MAP-ERROR", sprintf("service argument %n passed to WSDL::getOperationMap() is not defined by this WSDL (vaild services: %n)", $name, keys ($.services - "port"));

	return $.idocmap + $.imap;
    }

    private getNSPrefixes($a) {
	#printf("DEBUG: getNSPrefixes() %n\n", $a);
	foreach my $k in (keys $a) {
	    my $ns = ($k =~ x/xmlns:(\w+)/)[0];
	    
	    if (!exists $ns)
		continue;
	    $.ns.map.$ns = $a.$k;

	    if ($a.$k == XSD_NS)
		$.ns.xml_schema.$ns = True;

	    if ($a.$k == SOAP_12_NS)
		$.soap12 = True;
	}
	# default has to be quoted because it's a reserved word
	if (exists $a.xmlns)
	    $.ns."default" = $a.xmlns;
	$.ns.target = $a.targetNamespace;
    }

    private getBaseType($t) {
	if (exists $.base_type.$t)
	    return $.base_type.$t;

	return $.base_type.$t = new XSDBaseType($t);
    }

    private resolveType($v) {
	if (exists $v.ns && $.ns.xml_schema.($v.ns))
	    return $.getBaseType($v.val);

	# find type
	if (!exists $.types.($v.val))
	    throw WSDL_ERROR, sprintf("cannot resolve type %s%s", exists $v.ns ? sprintf("%s:", $v.ns) : "", $v.val);

	return $.types.($v.val);
    }

    # parse XSD schema types
    private parseTypes($data, $http_client, $http_headers) {
	WSDL::XSDBase::removeNS(\$data);

	my $sa = $data.schema."^attributes^";
	if ($sa.elementFormDefault == "qualified")
	    $.usedocns = True;
	foreach my $schema in ($data.schema) {
	    WSDL::XSDBase::removeNS(\$schema);
	    #printf("DEBUG: WebService::parseTypes() schema=%N\n", $schema);

	    # process XSD import if present
	    if (exists $schema.import) {
		my $a = $schema.import."^attributes^";
		# import file
		if (strlen($a.schemaLocation)) {
		    my $xsd = WSDLLib::getFileFromURL($a.schemaLocation, "http", $http_client, $http_headers);
		    if (!strlen($xsd))
			throw WSDL_ERROR, sprintf("invalid XSD import: %n returns an empty file", $a.schemaLocation);

		    # parse imported XSD file and include types in our list
		    # also saves namespace definitions and restores on exit
		    my $xih = new XSDImportHelper($self, $xsd);

		    # parse XML to Qore data structure ignoring XML element order
		    $xsd = parseXMLAsData($xsd);

		    # parse namespace definitions in schema attributes
		    my $kl = keys $xsd;
		    if (elements $kl != 1)
			throw WSDL_ERROR, sprintf("expecing a single element indicating an XSD schema in the imported file; got instead: %n", $kl);

		    # get schema member name
		    my $sk = $kl[0];

		    # get namespace info in new schema
		    $.getNSPrefixes($xsd.$sk."^attributes^");
		    
		    # verify that the top-level key indicates an XSD schema definition
		    my ($ns, $mem) = ($sk =~ x/^(\w+):(\w+)$/);
		    if (exists $ns && !$.ns.xml_schema.$ns)
			throw WSDL_ERROR, sprintf("expecing imported schema definition to be in XSD namespace %n, got %n=%n instead", XSD_NS, $ns, $.ns.map.$ns);

		    $.parseTypes($xsd);
		}
	    }

	    my $unresolved = new LateResolverHelper();
	    foreach my $st in ($schema.simpleType) {
		my $t = new XSDSimpleType($st, $.ns, $unresolved);
		$.types.($t.name) = $t;
	    }

	    foreach my $ct in ($schema.complexType) {
		my $t = new XSDComplexType($ct, $.ns, $unresolved);
		$.types.($t.name) = $t;
	    }

	    # resolve types
	    foreach my $e in ($unresolved.getList())
		$e.type = $.resolveType($e.type);

	    foreach my $t in (keys $.types) {		
		if ($.types.$t instanceof XSDComplexType) {
		    #foreach my $e in (keys $.types.$t."elements")
		    #    if (!($.types.$t."elements".$e.type instanceof XSDData))
		    #	    $.types.$t."elements".$e.type = $.resolveType($.types.$t."elements".$e.type);

		    # process restriction info
		    if (exists $.types.$t.restriction) {
		    	my $et = $.types.$t.restriction;
			if (!exists $.types.$et)
			    throw WSDL_ERROR, sprintf("complexType %s should be restricted from base type %n, but type %n is not defined", $t, $et, $et);
			# combine base type and new type
			$.types.$t.elements = $.types.$et.elements + $.types.$t.elements;
			printf("extended %s with %s (%n)\n", $t, $et, keys $.types.$t.elements);
		    }

		    # process extension info
		    if (exists $.types.$t.extension) {
			my $et = $.types.$t.extension;
			if (!exists $.types.$et)
			    throw WSDL_ERROR, sprintf("complexType %s should be extended by %n, but type %n is not defined", $t, $et, $et);
			# combine base type and new type
			$.types.$t.elements = $.types.$et.elements + $.types.$t.elements;
			#printf("extended %s with %s (%n)\n", $t, $et, keys $.types.$t.elements);
		    }

		    # process array info
		    if (exists $.types.$t.array) {
			my $et = $.types.$t.array.val;
			$.types.$t.array = new XSDArrayType(exists $.types.$et ? $.types.$et : $et);
		    }
		}
	    }

	    # make element map
	    foreach my $el in ($schema.element) {
		my $attr = $el."^attributes^";
		if (exists $attr.type) {
		    my $t = WSDL::XSDBase::doType($attr.type, $.ns);
		    if (!($t instanceof XSDData)) {
			$t = $t.val;
			if (!exists $.types.$t)
			    throw WSDL_ERROR, sprintf("cannot resolve element %n type %n", $el, $t);
			#printf("DEBUG: adding element %n type %n\n", $attr.name, $t);
			$.element_map.($attr.name) = new WSDL::XSDElement(("^attributes^":("name":$attr.name)), $.types.$t, $unresolved);
		    }
		    else {
			#printf("DEBUG: adding element %n type %n\n", $attr.name, $t);
			$.element_map.($attr.name) = new WSDL::XSDElement(("^attributes^":("name":$attr.name)), $t, $unresolved);
		    }
		}
		else {
		    #printf("DEBUG: adding element %n\n", $attr.name);
		    $.element_map.($attr.name) = new WSDL::XSDElement($el, $.ns, $unresolved);
		}
	    }

	    # resolve types
	    foreach my $e in ($unresolved.getList())
		if (!($e.type instanceof XSDData))
		    $e.type = $.resolveType($e.type);
	}
    }

    private parseMessage($message) {
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

    private parseService($svc) {
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

    private parsePortType($data) {
	# setup list of services defined in this WSDL
	$.services = ();

        foreach my $port in ($data) {
	    WSDL::XSDBase::removeNS(\$port);
	    #printf("DEBUG: portType=%N\n", $port);
	    my $name = $port."^attributes^".name;
	    $.services += $name;
	    foreach my $p in ($port.operation) {
		my $op = new WSOperation($p, $.types, $.ns.target, $.messages, $.soap12, $.usedocns);
		$.portType.$name.operations.($op.name) = $op;
		#printf("DEBUG: %n registered operation %n\n", $name, $op.name);
		$.opmap.($op.name) = $op;
	    }
	}
    }

    private parseBinding($data) {
	WSDL::XSDBase::removeNS(\$data);
	$data += $data."^attributes^";
	delete $data."^attributes^";

	my $docstyle;
	if (exists $data.binding) {
	    $data.binding += $data.binding."^attributes^";
	    delete $data.binding."^attributes^";
	    if ($data.binding.style == "document")
		$docstyle = True;
	}

	foreach my $ophash in ($data.operation) {
	    my $name = $ophash."^attributes^".name;
	    if (!exists $name)
		throw WSDL_ERROR, sprintf("binding for %n references unnamed operation (%n)", $data.name, $ophash);

	    my $op = $.opmap.$name;
	    if (!exists $op)
		throw WSDL_ERROR, sprintf("binding for %n references unknown operation %n", $data.name, $name);

	    WSDL::XSDBase::removeNS(\$ophash);

	    my $sa = $ophash.operation."^attributes^".soapAction;
	    if (exists $sa)
		$op.soapAction = $sa;

	    if ($docstyle || $ophash.operation."^attributes^".style == "document") {
		my $element = $op.setDocStyle(\$.idocmap);
	    }

	    WSDL::XSDBase::removeNS(\$ophash.input);
	    if (exists $ophash.input.body) {
		if ($ophash.input.body."^attributes^".use == "encoded") {
		    $op.input.encoded = True;
		    #printf("DEBUG: setting encoding = True for %n.%n input\n", $data.name, $name);
		}
	    }
	    else if (exists $ophash.input.multipartRelated) {
		$op.setInputMultipart(MPT_RELATED);
		WSDL::XSDBase::removeNS(\$ophash.input.multipartRelated);		

		if (!exists $ophash.input.multipartRelated.part)
		    throw WSDL_ERROR, sprintf("missing part definition(s) in input message definition for operation %n: %n", $name, $ophash);

		foreach my $part in ($ophash.input.multipartRelated.part) {
		    WSDL::XSDBase::removeNS(\$part);
		    $op.addInputPart($part);
		}
	    }
	    else
		throw WSDL_ERROR, sprintf("cannot parse input message definition for operation %n: %n", $name, $ophash);

	    WSDL::XSDBase::removeNS(\$ophash.output);
	    if (exists $ophash.output.body) {
		if ($ophash.output.body."^attributes^".use == "encoded") {
		    $op.output.encoded = True;
		    #printf("DEBUG: setting encoding = True for %n.%n output\n", $data.name, $name);
		}
	    }
	    else if (exists $ophash.output.multipartRelated) {
		$op.setOutputMultipart(MPT_RELATED);
		WSDL::XSDBase::removeNS(\$ophash.output.multipartRelated);		

		if (!exists $ophash.output.multipartRelated.part)
		    throw WSDL_ERROR, sprintf("missing part definition(s) in output message definition for operation %n: %n", $name, $ophash);

		foreach my $part in ($ophash.output.multipartRelated.part) {
		    WSDL::XSDBase::removeNS(\$part);
		    $op.addOutputPart($part);
		}
	    }
	    else
		throw WSDL_ERROR, sprintf("cannot parse output message definition for operation %n: %n", $name, $ophash);
	}

	$.binding = $data;
    }

    isSoap12() {
	return $.soap12;
    }

    getWSDL() {
	return $.wsdl;
    }
}
