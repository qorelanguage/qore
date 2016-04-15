/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  common.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 David Nichols

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

#ifndef _QORE_COMMON_H

#define _QORE_COMMON_H

/** @file common.h
    provides type and other definitions for the Qore library
 */

#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>

#include <string>
#include <functional>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <set>

//! cross-platform define for AF_UNSPEC
#define Q_AF_UNSPEC -1

//! cross-platform define for AF_INET
#define Q_AF_INET -2

//! cross-platform define for AF_INET6
#define Q_AF_INET6 -3

//! platform-independent define for SOCK_STREAM
#define Q_SOCK_STREAM -1

//! used to identify unique Qore data and parse types (descendents of AbstractQoreNode)
typedef int16_t qore_type_t;

//! used for sizes (same range as a pointer)
typedef size_t qore_size_t;

//! used for offsets that could be negative
typedef intptr_t qore_offset_t;

//! used for the unique class ID for QoreClass objects
typedef unsigned qore_classid_t;

//! set of integers
typedef std::set<int> int_set_t;

//! qore library and module license type identifiers
enum qore_license_t {
   QL_GPL = 0,         //!< code to be used under the GPL license
   QL_LGPL = 1,        //!< code to be used under the LGPL license
   QL_MIT = 2          //!< code to be used under the MIT license
};

#if defined _MSC_VER || ((defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__)
#define _Q_WINDOWS 1
#ifdef _WIN64
#define _Q_WINDOWS 1
#define _Q_WINDOWS64 1
#endif
#endif

#ifdef _Q_WINDOWS
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
  #endif
  #define DLLLOCAL

  #define QLLD "%I64d"
  #define QLLX "%I64x"
  #define QLLDx(a) "%" #a "I64d"
  #define QORE_DIR_SEP '\\'
  #define QORE_DIR_SEP_STR "\\"
#else
  #ifdef HAVE_GCC_VISIBILITY
    #define DLLEXPORT __attribute__ ((visibility("default")))
    #define DLLLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define DLLEXPORT
    #define DLLLOCAL
  #endif
  #define QLLD "%lld"
  #define QLLX "%llx"
  #define QLLDx(a) "%" #a "lld"
  #define QORE_DIR_SEP '/'
  #define QORE_DIR_SEP_STR "/"
#endif

#define _Q_MAKE_STRING(x) #x
#define MAKE_STRING_FROM_SYMBOL(x) _Q_MAKE_STRING(x)

class AbstractQoreNode;
class QoreListNode;
class ExceptionSink;
class QoreObject;
class AbstractPrivateData;
class QoreMethod;
class QoreBuiltinMethod;
class QoreClass;
class QoreTypeInfo;
struct QoreValue;
class QoreValueList;

//! functor template for calling free() on pointers
template <typename T> struct free_ptr : std::unary_function <T*, void> {
   DLLLOCAL void operator()(T* ptr) {
      free(ptr);
   }
};

//! functor template for deleting elements
template <typename T> struct simple_delete {
   DLLLOCAL void operator()(T* ptr) {
      delete ptr;
   }
};

//! functor template for dereferencing elements
template <typename T> struct simple_deref {
   DLLLOCAL void operator()(T* ptr) {
      ptr->deref();
   }
   DLLLOCAL void operator()(T* ptr, ExceptionSink* xsink) {
      ptr->deref(xsink);
   }
};

//! for simple c-string less-than comparisons
class ltstr {
public:
   DLLLOCAL bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
   }
};

//! for simple c-string case-insensitive less-than comparisons
class ltcstrcase {
public:
   DLLLOCAL bool operator()(const char* s1, const char* s2) const {
      return strcasecmp(s1, s2) < 0;
   }
};

//! for std::string case-insensitive less-than comparisons
class ltstrcase {
public:
   DLLLOCAL bool operator()(std::string s1, std::string s2) const {
      return strcasecmp(s1.c_str(), s2.c_str()) < 0;
   }
};

class eqstr {
public:
   DLLLOCAL bool operator()(const char* s1, const char* s2) const {
      return !strcmp(s1, s2);
   }
};

class eqstrcase {
public:
   DLLLOCAL bool operator()(const char* s1, const char* s2) const {
      return !strcasecmp(s1, s2);
   }
};

