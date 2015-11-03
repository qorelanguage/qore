/*
  QoreLib.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#ifndef _QORE_QORELIB_H

#define _QORE_QORELIB_H

#include <qore/common.h>
#include <qore/QoreThreadLock.h>
#include <qore/qore_bitopts.h>
#include <qore/safe_dslist>

#include <time.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>

/** @file QoreLib.h
    contains prototypes for various helper functions in the Qore library
 */

//! defined because this version of Qore has the XmlNode Qore class
#define _QORE_HAS_QORE_XMLNODE_CLASS 1

//! defined because this version of Qore has the XmlReader Qore class
#define _QORE_HAS_QORE_XMLREADER_CLASS 1

//! defined because this version of Qore has the XmlDoc Qore class
#define _QORE_HAS_QORE_XMLDOC_CLASS 1

//! function to try and make a class name out of a file path, returns a new string that must be free()ed
DLLEXPORT char *make_class_name(const char *fn);

//! a string formatting function that works with Qore data structures
DLLEXPORT QoreStringNode *q_sprintf(const class QoreListNode *params, int field, int offset, class ExceptionSink *xsink);

//! a string formatting function that works with Qore data structures
DLLEXPORT QoreStringNode *q_vsprintf(const class QoreListNode *params, int field, int offset, class ExceptionSink *xsink);

//! thread-safe version of "localtime()"
DLLEXPORT struct tm *q_localtime(const time_t *clock, struct tm *tms);

//! thread-safe version of "gmtime()"
DLLEXPORT struct tm *q_gmtime(const time_t *clock, struct tm *tms);

//! thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_basename(const char *path);

//! returns a pointer within the same string
DLLEXPORT char *q_basenameptr(const char *path);

//! thread-safe dirname function (resulting pointer must be free()ed)
DLLEXPORT char *q_dirname(const char *path);

//! frees memory if there is an allocation error
DLLEXPORT void *q_realloc(void *ptr, size_t size);

//! thread-safe version of getpwuid(): returns a Qore hash of the passwd information from the uid if possible, otherwise 0
DLLEXPORT QoreHashNode *q_getpwuid(uid_t uid);

//! thread-safe version of getpwnam(): returns a Qore hash of the passwd information from the username if possible, otherwise 0
DLLEXPORT QoreHashNode *q_getpwnam(const char *name);

//! thread-safe version of getgrgid(): returns a Qore hash of the group information from the gid if possible, otherwise 0
DLLEXPORT QoreHashNode *q_getgrgid(uid_t uid);

//! thread-safe version of getgrnam(): returns a Qore hash of the group information from the group name if possible, otherwise 0
DLLEXPORT QoreHashNode *q_getgrnam(const char *name);

//! thread-safe way to lookup a uid from a username
/**
   @param name the username to look up
   @param uid the uid returned
   @return 0 for no error, non-zero is an error code like errno
*/
int q_uname2uid(const char *name, uid_t &uid);

//! thread-safe way to lookup a gid from a group name
/**
   @param name the group to look up
   @param gid the gid returned
   @return 0 for no error, non-zero is an error code like errno
*/
int q_gname2gid(const char *name, gid_t &gid);

//! sets up the Qore ARGV and QORE_ARGV values
DLLEXPORT void qore_setup_argv(int pos, int argc, char *argv[]);

//! returns the license type that the library has been initialized under
DLLEXPORT qore_license_t qore_get_license();

//! instead of calling "exit()", call qore_exit_process() to exit without risking a crash if other threads are running
DLLEXPORT void qore_exit_process(int rc);

//! STL-like list containing all presently-loaded Qore features
/** this list must be thread-safe for reading, writing under a lock
 */
