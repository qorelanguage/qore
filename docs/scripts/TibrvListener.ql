return 
    ( 
      "TibrvListener" :
      ( "desc" : 
	( "^value^" : "The TibrvListener object is used to receive messages using the TIBCO Rendezvous reliable messaging protocol.  To send messages, use the ",
	  "link" : 
	  ( "^attributes^" : 
	    ( "linkend" : "TibrvSender_Class" ),
	    "^value^" : "TibrvSender Class" ),
	  "^value^1" : ".  Each TibrvListener object is set with a string encoding value, which is by default set to the default encoding for qore (see ",
	  "link^1" : 
	  ( "^attributes^" : 
	    ( "linkend" : "Qore_Strings_and_Character_Set_Encoding" ),
	    "^value^" : "Qore Strings and Character Set Encoding" ),
	  "^value^2" : ").  Any strings received by this object will be assumed to be in this encoding.  To set or check the encoding for TibrvListener objects, see the ",
	  "link^2" : 
	  ( "^attributes^" : 
	    ( "linkend" : "TibrvListener_setStringEncoding" ),
	    "^value^" : "TibrvListener::setStringEncoding()" ),
	  "^value^3" : " and ",
	  "link^3" : 
	  ( "^attributes^" : 
	    ( "linkend" : "TibrvListener_getStringEncoding" ),
	    "^value^" : "TibrvListener::getStringEncoding()" ),
	  "^value^4" : " methods." ),
	"methods" :
	( "constructor" :
	  ( "desc" : "Creates the TibrvListener object based on the parameters passed.",
	    "exceptions" :
	    ( "TIBRVTRANSPORT-CONSTRUCTOR-ERROR" : "there was an error creating the transport object",
	      "TIBRVLISTENER-CONSTRUCTOR-ERROR" : "there was an error creating the listener object (ex: invalid subject, etc)" ),
	    "args" : 
	    ( "subject" :
	      ( "desc" : "The subject to listen for (may contain wildcards, i.e. 'TEST.ADAPTER.>')" ),
	      "desc" :
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
	  ( "desc" : "Destroys the TibrvListener object." ),
	  "copy" :
	  ( "desc" : "Copying objects of this class is not supported, an exception will be thrown.",
	    "exceptions" :
	    ( "TIBRVLISTENER-COPY-ERROR" : "objects of this class may not be copied" ),
	  ),
	  "getMessage" :
	  ( "desc" : "Waits for a message event matching the subject string for this object.",
	    "long" : 
	    ( "^value^" : "Waits for a message event matching the subject string passed.  If a timeout value is passed, then if no suitable event is received within the timeout period, ",
	      "command" : "NOTHING",
	      "^value^1" : " is returned.  Note that this method may return before the timeout period has expired in case that an event is posted to the queue that does not contain message data.  In this case the method returns as with a normal timeout." ),
	    "args" : 
	    ( "timeout" :
	      ( "optional" : True, 
		"desc" : 
		( "^value^" :"The timeout value in milliseconds.  Can also be a relative time (i.e. ",
		  "code" : "1250ms",
		  "^value^1" : ") in order to make the units clear in the source.  If no timeout value is passed to the method (or the timeout value is negative) then the timeout period is infinite, therefore it's recommended to send a timeout value." ),
		"optional" : True,
		"type" : "Integer" ) ),
	    "rv" : 
            ( "type" : "Hash",
              "desc" : "The return hash will have the following keys: 'msg' containing the return message data, 'subject' giving the subject of the reply message, and optionally 'replySubject' giving a subject name for a subsequent reply message" ),
	    "exceptions" :
	    ( "TIBRVLISTENER-GETMESSAGE-ERROR" : "There was an error dispatching the event on the internal queue",
	      "TIBRV-DEMARSHALLING-ERROR" : "There was an error deserializing the Tibco Rendezvous data to a qore data structore" ) ),
	  "getQueueSize" :
	  ( "desc" : "Returns the number of messages in the queue.",
	    "rv" : 
            ( "type" : "Integer",
              "desc" : "The number of messages in the queue." ), 
	    "exceptions" :
	    ( "TIBRVLISTENER-GETQUEUESIZE-ERROR" : "An internal error occurred." ) ),
	  "createInboxName" :
	  ( "desc" : "Creates a subject name that will be delivered by a point-to-point message directly to this listener object when used as a replySubject in an outgoing message.",
	    "rv" : 
	    ( "type" : "String",
	      "desc" : "The string to use as a replySubject in an outgoing message to ensure that any reply will be a point-to-point reply directly to this listener object" ),
	    "exceptions" :
	    ( "TIBRVLISTENER-CREATEINBOXNAME-ERROR" : "An error occurred creating the inbox name" ) ),
	  "setStringEncoding" :
	  ( "desc" : "Sets the string encoding for the object; any strings deserialized with this object will be tagged with this character encoding.",
	    "args" : 
	    ( "encoding" : 
	      ( "type" : "String",
		"desc" : "The string encoding to use for this object." ) ),
	    "exceptions" :
	    ( "TIBRVLISTENER-SETSTRINGENCODING-ERROR" : "missing encoding parameter from method call" ) ),
	  "getStringEncoding" :
	  ( "desc" : "Returns the character encoding used for the object",
	    "rv" : 
	    ( "desc" : "The character encoding used for the object.",
	      "type" : "String" ) ) ) ) );
