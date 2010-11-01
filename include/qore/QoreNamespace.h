/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespace.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
class MethodVariantBase;

//! namespace class handler function type
/** called when a class cannot be found in the namespace
    @param ns namespace pointer
    @param cname unqualified class name to load
    @return 0=class cannot be loaded and mapped, otherwise class pointer from new QoreClass added to ns
 */
typedef QoreClass *(*q_ns_class_handler_t)(QoreNamespace *ns, const char *cname);

//! contains constants, classes, and subnamespaces in QoreProgram objects
class QoreNamespace {
   friend class QoreNamespaceList;
   friend class RootQoreNamespace;

protected:
   struct qore_ns_private *priv; // private implementation

   DLLLOCAL AbstractQoreNode *parseMatchScopedConstantValue(const NamedScope *name, int *matched, const QoreTypeInfo *&typeInfo) const;
   DLLLOCAL QoreClass *parseMatchScopedClass(const NamedScope *name, int *matched);
   DLLLOCAL QoreClass *parseMatchScopedClassWithMethod(const NamedScope *nscope, int *matched);
   DLLLOCAL QoreNamespace *parseMatchNamespace(const NamedScope *nscope, int *matched) const;
   DLLLOCAL void assimilate(QoreNamespace *ns);
   DLLLOCAL QoreNamespace *parseFindLocalNamespace(const char *name) const;
   DLLLOCAL QoreClass *parseFindLocalClass(const char *name) const;
   DLLLOCAL QoreNamespace *resolveNameScope(const NamedScope *name) const;
   DLLLOCAL AbstractQoreNode *getConstantValue(const char *name, const QoreTypeInfo *&typeInfo) const;
   DLLLOCAL QoreNamespace(const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreNamespace(const QoreNamespace&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreNamespace& operator=(const QoreNamespace&);

   //! function not exported in the public API
   DLLLOCAL QoreNamespace(const QoreNamespace &old, int64 po);

public:
   //! creates a namespace with the given name
   /** the name of a subnamespace must be unique in the parent namespace and must not have the same name as a class in the parent namespace either
       @param n the name of the namespace
   */
   DLLEXPORT QoreNamespace(const char *n);

   //! destroys the object and frees all associated memory
   DLLEXPORT ~QoreNamespace();

   //! adds a constant definition to the namespace
   /** use addConstant(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) when adding
       constants of externally-defined base (non-class) types; all other types (and all objects) can have
       their type information automatically added
       @param name the name of the constant to add
       @param value the value of the constant
   */
   DLLEXPORT void addConstant(const char *name, AbstractQoreNode *value);

   //! adds a constant definition to the namespace with type information
   /**
      @param name the name of the constant to add
      @param value the value of the constant
      @param typeInfo the type of the constant
      @see QoreTypeInfoHelper
   */
   DLLEXPORT void addConstant(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo);

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

   //! returns a deep copy of the namespace; DEPRECATED: use copy(int64) instead
   /** @param po parse options to use when copying the namespace
       @return a deep copy of the namespace
    */
   DLLEXPORT QoreNamespace *copy(int po) const;

   //! returns a deep copy of the namespace
   /** @param po parse options to use when copying the namespace
       @return a deep copy of the namespace
    */
   DLLEXPORT QoreNamespace *copy(int64 po = PO_DEFAULT) const;

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

   //! adds a namespace to the namespace tree
   /** the namespace must be unique, must also not clash with a class name in the same parent namespace
       @param ns the namespace to add, memory is now owned by parent namespace
    */
   DLLEXPORT void addNamespace(QoreNamespace *ns);

   //! finds a Namespace based on the argument; creates it (or the whole path) if necessary
   /** can only be called in the parse lock
       @param nspath must be a complete path ("ns1::ns2[::ns3...]" to a namespace, which will be found or created in this namespace
       @return the namespace found or created according to the path
    */
   DLLEXPORT QoreNamespace *findCreateNamespacePath(const char *nspath);

   //! finds a class in this namespace, does not search child namespaces
   /** can only be called in the parse lock
       does not call the class handler
       @param cname the class name to find in this namespace, must be unqualified (without a namespace path)
       @return the class found or 0 if not present
    */
   DLLEXPORT QoreClass *findLocalClass(const char *cname) const;

   //! finds a subnamespace in this namespace, does not search child namespaces
   /** can only be called in the parse lock
       @param nsname the subnamespace name to find in this namespace, must be unqualified (without a namespace path)
       @return the namespace found or 0 if not present
    */
   DLLEXPORT QoreNamespace *findLocalNamespace(const char *nsname) const;

   //! sets the namespace class handler
   /** to be called when a class cannot be found in the namespace
       @param class_handler pointer to the class handler function
   */
   DLLEXPORT void setClassHandler(q_ns_class_handler_t class_handler);

   //! returns a pointer to the parent namespace or 0 if there is no parent
   /** @return a pointer to the parent namespace or 0 if there is no parent
    */
   DLLEXPORT const QoreNamespace *getParent() const;

   // parse-only interfaces are not exported
   DLLLOCAL QoreNamespace();
   DLLLOCAL void addClass(const NamedScope *n, QoreClass *oc);

   DLLLOCAL void parseAddConstant(const NamedScope *name, AbstractQoreNode *value);

   DLLLOCAL void addClass(QoreClass *oc);
   DLLLOCAL void parseAddNamespace(QoreNamespace *ns);
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
protected:
   QoreNamespace *qoreNS;

   DLLLOCAL QoreNamespace *rootResolveNamespace(const NamedScope *nscope);
   DLLLOCAL void addQoreNamespace(QoreNamespace *qns);
   // private constructor
   DLLLOCAL RootQoreNamespace(QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl);
   // private constructor
   DLLLOCAL RootQoreNamespace();
   DLLLOCAL QoreClass *rootFindScopedClassWithMethod(const NamedScope *nscope, int *matched);
   DLLLOCAL QoreClass *rootFindScopedClass(const NamedScope *name, int *matched);
   DLLLOCAL QoreClass *rootFindChangeClass(const char *name);
   DLLLOCAL AbstractQoreNode *rootFindConstantValue(const char *name, const QoreTypeInfo *&typeInfo) const;
   DLLLOCAL AbstractQoreNode *rootFindScopedConstantValue(const NamedScope *name, int *matched, const QoreTypeInfo *&typeInfo) const;

public:
   //! returns a pointer to the QoreNamespace for the "Qore" namespace
   /**
      @return a pointer to the QoreNamespace for the "Qore" namespace
   */
   DLLEXPORT QoreNamespace *rootGetQoreNamespace() const;

   DLLLOCAL RootQoreNamespace(QoreNamespace *&QoreNS, int64 po = PO_DEFAULT);
   DLLLOCAL ~RootQoreNamespace();
   DLLLOCAL RootQoreNamespace *copy(int64 po = PO_DEFAULT) const;
   DLLLOCAL QoreClass *rootFindClass(const char *name) const;
   DLLLOCAL void rootAddClass(const NamedScope *name, QoreClass *oc);
   DLLLOCAL void rootAddConstant(const NamedScope *name, AbstractQoreNode *value);
   DLLLOCAL AbstractQoreNode *findConstantValue(const NamedScope *name, int level, const QoreTypeInfo *&typeInfo) const;
   DLLLOCAL AbstractQoreNode *findConstantValue(const char *name, int level, const QoreTypeInfo *&typeInfo) const;
   DLLLOCAL QoreClass *parseFindClass(const char *name) const;
   DLLLOCAL QoreClass *parseFindScopedClass(const NamedScope *name);
   DLLLOCAL QoreClass *parseFindScopedClassWithMethod(const NamedScope *name);
   // returns 0 for success, non-zero for error
   DLLLOCAL int resolveSimpleConstant(AbstractQoreNode **, int level, const QoreTypeInfo *&) const;
   // returns 0 for success, non-zero for error
   DLLLOCAL int resolveScopedConstant(AbstractQoreNode **, int level, const QoreTypeInfo *&) const;
   // returns 0 for success, non-zero for error
   DLLLOCAL int addMethodToClass(const NamedScope *name, MethodVariantBase *qcmethod, bool static_flag);
};

#endif // QORE_NAMESPACE_H
