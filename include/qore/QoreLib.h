/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreLib.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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

//! not defined because this version of Qore has moved all XML functionality to the "xml" module
#undef _QORE_HAS_QORE_XMLNODE_CLASS

//! not defined because this version of Qore has moved all XML functionality to the "xml" module
#undef _QORE_HAS_QORE_XMLREADER_CLASS

//! not defined because this version of Qore has moved all XML functionality to the "xml" module
#undef _QORE_HAS_QORE_XMLDOC_CLASS

//! defined because this version of Qore supports hard typing, overloading, default arguments, etc
#define _QORE_HAS_HARD_TYPING 1

//! defined because this version of Qore supports the execRaw() DBI function
#define _QORE_HAS_DBI_EXECRAW 1

//! defined because this version of Qore has time zone support
#define _QORE_HAS_TIME_ZONES 1

//! defined because this version of Qore has thread resource IDs
#define _QORE_HAS_THREAD_RESOURCE_IDS 1

//! defined because this version of Qore has the prepared statement API
#define _QORE_HAS_PREPARED_STATMENT_API 1

//! defined because this version of Qore has the Datasource::activeTransaction() function
#define _QORE_HAS_DATASOURCE_ACTIVETRANSACTION 1

//! defined because this version of Qore supports the DBI selectRow() function
#define _QORE_HAS_DBI_SELECT_ROW 1

//! defined because this version of Qore supports the number type (QoreNumberNode)
#define _QORE_HAS_NUMBER_TYPE 1

//! defined because this version of Qore has the q_path_is_readable() function
#define _QORE_HAS_PATH_IS_READABLE 1

//! defined because this version of Qore supports the DBI option APIs
#define _QORE_HAS_DBI_OPTIONS 1

//! defined because this version of Qore has the find_create_timezone() function
#define _QORE_HAS_FIND_CREATE_TIMEZONE 1

//! defined because this version of Qore has a QoreNumberNode constructor with a precision specifier
#define _QORE_HAS_NUMBER_CONS_WITH_PREC 1

//! defined because this version of Qore has the QoreFileOjectHelper class
#define _QORE_HAS_FILE_OBJECT_HELPER 1

//! defined because this version of Qore has the QoreQueueOjectHelper class
#define _QORE_HAS_QUEUE_OBJECT_HELPER 1

//! defined becaus this version of Qore has the QoreHttpClientObject class
#define _QORE_HAS_QOREHTTPCLIENTOBJECT 1

//! defined because this version of Qore supports the DBI describe API
#define _QORE_HAS_DBI_DESCRIBE 1

//! defined because this version of Qore supports the DBI event API
#define _QORE_HAS_DBI_EVENTS 1

//! defined because this version of Qore has a Queue class definition in public headers
#define _QORE_HAS_QUEUE_OBJECT 1

//! defined because this version of Qore has a public Socket performance API
#define _QORE_HAS_SOCKET_PERF_API 1

//! defined because this version of Qore has the QL_MIT license enum value
#define _QORE_HAS_QL_MIT 1

// qore code flags
#define QC_NO_FLAGS                 0   //! no flag
#define QC_NOOP               (1 << 0)  //! this variant is a noop, meaning it returns a constant value with the given argument types
#define QC_USES_EXTRA_ARGS    (1 << 1)  //! code accesses arguments beyond the declared parameter list
#define QC_CONSTANT_INTERN    (1 << 2)  //! internal constant flag, use QC_CONSTANT instead
#define QC_DEPRECATED         (1 << 3)  //! function or method is deprecated and will be removed in a later release
#define QC_RET_VALUE_ONLY     (1 << 4)  //! code only returns a value and has no other side effects
#define QC_RUNTIME_NOOP       (1 << 5)  //! this variant is a noop like QC_NOOP, but additionally is not available to programs executing with %require-types (PO_REQUIRE_TYPES)

// composite flags
#define QC_CONSTANT (QC_CONSTANT_INTERN | QC_RET_VALUE_ONLY) //! code is safe to use in a constant expression (i.e. has no side effects, does not change internal state, cannot throw an exception under any circumstances, just returns a calculation based on its arguments)

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

