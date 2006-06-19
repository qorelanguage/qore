# XML-RPC client class definition
# subclassed from the HTTPClient class

%include HTTPClient.qc

namespace XmlRpcClient
{
    const Version = "0.2.2";
    const DefaultPath = "RPC2";
    const P_XmlRpc = ( "xmlrpc"  : ( "port" : 8100, "ssl" : False ),
		       "xmlrpcs" : ( "port" : 8100, "ssl" : True ) );
}

class XmlRpcClient::XmlRpcClient inherits HTTPClient::HTTPClient
{
    # calls the base class HTTPClient constructor, overrides the "protocols" key to "xmlrpc"
    constructor($opts) : HTTPClient::HTTPClient($opts + ( "protocols" : P_XmlRpc ))
    {
	$.path = $.default_path;
	if (!exists $.path)
	    $.path = XmlRpcClient::DefaultPath;
	
	$.clientversion = sprintf("Qore XML-RPC Client v%s", XmlRpcClient::Version);
	
	$.headers = ( "Content-Type" : "text/xml",
		      "Accept"       : "text/xml",
		      "User-Agent"   : $.clientversion);

	$.connect();
    }

    # makes a call to the XML-RPC server using the second argument as the list of arguments to send
    callArgs($method, $args)
    {
	$.connect();
	
	# create outgoing message in XmlRpc Call format
	my $msg = makeXMLRPCCallStringArgs($method, $args);
	#printf("msg=%N\n", $msg);
	# send the message to the server and get the response as an XML string
	my $ans = $.post($.path, $msg, $.headers);
	#printf("ans=%N\n", $ans);
	# parse the response and return a qore data structure
	return parseXMLRPCResponse($ans);
    }

    # makes a method call to the XML-RPC server using the remaining arguments after the method name
    # as the list of arguments to send
    call($method)
    {
	return $.callArgs($method, $argv);
    }
}
