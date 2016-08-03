/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClass.h

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

#ifndef _QORE_QORECLASS_H

#define _QORE_QORECLASS_H

#include <stdarg.h>
#include <string>

// all qore class IDs
DLLEXPORT extern qore_classid_t CID_AUTOGATE;
DLLEXPORT extern qore_classid_t CID_AUTOLOCK;
DLLEXPORT extern qore_classid_t CID_AUTOREADLOCK;
DLLEXPORT extern qore_classid_t CID_AUTOWRITELOCK;
DLLEXPORT extern qore_classid_t CID_CONDITION;
DLLEXPORT extern qore_classid_t CID_COUNTER;
DLLEXPORT extern qore_classid_t CID_DATASOURCE;
DLLEXPORT extern qore_classid_t CID_DATASOURCEPOOL;
DLLEXPORT extern qore_classid_t CID_FILE;
DLLEXPORT extern qore_classid_t CID_FTPCLIENT;
DLLEXPORT extern qore_classid_t CID_GATE;
DLLEXPORT extern qore_classid_t CID_GETOPT;
DLLEXPORT extern qore_classid_t CID_HTTPCLIENT;
DLLEXPORT extern qore_classid_t CID_MUTEX;
DLLEXPORT extern qore_classid_t CID_PROGRAM;
DLLEXPORT extern qore_classid_t CID_QUEUE;
DLLEXPORT extern qore_classid_t CID_RWLOCK;
DLLEXPORT extern qore_classid_t CID_SSLCERTIFICATE;
DLLEXPORT extern qore_classid_t CID_SSLPRIVATEKEY;
DLLEXPORT extern qore_classid_t CID_SEQUENCE;
DLLEXPORT extern qore_classid_t CID_SOCKET;
DLLEXPORT extern qore_classid_t CID_TERMIOS;

DLLEXPORT extern QoreClass* QC_QUEUE;
DLLEXPORT extern QoreClass* QC_HTTPCLIENT;

class BCList;
class BCSMList;
class BCAList;
class QoreObject;
class QoreClass;
class BCEAList;
class ParamList;
class QoreMemberInfo;
class BuiltinMethod;
class AbstractQoreFunctionVariant;
class AbstractFunctionSignature;
class UserMethod;
class BCANode;
class qore_method_private;
class MethodFunctionBase;
class QoreExternalMethodVariant;
class QoreExternalStaticMethodVariant;
class NamedScope;
class ConstantList;
class MethodVariantBase;

//! a method in a QoreClass
/** methods can be implemented in the Qore language (user methods) or in C++ (builtin methods)
    @see QoreClass
 */