//! returns the seconds from the epoch
DLLEXPORT int64 q_epoch();

//! returns the seconds and microseconds from the epoch
DLLEXPORT int64 q_epoch_us(int &us);

//! returns the seconds and nanoseconds from the epoch
DLLEXPORT int64 q_epoch_ns(int &us);

//! thread-safe basename function (resulting pointer must be free()ed)
DLLEXPORT char *q_basename(const char *path);

//! returns a pointer within the same string
DLLEXPORT char *q_basenameptr(const char *path);

//! thread-safe dirname function (resulting pointer must be free()ed)
DLLEXPORT char *q_dirname(const char *path);

//! frees memory if there is an allocation error
DLLEXPORT void *q_realloc(void *ptr, size_t size);

#if (!defined _WIN32 && !defined __WIN32__) || defined __CYGWIN__
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
#endif // ! windows

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

//! for getting an integer number of microseconds, with 0 as the default, from either a relative time value or an integer value
DLLEXPORT int64 getMicroSecZeroInt64(const AbstractQoreNode *a);

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

static inline qore_type_t get_node_type(const AbstractQoreNode *n) {
   return n ? n->getType() : NT_NOTHING;
}

class BinaryNode;
class QoreStringNode;
class ExceptionSink;

typedef QoreStringNode* (*qore_uncompress_to_string_t)(const BinaryNode* b, const QoreEncoding* enc, ExceptionSink* xsink);

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

//! parses a string of base64-encoded data and returns a BinaryNode
DLLEXPORT BinaryNode *parseBase64(const char *buf, int len, ExceptionSink *xsink);

//! parses a string of hex characters and returns a BinaryNode
DLLEXPORT BinaryNode *parseHex(const char *buf, int len, ExceptionSink *xsink);

class AbstractQoreZoneInfo;

//! returns a time zone for the given time zone UTC offset
DLLEXPORT const AbstractQoreZoneInfo* findCreateOffsetZone(int seconds_east);

//! returns a time zone for the given region name or UTC offset given as a string ("+01:00")
/** @param name the name of the region to find or a UTC offset given as a string ("+01:00")
    @param xsink if the given region is not found or valid or any error occur finding or loading the given region, exception info is stored here and the function returns 0

    @return the time zone region found or 0 if the timezone is UTC (also in case of an exception 0 is returned - check xsink after calling)
 */
DLLEXPORT const AbstractQoreZoneInfo* find_create_timezone(const char* name, ExceptionSink* xsink);

//! returns the UTC offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
DLLEXPORT int tz_get_utc_offset(const AbstractQoreZoneInfo* tz, int64 epoch_offset, bool &is_dst, const char *&zone_name);
//! returns true if the zone has daylight savings time ever
DLLEXPORT bool tz_has_dst(const AbstractQoreZoneInfo* tz);
//! returns the reion name for the given time zone
DLLEXPORT const char* tz_get_region_name(const AbstractQoreZoneInfo* tz);

