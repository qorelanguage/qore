/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BuiltinFunctionList.h

  Qore programming language

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

#ifndef _QORE_BUILTINFUNCTIONLIST_H

#define _QORE_BUILTINFUNCTIONLIST_H

#include <qore/common.h>
#include <qore/Restrictions.h>
#include <qore/QoreThreadLock.h>

#include <stdarg.h>

/** @file BuiltinFunctionList.h
    This header file is completely deprecated as of Qore 0.8.4.  All functions are now stored in namespaces (QoreNamespace).

    The functions defined in this header have been modified to try to add functions to the current "Qore" namespace - if no
    program is available, then the functions will be added to the staticSystemNamespace
 */

class QoreFunction;

//! the interface to the global list of all builtin functions in the library
/** The object is thread-safe; a hash or hash-map is used for lookups.
    There is only one of these, therefore we have static functions.
 */
class BuiltinFunctionList {
private:
   // not implemented
   DLLLOCAL BuiltinFunctionList(const BuiltinFunctionList&);
   DLLLOCAL BuiltinFunctionList& operator=(const BuiltinFunctionList&);
   DLLLOCAL void *operator new(size_t);

public:
   DLLLOCAL BuiltinFunctionList();

   //! adds a new builtin function variant to the "Qore" namespace
   /**
      @param name the name of the function
      @param f a pointer to the actual C++ function to be executed when the function is called
      @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none

      @deprecated use QoreNamespace::addBuiltinVariant() instead
   */
   DLLEXPORT static void add(const char *name, q_func_t f, int functional_domain = QDOM_DEFAULT);

   //! adds a new builtin function variant to the "Qore" namespace and allows for the return type and parameter types to be set
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param num_params the number of parameters specified for the function, there must be 2 arguments for each parameter, first, a const QoreTypeInfo *, giving the type information for the parameter, and second a const AbstractQoreNode * giving the default value if no argument is supplied by the caller

       @since qore 0.8

       @deprecated use QoreNamespace::addBuiltinVariant() instead
   */
   DLLEXPORT static void add2(const char *name, q_func_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a new builtin function variant to the "Qore" and allows for the return type and parameter types to be set from lists
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param typeList a list of types for each parameter; must have num_param entries
       @param defaultArgList a list of default argument values for each parameter if no argument is supplied by the caller

       @since qore 0.8

       @deprecated use QoreNamespace::addBuiltinVariant() instead
   */
   DLLEXPORT static void add3(const char *name, q_func_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, const type_vec_t &typeList = type_vec_t(), const arg_vec_t &defaultArgList = arg_vec_t());

   //! always returns 0
   /** @return 0
   */
   DLLEXPORT static int size();

   //! finds a function by its name in the current QoreProgram object; if there is none, then returns 0
   /** @return a pointer to the function found or 0 if not found or no current QoreProgram object

       @deprecated as of Qore 0.8.4
   */
   DLLEXPORT static const QoreFunction* find(const char *name);
};

//! old interface to the compeltely-removed builtin function list; now functions are stored in namespaces
/** @deprecated use QoreNamespace functions instead
 */
DLLEXPORT extern BuiltinFunctionList builtinFunctions;

#endif // _QORE_BUILTINFUNCTIONLIST_H