class QoreMethod {
   friend class StaticMethodCallNode;
   friend class QoreObject;
   friend class qore_class_private;
   friend class qore_method_private;
   friend class BCList;

private:
   //! private implementation of the method
   class qore_method_private* priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreMethod(const QoreMethod&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreMethod& operator=(const QoreMethod&);

public:
   //! DEPRECATED: always returns false, do not use
   /** this method no longer returns useful information due to method overloading
       @return DEPRECATED: always returns false, do not use
   */
   DLLEXPORT bool isSynchronized() const;

   //! DEPRECATED: always returns false, do not use
   /** this method no longer returns useful information due to method overloading
       @return always returns false
   */
   DLLEXPORT bool newCallingConvention() const;

   //! returns true if all variants of the method are user variants
   /** @return true if all variants of the method are user variants
    */
   DLLEXPORT bool isUser() const;

   //! returns true if all variants of the method are builtin variants
   /** @return true if all variants of the method are builtin variants
    */
   DLLEXPORT bool isBuiltin() const;

   //! returns true if all overloaded variants of a methods are private, false if at least one variant is public
   /** @return true if all overloaded variants of a methods are private, false if at least one variant is public
    */
   DLLEXPORT bool isPrivate() const;

   //! returns true if the method is static
   /**
      @return true if the method is static
   */
   DLLEXPORT bool isStatic() const;

   //! returns the method's name
   /**
      @return the method's name
   */
   DLLEXPORT const char* getName() const;

   //! returns the method's name
   /**
      @return the method's name
   */
   DLLEXPORT const std::string& getNameStr() const;

   //! returns a pointer to the parent class
   DLLEXPORT const QoreClass* getClass() const;

   //! returns the class name for the method
   DLLEXPORT const char* getClassName() const;

   //! returns true if a variant with the given parameter signature already exists in the method
   DLLEXPORT bool existsVariant(const type_vec_t& paramTypeInfo) const;

   /* returns the return type information for the method if it is available and if
      there is only one return type (there can be more return types if the method is
      overloaded)
   */
   DLLEXPORT const QoreTypeInfo* getUniqueReturnTypeInfo() const;

   //! evaluates the method and returns the value, does not reference the object for the call
   /** @note this method should only be used when the caller can guarantee that the object will not go out of scope during the call

       @since %Qore 0.8.12
    */
   DLLEXPORT QoreValue execManaged(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL QoreMethod(const QoreClass* p_class, MethodFunctionBase* n_func, bool n_static = false);

   DLLLOCAL ~QoreMethod();
   DLLLOCAL bool inMethod(const QoreObject* self) const;
   DLLLOCAL QoreMethod* copy(const QoreClass* p_class) const;
   DLLLOCAL void assign_class(const QoreClass* p_class);
   DLLLOCAL MethodFunctionBase* getFunction() const;

   //! returns true if all overloaded variants of a methods are private, false if at least one variant is public (including pending uncommitted variants)
   /** @return true if all overloaded variants of a methods are private, false if at least one variant is public (including pending uncommitted variants)
    */
   DLLLOCAL bool parseIsPrivate() const;
};

//! defines a Qore-language class
/** Qore's classes can be either implemented by Qore language code (user classes)
    or in C++ (builtin classes), or both, as in the case of a builtin class that
    also has user methods.
*/
class QoreClass {
   friend class BCList;
   friend class BCNode;
   friend class BCSMList;
   friend class qore_object_private;
   friend class qore_class_private;
   friend class QoreObject;
   friend class BCANode;
   friend class qore_method_private;
   friend class QoreMethodIterator;
   friend class QoreStaticMethodIterator;
   friend class ConstructorMethodFunction;

private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreClass& operator=(const QoreClass&);

   //! private implementation of the class
   class qore_class_private* priv;

   // private constructor only called when the class is copied
   DLLLOCAL QoreClass(qore_classid_t id, const char* nme);

   DLLLOCAL void insertMethod(QoreMethod* o);
   DLLLOCAL void insertStaticMethod(QoreMethod* o);
   DLLLOCAL QoreValue evalMethodGate(QoreObject* self, const char* nme, const QoreListNode* args, ExceptionSink* xsink) const;
   DLLLOCAL const QoreMethod* parseResolveSelfMethodIntern(const char* nme);

   //! evaluates a method on an object and returns the result
   /** if the method name is not valid or is private (and the call is made outside the object)
       then an exception will be raised and 0 will be returned.
       This function must only be called from QoreObject!
       @param self the object to execute the method on
       @param method_name the name of the method to execute
       @param args the arguments for the method
       @param xsink Qore-language exception information is added here
       @return the value returned by the method, can be 0
   */
   DLLLOCAL QoreValue evalMethod(QoreObject* self, const char* method_name, const QoreListNode* args, ExceptionSink* xsink) const;

   // This function must only be called from QoreObject
   DLLLOCAL QoreValue evalMemberGate(QoreObject* self, const QoreString *nme, ExceptionSink* xsink) const;
   // This function must only be called from QoreObject
   DLLLOCAL void execMemberNotification(QoreObject* self, const char* mem, ExceptionSink* xsink) const;
   // This function must only be called from QoreObject and BCList
   DLLLOCAL bool execDeleteBlocker(QoreObject* self, ExceptionSink* xsink) const;
   // This function must only be called from QoreObject
   DLLLOCAL void execDestructor(QoreObject* self, ExceptionSink* xsink) const;
   // This function is only called from BCList
   DLLEXPORT const QoreClass* getClassIntern(qore_classid_t cid, bool& priv) const;

public:
   //! creates the QoreClass object and assigns the name and the functional domain
   /** @note class names and subnamespaces names must be unique in a namespace; i.e. no class may have the same name as a subnamespace within a namespace and vice-versa
       @param n_name the name of the class
       @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
       @see QoreProgram
   */
   DLLEXPORT QoreClass(const char* n_name, int n_domain = QDOM_DEFAULT);

   //! creates the QoreClass object and assigns the name, the functional domain, and a custom QoreTypeInfo object created with AbstractQoreClassTypeInfoHelper
   /** @note class names and subnamespaces names must be unique in a namespace; i.e. no class may have the same name as a subnamespace within a namespace and vice-versa
       @param n_name the name of the class
       @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
       @param n_typeInfo the custom QoreTypeInfo object created with AbstractQoreClassTypeInfoHelper
       @see QoreProgram
       @see AbstractQoreClassTypeInfoHelper
   */
   DLLEXPORT QoreClass(const char* n_name, int64 n_domain, const QoreTypeInfo* n_typeInfo);

   //! copy constructor
   /** should be only called under the appropriate lock (ex: program parse lock while parsing)
    */
   DLLEXPORT QoreClass(const QoreClass &old);

   //! deletes the object and frees all memory
   DLLEXPORT ~QoreClass();

   //! adds a builtin method to a class
   /** @par Example:
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static QoreValue AL_lock(QoreObject* self, QoreAutoLock* m, const QoreListNode* args, q_rt_flags_t rtflag, ExceptionSink* xsink);
       ...
       // and then casted to (q_method_t) in the addMethod call:
       QC_AutoLock->addMethod("lock", (q_method_n_t)AL_lock, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
       @endcode

       in debuggging mode, the call will abort if the name of the method is
       "constructor", "destructor", or "copy", or if the method already exists
       in the class.
       To set the constructor method, call QoreClass::setConstructor().
       To set the destructor method, call QoreClass::setDestructor().
       To set the copy method, call QoreClass::setCopy().

       @param n_name the name of the method, must be unique in the class
       @param meth the method to be added
       @param priv if true then the variant will be added as a private variant

       @see Qoreclass::addStaticMethod()
       @see QoreClass::setConstructor()
       @see QoreClass::setDestructor()
       @see QoreClass::setCopy()
   */
   DLLEXPORT void addMethod(const char* n_name, q_method_n_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   DLLEXPORT void addStaticMethod(const char* n_name, q_func_n_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method to a class
   /** in debuggging mode, the call will abort if the name of the method is
       "constructor", "destructor", or "copy", or if the method already exists
       in the class.
       To set the constructor method, call QoreClass::setConstructor().
       To set the destructor method, call QoreClass::setDestructor().
       To set the copy method, call QoreClass::setCopy().
       @param n_name the name of the method, must be unique in the class
       @param meth the method to be added
       @param priv if true then the variant will be added as a private variant
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static AbstractQoreNode* AL_lock(QoreObject* self, QoreAutoLock *m, const QoreListNode* params, ExceptionSink* xsink);
       ...
       // and then casted to (q_method_t) in the addMethod call:
       QC_AutoLock->addMethod("lock", (q_method_t)AL_lock);
       @endcode
       @see Qoreclass::addStaticMethod()
       @see QoreClass::setConstructor()
       @see QoreClass::setDestructor()
       @see QoreClass::setCopy()

       @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
   */
   DLLEXPORT void addMethod(const char* n_name, q_method_t meth, bool priv = false);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended(const char* n_name, q_method_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended(const char* n_name, q_method_int64_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info from lists
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtendedList(const char* n_name, q_method_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin method with the new generic calling convention
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethod2(const char* n_name, q_method2_t meth, bool priv = false);

   //! adds a builtin method with the new calling convention and extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended2(const char* n_name, q_method2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with the new calling convention and extended information; additional functional domain info, return and parameter type info from lists
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtendedList2(const char* n_name, q_method2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin method with the even newer calling convention and extended information; additional functional domain info, return and parameter type info from lists
   /** @param ptr a pointer to user-defined data that will be passed to the method function when it is called
       @param n_name the name of the method to add the variant to
       @param meth the function pointer of the code to call
       @param n_flags flags for the code being added
       @param priv if true then the variant will be added as a private variant
       @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
       @param returnTypeInfo the type of value returned by the variant
       @param n_typeList a list of type information for parameters to the variant
       @param defaultArgList a list of default arguments to each parameter

       @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtendedList3(const void* ptr, const char* n_name, q_method3_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended3(const char* n_name, q_method_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended3(const char* n_name, q_method_int64_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended3(const char* n_name, q_method_bool_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addMethod(const char*, q_method_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addMethodExtended3(const char* n_name, q_method_double_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds an unimplemented abstract method variant to the class with extended information; with return and parameter type info
   DLLEXPORT void addAbstractMethodVariantExtended3(const char* n_name, bool priv = false, int64 n_flags = QC_NO_FLAGS, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method to a class
   /**
      @param n_name the name of the method, must be unique in the class
      @param meth the method to be added
      @param priv if true then the method will be added as a private method

      @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethod(const char* n_name, q_func_t meth, bool priv = false);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended(const char* n_name, q_func_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info from lists
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtendedList(const char* n_name, q_func_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin static method with the new generic calling convention
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethod2(const char* n_name, q_static_method2_t meth, bool priv = false);

   //! adds a builtin static method with the new generic calling convention with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended2(const char* n_name, q_static_method2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with the new generic calling convention with extended information; additional functional domain info, return and parameter type info from lists
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtendedList2(const char* n_name, q_static_method2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin static method with the even newer generic calling convention with extended information; additional functional domain info, return and parameter type info from lists
   /** @param ptr a pointer to user-defined data that will be passed to the method function when it is called
       @param n_name the name of the method to add the variant to
       @param meth the function pointer of the code to call
       @param priv if true then the variant will be added as a private variant
       @param n_flags flags for the code being added
       @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
       @param returnTypeInfo the type of value returned by the variant
       @param n_typeList a list of type information for parameters to the variant
       @param defaultArgList a list of default arguments to each parameter

       @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtendedList3(const void* ptr, const char* n_name, q_static_method3_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended3(const char* n_name, q_func_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended3(const char* n_name, q_func_int64_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended3(const char* n_name, q_func_bool_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! adds a builtin static method with extended information; additional functional domain info, return and parameter type info
   /** @deprecated for addStaticMethod(const char*, q_func_n_t, bool, int64, int64, const QoreTypeInfo*, unsigned, ...);
    */
   DLLEXPORT void addStaticMethodExtended3(const char* n_name, q_func_double_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   //! sets the builtin destructor method for the class
   /** you only need to implement destructor methods if the destructor should destroy the object
       before the reference count reaches zero.
       @param m the destructor method to run
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_destructor(QoreObject* self, QoreAutoLock *al, ExceptionSink* xsink);
       ...
       // and then casted to (q_destructor_t) in the addMethod call:
       QC_AutoLock->setDestructor((q_destructor_t)AL_destructor);
       @endcode
   */
   DLLEXPORT void setDestructor(q_destructor_t m);

   //! sets the builtin destructor method for the class with the new generic calling convention
   /** you only need to implement destructor methods if the destructor should destroy the object
       before the reference count reaches zero.
       @param m the destructor method to run
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_destructor(const QoreClass &thisclass, QoreObject* self, QoreAutoLock *al, ExceptionSink* xsink);
       ...
       // and then casted to (q_destructor_t) in the addMethod call:
       QC_AutoLock->setDestructor2((q_destructor_t)AL_destructor);
       @endcode
   */
   DLLEXPORT void setDestructor2(q_destructor2_t m);

   //! sets the builtin destructor method for the class with the new generic calling convention
   /** you only need to implement destructor methods if the destructor should destroy the object
       before the reference count reaches zero.
       @param ptr user-defined data that will be passed to the destructor when it's called
       @param m the destructor method to run
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_destructor(const QoreClass &thisclass, QoreObject* self, QoreAutoLock *al, ExceptionSink* xsink);
       ...
       // and then casted to (q_destructor_t) in the addMethod call:
       QC_AutoLock->setDestructor2((q_destructor_t)AL_destructor);
       @endcode
   */
   DLLEXPORT void setDestructor3(const void* ptr, q_destructor3_t m);

   //! sets the constructor method with extended information; can set a private constructor, set additional functional domain info, and parameter type info (or adds an overloaded variant)
   DLLEXPORT void addConstructor(q_constructor_n_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, unsigned num_params = 0, ...);

   //! sets the builtin constructor method for the class (or adds an overloaded variant)
   /**
      @param m the constructor method

      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructor(q_constructor_t m);

   //! sets the constructor method with extended information; can set a private constructor, set additional functional domain info, and parameter type info (or adds an overloaded variant)
   /*
      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructorExtended(q_constructor_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, unsigned num_params = 0, ...);

   //! sets the constructor method with extended information; can set a private constructor, set additional functional domain info, and parameter type info from lists (or adds an overloaded variant)
   /*
      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructorExtendedList(q_constructor_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! sets the builtin constructor method for the class using the new calling convention (or adds an overloaded variant)
   /**
      @param m the constructor method

      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructor2(q_constructor2_t m);

   //! sets the constructor method using the new calling convention with extended information; can set a private constructor, set additional functional domain info, and parameter type info (or adds an overloaded variant)
   /*
      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructorExtended2(q_constructor2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, unsigned num_params = 0, ...);

   //! sets the constructor method using the new calling convention with extended information; can set a private constructor, set additional functional domain info, and parameter type info from lists (or adds an overloaded variant)
   /*
      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructorExtendedList2(q_constructor2_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! sets the constructor method using the new calling convention with extended information; can set a private constructor, set additional functional domain info, and parameter type info from lists (or adds an overloaded variant)
   /** @param ptr a pointer to user-defined data that will be passed to the method function when it is called
       @param meth the function pointer of the code to call
       @param priv if true then the variant will be added as a private variant
       @param n_flags flags for the code being added
       @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
       @param n_typeList a list of type information for parameters to the variant
       @param defaultArgList a list of default arguments to each parameter

      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
    */
   DLLEXPORT void setConstructorExtendedList3(const void* ptr, q_constructor3_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, const type_vec_t& n_typeList = type_vec_t(), const arg_vec_t& defaultArgList = arg_vec_t());

   //! sets the constructor method with extended information; can set a private constructor, set additional functional domain info, and parameter type info (or adds an overloaded variant)
   /*
      @deprecated use addConstructor(q_constructor_n_t, bool priv, int64, int64, unsigned, ...) instead
   */
   DLLEXPORT void setConstructorExtended3(q_constructor_t meth, bool priv = false, int64 n_flags = QC_NO_FLAGS, int64 n_domain = QDOM_DEFAULT, unsigned num_params = 0, ...);

   //! sets the builtin constructor for system objects (ex: used as constant values)
   /** @note system constructors in a class hierarchy must call the base class constructors manually
       @param m the constructor method
   */
   DLLEXPORT void setSystemConstructor(q_system_constructor_t m);

   //! sets the builtin constructor for system objects (ex: used as constant values) using the new calling convention
   /** @note system constructors in a class hierarchy must call the base class constructors manually
       @param m the constructor method
   */
   DLLEXPORT void setSystemConstructor2(q_system_constructor2_t m);

   //! sets the builtin copy method for the class
   /** copy methods should either call QoreObject::setPrivate() or call xsink->raiseException()
       (but should not do both)
       @param m the copy method to set
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_copy(QoreObject* self, QoreObject* old, class QoreAutoLock *m, ExceptionSink* xsink)
       ...
       // and then casted to (q_copy_t) in the addMethod call:
       QC_AutoLock->setCopy((q_copy_t)AL_copy);
       @endcode
   */
   DLLEXPORT void setCopy(q_copy_t m);

   //! sets the builtin copy method for the class using the new generic calling convention
   /** copy methods should either call QoreObject::setPrivate() or call xsink->raiseException()
       (but should not do both)
       @param m the copy method to set
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_copy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, QoreAutoLock *m, ExceptionSink* xsink)
       ...
       // and then casted to (q_copy2_t) in the addMethod call:
       QC_AutoLock->setCopy((q_copy2_t)AL_copy);
       @endcode
   */
   DLLEXPORT void setCopy2(q_copy2_t m);

   //! sets the builtin copy method for the class using the new generic calling convention
   /** copy methods should either call QoreObject::setPrivate() or call xsink->raiseException()
       (but should not do both)
       @param ptr user-defined data that will be passed to the destructor when it's called
       @param m the copy method to set
       @code
       // the actual function can be declared with the class to be expected as the private data as follows:
       static void AL_copy(const QoreClass &thisclass, QoreObject* self, QoreObject* old, QoreAutoLock *m, ExceptionSink* xsink)
       ...
       // and then casted to (q_copy2_t) in the addMethod call:
       QC_AutoLock->setCopy((q_copy2_t)AL_copy);
       @endcode
   */
   DLLEXPORT void setCopy3(const void* ptr, q_copy3_t m);

   //! sets the deleteBlocker method for the class
   /** this method will be run when the object is deleted; it should be set only for classes where
       the objects' lifecycles are or may be managed externally.  This function must be called before
       this class is added as a parent class to any other class; if it is called classes have added
       this class as a parent class, then the child classes will not have their delete blocker flag
       set.
       @param m the deleteBlocker method to set
       @note delete blocker methods are called with the object's status lock held, therefore be very careful what you call from within the deleteBlocker function
   */
   DLLEXPORT void setDeleteBlocker(q_delete_blocker_t m);

   //! returns true if the member is private
   /**
       @param str the member name to check
       @return true if the member is private
   */
   DLLEXPORT bool isPrivateMember(const char* str) const;

   //! returns true if the member is private or public
   /**
       @param str the member name to check
       @param priv true if the member is private, false if public
       @return true if the member is private
   */
   DLLEXPORT bool isPublicOrPrivateMember(const char* str, bool& priv) const;

   //! creates a new object and executes the constructor on it and returns the new object
   /** if a Qore-language exception occurs, 0 is returned.
       @param args the arguments for the method
       @param xsink Qore-language exception information is added here

       @return the object created
   */
   DLLEXPORT QoreObject* execConstructor(const QoreValueList* args, ExceptionSink* xsink) const;

   //! creates a new object and executes the constructor on it and returns the new object
   /** if a Qore-language exception occurs, 0 is returned.
       @param args the arguments for the method
       @param xsink Qore-language exception information is added here

       @return the object created

       @deprecated use execConstructor(const QoreValueList*, ExceptionSink*) instead
   */
   DLLEXPORT QoreObject* execConstructor(const QoreListNode* args, ExceptionSink* xsink) const;

   //! creates a new "system" object for use as the value of a constant, executes the system constructor on it and returns the new object
   /** if a Qore-language exception occurs, 0 is returned
       @param code an optional code for the constructor; this parameter is here because passing a variable number of arguments requires at least one fixed parameter before the (possibly empty) list
       @return the object created
   */
   DLLEXPORT QoreObject* execSystemConstructor(int code = 0, ...) const;

   //! executes a class' "copy" method on an object and returns the new object (or 0 in the case of an exception)
   /** @param old the original object to copy
       @param xsink Qore-language exception information is added here
       @return the object created
   */
   DLLEXPORT QoreObject* execCopy(QoreObject* old, ExceptionSink* xsink) const;

   //! looks for a non-static method in the current class without searching base classes
   /** not thread-safe with parsing operations; do not call this function while new code
       is being parsed into the class
       @param name the name of the method
       @returns a pointer to the method found, or 0 if no such method exists in the class
   */
   DLLEXPORT const QoreMethod* findLocalMethod(const char* name) const;

   //! looks for a static method in the current class without searching base classes
   /** not thread-safe with parsing operations; do not call this function while new code
       is being parsed into the class
       @param name the name of the static method
       @returns a pointer to the method found, or 0 if no such method exists in the class
   */
   DLLEXPORT const QoreMethod* findLocalStaticMethod(const char* name) const;

   //! returns a list strings of all non-static methods in the class, the caller owns the reference count returned
   /** always returns a list; if there are no non-static methods then an empty list is returned
       @return a list strings of all non-static methods in the class, the caller owns the reference count returned
   */
   DLLEXPORT QoreListNode* getMethodList() const;

   //! returns a list strings of all static methods in the class, the caller owns the reference count returned
   /** always returns a list; if there are no static methods then an empty list is returned
       @return a list strings of all static methods in the class, the caller owns the reference count returned
   */
   DLLEXPORT QoreListNode* getStaticMethodList() const;

   //! returns a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
   /** if the class ID is equal to the current class or is a base class
       of the current class, the appropriate QoreClass pointer will be
       returned.  Do not delete or change the QoreClass* returned if
       non-null.
       FIXME: should return const QoreClass*, fix in next update
       @param cid the class ID of the QoreClass to find
       @return a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
   */
   DLLEXPORT QoreClass* getClass(qore_classid_t cid) const;

   //! returns a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy and sets a flag indicating if it's privately inherited or not
   /** if the class ID is equal to the current class or is a base class
       of the current class, the appropriate QoreClass pointer will be
       returned.
       @param cid the class ID of the QoreClass to find
       @param priv a flag indicating if the class is privately inherited or not
       @return a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
   */
   DLLEXPORT const QoreClass* getClass(qore_classid_t cid, bool& priv) const;

   //! returns a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy and sets a flag indicating if it's privately inherited or not
   /** if the class ID is equal to the current class or is a base class
       of the current class, the appropriate QoreClass pointer will be
       returned.
       @param qc the class to check the hierarchy for
       @param priv a flag indicating if the class is privately inherited or not
       @return a pointer to the QoreClass object corresponding to the class passed if it exists in the class hierarchy; in the case that the passed class if from a different QoreProgram object, the value returned could be a different pointer to the qc parameter passed
   */
   DLLEXPORT const QoreClass* getClass(const QoreClass& qc, bool& priv) const;

   //! returns the number of non-static methods in this class (user and builtin)
   DLLEXPORT int numMethods() const;

   //! returns the number of static methods in this class (user and builtin)
   DLLEXPORT int numStaticMethods() const;

   //! returns the number of non-static user methods in this class
   DLLEXPORT int numUserMethods() const;

   //! returns the number of static user methods in this class
   DLLEXPORT int numStaticUserMethods() const;

   //! returns true if the class implements a copy method
   DLLEXPORT bool hasCopy() const;

   //! returns the class ID of this class
   DLLEXPORT qore_classid_t getID() const;

   //! returns true if the class is a builtin class
   DLLEXPORT bool isSystem() const;

   //! returns true if the class implements a "memberGate" method
   DLLEXPORT bool hasMemberGate() const;

   //! returns true if the class implements a "methodGate" method
   DLLEXPORT bool hasMethodGate() const;

   //! returns true if the class implements a "memberNotification" method
   DLLEXPORT bool hasMemberNotification() const;

   //! returns the functional domain of the class
   /** DEPRECATED: use getDomain64() instead
    */
   DLLEXPORT int getDomain() const;

   //! returns the functional domain of the class
   /** added in qore 0.8.0
    */
   DLLEXPORT int64 getDomain64() const;

   //! returns the class name
   DLLEXPORT const char* getName() const;

   //! finds a non-static method in the class hierarchy
   // used at run-time
   DLLEXPORT const QoreMethod* findMethod(const char* nme) const;

   //! finds a static method in the class hierarchy
   // used at run-time
   DLLEXPORT const QoreMethod* findStaticMethod(const char* nme) const;

   //! finds a non-static method in the class hierarchy and sets the priv flag if it's a private method or not
   DLLEXPORT const QoreMethod* findMethod(const char* nme, bool& priv) const;

   //! finds a static method in the class hierarchy and sets the priv flag if it's a private method or not
   DLLEXPORT const QoreMethod* findStaticMethod(const char* nme, bool& priv) const;

   //! make a builtin class a child of another builtin class
   /** the xargs argument must not be used; before qore supported function overloading, base class arguments could be given here
       @param qc the base class to add
       @param xargs DEPRECATED must be 0; do not use
   */
   DLLEXPORT void addBuiltinBaseClass(QoreClass* qc, QoreListNode* xargs = 0);

   //! make a builtin class a child of another builtin class and ensures that the given class' private data will be used in all class methods
   /** In the case this function is used, this objects of class cannot have
       private data saved against the class ID.
       The xargs argument must not be used; before qore supported function overloading, base class arguments could be given here
       @param qc the base class to add
       @param xargs DEPRECATED must be 0; do not use
   */
   DLLEXPORT void addDefaultBuiltinBaseClass(QoreClass* qc, QoreListNode* xargs = 0);

   //! sets "virtual" base class for a class, meaning that the base class data is appropriate for use in the subclass builtin methods
   /** this method adds a base class placeholder for a subclass - where the subclass' private data
       object is actually a subclass of the parent class and all methods are virtual, so the
       base class' constructor, destructor, and copy constructor will never be run and the base
       class methods will be passed a pointer to the subclass' data
       @param qc the base class to add
   */
   DLLEXPORT void addBuiltinVirtualBaseClass(QoreClass* qc);

   //! call this function if your builtin class requires *all* methods (except the constructor) to be run in an RMutex lock
   /** use this for classes that require exclusive access to the private data in all functions
    */
   DLLEXPORT void setSynchronousClass();

   //! returns a const pointer to the QoreMethod object of the constuctor method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the constuctor method, if any is set
   */
   DLLEXPORT const QoreMethod* getConstructor() const;

   //! returns a const pointer to the QoreMethod object of the constuctor method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the constuctor method, if any is set
   */
   DLLEXPORT const QoreMethod* getSystemConstructor() const;

   //! returns a const pointer to the QoreMethod object of the constructor method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the constructor method, if any is set
   */
   DLLEXPORT const QoreMethod* getDestructor() const;

   //! returns a const pointer to the QoreMethod object of the destructor method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the destructor method, if any is set
   */
   DLLEXPORT const QoreMethod* getCopyMethod() const;

   //! returns a const pointer to the QoreMethod object of the memberGate method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the memberGate method, if any is set
   */
   DLLEXPORT const QoreMethod* getMemberGateMethod() const;

   //! returns a const pointer to the QoreMethod object of the methodGate method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the methodGate method, if any is set
   */
   DLLEXPORT const QoreMethod* getMethodGate() const;

   //! returns a const pointer to the QoreMethod object of the memberNotification method, if any is set
   /** executes in constant time
       @return a const pointer to the QoreMethod object of the memberNotification method, if any is set
   */
   DLLEXPORT const QoreMethod* getMemberNotificationMethod() const;

   //! returns the type information structure for this class
   DLLEXPORT const QoreTypeInfo* getTypeInfo() const;

   //! returns the "or nothing" type information structure for this class
   DLLEXPORT const QoreTypeInfo* getOrNothingTypeInfo() const;

   //! adds a public member
   DLLEXPORT void addPublicMember(const char* mem, const QoreTypeInfo* n_typeInfo, AbstractQoreNode* initial_value = 0);

   //! adds a private member
   DLLEXPORT void addPrivateMember(const char* mem, const QoreTypeInfo* n_typeInfo, AbstractQoreNode* initial_value = 0);

   //! sets a pointer to user-specific data in the class
   DLLEXPORT void setUserData(const void* ptr);

   //! retrieves the user-specific data pointer
   DLLEXPORT const void* getUserData() const;

   //! rechecks for inherited methods in base classes when adding builtin classes
   DLLEXPORT void recheckBuiltinMethodHierarchy();

   //! returns the user variant for the given non-static method and argument types
   /** argTypeList must have a non-null value for each type in the list
    */
   DLLEXPORT const QoreExternalMethodVariant* findUserMethodVariant(const char* name, const QoreMethod*& method, const type_vec_t& argTypeList) const;

   //! adds a class constant to the class
   /** @param name the name of the constant; cannot be the same as the name of a static variable
       @param value the value of the constant
       @param priv true if private, false if public
       @param typeInfo only need to set if the value assigned to the constant is not an internally-defined type and therefore the type info cannot be automatically determined, otherwise this parameter may be passed as NULL, in which case the type info will be automatically determined
    */
   DLLEXPORT void addBuiltinConstant(const char* name, AbstractQoreNode* value, bool priv = false, const QoreTypeInfo* typeInfo = 0);

   //! adds a static variable to the class
   /** @param name the name of the static variable; cannot be the same as the name of a class constant
       @param value the initial of the static variable
       @param priv true if private, false if public
       @param typeInfo only need to set if the initial value assigned to the static variable (as given by the 'value' parameter) is not an internally-defined type and therefore the type info cannot be automatically determined, otherwise this parameter may be passed as NULL, in which case the type info will be automatically determined
    */
   DLLEXPORT void addBuiltinStaticVar(const char* name, AbstractQoreNode* value, bool priv = false, const QoreTypeInfo* typeInfo = 0);

   //! returns true if the class has at least one abstract method variant
   /** @return true if the class has at least one abstract method variant
    */
   DLLEXPORT bool hasAbstract() const;

   //! constructor not exported in library's API
   DLLLOCAL QoreClass();

   // used when parsing, finds committed non-static methods within the entire class hierarchy (local class plus base classes)
   DLLLOCAL const QoreMethod* parseFindCommittedMethod(const char* nme);

   // adds public constants to pending list
   DLLLOCAL void parseAssimilatePublicConstants(ConstantList& cmap);
   // adds private constants to pending list
   DLLLOCAL void parseAssimilatePrivateConstants(ConstantList& cmap);
   // adds a single public constant to pending list
   DLLLOCAL void parseAddPublicConstant(const std::string& name, AbstractQoreNode* val);

   // returns 0 for success, -1 for error
   DLLLOCAL int parseAddBaseClassArgumentList(BCAList* bcal);
   // only called when parsing, sets the name of the class
   DLLLOCAL void setName(const char* n);

   DLLLOCAL qore_classid_t getIDForMethod() const;
   // get base class list to add virtual class indexes for private data
   DLLLOCAL BCSMList* getBCSMList() const;
   // returns true if the class has a delete_blocker function (somewhere in the hierarchy)
   DLLLOCAL bool has_delete_blocker() const;
   DLLLOCAL bool parseCheckHierarchy(const QoreClass* cls) const;
   DLLLOCAL bool parseHasPublicMembersInHierarchy() const;
   DLLLOCAL bool runtimeGetMemberInfo(const char* mem, const QoreTypeInfo*& memberTypeInfo, bool& priv) const;
   DLLLOCAL bool runtimeHasPublicMembersInHierarchy() const;
   // returns true if the class has one or more parent classes
   DLLLOCAL bool hasParentClass() const;
   DLLLOCAL bool hasPrivateCopyMethod() const;
   // returns the status including the pending variant (if any)
   DLLLOCAL bool parseHasPrivateCopyMethod() const;
   DLLLOCAL const QoreMethod* parseGetConstructor() const;
   // returns true if the class implements a "methodGate" method, also in pending uncommitted methods
   DLLLOCAL bool parseHasMethodGate() const;
   // called when there is an empty public member declaration or a "no_public" declaration
   DLLLOCAL void parseSetEmptyPublicMemberDeclaration();
   // unsets the public member flag for builtin classes
   DLLLOCAL void unsetPublicMemberFlag();
};

//! To be used to iterate through a class' normal (non-static) methods
class QoreMethodIterator {
private:
   void* priv;

public:
   DLLEXPORT QoreMethodIterator(const QoreClass* qc);
   DLLEXPORT ~QoreMethodIterator();
   DLLEXPORT bool next();
   DLLEXPORT const QoreMethod* getMethod() const;
};

//! To be used to iterate through a class' static methods
class QoreStaticMethodIterator {
private:
   void* priv;

public:
   DLLEXPORT QoreStaticMethodIterator(const QoreClass* qc);
   DLLEXPORT ~QoreStaticMethodIterator();
   DLLEXPORT bool next();
   DLLEXPORT const QoreMethod* getMethod() const;
};

#endif // _QORE_QORECLASS_H
