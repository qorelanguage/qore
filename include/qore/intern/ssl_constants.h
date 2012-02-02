/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ssl_constants.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

static inline const char *getSSLCVCode(int code) {
   switch (code) {
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
