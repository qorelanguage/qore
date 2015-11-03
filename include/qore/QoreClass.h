/*
  QoreClass.h

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

#ifndef _QORE_QORECLASS_H

#define _QORE_QORECLASS_H

#include <stdarg.h>

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
DLLEXPORT extern qore_classid_t CID_JSONRPCCLIENT;
DLLEXPORT extern qore_classid_t CID_MUTEX;
DLLEXPORT extern qore_classid_t CID_PROGRAM;
DLLEXPORT extern qore_classid_t CID_QUEUE;
DLLEXPORT extern qore_classid_t CID_RWLOCK;
DLLEXPORT extern qore_classid_t CID_SSLCERTIFICATE;
DLLEXPORT extern qore_classid_t CID_SSLPRIVATEKEY;
DLLEXPORT extern qore_classid_t CID_SEQUENCE;
DLLEXPORT extern qore_classid_t CID_SOCKET;
DLLEXPORT extern qore_classid_t CID_XMLRPCCLIENT;

class BCList;
class BCSMList;
class QoreObject;
class QoreClass;
class BCEAList;

//! a method in a QoreClass
/** methods can be implemented in the Qore language (user methods) or in C++ (builtin methods)
    @see QoreClass
 */
class QoreMethod {
      friend class QoreObject;
      friend class StaticMethodCallNode;

