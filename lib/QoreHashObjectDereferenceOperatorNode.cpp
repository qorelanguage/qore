/*
  QoreHashObjectDereferenceOperatorNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/typed_hash_decl_private.h"

QoreString QoreHashObjectDereferenceOperatorNode::op_str(". or {} operator expression");

AbstractQoreNode* QoreHashObjectDereferenceOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& returnTypeInfo) {
   // turn off "return value ignored" flags
   pflag &= ~(PF_RETURN_VALUE_IGNORED);

   assert(!typeInfo);
   assert(!returnTypeInfo);

   const QoreTypeInfo* lti = 0, *rti = 0;

   left = left->parseInit(oflag, pflag, lvids, lti);
   right = right->parseInit(oflag, pflag & ~(PF_FOR_ASSIGNMENT), lvids, rti);

   bool for_assignment = pflag & PF_FOR_ASSIGNMENT;

   printd(5, "QoreHashObjectDereferenceOperatorNode::parseInitImpl() l: %p %s (%s) r: %p %s\n", lti, QoreTypeInfo::getName(lti), QoreTypeInfo::getUniqueReturnClass(lti) ? QoreTypeInfo::getUniqueReturnClass(lti)->getName() : "n/a", rti, QoreTypeInfo::getName(rti));

   if (for_assignment && left && check_lvalue(left))
      parse_error(loc, "expression used for assignment requires an lvalue, got '%s' instead", left->getTypeName());

   const QoreTypeInfo* complexKeyTypeInfo = nullptr;

   if (QoreTypeInfo::hasType(lti)) {
      bool can_be_obj = QoreTypeInfo::parseAccepts(objectTypeInfo, lti);
      bool can_be_hash = QoreTypeInfo::parseAccepts(hashTypeInfo, lti);

      bool is_obj = can_be_obj ? QoreTypeInfo::isType(lti, NT_OBJECT) : false;
      bool is_hash = can_be_hash ? QoreTypeInfo::isType(lti, NT_HASH) : false;

      const QoreClass *qc = QoreTypeInfo::getUniqueReturnClass(lti);
      // see if we can check for legal access
      if (qc && right) {
         qore_type_t rt = right->getType();
         if (rt == NT_STRING) {
            const char* member = reinterpret_cast<const QoreStringNode*>(right)->getBuffer();
            qore_class_private::parseCheckMemberAccess(*qc, loc, member, returnTypeInfo, pflag);
         }
         else if (rt == NT_LIST) { // check object slices as well if strings are available
            ConstListIterator li(reinterpret_cast<const QoreListNode*>(right));
            while (li.next()) {
               if (li.getValue() && li.getValue()->getType() == NT_STRING) {
                  const char* member = reinterpret_cast<const QoreStringNode*>(li.getValue())->getBuffer();
                  const QoreTypeInfo* mti = 0;
                  qore_class_private::parseCheckMemberAccess(*qc, loc, member, mti, pflag);
               }
            }
         }
      }
      else {
         const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(lti);
         if (hd) {
            if (right) {
               qore_type_t rt = right->getType();
               if (rt == NT_STRING) {
                  const char* member = reinterpret_cast<const QoreStringNode*>(right)->c_str();
                  typed_hash_decl_private::get(*hd)->parseCheckMemberAccess(loc, member, returnTypeInfo, pflag);
               }
               else if (rt == NT_LIST) { // check object slices as well if strings are available
                  ConstListIterator li(reinterpret_cast<const QoreListNode*>(right));
                  while (li.next()) {
                     if (li.getValue() && li.getValue()->getType() == NT_STRING) {
                        const char* member = reinterpret_cast<const QoreStringNode*>(li.getValue())->c_str();
                        const QoreTypeInfo* mti = nullptr;
                        typed_hash_decl_private::get(*hd)->parseCheckMemberAccess(loc, member, mti, pflag);
                     }
                  }
               }
            }
         }
         else {
            complexKeyTypeInfo = QoreTypeInfo::getUniqueReturnComplexHash(lti);
         }
      }

      // if we are taking a slice of an object or a hash, then the return type is a hash
      if (QoreTypeInfo::hasType(rti)) {
         if (QoreTypeInfo::isType(rti, NT_LIST) && (is_obj || is_hash))
            returnTypeInfo = complexKeyTypeInfo ? lti : hashTypeInfo;
         else if (complexKeyTypeInfo && !QoreTypeInfo::parseReturns(rti, NT_LIST))
            returnTypeInfo = complexKeyTypeInfo;
      }

      // if we are trying to convert to a hash
      if (for_assignment) {
         // only throw a parse exception if parse exceptions are enabled
         if (!can_be_hash
            && !can_be_obj
            && getProgram()->getParseExceptionSink()) {
            QoreStringNode* edesc = new QoreStringNode("cannot convert lvalue defined as ");
            QoreTypeInfo::getThisType(lti, *edesc);
            edesc->sprintf(" to a hash using the '.' or '{}' operator in an assignment expression");
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
         }
      }
      else if (!can_be_hash && !can_be_obj) {
         QoreStringNode* edesc = new QoreStringNode("left-hand side of the expression with the '.' or '{}' operator is ");
         QoreTypeInfo::getThisType(lti, *edesc);
         edesc->concat(" and so this expression will always return NOTHING; the '.' or '{}' operator only returns a value with hashes and objects");
         qore_program_private::makeParseWarning(getProgram(), loc, QP_WARN_INVALID_OPERATION, "INVALID-OPERATION", edesc);
         returnTypeInfo = nothingTypeInfo;
      }
   }

   //printd(5, "QoreHashObjectDereferenceOperatorNode::parseInitImpl() rightTypeInfo: %s !rightTypeInfo->canConvertToScalar(): %d !listTypeInfo->parseAccepts(rightTypeInfo): %d\n", QoreTypeInfo::getName(rti), !QoreTypeInfo::canConvertToScalar(rti), !QoreTypeInfo::parseAccepts(listTypeInfo, rti));

   //printd(5, "QoreHashObjectDereferenceOperatorNode::parseInitImpl() l: '%s' r: '%s' -> '%s'\n", QoreTypeInfo::getName(lti), QoreTypeInfo::getName(rti), QoreTypeInfo::getName(returnTypeInfo));

   // issue a warning if the right side of the expression cannot be converted to a string
   // and can not be a list (for a slice)
   if (!QoreTypeInfo::canConvertToScalar(rti) && !QoreTypeInfo::parseAccepts(listTypeInfo, rti))
      // FIXME: should be "non-string-or-list warning"
      rti->doNonStringWarning(loc, "the right side of the expression with the '.' or '{}' operator is ");

   typeInfo = returnTypeInfo;
   return this;
}

QoreValue QoreHashObjectDereferenceOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder lh(left, xsink);
   if (*xsink)
      return QoreValue();
   ValueEvalRefHolder rh(right, xsink);
   if (*xsink)
      return QoreValue();

   if (lh->getType() == NT_HASH) {
      const QoreHashNode* h = lh->get<const QoreHashNode>();

      if (rh->getType() == NT_LIST)
         return h->getSlice(rh->get<const QoreListNode>(), xsink);

      QoreStringNodeValueHelper key(*rh);
      QoreValue v = h->getValueKeyValue(**key, xsink);
      return *xsink ? v.refSelf() : QoreValue();
   }
   if (lh->getType() != NT_OBJECT)
      return QoreValue();

   QoreObject* o = const_cast<QoreObject*>(lh->get<const QoreObject>());

   if (rh->getType() == NT_LIST)
      return o->getSlice(rh->get<const QoreListNode>(), xsink);

   QoreStringNodeValueHelper key(*rh);
   ValueHolder rv(o->evalMember(*key, xsink), xsink);
   return *xsink ? QoreValue() : rv.release();
}