//! for char less-than comparisons
class ltchar {
public:
   DLLLOCAL bool operator()(const char s1, const char s2) const {
      return s1 < s2;
   }
};

//! non-thread-safe vector for storing "char*" that you want to delete
class cstr_vector_t : public std::vector<char*> {
public:
   DLLLOCAL ~cstr_vector_t() {
      std::for_each(begin(), end(), free_ptr<char>());
   }
};

//! vector of type information for parameter lists
typedef std::vector<const QoreTypeInfo*> type_vec_t;

//! vector of value information for default argument lists
typedef std::vector<AbstractQoreNode*> arg_vec_t;

//! vector of parameter names for parameter lists
typedef std::vector<std::string> name_vec_t;

//! 64bit integer type, cannot use int64_t here since it breaks the API on some 64-bit systems due to equivalence between long int and int
typedef long long int64;

//! runtime code execution flags
typedef uint64_t q_rt_flags_t;

//! the type used for builtin function signatures
/** @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function; the caller owns any reference returned in the return value
 */
typedef QoreValue (*q_func_n_t)(const QoreValueList* args, q_rt_flags_t flags, ExceptionSink* xsink);

//! the type used for builtin function signatures
/** @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_func_t)(const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin function signatures returning an integer value
typedef int64 (*q_func_int64_t)(const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin function signatures returning a boolean value
typedef bool (*q_func_bool_t)(const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin function signatures returning an double value
typedef double (*q_func_double_t)(const QoreListNode* args, ExceptionSink* xsink);

//! the new type used for builtin QoreClass method signatures
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef QoreValue (*q_method_n_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreValueList* args, q_rt_flags_t flags, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_method_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures, returns int64
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the integer return value of the function
 */
