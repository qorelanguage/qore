# SoapClient class

# a minimal SOAP client using WSDL, XSD, SOAP support implemented in WSDL.qc
# by David Nichols

# to use this class: %include WSDL.qc
#                    %include SoapClient.qc
#
# the constructor takes named arguments in the form of a hash
# vaild arguments are:
# required keys: one of: "wsdl"      : a string defining the WSDL
#                        "wsdl_file" : the filename of the WSDL 
# optional keys:         "service"   : the name of the "portType" to use (if
#                                      more than 1 portType is defined in the 
#                                      WSDL then this key is mandatory
#                        "url"       : to override the URL defined in the WSDL
#
# create messages by setting up a Qore data structure corresponding to the SOAP
# message.  Exceptions will be thrown if either the outgoing or the response
# message do not respond to the WSDL.   The exceptions should be fairly verbose
# to allow you to quickly correct any mistakes.
#
# currently the WSDL implementation is very basic so any messages using
# unimplemented features of SOAP or XSD will fail.
#
# example: (make sure the files are in the same directory or in the
#           QORE_INCLUDE_DIR path)
#
# %include WSDL.qc
# %include SoapClient.qc
# my $sc = new SoapClient(("wsdl_file" : "my-service.wsdl", "url" : "https://localhost:8080" ));
# my $result = $sc.call("SubmitDocument", $msg);

namespace SoapClient {
    const Version = "0.1";
    const Headers = ("Content-Type":"application/soap+xml", "Accept":"application/soap+xml, text/xml", "User-Agent":("Qore Soap Client " + SoapClient::Version));
}
 
class SoapClient::SoapClient {
    private 
    $.hc,    # HTTP client
    $.wsdl,  # web service definition
    $.svc;   # service name

    constructor($h)
    {
	if (type($h) != Type::Hash)
	    throw "SOAP-CLIENT-ERROR", "missing option hash argument to SoapClient::constructor() ('wsdl' or 'wsdl_file' keys required)";
	
	my $wsdl;
	
	# get web service definition
	if (exists $h.wsdl_file) {
	    my $f = new File();
	    $f.open2($h.wsdl_file);
	    $wsdl = $f.read(-1);
	}
	else if (exists $h.wsdl)
	    $wsdl = $h.wsdl;
	else
	    throw "SOAP-CLIENT-ERROR", "neither one of required 'wsdl' or 'wsdl_file' keys is present in the hash argument to SoapClient::constructor()";
	
	$.wsdl = new WebService($wsdl);
	
	# set service
	# get list of services in this wsdl
	my $svcs = keys $.wsdl.portType;
	if (elements $svcs > 1 && !exists $h.service)
	    throw "SOAP-CLIENT-ERROR", sprintf("no 'service' key passed in the option hash argument to SoapClient::constructor() (WSDL defines the following services: %n)", $svcs);
	
	if (exists $h.service) {
	    if (!inlist($h.service, $svcs))
		throw "SOAP-CLIENT-ERROR", sprintf("service %n is not defined by this WSDL (valid services: %n)", $h.service, $svcs);
	    $.svc = $h.service;
	}
	else
	    $.svc = $svcs[0];
	
	if (exists $h.url)
	    $.url = $h.url;
	else
	    $.url = $.wsdl.services.port.$.svc.address;
	
	#printf("DEBUG: set url to %n\n", $.url);
	
	$.hc = new HTTPClient(( "url" : $.url ));
    }
   
    call($operation, $h, $info)
    {
	my $op = $.wsdl.portType.$.svc.operations.$operation;
	if (!exists $op)
	    throw "SOAP-CLIENT-ERROR", sprintf("operation %n does not exist (operations defined by service %n: %n)", 
					       $operation, $.svc, keys $.wsdl.portType.$.svc.operations);

	my $msg = $op.serializeRequest($h);
	# we have to write the request key after the HTTPClient::post() call
	on_exit $info.request = $msg;
	
	#printf("DEBUG url=%n, msg=%n\n", $.url, $info.request);
	$info.response = $.hc.post($.url, $msg, SoapClient::Headers, \$info);

	#printf("DEBUG ans=%n\n", $info.response);
	return $op.deserializeResponse(parseXML($info.response));
    }

    setEncoding($str) {
	$.hc.setEncoding($str);
    }

    getEncoding() {
	return $.hc.getEncoding();
    }
}
