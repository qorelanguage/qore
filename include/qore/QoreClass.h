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

// all qore class IDs
DLLEXPORT extern int CID_AUTOGATE;
DLLEXPORT extern int CID_AUTOLOCK;
DLLEXPORT extern int CID_AUTOREADLOCK;
DLLEXPORT extern int CID_AUTOWRITELOCK;
DLLEXPORT extern int CID_CONDITION;
DLLEXPORT extern int CID_COUNTER;
DLLEXPORT extern int CID_DATASOURCE;
DLLEXPORT extern int CID_DATASOURCEPOOL;
DLLEXPORT extern int CID_FILE;
DLLEXPORT extern int CID_FTPCLIENT;
DLLEXPORT extern int CID_GATE;
DLLEXPORT extern int CID_GETOPT;
DLLEXPORT extern int CID_HTTPCLIENT;
DLLEXPORT extern int CID_JSONRPCCLIENT;
DLLEXPORT extern int CID_MUTEX;
DLLEXPORT extern int CID_PROGRAM;
//DLLEXPORT extern int CID_QUERY;
DLLEXPORT extern int CID_QUEUE;
DLLEXPORT extern int CID_RWLOCK;
DLLEXPORT extern int CID_SSLCERTIFICATE;
DLLEXPORT extern int CID_SSLPRIVATEKEY;
DLLEXPORT extern int CID_SEQUENCE;
DLLEXPORT extern int CID_SOCKET;
DLLEXPORT extern int CID_XMLRPCCLIENT;

struct qore_method_private;

class BCList;
class BCSMList;

class QoreMethod {
   private:
      struct qore_method_private *priv; // private implementation

      DLLLOCAL QoreMethod(const class QoreClass *p_class);
      DLLLOCAL void userInit(UserFunction *u, int p);

      // not implemented
      DLLLOCAL QoreMethod(const QoreMethod&);
      DLLLOCAL QoreMethod& operator=(const QoreMethod&);

   public:
      DLLEXPORT class QoreNode *eval(class QoreObject *self, const class QoreNode *args, class ExceptionSink *xsink) const;
      DLLEXPORT bool isSynchronized() const;
      DLLEXPORT int getType() const;
      DLLEXPORT bool isPrivate() const;
      DLLEXPORT const char *getName() const;