class FeatureList : public safe_dslist<std::string> {
   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL FeatureList(const FeatureList&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL FeatureList& operator=(const FeatureList&);

   public:
      //! initialized by the library, constructor not exported
      DLLLOCAL FeatureList();

      //! destructor not exported
      DLLLOCAL ~FeatureList();
};

//! list of qore features
DLLEXPORT extern FeatureList qoreFeatureList;

//! find one of any characters in a string
static inline char *strchrs(const char *str, const char *chars) {
   while (*str) {
      if (strchr(chars, *str))
	 return (char *)str;
      str++;
   }
   return 0;
}

//! find a character in a string up to len
static inline char *strnchr(const char *str, int len, char c) {
   int i = 0;
   while (i++ != len) {
      if (*str == c)
	 return (char *)str;
      ++str;
   }
   return 0;
}

//! convert a string to lower-case in place
static inline void strtolower(char *str) {
   while (*(str)) {
      (*str) = tolower(*str);
      str++;
   }
}

//! convert a string to upper-case in place
static inline char *strtoupper(char *str) {
   char *p = str;
   while (*(p)) {
      *p = toupper(*p);
      p++;
   }
   return str;
}

//! for getting an integer number of seconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getSecZeroInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getSecZeroBigInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int getSecMinusOneInt(const AbstractQoreNode *a);

//! for getting an integer number of seconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getSecMinusOneBigInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getMsZeroInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getMsZeroBigInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int getMsMinusOneInt(const AbstractQoreNode *a);

//! for getting an integer number of milliseconds, with -1 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getMsMinusOneBigInt(const AbstractQoreNode *a);

//! for getting an integer number of microseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int getMicroSecZeroInt(const AbstractQoreNode *a);

//! to check if an AbstractQoreNode object is NOTHING
static inline bool is_nothing(const AbstractQoreNode *n) {
   if (!n || n->getType() == NT_NOTHING)
      return true;
   
   return false;
}

//! to deref an AbstractQoreNode (when the pointer may be 0)
static inline void discard(AbstractQoreNode *n, ExceptionSink *xsink) {
   if (n)
      n->deref(xsink);
}

static inline const char *get_type_name(const AbstractQoreNode *n) {
   return n ? n->getTypeName() : "NOTHING";
}

class BinaryNode;
class QoreStringNode;
class ExceptionSink;

//! compresses data with the DEFLATE algorithm
DLLEXPORT BinaryNode     *qore_deflate(void *ptr, unsigned long len, int level, ExceptionSink *xsink);
//! decompresses data compressed with the DEFLATE algorithm to a string
DLLEXPORT QoreStringNode *qore_inflate_to_string(const BinaryNode *b, const QoreEncoding *enc, ExceptionSink *xsink);
//! decompresses data compressed with the DEFLATE algorithm to a binary
DLLEXPORT BinaryNode     *qore_inflate_to_binary(const BinaryNode *b, ExceptionSink *xsink);
//! gzips data
DLLEXPORT BinaryNode     *qore_gzip(void *ptr, unsigned long len, int level, ExceptionSink *xsink);
//! gunzips compressed data to a string
DLLEXPORT QoreStringNode *qore_gunzip_to_string(const BinaryNode *bin, const QoreEncoding *enc, ExceptionSink *xsink);
//! gunzips compressed data to a binary
DLLEXPORT BinaryNode     *qore_gunzip_to_binary(const BinaryNode *bin, ExceptionSink *xsink);
//! compresses data with bzip2
DLLEXPORT BinaryNode     *qore_bzip2(void *ptr, unsigned long len, int level, ExceptionSink *xsink);
//! decompresses bzip2 data to a string
DLLEXPORT QoreStringNode *qore_bunzip2_to_string(const BinaryNode *bin, const QoreEncoding *enc, ExceptionSink *xsink);
//! decompresses bzip2 data to a binary
DLLEXPORT BinaryNode     *qore_bunzip2_to_binary(const BinaryNode *bin, ExceptionSink *xsink);

//! option: atomic operations
#define QORE_OPT_ATOMIC_OPERATIONS       "atomic operations"
//! option: stack guard
#define QORE_OPT_STACK_GUARD             "stack guard"
//! option: runtime stack tracing
#define QORE_OPT_RUNTIME_STACK_TRACE     "runtime stack tracing"
//! option: library debugging
#define QORE_OPT_LIBRARY_DEBUGGING       "library debugging"
//! option: ssh224 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_SHA224                  "openssl sha224"
//! option: ssh256 algorithm supported (depends on openssl used to compile qore) 
#define QORE_OPT_SHA256                  "openssl sha256"
//! option: ssh384 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_SHA384                  "openssl sha384"
//! option: ssh512 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_SHA512                  "openssl sha512"
//! option: mdc2 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_MDC2                    "openssl mdc2"
//! option: rc5 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_RC5                     "openssl rc5"
//! option: round() function available
#define QORE_OPT_FUNC_ROUND              "round()"
//! option: timegm() function available
#define QORE_OPT_FUNC_TIMEGM             "timegm()"
//! option: seteuid() function available
#define QORE_OPT_FUNC_SETEUID            "seteuid()"
//! option: setegid() function available
#define QORE_OPT_FUNC_SETEGID            "setegid()"
//! option: parseXMLWithSchema() function available (depends on libxml2)
#define QORE_OPT_FUNC_PARSEXMLWITHSCHEMA "parseXMLWithSchema()"
//! option: parseXMLWithRelaxNG() function available (depends on libxml2)
#define QORE_OPT_FUNC_PARSEXMLWITHRELAXNG "parseXMLWithRelaxNG()"

//! option type feature
#define QO_OPTION     0
#define QO_ALGORITHM  1
#define QO_FUNCTION   2

//! definition of the elements in the qore_option_list
struct qore_option_s {
      const char *option;   //!< name of the option
      const char *constant; //!< name of the constant for this option
      int type;             //!< the type of the option
      bool value;           //!< the value of the option
};

//const char *qore_get_option_list();

//! list of qore options
DLLEXPORT extern const qore_option_s *qore_option_list;
//! number of elements in the option list
DLLEXPORT extern size_t qore_option_list_size;

#endif // _QORE_QORELIB_H