typedef int64 (*q_method_int64_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures when called with the even newer generic calling convention supporting hard typing and method variants, returns int
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the integer return value of the function
 */
typedef int (*q_method_int_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures when called with the even newer generic calling convention supporting hard typing and method variants, returns bool
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the integer return value of the function
 */
typedef bool (*q_method_bool_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures when called with the even newer generic calling convention supporting hard typing and method variants, returns double
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the integer return value of the function
 */
typedef double (*q_method_double_t)(QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures when called with the new generic calling convention
/** @param method a constant reference to the QoreMethod being called
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_method2_t)(const QoreMethod& method, QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass method signatures when called with the even newer generic calling convention supporting hard typing and method variants
/** @param method a constant reference to the QoreMethod being called
    @param typeList a constant reference to the list of types defined for the variant being called
    @param ptr a pointer to user-defined member set when the variant is added to the method
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_method3_t)(const QoreMethod& method, const type_vec_t& typeList, const void* ptr, QoreObject* self, AbstractPrivateData* private_data, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin static method signatures for static methods using the new generic calling convention
/** @param method a constant reference to the QoreMethod being called
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_static_method2_t)(const QoreMethod& method, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin static method signatures for static methods using the even newer generic calling convention supporting hard typing and method variants
/** @param method a constant reference to the QoreMethod being called
    @param typeList a constant reference to the list of types defined for the variant being called
    @param ptr a pointer to user-defined member set when the variant is added to the method
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()

    @return the return value of the function (can be 0); the caller owns any reference returned in the return value
 */
typedef AbstractQoreNode* (*q_static_method3_t)(const QoreMethod& method, const type_vec_t& typeList, const void* ptr, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass constructor method signatures
/** @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor_n_t)(QoreObject* self, const QoreValueList* args, q_rt_flags_t rtflags, ExceptionSink* xsink);

//! the type used for builtin QoreClass constructor method signatures
/** @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor_t)(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass constructor method signatures using the new generic calling convention
/** @param thisclass a constant reference to the QoreClass being constructed (in a heirarchy, could be different than the QoreClass returned from QoreObject::getClass()
    @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor2_t)(const QoreClass& thisclass, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass constructor method signatures using the even newer generic calling convention supporting hard typing and method variants
/** @param thisclass a constant reference to the QoreClass being constructed (in a heirarchy, could be different than the QoreClass returned from QoreObject::getClass()
    @param typeList a constant reference to the list of types defined for the variant being called
    @param ptr a pointer to user-defined member set when the variant is added to the method
    @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor3_t)(const QoreClass& thisclass, const type_vec_t& typeList, const void* ptr, QoreObject* self, const QoreListNode* args, ExceptionSink* xsink);

//! the type used for builtin QoreClass system constructor method signatures
/** System constructors are called for objects that are created automatically by the library, normally to be assigned to constants.
    System objects are treated specially by the Qore library as they are not associated with any QoreProgram object.
    Additionally, system object constructors are not allowed to raise exceptions.
    @param self the QoreObject that the function is being executed on
    @param code this argument is necessary in order to be able to provide the va_list in the following argument due to the way QoreClass::execSystemConstructor() is called.  If not required by the constuctor, this argument can be ignored.
    @param args a variable-length list of arguments to the system constructor
 */
typedef void (*q_system_constructor_t)(QoreObject* self, int code, va_list args);

//! the type used for builtin QoreClass system constructor method signatures using the new generic calling convention
/** System constructors are called for objects that are created automatically by the library, normally to be assigned to constants.
    System objects are treated specially by the Qore library as they are not associated with any QoreProgram object.
    Additionally, system object constructors are not allowed to raise exceptions.
    @param self the QoreObject that the function is being executed on
    @param code this argument is necessary in order to be able to provide the va_list in the following argument due to the way QoreClass::execSystemConstructor() is called.  If not required by the constuctor, this argument can be ignored.
    @param args a variable-length list of arguments to the system constructor
 */
typedef void (*q_system_constructor2_t)(const QoreClass& thisclass, QoreObject* self, int code, va_list args);

//! the type used for builtin QoreClass destructor signatures
/** destructors are optional, but, if present, must call AbstractPrivateData::deref() on any private data (if present)
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_destructor_t)(QoreObject* self, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the type used for builtin QoreClass destructor signatures with the new generic calling convention
/** destructors are optional, but, if present, must call AbstractPrivateData::deref() on any private data (if present)
    @param thisclass a constant reference to the QoreClass
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_destructor2_t)(const QoreClass& thisclass, QoreObject* self, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the type used for builtin QoreClass destructor signatures with the new generic calling convention and user-defined data
/** destructors are optional, but, if present, must call AbstractPrivateData::deref() on any private data (if present)
    @param thisclass a constant reference to the QoreClass
    @param ptr a pointer to user-defined member set when the variant is added to the method
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_destructor3_t)(const QoreClass& thisclass, const void* ptr, QoreObject* self, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the type used for builtin QoreClass copy signatures
/** this function must set any private data against the new object by calling QoreObject::setPrivate() on \c self
    @param self the QoreObject that the function is being executed on (the new copy of the object)
    @param old the object being copied
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_copy_t)(QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the type used for builtin QoreClass copy signatures with the new generic calling convention
/** this function must set any private data against the new object by calling QoreObject::setPrivate() on \c self
    @param thisclass a constant reference to the QoreClass being copied
    @param self the QoreObject that the function is being executed on (the new copy of the object)
    @param old the object being copied
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_copy2_t)(const QoreClass& thisclass, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the type used for builtin QoreClass copy signatures with the new generic calling convention
/** this function must set any private data against the new object by calling QoreObject::setPrivate() on \c self
    @param thisclass a constant reference to the QoreClass being copied
    @param ptr a pointer to user-defined member set when the variant is added to the method
    @param self the QoreObject that the function is being executed on (the new copy of the object)
    @param old the object being copied
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_copy3_t)(const QoreClass& thisclass, const void* ptr, QoreObject* self, QoreObject* old, AbstractPrivateData* private_data, ExceptionSink* xsink);

//! the typed used for QoreClass deleteBlocker signatures
/**
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @return false if the object may be deleted normally, true if the deletion should be suppressed
 */
typedef bool (*q_delete_blocker_t)(QoreObject* self, AbstractPrivateData* private_data);

//! type for thread resource IDs (unique within a single running qore library process)
/** @see qore_get_trid()
 */
typedef unsigned q_trid_t;

DLLEXPORT long long q_atoll(const char* str);

#endif // _QORE_COMMON_H
