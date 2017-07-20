/*
  QoreCastOperatorNode.cpp

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
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/QoreHashNodeIntern.h"

QoreString QoreParseCastOperatorNode::cast_str("cast operator expression");

// if del is true, then the returned QoreString* should be deleted, if false, then it must not be
QoreString* QoreParseCastOperatorNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = false;
   return &cast_str;
}

int QoreParseCastOperatorNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.concat(&cast_str);
   return 0;
}

AbstractQoreNode* QoreParseCastOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
   assert(!typeInfo);

   const QoreTypeInfo* expTypeInfo = nullptr;
   if (exp)
      exp = exp->parseInit(oflag, pflag, lvids, expTypeInfo);

   // check special cases
   if (pti->cscope->size() == 1 && pti->subtypes.empty()) {
      // check special case of cast<object>(...)
      if (!strcmp(pti->cscope->ostr, "object")) {
         // if the class is "object", then set qc = 0 to use as a catch-all and generic "cast to object"
         if (QoreTypeInfo::parseReturns(typeInfo, NT_OBJECT) == QTI_NOT_EQUAL)
            parse_error(loc, "cast<object>(%s) is invalid; cannot cast from %s to object", QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(typeInfo));
         typeInfo = objectTypeInfo;
         if (exp) {
            ReferenceHolder<> holder(this, nullptr);
            return new QoreClassCastOperatorNode(loc, nullptr, takeExp());
         }
         // parse exception already raised; current expression invalid
         return this;
      }
      // check special case of cast<hash>(...)
      if (!strcmp(pti->cscope->ostr, "hash")) {
         if (QoreTypeInfo::parseReturns(typeInfo, NT_HASH) == QTI_NOT_EQUAL)
            parse_error(loc, "cast<hash>(%s) is invalid; cannot cast from %s to hash", QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(typeInfo));
         typeInfo = hashTypeInfo;
         if (exp) {
            ReferenceHolder<> holder(this, nullptr);
            return new QoreHashDeclCastOperatorNode(loc, nullptr, takeExp());
         }
         // parse exception already raised; current expression invalid
         return this;
      }
   }

   typeInfo = QoreParseTypeInfo::resolveAndDelete(pti, loc);
   pti = nullptr;

   // parse exception already raised; current expression invalid
   if (!exp)
      return this;

   {
      const QoreClass* qc = QoreTypeInfo::getUniqueReturnClass(typeInfo);
      if (qc) {
         if (QoreTypeInfo::parseReturns(expTypeInfo, qc) == QTI_NOT_EQUAL)
            parse_error(loc, "cast<%s>(%s) is invalid; cannot cast from %s to %s", QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(typeInfo));
         else {
            assert(exp);
            ReferenceHolder<> holder(this, nullptr);
            return new QoreClassCastOperatorNode(loc, nullptr, takeExp());
         }
         return this;
      }
   }

   {
      const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(typeInfo);
      if (hd) {
         qore_type_result_e r = QoreTypeInfo::parseReturns(expTypeInfo, NT_HASH);
         if (r == QTI_NOT_EQUAL)
             parse_error(loc, "cast<%s>(%s) is invalid; cannot cast from %s to (hashdecl) %s", QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(typeInfo));

         bool runtime_check = false;
         typed_hash_decl_private::get(*hd)->parseCheckHashDeclInitialization(loc, expTypeInfo, exp, "cast<> operation", runtime_check, false);

         typeInfo = hd->getTypeInfo();
         if (exp) {
            ReferenceHolder<> holder(this, nullptr);
            return new QoreHashDeclCastOperatorNode(loc, hd, takeExp());
         }
      }
   }

   {
      const QoreTypeInfo* ti = QoreTypeInfo::getUniqueReturnComplexHash(typeInfo);
      if (ti) {
         qore_type_result_e r = QoreTypeInfo::parseReturns(expTypeInfo, NT_HASH);
         if (r == QTI_NOT_EQUAL)
             parse_error(loc, "cast<%s>(%s) is invalid; cannot cast from %s to hash<string, %s>", QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(expTypeInfo), QoreTypeInfo::getName(ti));

         // check for cast<> compatibility
         qore_hash_private::parseCheckComplexHashInitialization(loc, ti, expTypeInfo, exp, "cast to", false);

         if (exp) {
            ReferenceHolder<> holder(this, nullptr);
            return new QoreComplexHashCastOperatorNode(loc, typeInfo, takeExp());
         }
      }
   }

   parse_error(loc, "cannot cast<> to type '%s'", QoreTypeInfo::getName(typeInfo));;
   return this;
}

QoreValue QoreClassCastOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder rv(exp, xsink);
   if (*xsink)
      return QoreValue();

   if (rv->getType() != NT_OBJECT) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to %s'%s'", rv->getTypeName(), qc ? "class " : "", qc ? qc->getName() : "object");
      return QoreValue();
   }

   const QoreObject* obj = rv->get<const QoreObject>();
   if (qc) {
      const QoreClass* oc = obj->getClass();
      bool priv;
      const QoreClass* tc = oc->getClass(*qc, priv);
      if (!tc) {
         xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to class '%s'", obj->getClassName(), qc->getName());
         return QoreValue();
      }
      //printd(5, "QoreCastOperatorNode::evalValueImpl() %s -> %s priv: %d\n", oc->getName(), tc->getName(), priv);
      if (priv && !qore_class_private::runtimeCheckPrivateClassAccess(*tc)) {
         xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to privately-accessible class '%s' in this context", obj->getClassName(), qc->getName());
         return QoreValue();
      }
   }

   return rv.takeValue(needs_deref);
}

QoreValue QoreHashDeclCastOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder rv(exp, xsink);
   if (*xsink)
      return QoreValue();

   if (rv->getType() != NT_HASH) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to hashdecl '%s'", rv->getTypeName(), hd->getName());
      return QoreValue();
   }

   const QoreHashNode* h = rv->get<const QoreHashNode>();

   const TypedHashDecl* vhd = h->getHashDecl();

   if (!hd) {
      if (!vhd)
         return rv.takeValue(needs_deref);
      needs_deref = true;
      return qore_hash_private::getPlainHash(static_cast<QoreHashNode*>(rv.getReferencedValue()));
   }

   // if we already have the expected type, then there's nothing more to do
   if (vhd && typed_hash_decl_private::get(*vhd)->equal(*typed_hash_decl_private::get(*hd)))
      return rv.takeValue(needs_deref);

   // do the runtime cast
   return typed_hash_decl_private::get(*hd)->newHash(h, true, xsink);
}

QoreValue QoreComplexHashCastOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder rv(exp, xsink);
   if (*xsink)
      return QoreValue();

   if (rv->getType() != NT_HASH) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to '%s'", rv->getTypeName(), QoreTypeInfo::getName(typeInfo));
      return QoreValue();
   }

   // do the runtime case
   return qore_hash_private::newComplexHashFromHash(typeInfo, static_cast<QoreHashNode*>(rv.getReferencedValue()), xsink);
}
