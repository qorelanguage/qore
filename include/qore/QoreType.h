/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreType.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORETYPE_H

#define _QORE_QORETYPE_H

#include <qore/common.h>
#include <qore/node_types.h>

#include <map>

// global default values
DLLEXPORT extern QoreListNode* emptyList;
DLLEXPORT extern QoreHashNode* emptyHash;
DLLEXPORT extern QoreStringNode* NullString;
DLLEXPORT extern DateTimeNode* ZeroDate;
DLLEXPORT extern QoreBigIntNode* Zero;
DLLEXPORT extern QoreFloatNode* ZeroFloat;
DLLEXPORT extern QoreNumberNode* ZeroNumber, * InfinityNumber, * NaNumber, * piNumber;

DLLEXPORT extern QoreString NothingTypeString, NullTypeString, TrueString,
   FalseString, EmptyHashString, EmptyListString;

class QoreTypeInfo;
DLLEXPORT extern const QoreTypeInfo* anyTypeInfo,
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
   *numberTypeInfo,
   *runTimeClosureTypeInfo,
   *callReferenceTypeInfo,
   *referenceTypeInfo,
   *userReferenceTypeInfo,
   *codeTypeInfo,                 // either closure or callref
   *softBigIntTypeInfo,           // converts to int from float, string, and bool
   *softFloatTypeInfo,            // converts to float from int, string, and bool
   *softNumberTypeInfo,           // xxx
   *softBoolTypeInfo,             // converts to bool from int, float, and string
   *softStringTypeInfo,           // converts to string from int, float, and bool
   *softDateTypeInfo,             // converts to date from int, float, bool, and string
   *softListTypeInfo,             // converts NOTHING -> empty list, list -> the same list, and everything else: list(arg)
   *somethingTypeInfo,            // i.e. not "NOTHING"
   *dataTypeInfo,                 // either string or binary
   *timeoutTypeInfo,              // accepts int or date and returns int giving timeout in milliseconds
   *bigIntOrFloatTypeInfo,        // accepts int or float and returns the same
   *bigIntFloatOrNumberTypeInfo,  // accepts int or float and returns the same
   *floatOrNumberTypeInfo,        // accepts float or number and returns the same

   *bigIntOrNothingTypeInfo,
   *floatOrNothingTypeInfo,
   *numberOrNothingTypeInfo,
   *stringOrNothingTypeInfo,
   *boolOrNothingTypeInfo,
   *binaryOrNothingTypeInfo,
   *objectOrNothingTypeInfo,
   *dateOrNothingTypeInfo,
   *hashOrNothingTypeInfo,
   *listOrNothingTypeInfo,
   *nullOrNothingTypeInfo,
   *codeOrNothingTypeInfo,
   *dataOrNothingTypeInfo,
   *referenceOrNothingTypeInfo,

   *softBigIntOrNothingTypeInfo,
   *softFloatOrNothingTypeInfo,
   *softNumberOrNothingTypeInfo,
   *softBoolOrNothingTypeInfo,
   *softStringOrNothingTypeInfo,
   *softDateOrNothingTypeInfo,
   *softListOrNothingTypeInfo,
   *timeoutOrNothingTypeInfo;

DLLEXPORT qore_type_t get_next_type_id();

//! true = not equal, false = equal
DLLEXPORT bool compareHard(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);
//! true = not equal, false = equal
DLLEXPORT bool compareSoft(const AbstractQoreNode* l, const AbstractQoreNode* r, ExceptionSink* xsink);
//! true = not equal, false = equal
DLLEXPORT bool q_compare_soft(const QoreValue l, const QoreValue r, ExceptionSink* xsink);

static inline AbstractQoreNode* boolean_false() {
   return &False;
}

static inline AbstractQoreNode* boolean_true() {
   return &True;
}

static inline QoreBigIntNode* zero() {
   Zero->ref();
   return Zero;
}

static inline QoreFloatNode* zero_float() {
   ZeroFloat->ref();
   return ZeroFloat;
}

static inline QoreNumberNode* zero_number() {
   ZeroNumber->ref();
   return ZeroNumber;
}

