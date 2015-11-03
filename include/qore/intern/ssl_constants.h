/*
  ssl_constants.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_SSL_CONSTANTS_H

#define _QORE_SSL_CONSTANTS_H

#include <openssl/ssl.h>

static inline void addSSLConstants(class QoreNamespace *ns)
{
   ns->addConstant("X509_V_OK", new QoreBigIntNode(X509_V_OK));
   ns->addConstant("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT));
   ns->addConstant("X509_V_ERR_UNABLE_TO_GET_CRL", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_GET_CRL));
   ns->addConstant("X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE));
   ns->addConstant("X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE));
   ns->addConstant("X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY));
   ns->addConstant("X509_V_ERR_CERT_SIGNATURE_FAILURE", new QoreBigIntNode(X509_V_ERR_CERT_SIGNATURE_FAILURE));
   ns->addConstant("X509_V_ERR_CRL_SIGNATURE_FAILURE", new QoreBigIntNode(X509_V_ERR_CRL_SIGNATURE_FAILURE));
   ns->addConstant("X509_V_ERR_CERT_NOT_YET_VALID", new QoreBigIntNode(X509_V_ERR_CERT_NOT_YET_VALID));
   ns->addConstant("X509_V_ERR_CERT_HAS_EXPIRED", new QoreBigIntNode(X509_V_ERR_CERT_HAS_EXPIRED));
   ns->addConstant("X509_V_ERR_CRL_NOT_YET_VALID", new QoreBigIntNode(X509_V_ERR_CRL_NOT_YET_VALID));
   ns->addConstant("X509_V_ERR_CRL_HAS_EXPIRED", new QoreBigIntNode(X509_V_ERR_CRL_HAS_EXPIRED));
   ns->addConstant("X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD", new QoreBigIntNode(X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD));
   ns->addConstant("X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD", new QoreBigIntNode(X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD));
   ns->addConstant("X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD", new QoreBigIntNode(X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD));
   ns->addConstant("X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD", new QoreBigIntNode(X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD));
   ns->addConstant("X509_V_ERR_OUT_OF_MEM", new QoreBigIntNode(X509_V_ERR_OUT_OF_MEM));
   ns->addConstant("X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT", new QoreBigIntNode(X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT));
   ns->addConstant("X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN", new QoreBigIntNode(X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN));
   ns->addConstant("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY));
   ns->addConstant("X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE", new QoreBigIntNode(X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE));
   ns->addConstant("X509_V_ERR_CERT_CHAIN_TOO_LONG", new QoreBigIntNode(X509_V_ERR_CERT_CHAIN_TOO_LONG));
   ns->addConstant("X509_V_ERR_CERT_REVOKED", new QoreBigIntNode(X509_V_ERR_CERT_REVOKED));
   ns->addConstant("X509_V_ERR_INVALID_CA", new QoreBigIntNode(X509_V_ERR_INVALID_CA));
   ns->addConstant("X509_V_ERR_PATH_LENGTH_EXCEEDED", new QoreBigIntNode(X509_V_ERR_PATH_LENGTH_EXCEEDED));
   ns->addConstant("X509_V_ERR_INVALID_PURPOSE", new QoreBigIntNode(X509_V_ERR_INVALID_PURPOSE));
   ns->addConstant("X509_V_ERR_CERT_UNTRUSTED", new QoreBigIntNode(X509_V_ERR_CERT_UNTRUSTED));
   ns->addConstant("X509_V_ERR_CERT_REJECTED", new QoreBigIntNode(X509_V_ERR_CERT_REJECTED));
   ns->addConstant("X509_V_ERR_SUBJECT_ISSUER_MISMATCH", new QoreBigIntNode(X509_V_ERR_SUBJECT_ISSUER_MISMATCH));
   ns->addConstant("X509_V_ERR_AKID_SKID_MISMATCH", new QoreBigIntNode(X509_V_ERR_AKID_SKID_MISMATCH));
   ns->addConstant("X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH", new QoreBigIntNode(X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH));
   ns->addConstant("X509_V_ERR_KEYUSAGE_NO_CERTSIGN", new QoreBigIntNode(X509_V_ERR_KEYUSAGE_NO_CERTSIGN));
   ns->addConstant("X509_V_ERR_APPLICATION_VERIFICATION", new QoreBigIntNode(X509_V_ERR_APPLICATION_VERIFICATION));

   class QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("X509_V_OK", new QoreStringNode("OK"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT", new QoreStringNode("Unable to get issuer certificate"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_GET_CRL", new QoreStringNode("Unable to get certificate CRL"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE", new QoreStringNode("Unable to decrypt certificate's signature. This means that the actual signature value could not be determined rather than it not matching the expected value; this is only meaningful for RSA"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE", new QoreStringNode("Unable to decrypt CRL's signature"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY", new QoreStringNode("Unable to decode issuer public key (SubjectPublicKeyInfo)"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_SIGNATURE_FAILURE", new QoreStringNode("Certificate signature failure; the signature of the certificate is invalid"), NULL);
   h->setKeyValue("X509_V_ERR_CRL_SIGNATURE_FAILURE", new QoreStringNode("CRL signature failure; the signature of the certificate is invalid"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_NOT_YET_VALID", new QoreStringNode("Certificate is not yet valid"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_HAS_EXPIRED", new QoreStringNode("Certificate has expired"), NULL);
   h->setKeyValue("X509_V_ERR_CRL_NOT_YET_VALID", new QoreStringNode("CRL is not yet valid"), NULL);
   h->setKeyValue("X509_V_ERR_CRL_HAS_EXPIRED", new QoreStringNode("CRL has expired"), NULL);
   h->setKeyValue("X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD", new QoreStringNode("Format error in certificate's notBefore field (invalid time)"), NULL);
   h->setKeyValue("X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD", new QoreStringNode("Format error in certificate's notAfter field (invalid time)"), NULL);
   h->setKeyValue("X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD", new QoreStringNode("Format error in CRL's lastUpdate field (invalid time)"), NULL);
   h->setKeyValue("X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD", new QoreStringNode("Format error in CRL's nextUpdate field (invalid time)"), NULL);
   h->setKeyValue("X509_V_ERR_OUT_OF_MEM", new QoreStringNode("Out of memory error"), NULL);
   h->setKeyValue("X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT", new QoreStringNode("Certificate is self-signed and cannot be found in the trusted list"), NULL);
   h->setKeyValue("X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN", new QoreStringNode("Self signed certificate in certificate chain"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY", new QoreStringNode("Unable to get local issuer certificate. This normally means the list of trusted certificates is not complete"), NULL);
   h->setKeyValue("X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE", new QoreStringNode("Unable to verify the first certificate"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_CHAIN_TOO_LONG", new QoreStringNode("Certificate chain too long"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_REVOKED", new QoreStringNode("Certificate has been revoked"), NULL);
   h->setKeyValue("X509_V_ERR_INVALID_CA", new QoreStringNode("Invalid CA certificate"), NULL);
   h->setKeyValue("X509_V_ERR_PATH_LENGTH_EXCEEDED", new QoreStringNode("The basicConstraints pathlength parameter has been exceeded"), NULL);
   h->setKeyValue("X509_V_ERR_INVALID_PURPOSE", new QoreStringNode("The certificate cannot be used for the specified purpose"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_UNTRUSTED", new QoreStringNode("Root CA is not marked as trusted for the specified purpose"), NULL);
   h->setKeyValue("X509_V_ERR_CERT_REJECTED", new QoreStringNode("Root CA is marked to reject the specified purpose"), NULL);
   h->setKeyValue("X509_V_ERR_SUBJECT_ISSUER_MISMATCH", new QoreStringNode("The current candidate issuer certificate was rejected because its subject name did not match the issuer name of the current certificate"), NULL);
   h->setKeyValue("X509_V_ERR_AKID_SKID_MISMATCH", new QoreStringNode("The current candidate issuer certificate was rejected because its subject key identifier was present and did not match the authority key identifier of the current certificate"), NULL);
   h->setKeyValue("X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH", new QoreStringNode("Issuer name and serial number of candidate certificate do not match the authority key identifier of the current certificate"), NULL);
   h->setKeyValue("X509_V_ERR_KEYUSAGE_NO_CERTSIGN", new QoreStringNode("The keyUsage extension does not permit certificate signing"), NULL);
   h->setKeyValue("X509_V_ERR_APPLICATION_VERIFICATION", new QoreStringNode("Verification failure"), NULL);

   ns->addConstant("X509_VerificationReasons", h);
}

static inline const char *getSSLCVCode(int code)
{
   switch (code)
   {
      case 0:
	 return "X509_V_OK";
      case 2:
	 return "X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT";
      case 3:
	 return "X509_V_ERR_UNABLE_TO_GET_CRL";
      case 4:
	 return "X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE";
      case 5:
	 return "X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE";
      case 6:
	 return "X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY";
      case 7:
	 return "X509_V_ERR_CERT_SIGNATURE_FAILURE";
      case 8:
	 return "X509_V_ERR_CRL_SIGNATURE_FAILURE";
      case 9:
	 return "X509_V_ERR_CERT_NOT_YET_VALID";
      case 10:
	 return "X509_V_ERR_CERT_HAS_EXPIRED";
      case 11:
	 return "X509_V_ERR_CRL_NOT_YET_VALID";
      case 12:
	 return "X509_V_ERR_CRL_HAS_EXPIRED";
      case 13:
	 return "X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD";
      case 14:
	 return "X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD";
      case 15:
	 return "X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD";
      case 16:
	 return "X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD";
      case 17:
	 return "X509_V_ERR_OUT_OF_MEM";
      case 18:
	 return "X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT";
      case 19:
	 return "X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN";
      case 20:
	 return "X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY";
      case 21:
	 return "X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE";
      case 22:
	 return "X509_V_ERR_CERT_CHAIN_TOO_LONG";
      case 23:
	 return "X509_V_ERR_CERT_REVOKED";
      case 24:
	 return "X509_V_ERR_INVALID_CA";
      case 25:
	 return "X509_V_ERR_PATH_LENGTH_EXCEEDED";
      case 26:
	 return "X509_V_ERR_INVALID_PURPOSE";
      case 27:
	 return "X509_V_ERR_CERT_UNTRUSTED";
      case 28:
	 return "X509_V_ERR_CERT_REJECTED";
      case 29:
	 return "X509_V_ERR_SUBJECT_ISSUER_MISMATCH";
      case 30:
	 return "X509_V_ERR_AKID_SKID_MISMATCH";
      case 31:
	 return "X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH";
      case 32:
	 return "X509_V_ERR_KEYUSAGE_NO_CERTSIGN";
      case 50:
	 return "X509_V_ERR_APPLICATION_VERIFICATION";
   }
   return NULL;
}

#endif