      DLLLOCAL QoreMethod(class UserFunction *u, int p);
      DLLLOCAL QoreMethod(const class QoreClass *p_class, class BuiltinMethod *b, bool n_priv = false);
      DLLLOCAL ~QoreMethod();
      DLLLOCAL bool inMethod(const class QoreObject *self) const;
      DLLLOCAL void evalConstructor(class QoreObject *self, const QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL void evalDestructor(class QoreObject *self, class ExceptionSink *xsink) const;
      DLLLOCAL void evalSystemConstructor(class QoreObject *self, const QoreNode *args, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL void evalSystemDestructor(class QoreObject *self, class ExceptionSink *xsink) const;
      DLLLOCAL void evalCopy(class QoreObject *self, class QoreObject *old, class ExceptionSink *xsink) const;
      DLLLOCAL class QoreMethod *copy(const class QoreClass *p_class) const;
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstructor(class BCList *bcl);
      // only called when method is user
      DLLLOCAL const QoreClass *get_class() const;
      DLLLOCAL void assign_class(const QoreClass *p_class);
};

/*
  QoreClass
  defines a qore-language class
*/
struct qore_qc_private; 

class QoreClass{
      friend class BCList;
      friend class BCSMList;

   private:
      struct qore_qc_private *priv;  // private implementation (pimpl)

      // private constructor only called when the class is copied
      DLLLOCAL QoreClass(int id, const char *nme);
      DLLLOCAL const class QoreMethod *parseFindMethod(const char *name);
      DLLLOCAL void insertMethod(class QoreMethod *o);
      // checks for all special methods except constructor & destructor
      DLLLOCAL void checkSpecialIntern(const QoreMethod *m);
      // checks for all special methods
      DLLLOCAL void checkSpecial(const QoreMethod *m);
      DLLLOCAL class QoreNode *evalMethodGate(class QoreObject *self, const char *nme, const class QoreNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL const QoreMethod *resolveSelfMethodIntern(const char *nme);
      DLLLOCAL void delete_pending_methods();
      DLLLOCAL BCAList *getBaseClassConstructorArgumentList() const;

      DLLLOCAL QoreClass(const QoreClass&); // not implemented
      DLLLOCAL QoreClass& operator=(const QoreClass&); // not implemented

   public:
      DLLEXPORT QoreClass(const char *nme, int dom = 0);
      DLLEXPORT ~QoreClass();
      
      DLLEXPORT void addMethod(const char *nme, q_method_t m, bool priv = false);
      DLLEXPORT void setDestructor(q_destructor_t m);
      DLLEXPORT void setConstructor(q_constructor_t m);
      DLLEXPORT void setSystemConstructor(q_constructor_t m);
      DLLEXPORT void setCopy(q_copy_t m);
      // this method takes ownership of *name
      DLLEXPORT void addPrivateMember(char *name);
      DLLEXPORT bool isPrivateMember(const char *str) const;
      DLLEXPORT class QoreNode *evalMethod(class QoreObject *self, const char *nme, const class QoreNode *args, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *execConstructor(const class QoreNode *args, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *execSystemConstructor(const class QoreNode *args, class ExceptionSink *xsink) const;
      DLLEXPORT class QoreNode *execCopy(class QoreObject *old, class ExceptionSink *xsink) const;
      DLLEXPORT const QoreMethod *findLocalMethod(const char *name) const;
      DLLEXPORT class QoreList *getMethodList() const;
      DLLEXPORT class QoreClass *getClass(int cid) const;
      DLLEXPORT int numMethods() const;
      DLLEXPORT bool hasCopy() const;
      DLLEXPORT int getID() const;
      DLLEXPORT bool isSystem() const;
      DLLEXPORT bool hasMemberGate() const;
      DLLEXPORT int getDomain() const;
      DLLEXPORT const char *getName() const;
      // used at run-time
      DLLEXPORT const QoreMethod *findMethod(const char *nme) const;
      DLLEXPORT const QoreMethod *findMethod(const char *nme, bool *priv) const;

      // make a builtin class a child of another builtin class, private inheritance makes no sense
      // (there would be too much overhead to use user-level qore interfaces to call private methods)
      // but base class constructor arguments can be given
      // this function takes over the reference for the xargs argument
      DLLEXPORT void addBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs = NULL);

      // this method will do the same as above but will also ensure that the given class' private data
      // will be used in all object methods - in this case the class cannot have any private data
      // this function takes over the reference for the xargs argument
      DLLEXPORT void addDefaultBuiltinBaseClass(class QoreClass *qc, class QoreNode *xargs = NULL);

      // this method adds a base class placeholder for a subclass - where the subclass' private data 
      // object is actually a subclass of the parent class and all methods are virtual, so the
      // base class' constructor, destructor, and copy constructor will never be run and the base
      // class methods will be passed a pointer to the subclass' data
      DLLEXPORT void addBuiltinVirtualBaseClass(class QoreClass *qc);

      DLLLOCAL QoreClass();
      DLLLOCAL void addMethod(class QoreMethod *f);
      DLLLOCAL class QoreNode *evalMemberGate(class QoreObject *self, const QoreString *nme, class ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassConstructor(class QoreObject *self, class BCEAList *bceal, class ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassSystemConstructor(class QoreObject *self, class BCEAList *bceal, class ExceptionSink *xsink) const;      
      DLLLOCAL void execDestructor(class QoreObject *self, class ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassDestructor(class QoreObject *self, class ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassSystemDestructor(class QoreObject *self, class ExceptionSink *xsink) const;
      DLLLOCAL void execSubclassCopy(class QoreObject *self, class QoreObject *old, class ExceptionSink *xsink) const;
      DLLLOCAL const QoreMethod *resolveSelfMethod(const char *nme);
      DLLLOCAL const QoreMethod *resolveSelfMethod(class NamedScope *nme);
      DLLLOCAL void addDomain(int dom);
      DLLLOCAL class QoreClass *copyAndDeref();
      DLLLOCAL void addBaseClassesToSubclass(class QoreClass *sc, bool is_virtual);
      // used when parsing
      DLLLOCAL const QoreMethod *findParseMethod(const char *nme);
      // returns 0 for success, -1 for error
      DLLLOCAL int parseAddBaseClassArgumentList(class BCAList *bcal);
      // only called when parsing, sets the name of the class
      DLLLOCAL void setName(const char *n);
      // returns true if reference count is 1
      DLLLOCAL bool is_unique() const;
      // references and returns itself
      DLLLOCAL class QoreClass *getReference();
      // dereferences the class, deletes if reference count is 0
      DLLLOCAL void nderef();
      DLLLOCAL void parseInit();
      DLLLOCAL void parseCommit();
      DLLLOCAL void parseRollback();
      DLLLOCAL int getIDForMethod() const;
      DLLLOCAL void parseSetBaseClassList(class BCList *bcl);
      // get base class list to add virtual class indexes for private data
      DLLLOCAL BCSMList *getBCSMList() const;
};

#endif // _QORE_QORECLASS_H
