
$classes = 
    ( 
      "TibrvSender" :
      ( "methods" :
	( "constructor" :
	  ( "desc" : "Creates the TibrvSender object based on the parameters passed.",
	    "ret" : "n/a",
	    "exceptions" :
	    ( "TIBRVTRANSPORT-CONSTRUCTOR-ERROR" : "there was an error creating the transport object" ),
	    "args" : "[desc, service, network, daemon]" ),
	  "destructor" :
	  ( "desc" : "Destroys the TibrvSender object.",
	    "ret" : "n/a" ),
	  "copy" :
	  ( "desc" : "Copying object of this class is not supported, an exception will be thrown..",
	    "ret" : "n/a",
	    "exceptions" :
	    ( "TIBRVTRANSPORT-COPY-ERROR" : "objects of this class may not be copied" ),
	  ),
	  "sendSubject" :
	  ( "desc" : "Sends a message using the reliable protocol to the given subject.",
	    "ret" : "n/a",
	    "args" : ("subject", "message"),
	    "exceptions" :
	    ( "TIBRVSENDER-SENDSUBJECT-ERROR" : "missing subject or message parameter",
	      "TIBRV-MARSHALLING-ERROR" : "There was an error serializing the qore data to Tibco Rendezvous format",
	      "TIBRV-SEND-ERROR" : "There was an error sending the message" ) ),
	  "sendSubjectWithSyncReply" :
	  ( "desc" : "Sends a message using the reliable protocol to the given subject and returns the reply.",
	    "long" : "Sends a message using the reliable protocol to the given subject and returns the reply.  The reply is returned in the form of a hash with the following keys: msg = reply data, replySubject = reply subject string, if any, subject = subject string used to send the message",
	    "ret" : "Hash",
	    "args" : ("subject", "message", "[timeout_ms]"),
	    "exceptions" :
	    ( "TIBRVSENDER-SENDSUBJECTWITHSYNCREPLY-ERROR" : "missing subject or message parameter",
	      "TIBRV-MARSHALLING-ERROR" : "There was an error serializing the qore data to Tibco Rendezvous format",
	      "TIBRV-DEMARSHALLING-ERROR" : "There was an error deserializing the Tibco Rendezvous data to qore data structures",
	      "TIBRV-SENDREQUEST-ERROR" : "There was an error sending the message" ) ),
	  "setStringEncoding" :
	  ( "desc" : "Sets the string encoding for the object; any strings serialized with this object will be converted to this character encoding if necessary.",
	    "ret" : "n/a",
	    "exceptions" :
	    ( "TIBRVSENDER-SETSTRINGENCODING-ERROR" : "missing encoding parameter from method call" ) ),
	  "getStringEncoding" :
	  ( "desc" : "Returns the encoding used for the object",
	    "ret" : "n/a" )
	  
	  ) ) );

return $classes;
