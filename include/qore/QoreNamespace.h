/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespace.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  namespaces are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef _QORE_QORENAMESPACE_H

#define _QORE_QORENAMESPACE_H

#include <string.h>
#include <stdlib.h>

#include <string>

//! namespace class handler function type
/** called when a class cannot be found in the namespace
    @param ns namespace pointer
    @param cname unqualified class name to load
    @return 0=class cannot be loaded and mapped, otherwise class pointer from new QoreClass added to ns
 */
typedef QoreClass* (*q_ns_class_handler_t)(QoreNamespace* ns, const char* cname);

//! contains constants, classes, and subnamespaces in QoreProgram objects
class QoreNamespace {
   friend class QoreNamespaceList;
   friend class RootQoreNamespace;
   friend class qore_ns_private;
   friend class qore_root_ns_private;
   friend struct NSOInfoBase;

private:
   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreNamespace(const QoreNamespace&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreNamespace& operator=(const QoreNamespace&);

protected:
   class qore_ns_private* priv; // private implementation

   // protected, function not exported in the API
   DLLLOCAL QoreNamespace(qore_ns_private* p);

public:
   //! creates a namespace with the given name
   /** the name of a subnamespace must be unique in the parent namespace and must not have the same name as a class in the parent namespace either
       @param n the name of the namespace
   */
   DLLEXPORT QoreNamespace(const char* n);

   //! destroys the object and frees all associated memory
   DLLEXPORT ~QoreNamespace();

   //! adds a constant definition to the namespace
   /** use addConstant(const char* name, AbstractQoreNode* value, const QoreTypeInfo* typeInfo) when adding
       constants of externally-defined base (non-class) types; all other types (and all objects) can have
       their type information automatically added
       @param name the name of the constant to add
       @param value the value of the constant
   */
   DLLEXPORT void addConstant(const char* name, AbstractQoreNode* value);

   //! adds a constant definition to the namespace with type information
   /**
      @param name the name of the constant to add
      @param value the value of the constant
      @param typeInfo the type of the constant
      @see QoreTypeInfoHelper
   */
   DLLEXPORT void addConstant(const char* name, AbstractQoreNode* value, const QoreTypeInfo* typeInfo);

   //! adds a class to a namespace
   /**
      @param oc the class to add to the namespace
   */
   DLLEXPORT void addSystemClass(QoreClass* oc);

   //! returns a deep copy of the namespace; DEPRECATED: use copy(int64) instead
   /** @param po parse options to use when copying the namespace
       @return a deep copy of the namespace
    */
   DLLEXPORT QoreNamespace* copy(int po) const;

   //! returns a deep copy of the namespace
   /** @param po parse options to use when copying the namespace
       @return a deep copy of the namespace
    */
   DLLEXPORT QoreNamespace* copy(int64 po = PO_DEFAULT) const;

   //! gets a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
   /**
      @see QoreHashNode
      @see QoreListNode
      @return a hash of all classes in the namespace, the hash keys are the class names and the values are lists of strings giving the method names
   */
   DLLEXPORT QoreHashNode* getClassInfo() const;

   //! a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
   /**
      @see QoreHashNode
      @see QoreListNode
      @return a hash of all constants in the namespace, the hash keys are the constant names and the values are the values of the constants
   */
   DLLEXPORT QoreHashNode* getConstantInfo() const;

   //! returns a hash giving information about the definitions in the namespace
   /** the return value has the following keys: "constants", "classes", and "subnamespaces"
       having as values the result of calling QoreNamespace::getConstantInfo(),
       QoreNamespace::getClassInfo(), and a hash of subnamespace names having as values
       the result of calling this function on each, respectively.
       @return a hash giving information about the definitions in the namespace
   */
   DLLEXPORT QoreHashNode* getInfo() const;

   //! returns the name of the namespace
   /**
      @return the name of the namespace
   */
   DLLEXPORT const char* getName() const;

   //! adds a namespace to the namespace tree
   /** the namespace must be unique, must also not clash with a class name in the same parent namespace
       @param ns the namespace to add, memory is now owned by parent namespace
    */
   DLLEXPORT void addNamespace(QoreNamespace* ns);

   //! adds a subnamespace to the namespace
   /** use this function when the QoreNamespace can be added directly to the tree
       (does not need to be merged with another namespace of the same name and does
       not contain user code)
       @param ns the subnamespace to add to the namespace
   */
   DLLEXPORT void addInitialNamespace(QoreNamespace* ns);

   //! finds a Namespace based on the argument; creates it (or the whole path) if necessary
   /** can only be called in the parse lock
       @param nspath must be a complete path ("ns1::ns2[::ns3...]" to a namespace, which will be found or created in this namespace
       @return the namespace found or created according to the path
    */
   DLLEXPORT QoreNamespace* findCreateNamespacePath(const char* nspath);

   //! finds a class in this namespace, does not search child namespaces
   /** can only be called in the parse lock
       does not call the class handler
       @param cname the class name to find in this namespace, must be unqualified (without a namespace path)
       @return the class found or 0 if not present
    */
   DLLEXPORT QoreClass* findLocalClass(const char* cname) const;

   //! finds a subnamespace in this namespace, does not search child namespaces
   /** can only be called in the parse lock
       @param nsname the subnamespace name to find in this namespace, must be unqualified (without a namespace path)
       @return the namespace found or 0 if not present
    */
   DLLEXPORT QoreNamespace* findLocalNamespace(const char* nsname) const;

   //! sets the namespace class handler
   /** to be called when a class cannot be found in the namespace
       @param class_handler pointer to the class handler function
   */
   DLLEXPORT void setClassHandler(q_ns_class_handler_t class_handler);

   //! returns a pointer to the parent namespace or 0 if there is no parent
   /** @return a pointer to the parent namespace or 0 if there is no parent
    */
   DLLEXPORT const QoreNamespace* getParent() const;

   //! this function must be called before the QoreNamespace object is deleted or a crash could result due if constants and/or class static vars contain objects
   DLLEXPORT void deleteData(ExceptionSink* xsink);

   // adds a function variant
   DLLEXPORT void addBuiltinVariant(const char* name, q_func_n_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   // @deprecated superceded by value version
   DLLEXPORT void addBuiltinVariant(const char* name, q_func_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   // @deprecated superceded by value version
   DLLEXPORT void addBuiltinVariant(const char* name, q_func_int64_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   // @deprecated superceded by value version
   DLLEXPORT void addBuiltinVariant(const char* name, q_func_double_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);

   // @deprecated superceded by value version
   DLLEXPORT void addBuiltinVariant(const char* name, q_func_bool_t f, int64 code_flags = QC_NO_FLAGS, int64 functional_domain = QDOM_DEFAULT, const QoreTypeInfo* returnTypeInfo = 0, unsigned num_params = 0, ...);
};

//! the root namespace of a QoreProgram object
/** is a specialization of QoreNamespace that provides functionality specific to the root namespace
    this class' constructor and destructors are private, so the class may change without affecting the library's ABI
    @see QoreNamespace
*/
class RootQoreNamespace : public QoreNamespace {
   friend class qore_ns_private;
   friend class qore_root_ns_private;
   friend class StaticSystemNamespace;

private:
   DLLLOCAL RootQoreNamespace(class qore_root_ns_private* p);

protected:
   // private implementation
   class qore_root_ns_private* rpriv;

public:
   //! returns a pointer to the QoreNamespace for the "Qore" namespace
   /**
       @return a pointer to the QoreNamespace for the "Qore" namespace; do not delete the object returned
   */
   DLLEXPORT QoreNamespace* rootGetQoreNamespace() const;

   //! destructor is not exported in the library's public API
   DLLLOCAL ~RootQoreNamespace();
};

#endif // QORE_NAMESPACE_H
