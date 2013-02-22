/*
  ReferenceHelper.cpp

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

#include <qore/Qore.h>

#include <qore/intern/ReferenceHelper.h>

ReferenceHelper::ReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) {
   xsink->raiseException("RUNTIME-TYPE-ERROR", "this module uses an outdated, unsafe, and removed API (ReferenceHelper); the module must be updated to use the new type 'QoreTypeSafeReferenceHelper' instead");
   vp = 0;
}

ReferenceHelper::~ReferenceHelper() {
}

AbstractQoreNode *ReferenceHelper::getUnique(ExceptionSink *xsink) {
   xsink->raiseException("RUNTIME-TYPE-ERROR", "this module uses an outdated, unsafe, and removed API (ReferenceHelper); the module must be updated to use the new type 'QoreTypeSafeReferenceHelper' instead");
   return 0;
}

int ReferenceHelper::assign(AbstractQoreNode *val, ExceptionSink *xsink) {
   ReferenceHolder<> del(val, xsink);
   xsink->raiseException("RUNTIME-TYPE-ERROR", "this module uses an outdated, unsafe, and removed API (ReferenceHelper); the module must be updated to use the new type 'QoreTypeSafeReferenceHelper' instead");
   return -1;
}

void ReferenceHelper::swap(ReferenceHelper &other) {
}

const AbstractQoreNode *ReferenceHelper::getValue() const {
   return 0;
}

struct qore_type_safe_ref_helper_priv_t : public LValueHelper {
   // dummy ptr for assignments to non-node-based lvalues
   mutable AbstractQoreNode *dummy;
   mutable bool assign_dummy : 1;

   DLLLOCAL qore_type_safe_ref_helper_priv_t(const ReferenceNode *ref, ExceptionSink *xsink) : LValueHelper(*ref, xsink), dummy(0), assign_dummy(false) {
   }

   DLLLOCAL qore_type_safe_ref_helper_priv_t(const AbstractQoreNode *exp, ExceptionSink *xsink) : LValueHelper(exp, xsink), dummy(0) {
   }

   DLLLOCAL ~qore_type_safe_ref_helper_priv_t() {
      if (*this && assign_dummy) {
	 assign(dummy);
      }
      else
	 discardDummy();
   }

   DLLLOCAL int discardDummy() const {
      if (assign_dummy)
	 assign_dummy = false;
      if (dummy) {
	 dummy->deref(vl.xsink);
	 dummy = 0;
	 return *vl.xsink;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode *getUnique(ExceptionSink *xsink) {
      if (isOptimized()) {
	 if (discardDummy())
	    return 0;

	 dummy = getReferencedValue();
	 assign_dummy = true;
	 return dummy;
      }

      ensureUnique();
      return LValueHelper::getValue();
   }

   DLLLOCAL int assign(AbstractQoreNode* val) {
      return LValueHelper::assign(val, "<reference>");
   }

   DLLLOCAL void swap(qore_type_safe_ref_helper_priv_t &other) {
      assert(false);
   }

   DLLLOCAL const AbstractQoreNode *getValue() const {
      if (isOptimized()) {
	 if (discardDummy())
	    return 0;

	 dummy = getReferencedValue();
	 return dummy;
      }

      return LValueHelper::getValue();
   }

   DLLLOCAL qore_type_t getType() const {
      return LValueHelper::getType();
   }

   DLLLOCAL const char* getTypeName() const {
      return LValueHelper::getTypeName();
   }
};


QoreTypeSafeReferenceHelper::QoreTypeSafeReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) : priv(new qore_type_safe_ref_helper_priv_t(ref, xsink)) {
}

QoreTypeSafeReferenceHelper::QoreTypeSafeReferenceHelper(const ReferenceNode *ref, ExceptionSink *xsink) : priv(new qore_type_safe_ref_helper_priv_t(ref, xsink)) {
}

QoreTypeSafeReferenceHelper::~QoreTypeSafeReferenceHelper() {
   delete priv;
}

AbstractQoreNode *QoreTypeSafeReferenceHelper::getUnique(ExceptionSink *xsink) {
   return priv->getUnique(xsink);
}

int QoreTypeSafeReferenceHelper::assign(AbstractQoreNode *val, ExceptionSink *xsink) {
   return priv->assign(val);
}

int QoreTypeSafeReferenceHelper::assign(AbstractQoreNode *val) {
   return priv->assign(val);
}

int QoreTypeSafeReferenceHelper::assignBigInt(int64 v) {
   return priv->assignBigInt(v);
}

int QoreTypeSafeReferenceHelper::assignFloat(double v) {
   return priv->assignFloat(v);
}

void QoreTypeSafeReferenceHelper::swap(QoreTypeSafeReferenceHelper &other) {
   return priv->swap(*other.priv);
}

const AbstractQoreNode *QoreTypeSafeReferenceHelper::getValue() const {
   return priv->getValue();
}

QoreTypeSafeReferenceHelper::operator bool() const {
   return *priv;
}

qore_type_t QoreTypeSafeReferenceHelper::getType() const { 
   return priv->getType();
}

const char* QoreTypeSafeReferenceHelper::getTypeName() const { 
   return priv->getTypeName();
}

