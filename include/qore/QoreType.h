/*
  QoreType.h
  
  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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

#ifndef _QORE_QORETYPE_H

#define _QORE_QORETYPE_H

#include <qore/common.h>
#include <qore/node_types.h>

#include <map>

// global default values
DLLEXPORT extern QoreListNode *emptyList;
DLLEXPORT extern QoreHashNode *emptyHash;
DLLEXPORT extern QoreStringNode *NullString;
DLLEXPORT extern DateTimeNode *ZeroDate;

DLLEXPORT extern QoreString NothingTypeString, NullTypeString, TrueString, 
   FalseString, EmptyHashString, EmptyListString;

class QoreTypeInfo;
DLLEXPORT extern const QoreTypeInfo *bigIntTypeInfo, *floatTypeInfo, *boolTypeInfo, 
   *stringTypeInfo, *binaryTypeInfo, *dateTypeInfo, *objectTypeInfo, *hashTypeInfo, 
   *listTypeInfo, *nothingTypeInfo, *nullTypeInfo, *runTimeClosureTypeInfo,
   *callReferenceTypeInfo;

DLLEXPORT qore_type_t get_next_type_id();

DLLLOCAL void init_qore_types();
DLLLOCAL void delete_qore_types();

DLLEXPORT bool compareHard(const AbstractQoreNode *l, const AbstractQoreNode *r, class ExceptionSink *xsink);
DLLEXPORT bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, class ExceptionSink *xsink);

static inline AbstractQoreNode *boolean_false() {
   return &False;
}

static inline AbstractQoreNode *boolean_true() {
   return &True;
}

static inline class QoreBigIntNode *zero() {
   return new QoreBigIntNode();
}

static inline class AbstractQoreNode *zero_float() {
   return new QoreFloatNode(0.0);
}

static inline DateTimeNode *zero_date() {
   ZeroDate->ref();
   return ZeroDate;
}

static inline class QoreStringNode *null_string() {
   NullString->ref();
   return NullString;
}

//! used for return values when checking types with functions that return numeric codes; only the first 2 should be returned from types implemented by external modules
#define QTI_NOT_EQUAL   0  //!< not equal
#define QTI_AMBIGUOUS   1  //!< the types can be converted to the target type
#define QTI_IDENT       2  //!< the types are identical
#define QTI_RECHECK     3  //!< possibly not equal; must be rechecked after types are resolved

//! this class is private; not exported
class ExternalTypeInfo;

//! helper type to allocate and manage QoreTypeInfo objects (not exported by the library)
/** should be used to allocate and deallocate QoreTypeInfo objects for new types created in modules
 */
class QoreTypeInfoHelper {
protected:
   ExternalTypeInfo *typeInfo;

   DLLLOCAL QoreTypeInfoHelper(ExternalTypeInfo *n_typeInfo) : typeInfo(n_typeInfo) {
   }

public:
   //! allocates a QoreTypeInfo object with no type information
   DLLEXPORT QoreTypeInfoHelper(const char *n_tname);
   //! allocates a QoreTypeInfo object of the requested type
   DLLEXPORT QoreTypeInfoHelper(qore_type_t id, const char *n_tname);
   //! deallocates the managed QoreTypeInfo object
   DLLEXPORT virtual ~QoreTypeInfoHelper();
   //! returns a pointer to the object
   DLLEXPORT const QoreTypeInfo *getTypeInfo() const;
   //! assigns the typeid to the object
   DLLEXPORT void assign(qore_type_t id);

   DLLEXPORT virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const;
   DLLEXPORT virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const;
   DLLEXPORT virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const;
};

class AbstractQoreClassTypeInfoHelper : public QoreTypeInfoHelper {
protected:
   QoreClass *qc;

public:
   //! allocates a QoreTypeInfo object and creates the QoreClass
   DLLEXPORT AbstractQoreClassTypeInfoHelper(const char *name, int n_domain = QDOM_DEFAULT);
   //! delets the QoreClass object managed if it has not been retrieved
   DLLEXPORT ~AbstractQoreClassTypeInfoHelper();
   //! returns the QoreClass object created and zeros out the internal pointer (can only be called once)
   DLLEXPORT QoreClass *getClass();
   //! returns true if the object still holds the class, false if not
   DLLEXPORT bool hasClass() const;

   //! must be reimplemented in derived class
   DLLEXPORT virtual bool checkTypeInstantiationImpl(AbstractQoreNode *&n, ExceptionSink *xsink) const = 0;
   //! must be reimplemented in derived class
   DLLEXPORT virtual int testTypeCompatibilityImpl(const AbstractQoreNode *n) const = 0;
   //! must be reimplemented in derived class
   DLLEXPORT virtual int parseEqualImpl(const QoreTypeInfo *typeInfo) const = 0;
};

DLLEXPORT int testObjectClassAccess(const QoreObject *obj, const QoreClass *classtoaccess);
DLLEXPORT const QoreClass *typeInfoGetClass(const QoreTypeInfo *typeInfo);
DLLEXPORT qore_type_t typeInfoGetType(const QoreTypeInfo *typeInfo);
DLLEXPORT const char *typeInfoGetName(const QoreTypeInfo *typeInfo);

#endif // _QORE_QORETYPE_H
