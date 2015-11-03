/*
  Namespace.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef _QORE_QORENAMESPACE_H

#define _QORE_QORENAMESPACE_H

#include <string.h>
#include <stdlib.h>

#include <string>

class RootQoreNamespace;
class ConstantList;
class QoreNamespaceList;
class QoreClassList;
class NamedScope;

//! contains constants, classes, and subnamespaces in QoreProgram objects
class QoreNamespace {
      friend class QoreNamespaceList;
      friend class RootQoreNamespace;

  private:
      struct qore_ns_private *priv; // private implementation

      DLLLOCAL AbstractQoreNode *parseMatchScopedConstantValue(NamedScope *name, int *matched) const;
      DLLLOCAL QoreClass *parseMatchScopedClass(NamedScope *name, int *matched) const;
      DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(NamedScope *nscope, int *matched) const;
      DLLLOCAL QoreNamespace *parseMatchNamespace(NamedScope *nscope, int *matched) const;
      DLLLOCAL void assimilate(QoreNamespace *ns);
      DLLLOCAL QoreNamespace *findNamespace(const char *name) const;
      DLLLOCAL QoreNamespace *resolveNameScope(NamedScope *name) const;
      DLLLOCAL AbstractQoreNode *getConstantValue(const char *name) const;
      DLLLOCAL QoreNamespace(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl, QoreClassList *pend_ocl, ConstantList *pend_cl, QoreNamespaceList *pend_nnsl);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNamespace(const QoreNamespace&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreNamespace& operator=(const QoreNamespace&);

   public:
      //! creates a namespace with the given name
      /** the name of a subnamespace must be unique in the parent namespace and must not have the same name as a class in the parent namespace either
	  @param n the name of the namespace
       */
      DLLEXPORT QoreNamespace(const char *n);

      //! destroys the object and frees all associated memory
      DLLEXPORT ~QoreNamespace();

      //! adds a constant definition to the namespace
      /**
	 @param name the name of the constant to add
	 @param value the value of the constant
       */
      DLLEXPORT void addConstant(const char *name, AbstractQoreNode *value);

      //! adds a class to a namespace
      /**
	 @param oc the class to add to the namespace
      */
      DLLEXPORT void addSystemClass(QoreClass *oc);

      //! adds a subnamespace to the namespace
      /** use this function when the QoreNamespace can be added directly to the tree
	  (does not need to be merged with another namespace of the same name and does
	  not contain user code)
	  @param ns the subnamespace to add to the namespace
       */
      DLLEXPORT void addInitialNamespace(QoreNamespace *ns);

      //! returns a deep copy of the namespace
      /** @param po parse options to use when copying the namespace
       */
      DLLEXPORT QoreNamespace *copy(int po = 0) const;

      // info
      //! gets a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
      /**
	 @see QoreHashNode
	 @see QoreListNode
	 @return a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
       */
      DLLEXPORT QoreHashNode *getClassInfo() const;

      //! a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
      /**
	 @see QoreHashNode
	 @see QoreListNode
	 @return a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
       */      
      DLLEXPORT QoreHashNode *getConstantInfo() const;

      //! returns a hash giving information about the definitions in the namespace
      /** the return value has the following keys: "constants", "classes", and "subnamespaces"
	  having as values the result of calling QoreNamespace::getConstantInfo(), 
	  QoreNamespace::getClassInfo(), and a hash of subnamespace names having as values
	  the result of calling this function on each, respectively.
	  @return a hash giving information about the definitions in the namespace
      */
      DLLEXPORT QoreHashNode *getInfo() const;

      //! returns the name of the namespace
      /**
	 @return the name of the namespace
      */
      DLLEXPORT const char *getName() const;

      //! adds a namespace to the pending namespace tree
      /** namespace objects are merged if there is a namespace with the same name;
	  duplicate objects are checked and user code is initialized
	  @param ns the namespace to add
       */
      DLLEXPORT void addNamespace(QoreNamespace *ns);

      // parse-only interfaces are not exported
      DLLLOCAL QoreNamespace();
      DLLLOCAL void addClass(NamedScope *n, QoreClass *oc);
      DLLLOCAL void addConstant(NamedScope *name, AbstractQoreNode *value);
      DLLLOCAL void addClass(QoreClass *oc);
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstants();
      DLLLOCAL void parseRollback();
      DLLLOCAL void parseCommit();
      DLLLOCAL void setName(const char *nme);

      //! destroys the object and frees all associated memory (not exported)
      DLLLOCAL void purge();
};

//! the root namespace of a QoreProgram object
/** is a specialization of QoreNamespace that provides functionality specific to the root namespace
    this class' constructor and destructors are private, so the class may change without affecting the library's ABI
    @see QoreNamespace
 */
class RootQoreNamespace : public QoreNamespace {
   private:
      QoreNamespace *qoreNS;

      // each class that can have system constant objects must be
      // dereferenced last when the namespace is destroyed
      QoreClass *File;

      DLLLOCAL QoreNamespace *rootResolveNamespace(NamedScope *nscope);
      DLLLOCAL void addQoreNamespace(QoreNamespace *qns);
      // private constructor
      DLLLOCAL RootQoreNamespace(QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl, QoreClassList *pend_ocl, ConstantList *pend_cl, QoreNamespaceList *pend_nsl);
      DLLLOCAL QoreClass *rootFindScopedClassWithMethod(NamedScope *nscope, int *matched) const;
      DLLLOCAL QoreClass *rootFindScopedClass(NamedScope *name, int *matched) const;
      DLLLOCAL QoreClass *rootFindChangeClass(const char *name);
      DLLLOCAL AbstractQoreNode *rootFindConstantValue(const char *name) const;
      DLLLOCAL AbstractQoreNode *rootFindScopedConstantValue(NamedScope *name, int *matched) const;

   public:
      //! returns a pointer to the QoreNamespace for the "Qore" namespace
      /**
	 @return a pointer to the QoreNamespace for the "Qore" namespace
       */
      DLLEXPORT QoreNamespace *rootGetQoreNamespace() const;

      DLLLOCAL RootQoreNamespace(QoreNamespace **QoreNS);
      DLLLOCAL ~RootQoreNamespace();
      DLLLOCAL RootQoreNamespace *copy(int po = 0) const;
      DLLLOCAL QoreClass *rootFindClass(const char *name) const;
      DLLLOCAL void rootAddClass(NamedScope *name, QoreClass *oc);
      DLLLOCAL void rootAddConstant(NamedScope *name, AbstractQoreNode *value);
      DLLLOCAL AbstractQoreNode *findConstantValue(NamedScope *name, int level) const;
      DLLLOCAL AbstractQoreNode *findConstantValue(const char *name, int level) const;
      DLLLOCAL QoreClass *parseFindClass(const char *name) const;
      DLLLOCAL QoreClass *parseFindScopedClass(NamedScope *name) const;
      DLLLOCAL QoreClass *parseFindScopedClassWithMethod(NamedScope *name) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveSimpleConstant(AbstractQoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int parseInitConstantValue(AbstractQoreNode **, int level);
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveScopedConstant(AbstractQoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int addMethodToClass(NamedScope *name, QoreMethod *qcmethod, class BCAList *bcal);
};

#endif // QORE_NAMESPACE_H
