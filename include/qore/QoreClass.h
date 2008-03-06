/*
  QoreClass.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

//! a method in a QoreClass
/** methods can be implemented in the Qore language (user methods) or in C++ (builtin methods)
    @see QoreClass
 */
class QoreMethod {
   private:
      //! private implementation of the method
      struct qore_method_private *priv;

      //! private constructor
      DLLLOCAL QoreMethod(const class QoreClass *p_class);

      DLLLOCAL void userInit(UserFunction *u, int p);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreMethod(const QoreMethod&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreMethod& operator=(const QoreMethod&);

   public:
      //! evaluates the method and returns the result
      /** 
	  @param self a pointer to the object the method will be executed on
	  @param args the list of arguments to the method
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation (can be 0)
       */
      DLLEXPORT AbstractQoreNode *eval(class QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) const;

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

      //! returns true if the method is a private method of the class
      /**
	 @return true if the method is a private method of the class
       */
      DLLEXPORT bool isPrivate() const;

      //! returns the method's name
      /**
	 @return the method's name
       */
      DLLEXPORT const char *getName() const;

      DLLLOCAL QoreMethod(class UserFunction *u, int p);
      DLLLOCAL QoreMethod(const class QoreClass *p_class, class BuiltinMethod *b, bool n_priv = false);
      DLLLOCAL ~QoreMethod();
      DLLLOCAL int getType() const;
      DLLLOCAL bool inMethod(const class QoreObject *self) const;
      DLLLOCAL void evalConstructor(class QoreObject *self, const QoreListNode *args, BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL void evalDestructor(class QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void evalSystemConstructor(class QoreObject *self, int code, va_list args) const;
      DLLLOCAL void evalSystemDestructor(class QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(class QoreObject *self, class QoreObject *old, ExceptionSink *xsink) const;
      DLLLOCAL QoreMethod *copy(const class QoreClass *p_class) const;
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstructor(BCList *bcl);
      // only called when method is user
      DLLLOCAL const QoreClass *get_class() const;
      DLLLOCAL void assign_class(const QoreClass *p_class);
};

//! defines a Qore-language class
/** Qore's classes can be either implemented by Qore language code (user classes)
    or in C++ (builtin classes), or both, as in the case of a builtin class that
    also has user methods.
 */
class QoreClass{
      friend class BCList;
      friend class BCSMList;

   private:
      //! private implementation of the class
      struct qore_qc_private *priv;

      // private constructor only called when the class is copied
      DLLLOCAL QoreClass(qore_classid_t id, const char *nme);
      DLLLOCAL const class QoreMethod *parseFindMethod(const char *name);
      DLLLOCAL void insertMethod(class QoreMethod *o);
      DLLLOCAL AbstractQoreNode *evalMethodGate(class QoreObject *self, const char *nme, const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL const QoreMethod *resolveSelfMethodIntern(const char *nme);
      DLLLOCAL BCAList *getBaseClassConstructorArgumentList() const;

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreClass(const QoreClass&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreClass& operator=(const QoreClass&);

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
	  @see QoreClass::setConstructor()
	  @see QoreClass::setDestructor()
	  @see QoreClass::setCopy()
       */
      DLLEXPORT void addMethod(const char *n_name, q_method_t meth, bool priv = false);

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

      //! sets the builtin constructor method for the class
      /**
	 @param m the constructor method
       */
      DLLEXPORT void setConstructor(q_constructor_t m);

      //! sets the builtin constructor for system objects (ex: used as constant values)
      /** @note system constructors in a class hierarchy must call the base class constructors manually
	  @param m the constructor method
       */
      DLLEXPORT void setSystemConstructor(q_system_constructor_t m);

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

      //! evaluates a method on an object and returns the result
      /** if the method name is not valid or is private (and the call is made outside the object)
	  then an exception will be raised and 0 will be returned.
	  @param self the object to execute the method on
	  @param method_name the name of the method to execute
	  @param args the arguments for the method
	  @param xsink Qore-language exception information is added here
	  @return the value returned by the method, can be 0
       */
      DLLEXPORT AbstractQoreNode *evalMethod(class QoreObject *self, const char *method_name, const QoreListNode *args, ExceptionSink *xsink) const;

      //! creates a new object and executes the constructor on it and returns the new object
      /** if a Qore-language exception occurs, 0 is returned.  To create a 
	  @param args the arguments for the method
	  @param xsink Qore-language exception information is added here
	  @return the object created
       */
      DLLEXPORT class QoreObject *execConstructor(const QoreListNode *args, ExceptionSink *xsink) const;

      //! creates a new "system" object for use as the value of a constant, executes the system constructor on it and returns the new object
      /** if a Qore-language exception occurs, 0 is returned
	  @param code an optional code for the constructor; this parameter is here because passing a variable number of arguments requires at least one fixed parameter before the (possibly empty) list
	  @return the object created
       */
      DLLEXPORT class QoreObject *execSystemConstructor(int code, ...) const;

      //! executes a class' "copy" method on an object and returns the new object (or 0 in the case of an exception)
      /** @param old the original object to copy
	  @param xsink Qore-language exception information is added here
	  @return the object created
       */
      DLLEXPORT class QoreObject *execCopy(class QoreObject *old, ExceptionSink *xsink) const;

      //! looks for a method in the current class without searching base classes
      /** @param name the name of the method
	  @returns a pointer to the method found, or 0 if no such method exists in the class
      */
      DLLEXPORT const QoreMethod *findLocalMethod(const char *name) const;

      //! returns a list strings of all methods in the class, the caller owns the reference count returned
      /** always returns a list; if there are no methods then an empty list is returned
	  @return a list strings of all methods in the class, the caller owns the reference count returned
       */
      DLLEXPORT class QoreListNode *getMethodList() const;

      //! returns a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
      /** if the class ID is equal to the current class or is a base class
	  of the current class, the appropriate QoreClass pointer will be
	  returned.
	  @param cid the class ID of the QoreClass to find
	  @return a pointer to the QoreClass object representing the class ID passed if it exists in the class hierarchy
       */
      DLLEXPORT QoreClass *getClass(qore_classid_t cid) const;
      
      //! returns the number of methods in this class
      DLLEXPORT int numMethods() const;

      //! returns true if the class implements a copy method
      DLLEXPORT bool hasCopy() const;

      //! returns the class ID of this class
      DLLEXPORT qore_classid_t getID() const;

      //! returns true if the class is a builtin class
      DLLEXPORT bool isSystem() const;

      //! returns true if the class implements a "memberGate" method
      DLLEXPORT bool hasMemberGate() const;

      //! returns the functional domain of the class
      DLLEXPORT int getDomain() const;

      //! returns the class name
      DLLEXPORT const char *getName() const;

      //! finds a method in the class hierarchy
      // used at run-time
      DLLEXPORT const QoreMethod *findMethod(const char *nme) const;

      //! finds a method in the class hierarchy and sets the priv flag if it's a private method or not
      DLLEXPORT const QoreMethod *findMethod(const char *nme, bool &priv) const;

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

      DLLLOCAL QoreClass();
      DLLLOCAL void addMethod(class QoreMethod *f);
      DLLLOCAL AbstractQoreNode *evalMemberGate(class QoreObject *self, const QoreString *nme, ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassConstructor(class QoreObject *self, class BCEAList *bceal, ExceptionSink *xsink) const;
      DLLLOCAL void execDestructor(class QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassDestructor(class QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassSystemDestructor(class QoreObject *self, ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassCopy(class QoreObject *self, class QoreObject *old, ExceptionSink *xsink) const;
      DLLLOCAL const QoreMethod *resolveSelfMethod(const char *nme);
      DLLLOCAL const QoreMethod *resolveSelfMethod(class NamedScope *nme);
      DLLLOCAL void addDomain(int dom);
      DLLLOCAL QoreClass *copyAndDeref();
      DLLLOCAL void addBaseClassesToSubclass(QoreClass *sc, bool is_virtual);
      // used when parsing
      DLLLOCAL const QoreMethod *findParseMethod(const char *nme);
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
};

#endif // _QORE_QORECLASS_H
