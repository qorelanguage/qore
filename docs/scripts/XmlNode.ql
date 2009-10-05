return 
    ( 
      "XmlNode" :
      ( "desc" : "The XmlNode class provides information about the components of an XML document.  This class currently cannot be constructed manually, but rather can only be returned by the [XmlDoc|XmlDoc_Class] class.",
	"methods" :
	( "constructor" :
	  ( "desc" : "Cannot be called manually; throws an exception.",
	    "exceptions" :
	    ( "XMLNODE-CONSTRUCTOR-ERROR" : "XmlNode objects cannot be constructed manually" )),
	  "destructor" :
	  ( "desc" : "Destroys the XmlNode object." ),
	  "copy" :
	  ( "desc" : "Creates an independent copy of the XmlNode object.",
	  ),
	  "childElementCount" :
	  ( "desc" : "Returns the number of child elements of the XmlNode.",
	    "rv" : ("type" : "Int", "desc" : "the number of child elements of the XmlNode" )),
	  "getSpacePreserve" :
	  ( "desc" : "Returns the space-preserving behavior of the XmlNode object.",
	    "rv" : ("type" : "Int", "desc" : "The space-preserving behavior of the XmlNode: -1 = xml:space is not inherited, 0 = default, 1 = preserve"),
	  ),
	  "getElementType" :
	  ( "desc" : "Returns the type of the XmlNode object; for possible values see [XML Element Type Constants|XML_Element_Type_Constants].",
	    "rv" : ("type" : "Int", "desc" : "the type of the XmlNode object; for possible values see [XML Element Type Constants|XML_Element_Type_Constants]"),
	  ),
	  "getElementTypeName" :
	  ( "desc" : "Returns the name of the type of the XmlNode object; for possible values see the [ElementTypeMap|ElementTypeMap] constant.",
	    "rv" : ("type" : "String", "desc" : "the name of the type of the XmlNode object; for possible values see the [ElementTypeMap|ElementTypeMap] constant"),
	  ),
	  "firstElementChild" :
	  ( "desc" : "Returns an [XmlNode|XmlNode_Class] object for the first child of the current XmlNode object that is an XML element, or NOTHING if there is none.",
	    "rv" : ("type" : "[XmlNode|XmlNode_Class] or NOTHING", "desc" : "returns an [XmlNode|XmlNode_Class] object for the first element child of the current XmlNode object, or NOTHING if there is none"),
	  ),
	  "getLastChild" :
	  ( "desc" : "Returns an [XmlNode|XmlNode_Class] object for the last child of the current XmlNode object, or NOTHING if there is none.",
	    "rv" : ("type" : "[XmlNode|XmlNode_Class] or NOTHING", "desc" : "returns an [XmlNode|XmlNode_Class] object for the last child of the current XmlNode object, or NOTHING if there is none"),
	  ),
	  "lastElementChild" :
	  ( "desc" : "Returns an [XmlNode|XmlNode_Class] object for the last child of the current XmlNode object that is an XML element, or NOTHING if there is none.",
	    "rv" : ("type" : "[XmlNode|XmlNode_Class] or NOTHING", "desc" : "returns an [XmlNode|XmlNode_Class] object for the last element child of the current XmlNode object, or NOTHING if there is none"),
	  ),
	  "nextElementSibling" :
	  ( "desc" : "Returns an [XmlNode|XmlNode_Class] object for the next element at the same level of the current XmlNode object, or NOTHING if there is none.",
	    "rv" : ("type" : "[XmlNode|XmlNode_Class] or NOTHING", "desc" : "returns an [XmlNode|XmlNode_Class] object for the next element at the same level of the current XmlNode object, or NOTHING if there is none"),
	  ),
	  "previousElementSibling" :
	  ( "desc" : "Returns an [XmlNode|XmlNode_Class] object for the previous element at the same level of the current XmlNode object, or NOTHING if there is none.",
	    "rv" : ("type" : "[XmlNode|XmlNode_Class] or NOTHING", "desc" : "returns an [XmlNode|XmlNode_Class] object for the previous element at the same level of the current XmlNode object, or NOTHING if there is none"),
	  ),
	  "getPath" :
	  ( "desc" : "Returns a string representing a structured path for the current node.",
	    "rv" : "a string representing a structured path for the current node",
	  ),
	  "getNsProp" :
	  ( "desc" : "Returns the value of the given property anchored in the given namespace, or NOTHING if no such property exists in the current XmlNode.",
	    "args" : 
	    ( "prop" :
	      ( "desc" : "The name of the property to retrieve" ), 
	      "namespace" : 
	      ( "desc" : "The name of the namespace of the property",
		"type" : "String" ) ),
	    "rv" : "the value of the property or NOTHING if it does not exist",
	    "exceptions" :
	    ( "XMLNODE-GETNSPROP-ERROR" : "missing or invalid argument",
	      )),
	  "getProp" :
	  ( "desc" : "Returns the value of the given property, or NOTHING if no such property exists in the current XmlNode.",
	    "args" : 
	    ( "prop" :
	      ( "desc" : "The name of the property to retrieve" ) ),
	    "rv" : "the value of the property or NOTHING if it does not exist",
	    "exceptions" :
	    ( "XMLNODE-GETPROP-ERROR" : "missing or invalid argument",
	      )),
	  "getContent" :
	  ( "desc" : "Returns a string of the content of the current node.",
	    "rv" : "a string of the content of the current node",
	  ),
	  "getName" :
	  ( "desc" : "Returns the name of the current node.",
	    "rv" : "the name of the current node",
	  ),
	  "getLang" :
	  ( "desc" : "Returns the language of the current node, determined by the value of the xml:lang attribute of this node or of the nearest ancestor.  If no such property is available, then NOTHING is returned.",
	    "rv" : "the language of the current node, determined by the value of the xml:lang attribute of this node or of the nearest ancestor.  If no such property is available, then NOTHING is returned.",
	  ),
	  "isText" :
	  ( "desc" : "Returns True if the node is a text node, False if not.",
	    "rv" : ("type":"Boolean","desc":"True if the node is a text node, False if not"),
	  ),
	  "isBlank" :
	  ( "desc" : "Returns True if the node is empty or whitespace only, False if not.",
	    "rv" : ("type":"Boolean","desc":"True if the node is empty or whitespace only, False if not"),
	  ),
	  "getXML" :
	  ( "desc" : "Returns XML corresponding to the current node and all its children.",
	    "rv" : "XML corresponding to the current node and all its children",
	  ),
	  
	)));
