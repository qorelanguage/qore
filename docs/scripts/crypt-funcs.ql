
$funcs = 
    ( 
      "des_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "des_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "des_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "des_ede_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for the two-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "des_ede_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for the two-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "des_ede_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Cipher Block Chaining function for the two-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "des_ede3_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for the three-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "des_ede3_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for the three-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "des_ede3_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Cipher Block Chaining function for the three-key triple ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.itl.nist.gov/fipspubs/fip46-2.htm" ), "^value^" : "DES" ),
	  "^value1^" : " algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DES-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "desx_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for RSA's ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "DESX" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DESX-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "desx_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for RSA's ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "DESX" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DESX-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "desx_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Encrypts data to a string using the Cipher Block Chaining function for RSA's ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "DESX" ),
	  "^value1^" : " algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "DESS-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "rc4_encrypt" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Alleged RC4 cipher algorithm, which should be compatible with ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC4(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC4-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "rc4_decrypt" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Alleged RC4 cipher algorithm, which should be compatible with ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC4(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC4-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "rc4_decrypt_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Alleged RC4 cipher algorithm, which should be compatible with ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC4(tm) algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC4-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "rc2_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC2(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "BLOWFISH-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "rc2_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC2(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC2-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "rc2_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC2(tm) algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC4-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "cast5_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://en.wikipedia.org/wiki/CAST5" ), "^value^" : "CAST5" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "CAST5-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "cast5_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://en.wikipedia.org/wiki/CAST5" ), "^value^" : "CAST5" ),
	  "^value1^" : " algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "CAST5-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "cast5_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Cipher Block Chaining function for the ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://en.wikipedia.org/wiki/CAST5" ), "^value^" : "CAST5" ),
	  "^value1^" : " algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "CAST5-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),

      "rc5_encrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Encrypts data using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC5(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC5-ENCRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : "string | binary" ),
      "rc5_decrypt_cbc" :
      ( "desc" : 
	( "^value^" : "Decrypts data using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC5(tm) algorithm." ),
	"ret" : "Binary",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC5-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      "rc5_decrypt_cbc_to_string" :
      ( "desc" : 
	( "^value^" : "Decrypts data to a string using the Cipher Block Chaining function for ",
	  "ulink" : ( "^attributes^" : ( "url" : "http://www.rsasecurity.com/" ), "^value^" : "RSA's" ),
	  "^value1^" : " RC5(tm) algorithm." ),
	"ret" : "String",
	"exceptions" :
	( "PARAMETER-ERROR" : "missing data (string or binary) parameter to function, invalid data type (expecing string or binary)",
	  "RC5-DECRYPT-PARAM-ERROR" : "missing or invalid key parameter (ex: invalid size) or invalid input vector (less than 8 bytes, only raised if input vector present)" ),
	"args" : ( "binary", "key", "[iv]" ) ),
      );

return $funcs;