   private:
      //! private implementation of the method
      struct qore_method_private *priv;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreMethod(const QoreMethod&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreMethod& operator=(const QoreMethod&);

      //! private constructor
      DLLLOCAL QoreMethod(const QoreClass *p_class, UserFunction *u, bool n_priv, bool n_static);

      //! evaluates the method and returns the result
      /** should only be called by QoreObject; use QoreObject::evalMethod(const QoreMethod &meth, const QoreListNode *args, ExceptionSink *xsink) instead
	  @param self a pointer to the object the method will be executed on
	  @param args the list of arguments to the method
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation (can be 0)
       */
      DLLLOCAL AbstractQoreNode *eval(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;

   public:
      //! returns true if the method is synchronized (has a recursive thread lock associated with it)
      /**
	 @return true if the method is synchronized (has a recursive thread lock associated with it)
       */
      DLLEXPORT bool isSynchronized() const;

      //! returns true if the method is a user-defined method (implemented with Qore-language code)
      /**
	 @return true if the method is a user-defined method (implemented with Qore-language code)
       */
      DLLEXPORT bool isUser() const;

      //! returns true if the method is builtin
      /**
	 @return true if the method is builtin
       */
      DLLEXPORT bool isBuiltin() const;

      //! returns true if the method is private
      /**
	 @return true if the method is private
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
      DLLEXPORT const char *getName() const;

      //! returns true if it's a builtin method with the new generic calling convention
      /**
	 @return true if it's a builtin method with the new generic calling convention
       */
      DLLEXPORT bool newCallingConvention() const;

      //! returns a pointer to the parent class
      DLLEXPORT const QoreClass *getClass() const;

      DLLLOCAL QoreMethod(UserFunction *u, bool n_priv, bool n_static);
      DLLLOCAL QoreMethod(const QoreClass *p_class, BuiltinMethod *b, bool n_priv = false, bool n_static = false);
      DLLLOCAL QoreMethod(const QoreClass *p_class, BuiltinMethod *b, bool n_priv, bool n_static, bool new_calling_convention);
      DLLLOCAL ~QoreMethod();
      DLLLOCAL int getType() const;
      DLLLOCAL bool inMethod(const QoreObject *self) const;
      DLLLOCAL void evalConstructor(QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;
      //DLLLOCAL void evalConstructor2(const QoreClass &thisclass, QoreObject *self, const QoreListNode *args, BCList *bcl, BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL void evalDestructor(QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void evalSystemConstructor(QoreObject *self, int code, va_list args) const;
      DLLLOCAL void evalSystemDestructor(QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const;
      DLLLOCAL bool evalDeleteBlocker(QoreObject *self) const;
      DLLLOCAL QoreMethod *copy(const class QoreClass *p_class) const;
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstructor(BCList *bcl);
      DLLLOCAL void assign_class(const QoreClass *p_class);
      DLLLOCAL const BuiltinFunction *getStaticBuiltinFunction() const;
      DLLLOCAL const UserFunction *getStaticUserFunction() const;
      DLLLOCAL bool existsUserParam(int i) const;
};

//! defines a Qore-language class
/** Qore's classes can be either implemented by Qore language code (user classes)
    or in C++ (builtin classes), or both, as in the case of a builtin class that
    also has user methods.
 */
class QoreClass {
      friend class BCList;
      friend class BCSMList;
      friend class QoreObject;

   private:
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreClass(const QoreClass&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreClass& operator=(const QoreClass&);

      //! private implementation of the class
      struct qore_qc_private *priv;

      // private constructor only called when the class is copied
      DLLLOCAL QoreClass(qore_classid_t id, const char *nme);

      // looks in current and pending method lists for non-static methods, only for local class
      DLLLOCAL const QoreMethod *parseFindMethod(const char *name);

      // looks in current and pending method lists for static methods, only for local class
      DLLLOCAL const QoreMethod *parseFindStaticMethod(const char *name);

      DLLLOCAL void insertMethod(QoreMethod *o);
      DLLLOCAL void insertStaticMethod(QoreMethod *o);
      DLLLOCAL AbstractQoreNode *evalMethodGate(QoreObject *self, const char *nme, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL const QoreMethod *resolveSelfMethodIntern(const char *nme);
      DLLLOCAL BCAList *getBaseClassConstructorArgumentList() const;

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
      DLLLOCAL AbstractQoreNode *evalMethod(QoreObject *self, const char *method_name, const QoreListNode *args, ExceptionSink *xsink) const;
      // This function must only be called from QoreObject
      DLLLOCAL AbstractQoreNode *evalMemberGate(QoreObject *self, const QoreString *nme, ExceptionSink *xsink) const;
      // This function must only be called from QoreObject
      DLLLOCAL void execMemberNotification(QoreObject *self, const char *mem, ExceptionSink *xsink) const;
      // This function must only be called from QoreObject and BCList
      DLLLOCAL bool execDeleteBlocker(QoreObject *self, ExceptionSink *xsink) const;
      // This function must only be called from BCList
      DLLLOCAL void execSubclassConstructor(QoreObject *self, BCEAList *bceal, ExceptionSink *xsink) const;
      // This function must only be called from QoreObject
      DLLLOCAL void execDestructor(QoreObject *self, ExceptionSink *xsink) const;
      // This function must only be called from BCSMList
      DLLLOCAL void execSubclassDestructor(QoreObject *self, ExceptionSink *xsink) const;
      // This function must only be called from BCSMList
      DLLLOCAL void execSubclassSystemDestructor(QoreObject *self, ExceptionSink *xsink) const;
      // This function must only be called from BCSMList
      DLLLOCAL void execSubclassCopy(QoreObject *self, QoreObject *old, ExceptionSink *xsink) const;

   public:
      //! creates the QoreClass object and assigns the name and the functional domain
      /** @note class names and subnamespaces names must be unique in a namespace; i.e. no class may have the same name as a subnamespace within a namespace and vice-versa
	  @param n_name the name of the class
	  @param n_domain the functional domain of the class to be used to enforce functional restrictions within a Program object
	  @see QoreProgram
       */
      DLLEXPORT QoreClass(const char *n_name, int n_domain = QDOM_DEFAULT);

      //! deletes the object and frees all memory
      DLLEXPORT ~QoreClass();

      //! adds a builtin method to a class
      /** in debuggging mode, the call will abort if the name of the method is
	  "constructor", "destructor", or "copy", or if the method already exists
	  in the class.
	  To set the constructor method, call QoreClass::setConstructor().
	  To set the destructor method, call QoreClass::setDestructor().
	  To set the copy method, call QoreClass::setCopy().
	  @param n_name the name of the method, must be unique in the class
	  @param meth the method to be added
	  @param priv if true then the method will be added as a private method
	  @code
	  // the actual function can be declared with the class to be expected as the private data as follows:
	  static AbstractQoreNode *AL_lock(QoreObject *self, QoreAutoLock *m, const QoreListNode *params, ExceptionSink *xsink);
	  ...
	  // and then casted to (q_method_t) in the addMethod call:
	  QC_AutoLock->addMethod("lock", (q_method_t)AL_lock);
	  @endcode
	  @see Qoreclass::addStaticMethod()
	  @see QoreClass::setConstructor()
	  @see QoreClass::setDestructor()
	  @see QoreClass::setCopy()
       */
      DLLEXPORT void addMethod(const char *n_name, q_method_t meth, bool priv = false);

      //! adds a builtin method with the new generic calling convention
      DLLEXPORT void addMethod2(const char *n_name, q_method2_t meth, bool priv = false);

      //! adds a builtin static method to a class
      /**
	  @param n_name the name of the method, must be unique in the class
	  @param meth the method to be added
	  @param priv if true then the method will be added as a private method	  
       */
      DLLEXPORT void addStaticMethod(const char *n_name, q_func_t meth, bool priv = false);

      //! adds a builtin static method with the new generic calling convention
      DLLEXPORT void addStaticMethod2(const char *n_name, q_static_method2_t meth, bool priv = false);

      //! sets the builtin destructor method for the class
      /** you only need to implement destructor methods if the destructor should destroy the object
	  before the reference count reaches zero.
	  @param m the destructor method to run
	  @code
	  // the actual function can be declared with the class to be expected as the private data as follows:
	  static void AL_destructor(QoreObject *self, QoreAutoLock *al, ExceptionSink *xsink);
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
	  static void AL_destructor(const QoreClass &thisclass, QoreObject *self, QoreAutoLock *al, ExceptionSink *xsink);
	  ...
	  // and then casted to (q_destructor_t) in the addMethod call:
	  QC_AutoLock->setDestructor2((q_destructor_t)AL_destructor);
	  @endcode
       */
      DLLEXPORT void setDestructor2(q_destructor2_t m);

      //! sets the builtin constructor method for the class
      /**
	 @param m the constructor method
       */
      DLLEXPORT void setConstructor(q_constructor_t m);

      //! sets the builtin constructor method for the class using the new calling convention
      /**
	 @param m the constructor method
       */
      DLLEXPORT void setConstructor2(q_constructor2_t m);

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
	  static void AL_copy(QoreObject *self, QoreObject *old, class QoreAutoLock *m, ExceptionSink *xsink)
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
	  static void AL_copy(const QoreClass &thisclass, QoreObject *self, QoreObject *old, QoreAutoLock *m, ExceptionSink *xsink)
	  ...
	  // and then casted to (q_copy2_t) in the addMethod call:
	  QC_AutoLock->setCopy((q_copy2_t)AL_copy);
	  @endcode
       */
      DLLEXPORT void setCopy2(q_copy2_t m);

      //! sets the deleteBlocker method for the class
      /** this method will be run when the object is deleted; it should be set only for classes where
	  the objects' lifecycles are or may be managed externally.
	  @param m the deleteBlocker method to set
	  @note delete blocker methods are called with the object's atomic reference lock held, therefore be very careful what you call from within the deleteBlocker function
      */
      DLLEXPORT void setDeleteBlocker(q_delete_blocker_t m);

      //! adds a name of a private member (not accessible from outside the class hierarchy)
      /** this method takes ownership of *name
	  @param name the name of the private member (ownership of the memory is assumed by the QoreClas object)
       */
      DLLEXPORT void addPrivateMember(char *name);

      //! returns true if the member is private
      /** 
	  @param str the member name to check
	  @return true if the member is private
       */
      DLLEXPORT bool isPrivateMember(const char *str) const;

      //! creates a new object and executes the constructor on it and returns the new object
      /** if a Qore-language exception occurs, 0 is returned.  To create a 
	  @param args the arguments for the method
	  @param xsink Qore-language exception information is added here
	  @return the object created
       */
      DLLEXPORT QoreObject *execConstructor(const QoreListNode *args, ExceptionSink *xsink) const;

      //! creates a new "system" object for use as the value of a constant, executes the system constructor on it and returns the new object
      /** if a Qore-language exception occurs, 0 is returned
	  @param code an optional code for the constructor; this parameter is here because passing a variable number of arguments requires at least one fixed parameter before the (possibly empty) list
	  @return the object created
       */
      DLLEXPORT QoreObject *execSystemConstructor(int code = 0, ...) const;

      //! executes a class' "copy" method on an object and returns the new object (or 0 in the case of an exception)
      /** @param old the original object to copy
	  @param xsink Qore-language exception information is added here
	  @return the object created
       */
      DLLEXPORT QoreObject *execCopy(QoreObject *old, ExceptionSink *xsink) const;

      //! looks for a non-static method in the current class without searching base classes
      /** @param name the name of the method
	  @returns a pointer to the method found, or 0 if no such method exists in the class
      */
      DLLEXPORT const QoreMethod *findLocalMethod(const char *name) const;

      //! looks for a static method in the current class without searching base classes
      /** @param name the name of the static method
	  @returns a pointer to the method found, or 0 if no such method exists in the class
      */
      DLLEXPORT const QoreMethod *findLocalStaticMethod(const char *name) const;

      //! returns a list strings of all non-static methods in the class, the caller owns the reference count returned
      /** always returns a list; if there are no non-static methods then an empty list is returned
	  @return a list strings of all non-static methods in the class, the caller owns the reference count returned
       */
      DLLEXPORT QoreListNode *getMethodList() const;

      //! returns a list strings of all static methods in the class, the caller owns the reference count returned
      /** always returns a list; if there are no static methods then an empty list is returned
	  @return a list strings of all static methods in the class, the caller owns the reference count returned
       */
      DLLEXPORT QoreListNode *getStaticMethodList() const;

      //! returns a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
      /** if the class ID is equal to the current class or is a base class
	  of the current class, the appropriate QoreClass pointer will be
	  returned.
	  @param cid the class ID of the QoreClass to find
	  @return a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
       */
      DLLEXPORT QoreClass *getClass(qore_classid_t cid) const;
      
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

      //! returns true if the class implements a "memberNotification" method
      DLLEXPORT bool hasMemberNotification() const;

      //! returns the functional domain of the class
      DLLEXPORT int getDomain() const;

      //! returns the class name
      DLLEXPORT const char *getName() const;

      //! finds a non-static method in the class hierarchy
      // used at run-time
      DLLEXPORT const QoreMethod *findMethod(const char *nme) const;

      //! finds a static method in the class hierarchy
      // used at run-time
      DLLEXPORT const QoreMethod *findStaticMethod(const char *nme) const;

      //! finds a non-static method in the class hierarchy and sets the priv flag if it's a private method or not
      DLLEXPORT const QoreMethod *findMethod(const char *nme, bool &priv) const;

      //! finds a static method in the class hierarchy and sets the priv flag if it's a private method or not
      DLLEXPORT const QoreMethod *findStaticMethod(const char *nme, bool &priv) const;

      //! make a builtin class a child of another builtin class
      /** Private inheritance makes no sense with builtin classes (there would be
	  too much overhead to use user-level qore interfaces to call private methods)
	  but base class constructor arguments can be given.
	  This function takes over ownership for the reference for the xargs pointer argument
	  @param qc the base class to add
	  @param xargs the argument expression for the base class constructor call
      */
      DLLEXPORT void addBuiltinBaseClass(QoreClass *qc, class QoreListNode *xargs = 0);

      //! make a builtin class a child of another builtin class and ensures that the given class' private data will be used in all class methods
      /** In the case this function is used, this objects of class cannot have
	  private data saved against the class ID.
	  This function takes over ownership for the reference for the xargs pointer argument
	  @param qc the base class to add
	  @param xargs the argument expression for the base class constructor call
      */
      DLLEXPORT void addDefaultBuiltinBaseClass(QoreClass *qc, class QoreListNode *xargs = 0);

      //! sets "virtual" base class for a class, meaning that the base class data is appropriate for use in the subclass builtin methods
      /** this method adds a base class placeholder for a subclass - where the subclass' private data 
	  object is actually a subclass of the parent class and all methods are virtual, so the
	  base class' constructor, destructor, and copy constructor will never be run and the base
	  class methods will be passed a pointer to the subclass' data
	  @param qc the base class to add
      */
      DLLEXPORT void addBuiltinVirtualBaseClass(QoreClass *qc);

      //! call this function if your builtin class requires *all* methods (except the constructor) to be run in an RMutex lock
      /** use this for classes that require exclusive access to the private data in all functions
       */
      DLLEXPORT void setSynchronousClass();

      //! returns a const pointer to the QoreMethod object of the constuctor method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the constuctor method, if any is set
      */
      DLLEXPORT const QoreMethod *getConstructor() const;

      //! returns a const pointer to the QoreMethod object of the constuctor method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the constuctor method, if any is set
      */
      DLLEXPORT const QoreMethod *getSystemConstructor() const;

      //! returns a const pointer to the QoreMethod object of the constructor method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the constructor method, if any is set
      */
      DLLEXPORT const QoreMethod *getDestructor() const;

      //! returns a const pointer to the QoreMethod object of the destructor method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the destructor method, if any is set
      */
      DLLEXPORT const QoreMethod *getCopyMethod() const;

      //! returns a const pointer to the QoreMethod object of the memberGate method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the memberGate method, if any is set
      */
      DLLEXPORT const QoreMethod *getMemberGateMethod() const;

      //! returns a const pointer to the QoreMethod object of the methodGate method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the methodGate method, if any is set
      */
      DLLEXPORT const QoreMethod *getMethodGate() const;

      //! returns a const pointer to the QoreMethod object of the memberNotification method, if any is set
      /** executes in constant time
	  @return a const pointer to the QoreMethod object of the memberNotification method, if any is set
      */
      DLLEXPORT const QoreMethod *getMemberNotificationMethod() const;

      DLLLOCAL QoreClass();
      DLLLOCAL void addMethod(QoreMethod *f);
      DLLLOCAL const QoreMethod *resolveSelfMethod(const char *nme);
      DLLLOCAL const QoreMethod *resolveSelfMethod(class NamedScope *nme);
      DLLLOCAL void addDomain(int dom);
      DLLLOCAL QoreClass *copyAndDeref();
      DLLLOCAL void addBaseClassesToSubclass(QoreClass *sc, bool is_virtual);

      // used when parsing, finds committed non-static methods within the entire class hierarchy (local class plus base classes)
      DLLLOCAL const QoreMethod *findParseMethod(const char *nme);

      // used when parsing, finds committed static methods within the entire class hierarchy (local class plus base classes)
      DLLLOCAL const QoreMethod *findParseStaticMethod(const char *nme);

      // returns 0 for success, -1 for error
      DLLLOCAL int parseAddBaseClassArgumentList(class BCAList *bcal);
      // only called when parsing, sets the name of the class
      DLLLOCAL void setName(const char *n);
      // returns true if reference count is 1
      DLLLOCAL bool is_unique() const;
      // references and returns itself
      DLLLOCAL QoreClass *getReference();
      // dereferences the class, deletes if reference count is 0
      DLLLOCAL void nderef();
      DLLLOCAL void parseInit();
      DLLLOCAL void parseCommit();
      DLLLOCAL void parseRollback();
      DLLLOCAL qore_classid_t getIDForMethod() const;
      DLLLOCAL void parseSetBaseClassList(BCList *bcl);
      // get base class list to add virtual class indexes for private data
      DLLLOCAL BCSMList *getBCSMList() const;
      // returns true if the class has a delete_blocker function (somewhere in the hierarchy)
      DLLLOCAL bool has_delete_blocker() const;
      // returns true if the class has a synchronous class somewhere in the class' hierarchy
      DLLLOCAL bool has_synchronous_in_hierarchy() const;
      // returns true if the class itself is synchronous
      DLLLOCAL bool is_synchronous_class() const;
      // one-time initialization
      DLLLOCAL void initialize();
      // looks in current and pending method lists for the entire hierarchy (local class plus base classes), non-static methods only
      DLLLOCAL const QoreMethod *parseFindMethodTree(const char *name);
      // looks in current and pending method lists for the entire hierarchy (local class plus base classes), static methods only
      DLLLOCAL const QoreMethod *parseFindStaticMethodTree(const char *name);
      // returns true if the class passed is equal to or in the class' hierarchy - to be called only at parse time or under the program's parse lock
      DLLLOCAL bool parseCheckHierarchy(const QoreClass *cls) const;
};

#endif // _QORE_QORECLASS_H
