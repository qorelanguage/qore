/*
  ReferenceHelper.cpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
   const QoreTypeInfo *typeInfo = 0;
   ObjMap omap;
   vp = get_var_value_ptr(ref->getExpression(), &vl, typeInfo, omap, xsink);
   if (!*xsink && typeInfo->hasType()) {
      // set the pointer to null so it cannot be used
      vp = 0;
      xsink->raiseException("RUNTIME-TYPE-ERROR", "this module uses an old data structure (ReferenceHelper) that is not type aware, and the referenced value has type restrictions; the module must be updated to use the new type 'QoreTypeSafeReferenceHelper' instead");
   }
}

ReferenceHelper::~ReferenceHelper() {
}

AbstractQoreNode *ReferenceHelper::getUnique(ExceptionSink *xsink) {
   if (!(*vp)) 
      return 0;
   
   if (!(*vp)->is_unique()) {
      AbstractQoreNode *old = *vp;
      (*vp) = old->realCopy();
      old->deref(xsink);
   }
   return *vp;
}

int ReferenceHelper::assign(AbstractQoreNode *val, ExceptionSink *xsink) {
   assert(vp);
   if (*vp) {
      (*vp)->deref(xsink);
      if (*xsink) {
	 (*vp) = 0;
	 discard(val, xsink);
	 return -1;
      }
   }
   (*vp) = val;
   return 0;
}

void ReferenceHelper::swap(ReferenceHelper &other) {
   assert(vp);
   AbstractQoreNode *t = *other.vp;
   *other.vp = *vp;
   *vp = t;
}

const AbstractQoreNode *ReferenceHelper::getValue() const {
   assert(vp);
   return *vp;
}

struct qore_type_safe_ref_helper_priv_t : public LValueHelper {
   // dummy ptr for assignments to non-node-based lvalues
   mutable AbstractQoreNode *dummy;
   mutable bool assign_dummy : 1;

   DLLLOCAL qore_type_safe_ref_helper_priv_t(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) : LValueHelper(ref->getExpression(), xsink), dummy(0), assign_dummy(false) {
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
	 dummy->deref(xsink);
	 dummy = 0;
	 return *xsink;
      }
      return 0;
   }

   DLLLOCAL AbstractQoreNode *getUnique(ExceptionSink *xsink) {
      if (isOptimized()) {
	 if (discardDummy())
	    return 0;

	 dummy = lv.local.v->eval(xsink);
	 assign_dummy = true;
	 return dummy;
      }

      if (ensure_unique())
	 return 0;

      return get_value();
   }

   DLLLOCAL int assign(AbstractQoreNode *val, ExceptionSink *xsink) {
      return LValueHelper::assign(val, "<reference>");
   }

   DLLLOCAL int assign(AbstractQoreNode *val) {
      return LValueHelper::assign(val, "<reference>");
   }

   DLLLOCAL void swap(qore_type_safe_ref_helper_priv_t &other) {
      assert(false);
   }

   DLLLOCAL const AbstractQoreNode *getValue() const {
      if (isOptimized()) {
	 if (discardDummy())
	    return 0;

	 dummy = lv.local.v->eval(xsink);
	 return dummy;
      }

      return LValueHelper::get_value();
   }

   DLLLOCAL qore_type_t getType() const {
      return LValueHelper::get_type();
   }
};


QoreTypeSafeReferenceHelper::QoreTypeSafeReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) : priv(new qore_type_safe_ref_helper_priv_t(ref, vl, xsink)) {
}

QoreTypeSafeReferenceHelper::~QoreTypeSafeReferenceHelper() {
   delete priv;
}

AbstractQoreNode *QoreTypeSafeReferenceHelper::getUnique(ExceptionSink *xsink) {
   return priv->getUnique(xsink);
}

int QoreTypeSafeReferenceHelper::assign(AbstractQoreNode *val, ExceptionSink *xsink) {
   return priv->assign(val, xsink);
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

