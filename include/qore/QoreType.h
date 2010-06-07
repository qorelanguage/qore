/* -*- mode: c++; indent-tabs-mode: nil -*- */
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
DLLEXPORT extern QoreBigIntNode *Zero;

DLLEXPORT extern QoreString NothingTypeString, NullTypeString, TrueString, 
   FalseString, EmptyHashString, EmptyListString;

class QoreTypeInfo;
DLLEXPORT extern const QoreTypeInfo *anyTypeInfo, 
   *bigIntTypeInfo, 
   *floatTypeInfo, 
   *boolTypeInfo, 
   *stringTypeInfo, 
   *binaryTypeInfo, 
   *dateTypeInfo, 
   *objectTypeInfo, 
   *hashTypeInfo, 
   *listTypeInfo, 
   *nothingTypeInfo, 
   *nullTypeInfo, 
   *runTimeClosureTypeInfo,
   *callReferenceTypeInfo, 
   *referenceTypeInfo, 
   *userReferenceTypeInfo,
   *codeTypeInfo,              // either closure or callref
   *softBigIntTypeInfo,        // converts to int from float, string, and bool
   *softFloatTypeInfo,         // converts to float from int, string, and bool
   *softBoolTypeInfo,          // converts to bool from int, float, and string
   *softStringTypeInfo,        // converts to string from int, float, and bool
   *somethingTypeInfo,         // i.e. not "NOTHING"
   *dataTypeInfo;              // either string or binary

DLLEXPORT qore_type_t get_next_type_id();

DLLEXPORT bool compareHard(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);
DLLEXPORT bool compareSoft(const AbstractQoreNode *l, const AbstractQoreNode *r, ExceptionSink *xsink);

static inline AbstractQoreNode *boolean_false() {
   return &False;
}

static inline AbstractQoreNode *boolean_true() {
   return &True;
}

static inline QoreBigIntNode *zero() {
   Zero->ref();
   return Zero;
}

static inline QoreFloatNode *zero_float() {
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

static inline QoreListNode *empty_list() {
   emptyList->ref();
   return emptyList;
}

static inline QoreHashNode *empty_hash() {
   emptyHash->ref();
   return emptyHash;
}

//! return type for type matching functions
enum qore_type_result_e {
   QTI_IGNORE      = -2,  //!< for internal use only
   QTI_UNASSIGNED  = -1,  //!< for internal use only

   QTI_NOT_EQUAL   =  0,  //!< types do not match
   QTI_AMBIGUOUS   =  1,  //!< types match, but are not identical
   QTI_IDENT       =  2   //!< types match perfectly
};

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

   //! this function must be reimplemented if setInputFilter() is called
   DLLLOCAL virtual AbstractQoreNode *acceptInputImpl(bool obj, int param_num, const char *param_name, AbstractQoreNode *n, ExceptionSink *xsink) const;

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
   //! add another type that the type accepts
   DLLEXPORT void addAcceptsType(const QoreTypeInfo *n_typeInfo);
   //! set a flag that means the type is equivalent to an integer
   DLLEXPORT void setInt();
   //! set a flag that means that if the return type is matched on input, it matches with QTI_IDENT
   DLLEXPORT void setExactReturn();
   //! set a flag that means that acceptInputImpl() has been reimplemented and should be used
   DLLEXPORT void setInputFilter();
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
};

DLLEXPORT int testObjectClassAccess(const QoreObject *obj, const QoreClass *classtoaccess);

DLLEXPORT const QoreClass *typeInfoGetUniqueReturnClass(const QoreTypeInfo *typeInfo);
DLLEXPORT bool typeInfoHasType(const QoreTypeInfo *typeInfo);
DLLEXPORT const char *typeInfoGetName(const QoreTypeInfo *typeInfo);
DLLEXPORT qore_type_result_e typeInfoAcceptsType(const QoreTypeInfo *typeInfo, const QoreTypeInfo *otherTypeInfo);
DLLEXPORT qore_type_result_e typeInfoReturnsType(const QoreTypeInfo *typeInfo, const QoreTypeInfo *otherTypeInfo);


#endif // _QORE_QORETYPE_H
