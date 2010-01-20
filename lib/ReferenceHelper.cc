/*
  ReferenceHelper.cc

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

#include <qore/Qore.h>

#include <qore/intern/ReferenceHelper.h>

ReferenceHelper::ReferenceHelper(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) {
   const QoreTypeInfo *typeInfo = 0;
   vp = get_var_value_ptr(ref->getExpression(), &vl, typeInfo, xsink);
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

// xxx implement type-safe operations
struct qore_type_safe_ref_helper_priv_t {
   AbstractQoreNode **vp;
   const QoreTypeInfo *typeInfo;

   DLLLOCAL qore_type_safe_ref_helper_priv_t(const ReferenceNode *ref, AutoVLock &vl, ExceptionSink *xsink) : typeInfo(0) {
      vp = get_var_value_ptr(ref->getExpression(), &vl, typeInfo, xsink);
   }
   DLLLOCAL AbstractQoreNode *getUnique(ExceptionSink *xsink) {
      if (!(*vp)) 
	 return 0;
   
      if (!(*vp)->is_unique()) {
	 AbstractQoreNode *old = *vp;
	 (*vp) = old->realCopy();
	 old->deref(xsink);
      }
      return *vp;
   }

   DLLLOCAL int assign(AbstractQoreNode *val, ExceptionSink *xsink) {
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

   DLLLOCAL void swap(qore_type_safe_ref_helper_priv_t &other) {
      assert(vp);
      AbstractQoreNode *t = *other.vp;
      *other.vp = *vp;
      *vp = t;
   }

   DLLLOCAL const AbstractQoreNode *getValue() const {
      assert(vp);
      return *vp;
   }

   DLLLOCAL operator bool() const { return vp != 0; }

   DLLLOCAL qore_type_t getType() const { return *vp ? (*vp)->getType() : NT_NOTHING; }
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

