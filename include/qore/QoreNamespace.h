/*
  Namespace.h

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

//! contains constants, classes, and subnamespaces in QoreProgram objects
class QoreNamespace
{
      friend class QoreNamespaceList;
      friend class RootQoreNamespace;

  private:
      struct qore_ns_private *priv; // private implementation

      DLLLOCAL class AbstractQoreNode *parseMatchScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *parseMatchScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL class QoreNamespace *parseMatchNamespace(class NamedScope *nscope, int *matched) const;
      DLLLOCAL void assimilate(class QoreNamespace *ns);
      DLLLOCAL class QoreNamespace *findNamespace(const char *name) const;
      DLLLOCAL class QoreNamespace *resolveNameScope(class NamedScope *name) const;
      DLLLOCAL class AbstractQoreNode *getConstantValue(const char *name) const;
      DLLLOCAL QoreNamespace(const char *n, class QoreClassList *ocl, class ConstantList *cl, class QoreNamespaceList *nnsl);
      DLLLOCAL QoreNamespace(class QoreClassList *ocl, class ConstantList *cl, class QoreNamespaceList *nnsl);

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
      DLLEXPORT void addConstant(const char *name, class AbstractQoreNode *value);

      //! adds a class to a namespace
      /**
	 @param oc the class to add to the namespace
      */
      DLLEXPORT void addSystemClass(class QoreClass *oc);

      //! adds a subnamespace to the namespace
      /**
	 @param ns the subnamespace to add to the namespace
       */
      DLLEXPORT void addInitialNamespace(class QoreNamespace *ns);

      //! returns a deep copy of the namespace
      /** @param po parse options to use when copying the namespace
       */
      DLLEXPORT class QoreNamespace *copy(int po = 0) const;

      // info
      //! gets a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
      /**
	 @see QoreHashNode
	 @see QoreListNode
	 @return a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
       */
      DLLEXPORT class QoreHashNode *getClassInfo() const;

      //! a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
      /**
	 @see QoreHashNode
	 @see QoreListNode
	 @return a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
       */      
      DLLEXPORT class QoreHashNode *getConstantInfo() const;

      //! returns a hash giving information about the definitions in the namespace
      /** the return value has the following keys: "constants", "classes", and "subnamespaces"
	  having as values the result of calling QoreNamespace::getConstantInfo(), 
	  QoreNamespace::getClassInfo(), and a hash of subnamespace names having as values
	  the result of calling this function on each, respectively.
	  @return a hash giving information about the definitions in the namespace
      */
      DLLEXPORT class QoreHashNode *getInfo() const;

      //! returns the name of the namespace
      /**
	 @return the name of the namespace
      */
      DLLEXPORT const char *getName() const;

      // parse-only interfaces are not exported
      DLLLOCAL QoreNamespace();
      DLLLOCAL void addClass(class NamedScope *n, class QoreClass *oc);
      DLLLOCAL void addConstant(class NamedScope *name, class AbstractQoreNode *value);
      DLLLOCAL void addClass(class QoreClass *oc);
      DLLLOCAL void addNamespace(class QoreNamespace *ns);
      DLLLOCAL void parseInit();
      DLLLOCAL void parseInitConstants();
      DLLLOCAL void parseRollback();
      DLLLOCAL void parseCommit();
      DLLLOCAL void setName(const char *nme);
};

//! the root namespace of a QoreProgram object
/** is a specialization of QoreNamespace that provides functionality specific to the root namespace
    @see QoreNamespace
 */
class RootQoreNamespace : public QoreNamespace
{
   private:
      class QoreNamespace *qoreNS;

      DLLLOCAL class QoreNamespace *rootResolveNamespace(class NamedScope *nscope);
      DLLLOCAL void addQoreNamespace(class QoreNamespace *qns);
      // private constructor
      DLLLOCAL RootQoreNamespace(class QoreClassList *ocl, class ConstantList *cl, class QoreNamespaceList *nnsl);

   public:
      //! returns a pointer to the QoreNamespace for the "Qore" namespace
      /**
	 @return a pointer to the QoreNamespace for the "Qore" namespace
       */
      DLLEXPORT class QoreNamespace *rootGetQoreNamespace() const;

      DLLLOCAL RootQoreNamespace(class QoreNamespace **QoreNS);
      DLLLOCAL ~RootQoreNamespace();
      DLLLOCAL class RootQoreNamespace *copy(int po = 0) const;
      DLLLOCAL class QoreClass *rootFindClass(const char *name) const;
      DLLLOCAL class QoreClass *rootFindChangeClass(const char *name);
      DLLLOCAL class AbstractQoreNode *rootFindConstantValue(const char *name) const;
      DLLLOCAL class AbstractQoreNode *rootFindScopedConstantValue(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClass(class NamedScope *name, int *matched) const;
      DLLLOCAL class QoreClass *rootFindScopedClassWithMethod(class NamedScope *nscope, int *matched) const;
      DLLLOCAL void rootAddClass(class NamedScope *name, class QoreClass *oc);
      DLLLOCAL void rootAddConstant(class NamedScope *name, class AbstractQoreNode *value);
      DLLLOCAL class AbstractQoreNode *findConstantValue(class NamedScope *name, int level) const;
      DLLLOCAL class AbstractQoreNode *findConstantValue(const char *name, int level) const;
      DLLLOCAL class QoreClass *parseFindClass(const char *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClass(class NamedScope *name) const;
      DLLLOCAL class QoreClass *parseFindScopedClassWithMethod(class NamedScope *name) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveSimpleConstant(class AbstractQoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int parseInitConstantValue(class AbstractQoreNode **, int level);
      // returns 0 for success, non-zero for error
      DLLLOCAL int resolveScopedConstant(class AbstractQoreNode **, int level) const;
      // returns 0 for success, non-zero for error
      DLLLOCAL int addMethodToClass(class NamedScope *name, class QoreMethod *qcmethod, class BCAList *bcal);
};

#endif // QORE_NAMESPACE_H
