/*
  common.h

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

#ifndef _QORE_COMMON_H

#define _QORE_COMMON_H

/** @file common.h
    provides type and other definitions for the Qore library
 */

#include <string.h>
#include <strings.h>

#include <string>
#include <functional>
#include <list>
#include <set>
#include <vector>
#include <algorithm>

//! used to identify unique Qore data and parse types (descendents of AbstractQoreNode)
typedef short qore_type_t;

//! used for sizes (same range as a pointer)
typedef unsigned long qore_size_t;

//! used for offsets that could be negative
typedef long qore_offset_t;

//! used for the unique class ID for QoreClass objects
typedef unsigned qore_classid_t;

//! set of integers
typedef std::set<int> int_set_t;

//! qore library and module license type identifiers
enum qore_license_t { QL_GPL = 0,         //!< code to be used under the GPL license
		      QL_LGPL = 1         //!< code to be used under the LGPL license
};

#ifdef _MSC_VER
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
  #endif
  #define DLLLOCAL
#else
  #ifdef HAVE_GCC_VISIBILITY
    #define DLLEXPORT __attribute__ ((visibility("default")))
    #define DLLLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define DLLEXPORT
    #define DLLLOCAL
  #endif
#endif

#define _Q_MAKE_STRING(x) #x
#define MAKE_STRING_FROM_SYMBOL(x) _Q_MAKE_STRING(x)

class AbstractQoreNode;
class QoreListNode;
class ExceptionSink;
class QoreObject;
class AbstractPrivateData;

//! functor template for calling free() on pointers
template <typename T> struct free_ptr : std::unary_function <T*, void> {
      void operator()(T *ptr) {
	 free(ptr);
      }
};

//! functor template for deleting elements
template <typename T> struct simple_delete {
      void operator()(T *ptr) {
	 delete ptr;
      }
};

//! functor template for dereferencing elements
template <typename T> struct simple_deref {
      void operator()(T *ptr) {
	 ptr->deref();
      }
      void operator()(T *ptr, ExceptionSink *xsink) {
	 ptr->deref(xsink);
      }
};

//! for simple c-string less-than comparisons
class ltstr {
  public:
   bool operator()(const char* s1, const char* s2) const {
      return strcmp(s1, s2) < 0;
   }
};

//! for simple c-string case-insensitive less-than comparisons
class ltcstrcase {
   public:
      bool operator()(const char* s1, const char* s2) const {
	 return strcasecmp(s1, s2) < 0;
      }
};

//! for std::string case-insensitive less-than comparisons
class ltstrcase {
   public:
      bool operator()(std::string s1, std::string s2) const {
	 return strcasecmp(s1.c_str(), s2.c_str()) < 0;
      }
};

//! for char less-than comparisons
class ltchar {
   public:
      bool operator()(const char s1, const char s2) const {
	 return s1 < s2;
      }
};

//! non-thread-safe vector for storing "char *" that you want to delete
class cstr_vector_t : public std::vector<char *> {
  public:
   DLLLOCAL ~cstr_vector_t() {
      std::for_each(begin(), end(), free_ptr<char>());
   }
};

#include <set>
typedef std::set<char *, ltstr> strset_t;

typedef long long int64;

#include <stdarg.h>

class QoreMethod;
class QoreBuiltinMethod;
class QoreClass;

//! the type used for builtin function signatures
/** @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
    @return the return value of the function (can be 0)
 */