static inline DateTimeNode* zero_date() {
   ZeroDate->ref();
   return ZeroDate;
}

static inline class QoreStringNode* null_string() {
   NullString->ref();
   return NullString;
}

static inline QoreListNode* empty_list() {
   emptyList->ref();
   return emptyList;
}

static inline QoreHashNode* empty_hash() {
   emptyHash->ref();
   return emptyHash;
}

static inline QoreNumberNode* pi_number() {
   piNumber->ref();
   return piNumber;
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
struct QoreValue;

//! helper type to allocate and manage QoreTypeInfo objects (not exported by the library)
/** should be used to allocate and deallocate QoreTypeInfo objects for new types created in modules
 */
class QoreTypeInfoHelper {
   friend class ExternalTypeInfo;

protected:
   ExternalTypeInfo* typeInfo;

   DLLLOCAL QoreTypeInfoHelper(ExternalTypeInfo* n_typeInfo) : typeInfo(n_typeInfo) {
   }

   //! this function must be reimplemented if setInputFilter() is called
   DLLEXPORT virtual bool acceptInputImpl(QoreValue& n, ExceptionSink* xsink) const;

public:
   //! allocates a QoreTypeInfo object with no type information
   DLLEXPORT QoreTypeInfoHelper(const char* n_tname);
   //! allocates a QoreTypeInfo object of the requested type
   DLLEXPORT QoreTypeInfoHelper(qore_type_t id, const char* n_tname);
   //! deallocates the managed QoreTypeInfo object
   DLLEXPORT virtual ~QoreTypeInfoHelper();
   //! returns a pointer to the object
   DLLEXPORT const QoreTypeInfo* getTypeInfo() const;
   //! assigns the typeid to the object
   DLLEXPORT void assign(qore_type_t id);
   //! add another type that the type accepts
   DLLEXPORT void addAcceptsType(const QoreTypeInfo* n_typeInfo);
   //! set a flag that means the type is equivalent to an integer
   DLLEXPORT void setInt();
   //! set a flag that means that if the return type is matched on input, it matches with QTI_AMBIGUOUS instead of QTI_IDENT
   DLLEXPORT void setInexactReturn();
   //! set a flag that means that acceptInputImpl() has been reimplemented and should be used
   DLLEXPORT void setInputFilter();
   //! set a flag so that any NT_INT in an accept list will match any type with is_int set with QTI_AMBIGUOUS
   DLLEXPORT void setIntMatch();

   DLLEXPORT int doAcceptError(bool priv_error, bool obj, int param_num, const char* param_name, AbstractQoreNode* n, ExceptionSink* xsink) const;
};

//! note that the QoreClass object created by this class must be deleted externally
class AbstractQoreClassTypeInfoHelper : public QoreTypeInfoHelper {
protected:
   QoreClass* qc;

public:
   //! allocates a QoreTypeInfo object and creates the QoreClass
   DLLEXPORT AbstractQoreClassTypeInfoHelper(const char* name, int n_domain = QDOM_DEFAULT);
   //! delets the QoreClass object managed if it has not been retrieved
   DLLEXPORT ~AbstractQoreClassTypeInfoHelper();
   //! returns the QoreClass object created and zeros out the class ptr; can only be called once
   DLLEXPORT QoreClass *getClass();
   //! returns true if this object is holding a class pointer, false if not
   DLLEXPORT bool hasClass() const;
};

DLLEXPORT int testObjectClassAccess(const QoreObject *obj, const QoreClass *classtoaccess);

DLLEXPORT const QoreClass *typeInfoGetUniqueReturnClass(const QoreTypeInfo* typeInfo);
DLLEXPORT bool typeInfoHasType(const QoreTypeInfo* typeInfo);
DLLEXPORT const char* typeInfoGetName(const QoreTypeInfo* typeInfo);
DLLEXPORT qore_type_result_e typeInfoAcceptsType(const QoreTypeInfo* typeInfo, const QoreTypeInfo* otherTypeInfo);
DLLEXPORT qore_type_result_e typeInfoReturnsType(const QoreTypeInfo* typeInfo, const QoreTypeInfo* otherTypeInfo);

#endif // _QORE_QORETYPE_H
