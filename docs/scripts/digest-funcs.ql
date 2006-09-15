
$funcs = 
    ( 
      "DSS" :
      ( "desc" : "Returns the DSS message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "DSS-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "DSS1" :
      ( "desc" : "Returns the DSS1 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "DSS1-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "MD2" :
      ( "desc" : "Returns the MD2 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "MD2-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "MD4" :
      ( "desc" : "Returns the MD4 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "MD4-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "MD5" :
      ( "desc" : "Returns the MD5 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "MD5-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "MSC2" :
      ( "desc" : "Returns the MSC2 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "MSC2-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "RIPEMD160" :
      ( "desc" : "Returns the RIPEMD160 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "RIPEMD160-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "SHA" :
      ( "desc" : "Returns the SHA message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "SHA-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      "SHA1" :
      ( "desc" : "Returns the SHA1 message digest of the supplied argument (for strings, the trailing null character is not included in the digest)",
	"ret" : "String",
	"exceptions" :
	( "SHA1-DIGEST-ERROR" : "missing data parameter, invalid data parameter (expecing string or binary)",
	  "DIGEST-ERROR" : "error calculating digest" ),
	"args" : "string | binary" ),

      );

return $funcs;