//! option: atomic operations
#define QORE_OPT_ATOMIC_OPERATIONS       "atomic operations"
//! option: stack guard
#define QORE_OPT_STACK_GUARD             "stack guard"
//! option: signal handling
#define QORE_OPT_SIGNAL_HANDLING         "signal handling"
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
//! option: md2 algorithm supported (depends on openssl used to compile qore)
#define QORE_OPT_MD2                     "openssl md2"
//! option: TermIOS class available
#define QORE_OPT_TERMIOS                 "termios"
//! option: file locking
#define QORE_OPT_FILE_LOCKING            "file locking"
//! option: unix user/group management functions available
#define QORE_OPT_UNIX_USERMGT            "unix user management"
//! option: unix file management functions available
#define QORE_OPT_UNIX_FILEMGT            "unix file management"
//! options: deterministic garbage collection
#define QORE_OPT_DETERMINISTIC_GC        "deterministic GC"
//! option: round() function available
#define QORE_OPT_FUNC_ROUND              "round()"
//! option: timegm() function available
#define QORE_OPT_FUNC_TIMEGM             "timegm()"
//! option: seteuid() function available
#define QORE_OPT_FUNC_SETEUID            "seteuid()"
//! option: setegid() function available
#define QORE_OPT_FUNC_SETEGID            "setegid()"
//! option: system() function available
#define QORE_OPT_FUNC_SYSTEM             "system()"
//! option: kill() function available
#define QORE_OPT_FUNC_KILL               "kill()"
//! option: fork() function available
#define QORE_OPT_FUNC_FORK               "fork()"
//! option: getppid() function available
#define QORE_OPT_FUNC_GETPPID            "getppid()"
//! option: statvfs() function available
#define QORE_OPT_FUNC_STATVFS            "statvfs()"
//! option: setsid() function available
#define QORE_OPT_FUNC_SETSID             "setsid()"
//! option: is_executable() function available
#define QORE_OPT_FUNC_IS_EXECUTABLE      "is_executable()"

//! option type feature
#define QO_OPTION     0
#define QO_ALGORITHM  1
#define QO_FUNCTION   2

//! definition of the elements in the qore_option_list
struct qore_option_s {
   const char* option;   //!< name of the option
   const char* constant; //!< name of the constant for this option
   int type;             //!< the type of the option
   bool value;           //!< the value of the option
};

//! returns the error string as a QoreStringNode
DLLEXPORT QoreStringNode *q_strerror(int errnum);
//! concatenates the error string to the given string
DLLEXPORT void q_strerror(QoreString &str, int errnum);

//! list of qore options
DLLEXPORT extern const qore_option_s *qore_option_list;
//! number of elements in the option list
DLLEXPORT extern size_t qore_option_list_size;

//! allows a module to take over ownership of a signal
/** @param sig signal number
    @param name module name taking ownership of managing the signal
    @return 0 for OK, non-zero for failed (error message)
 */
DLLEXPORT QoreStringNode *qore_reassign_signal(int sig, const char *name);

//! macro to return the maximum of 2 numbers
#define QORE_MAX(a, b) ((a) > (b) ? (a) : (b))

//! macro to return the minimum of 2 numbers
#define QORE_MIN(a, b) ((a) < (b) ? (a) : (b))

#define QORE_PARAM_NO_ARG (NULL)

// define QORE_PATH_MAX
#ifndef QORE_PATH_MAX
#ifdef _XOPEN_PATH_MAX
#define QORE_PATH_MAX _XOPEN_PATH_MAX
#else
#define QORE_PATH_MAX 1024
#endif
#endif

//! to set the time zone from the command line
/** @note this function can only be called when a program exists
 */
DLLEXPORT void parse_set_time_zone(const char *zone);

//! use this function instead of usleep(), as usleep() is not signal-safe on some platforms (ex: Solaris 8, 9)
DLLEXPORT int qore_usleep(int64 usecs);

//! platform-independent API that tells if the given path is readable by the current user
DLLEXPORT bool q_path_is_readable(const char* path);

//! tries to parse a boolean value - standard conversion or uses q_parse_bool(const char*) if it's a string
DLLEXPORT bool q_parse_bool(const AbstractQoreNode* n);

//! parses a string and returns a boolean (ie case-insensitive "on","true","enable*","yes" are True, the rest is interpreted as a number where 0=false, everything else=true)
DLLEXPORT bool q_parse_bool(const char* str);

//! returns the boolean value of qore library the given option name; false if the option is unknown
DLLEXPORT bool q_get_option_value(const char* opt);

//! returns the boolean value of qore library the given name of the constant for the option; false if the option constant name is unknown
DLLEXPORT bool q_get_option_constant_value(const char* opt);

//! concatenates UNIX-style permissions to perm and from mode and returns a string giving the file type
DLLEXPORT const char* q_mode_to_perm(mode_t mode, QoreString& perm);

#endif // _QORE_QORELIB_H
