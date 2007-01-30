return 
    ( 
      "TibrvSender" :
      ( "desc" : 
	( "^value^" : "The TibrvSender object is used to send messages using the TIBCO Rendezvous reliable messaging protocol.  Each TibrvSender object is set with a string encoding value, which is by default set to the default encoding for qore (see ",
	  "link" : 
	  ( "^attributes^" : 
	    ( "linkend" : "Qore_Strings_and_Character_Set_Encoding" ),
	    "^value^" : "Qore Strings and Character Set Encoding" ),
	  "^value^1" : ").  Any strings received by this object will be assumed to be in this encoding and any strings sent will be convert to this encoding if necessary.  To set or check the encoding for TibrvSender objects, see the ",
	  "link^1" : 
	  ( "^attributes^" : 
	    ( "linkend" : "TibrvSender_setStringEncoding" ),
	    "^value^" : "TibrvSender::setStringEncoding()" ),
	  "^value^2" : " and ",
	  "link^2" : 
	  ( "^attributes^" : 
	    ( "linkend" : "TibrvSender_getStringEncoding" ),
	    "^value^" : "TibrvSender::getStringEncoding()" ),
	  "^value^3" : " methods." ),
	"methods" :
	( "constructor" :
	  ( "desc" : "Creates the TibrvSender object based on the parameters passed.",
	    "exceptions" :
	    ( "TIBRVTRANSPORT-CONSTRUCTOR-ERROR" : "there was an error creating the transport object" ),
	    "args" : 
	    ( "desc" :
	      ( "optional" : True,
		"desc" : "The description for this transport object." ),
	      "service" :
	      ( "optional" : True,
		"desc" : "The service port number as a string" ),
	      "network" :
	      ( "optional" : True,
		"desc" : "The network value" ), 
	      "daemon" :
	      ( "optional" : True,
		"desc" : "The remote rvd daemon's IP address" ) ) ),
	  "destructor" :
	  ( "desc" : "Destroys the TibrvSender object." ),
	  "copy" :
	  ( "desc" : "Copying objects of this class is not supported, an exception will be thrown.",
	    "exceptions" :
	    ( "TIBRVTRANSPORT-COPY-ERROR" : "objects of this class may not be copied" ),
	  ),
	  "sendSubject" :
	  ( "desc" : "Sends a message using the reliable protocol to the given subject.",
	    "args" : 
	    ( "subject" :
	      ( "desc" : "The subject name for the message." ), 
	      "message" : 
	      ( "desc" : "The message to send as a hash",
		"type" : "Hash" ) ),
	    "exceptions" :
	    ( "TIBRVSENDER-SENDSUBJECT-ERROR" : "missing subject or message parameter",
	      "TIBRV-MARSHALLING-ERROR" : "There was an error serializing the qore data to Tibco Rendezvous format",
	      "TIBRV-SEND-ERROR" : "There was an error sending the message" ) ),
	  "sendSubjectWithSyncReply" :
	  ( "desc" : "Sends a message using the reliable protocol to the given subject and returns the reply.",
	    "long" : 
	    ( "^value^" : "Sends a message using the reliable protocol to the given subject and returns the reply.  A timeout value in milliseconds can be optionally included as the third parameter to the method.  If no reply is received within the timeout period, ",
	      "command" : "NOTHING",
	      "^value^1" : " is returned.  If a reply is received, the reply is convert to a qore data structure and returned in the form of a hash with the following keys: 'msg' containing the reply data, 'subject' giving the subject of the reply message, and optionally 'replySubject' giving a subject name for a subsequent reply." ),
	    "ret" : "Hash",
	    "args" : 
	    ( "subject" :
	      ( "desc" : "The subject name for the message." ), 
	      "message" : 
	      ( "desc" : "The message to send as a hash",
		"type" : "Hash" ),
	      "timeout" :
	      ( "desc" : 
		( "^value^" :"The timeout value in milliseconds.  Can also be a relative time (i.e. ",
		  "code" : "1250ms",
		  "^value^1" : ") in order to make the units clear in the source.  If no timeout value is passed to the method (or the timeout value is negative) then the timeout period is infinite, therefore it's recommended to send a timeout value." ),
		"optional" : True,
		"type" : "Integer" ) ),
	    "rv" : 
	    ( "type" : "Hash",
	      "desc" : "The return hash will have the following keys: 'msg' containing the return message data, 'subject' giving the subject of the reply message, and optionally 'replySubject' giving a subject name for a subsequent reply message" ),
	    "exceptions" :
	    ( "TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR" : "missing subject or message parameter",
	      "TIBRV-MARSHALLING-ERROR" : "There was an error serializing the qore data to Tibco Rendezvous format",
	      "TIBRV-DEMARSHALLING-ERROR" : "There was an error deserializing the Tibco Rendezvous data to qore data structures",
	      "TIBRV-SENDREQUEST-ERROR" : "There was an error sending the message" ) ),
	  "setStringEncoding" :
	  ( "desc" : "Sets the string encoding for the object; any strings serialized with this object will be converted to this character encoding if necessary.",
	    "args" : 
	    ( "encoding" : 
	      ( "type" : "String",
		"desc" : "The string encoding to use for this object." ) ),
	    "exceptions" :
	    ( "TIBRVSENDER-SETSTRINGENCODING-ERROR" : "missing encoding parameter from method call" ) ),
	  "getStringEncoding" :
	  ( "desc" : "Returns the encoding used for the object",
	    "rv" : 
	    ( "desc" : "The encoding used for the object.",
	      "type" : "String" ) ) ) ) );
