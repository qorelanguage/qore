/*
 QoreCastOperatorNode.cpp
 
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

QoreString QoreCastOperatorNode::cast_str("cast operator expression");

// if del is true, then the returned QoreString * should be castd, if false, then it must not be
QoreString *QoreCastOperatorNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = false;
   return &cast_str;
}

int QoreCastOperatorNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat(&cast_str);
   return 0;
}

AbstractQoreNode *QoreCastOperatorNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;
   
   if (!rv || rv->getType() != NT_OBJECT) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to class '%s'", get_type_name(*rv), qc->getName());
      return 0;
   }

   const QoreObject *obj = reinterpret_cast<const QoreObject *>(*rv);
   if (!obj->getClass(qc->getID())) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to class '%s'", obj->getClassName(), qc->getName());
      return 0;
   }

   return rv.release();
}

AbstractQoreNode *QoreCastOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder rv(exp, xsink);
   if (*xsink)
      return 0;
   
   if (!rv || rv->getType() != NT_OBJECT) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to class '%s'", get_type_name(*rv), qc->getName());
      return 0;
   }

   const QoreObject *obj = reinterpret_cast<const QoreObject *>(*rv);
   if (!obj->getClass(qc->getID())) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to class '%s'", obj->getClassName(), qc->getName());
      return 0;
   }

   if (rv.isTemp()) {
      needs_deref = false;
      return const_cast<AbstractQoreNode *>(*rv);
   }
   needs_deref = true;
   return rv.getReferencedValue();
}

AbstractQoreNode *QoreCastOperatorNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   qc = path->elements == 1 
      ? getRootNS()->parseFindClass(path->getIdentifier()) 
      : getRootNS()->parseFindScopedClass(path);

   //printd(5, "QoreCastOperatorNode::parseInit() this=%p resolved class %s\n", this, qc ? qc->getName() : "n/a");

   if (exp)
      exp = exp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, typeInfo);

   if (typeInfo->hasType()) {
      if (!typeInfo->qc) {
	 parse_error("cast<%s>(%s) is invalid; the cast operator can only work with classes", path->ostr, typeInfo->getName());
      }
      else if (qc && (qc->getTypeInfo()->parseEqual(typeInfo) == QTI_NOT_EQUAL) && (typeInfo->parseEqual(qc->getTypeInfo()) == QTI_NOT_EQUAL)) {
	 parse_error("cannot cast from %s to %s; the classes are not compatible", typeInfo->getName(), path->ostr);
      }
   }

   typeInfo = qc ? qc->getTypeInfo() : 0;   
   return this;
}