typedef AbstractQoreNode *(*q_func_t)(const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin QoreClass method signatures
/** @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
    @return the return value of the function (can be 0)
 */
typedef AbstractQoreNode *(*q_method_t)(QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin QoreClass method signatures when called with the new generic calling convention
/** @param method a constant reference to the QoreMethod being called
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
    @return the return value of the function (can be 0)
 */
typedef AbstractQoreNode *(*q_method2_t)(const QoreMethod &method, QoreObject *self, AbstractPrivateData *private_data, const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin static method signatures for static methods using the new generic calling convention
/** @param method a constant reference to the QoreMethod being called
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
    @return the return value of the function (can be 0)
 */
typedef AbstractQoreNode *(*q_static_method2_t)(const QoreMethod &method, const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin QoreClass constructor method signatures
/** @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor_t)(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin QoreClass constructor method signatures using the new generic calling convention
/** @param thisclass a constant reference to the QoreClass being constructed (in a heirarchy, could be different than the QoreClass returned from QoreObject::getClass()
    @param self the QoreObject that the function is being executed on
    @param args the list of arguments to the function (could be 0), use inline functions in params.h to access
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_constructor2_t)(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, ExceptionSink *xsink);

//! the type used for builtin QoreClass system constructor method signatures
/** System constructors are called for objects that are created automatically by the library, normally to be assigned to constants.
    System objects are treated specially by the Qore library as they are not associated with any QoreProgram object.
    Additionally, system object constructors are not allowed to raise exceptions.
    @param self the QoreObject that the function is being executed on
    @param code this argument is necessary in order to be able to provide the va_list in the following argument due to the way QoreClass::execSystemConstructor() is called.  If not required by the constuctor, this argument can be ignored.
    @param args a variable-length list of arguments to the system constructor
 */
typedef void (*q_system_constructor_t)(QoreObject *self, int code, va_list args);

//! the type used for builtin QoreClass system constructor method signatures using the new generic calling convention
/** System constructors are called for objects that are created automatically by the library, normally to be assigned to constants.
    System objects are treated specially by the Qore library as they are not associated with any QoreProgram object.
    Additionally, system object constructors are not allowed to raise exceptions.
    @param self the QoreObject that the function is being executed on
    @param code this argument is necessary in order to be able to provide the va_list in the following argument due to the way QoreClass::execSystemConstructor() is called.  If not required by the constuctor, this argument can be ignored.
    @param args a variable-length list of arguments to the system constructor
 */
typedef void (*q_system_constructor2_t)(const QoreClass &thisclass, QoreObject *self, int code, va_list args);

//! the type used for builtin QoreClass destructor signatures
/** destructors are optional, but, if present, must call AbstractPrivateData::deref() on any private data (if present)
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_destructor_t)(QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink);

//! the type used for builtin QoreClass destructor signatures with the new generic calling convention
/** destructors are optional, but, if present, must call AbstractPrivateData::deref() on any private data (if present)
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_destructor2_t)(const QoreClass &thisclass, QoreObject *self, AbstractPrivateData *private_data, ExceptionSink *xsink);

//! the type used for builtin QoreClass copy signatures
/** this function must set any private data against the new object by calling QoreObject::setPrivate() on \c self
    @param self the QoreObject that the function is being executed on (the new copy of the object)
    @param old the object being copied
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_copy_t)(QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink);

//! the type used for builtin QoreClass copy signatures with the new generic calling convention
/** this function must set any private data against the new object by calling QoreObject::setPrivate() on \c self
    @param thisclass a constant reference to the QoreClass being copied
    @param self the QoreObject that the function is being executed on (the new copy of the object)
    @param old the object being copied
    @param private_data the object's private data representing the state of the object for the current builtin class
    @param xsink Qore-language exception information should be stored here by calling ExceptionSink::raiseException()
 */
typedef void (*q_copy2_t)(const QoreClass &thisclass, QoreObject *self, QoreObject *old, AbstractPrivateData *private_data, ExceptionSink *xsink);

//! the typed used for QoreClass deleteBlocker signatures
/** 
    @param self the QoreObject that the function is being executed on
    @param private_data the object's private data representing the state of the object for the current builtin class
    @return false if the object may be deleted normally, true if the deletion should be suppressed
 */
typedef bool (*q_delete_blocker_t)(QoreObject *self, AbstractPrivateData *private_data);

DLLEXPORT long long q_atoll(const char *str);

#endif // _QORE_COMMON_H
