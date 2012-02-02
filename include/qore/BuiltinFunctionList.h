/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  BuiltinFunctionList.h

  Qore programming language

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

#ifndef _QORE_BUILTINFUNCTIONLIST_H

#define _QORE_BUILTINFUNCTIONLIST_H

#include <qore/common.h>
#include <qore/Restrictions.h>
#include <qore/QoreThreadLock.h>

#include <stdarg.h>

/** @file BuiltinFunctionList.h
    defines the BuiltinFunctionList class for the Qore library
 */

DLLLOCAL void init_builtin_functions();

class BuiltinFunction;

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
   DLLLOCAL ~BuiltinFunctionList();

   //! adds a new builtin function to the list
   /**
      @param name the name of the function
      @param f a pointer to the actual C++ function to be executed when the function is called
      @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
   */
   DLLEXPORT static void add(const char *name, q_func_t f, int functional_domain = QDOM_DEFAULT);

   //! adds a new builtin function to the list and allows for the return type and parameter types to be set
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param num_params the number of parameters specified for the function, there must be 2 arguments for each parameter, first, a const QoreTypeInfo *, giving the type information for the parameter, and second a const AbstractQoreNode * giving the default value if no argument is supplied by the caller

       @since qore 0.8
   */
   DLLEXPORT static void add2(const char *name, q_func_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a new builtin function to the list and allows for the return type and parameter types to be set
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param num_params the number of parameters specified for the function, there must be 2 arguments for each parameter, first, a const QoreTypeInfo *, giving the type information for the parameter, and second a const AbstractQoreNode * giving the default value if no argument is supplied by the caller

       @since qore 0.8.4
   */
   DLLEXPORT static void add2(const char *name, q_func_int64_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a new builtin function to the list and allows for the return type and parameter types to be set
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param num_params the number of parameters specified for the function, there must be 2 arguments for each parameter, first, a const QoreTypeInfo *, giving the type information for the parameter, and second a const AbstractQoreNode * giving the default value if no argument is supplied by the caller

       @since qore 0.8.4
   */
   DLLEXPORT static void add2(const char *name, q_func_double_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a new builtin function to the list and allows for the return type and parameter types to be set
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param num_params the number of parameters specified for the function, there must be 2 arguments for each parameter, first, a const QoreTypeInfo *, giving the type information for the parameter, and second a const AbstractQoreNode * giving the default value if no argument is supplied by the caller

       @since qore 0.8.4
   */
   DLLEXPORT static void add2(const char *name, q_func_bool_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a new builtin function to the list and allows for the return type and parameter types to be set from lists
   /** 
       @param name the name of the function
       @param f a pointer to the actual C++ function to be executed when the function is called
       @param code_flags flags for the function being added
       @param functional_domain a capability mask of the function so that access to the function can be restricted if necessary; use QDOM_DEFAULT for none
       @param returnTypeInfo the type information for the return value of this function
       @param typeList a list of types for each parameter; must have num_param entries
       @param defaultArgList a list of default argument values for each parameter if no argument is supplied by the caller

       @since qore 0.8
   */
   DLLEXPORT static void add3(const char *name, q_func_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo *returnTypeInfo = 0, const type_vec_t &typeList = type_vec_t(), const arg_vec_t &defaultArgList = arg_vec_t());

   //! returns the number of functions in the hash
   /**
      @return the number of functions in the hash
   */
   DLLEXPORT static int size();

   //! finds a function by its name
   /**
      @return a pointer to the function found
   */
   DLLEXPORT static const BuiltinFunction *find(const char *name);

   // internal functions
   DLLLOCAL void clear();
   DLLLOCAL static void init(QoreNamespace& ns);
};

//! interface to the global list of builtin functions in the qore library
DLLEXPORT extern BuiltinFunctionList builtinFunctions;

#endif // _QORE_BUILTINFUNCTIONLIST_H
