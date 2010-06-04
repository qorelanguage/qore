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

int QoreCastOperatorNode::evalIntern(const AbstractQoreNode *rv, ExceptionSink *xsink) const {
   if (!rv || rv->getType() != NT_OBJECT) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from type '%s' to %s'%s'", get_type_name(rv), qc ? "class " : "", qc ? qc->getName() : "object");
      return -1;
   }

   const QoreObject *obj = reinterpret_cast<const QoreObject *>(rv);
   if (qc && !obj->getClass(qc->getID())) {
      xsink->raiseException("RUNTIME-CAST-ERROR", "cannot cast from class '%s' to class '%s'", obj->getClassName(), qc->getName());
      return -1;
   }

   return 0;
}

AbstractQoreNode *QoreCastOperatorNode::evalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   if (evalIntern(*rv, xsink))
      return 0;

   return rv.release();
}

AbstractQoreNode *QoreCastOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder rv(exp, xsink);
   if (*xsink)
      return 0;
   
   if (evalIntern(*rv, xsink))
      return 0;

   if (rv.isTemp()) {
      needs_deref = false;
      return const_cast<AbstractQoreNode *>(*rv);
   }
   needs_deref = true;
   return rv.getReferencedValue();
}

AbstractQoreNode *QoreCastOperatorNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   bool err = false;
   if (path->elements == 1) {
      // if the class is "object", then set qc = 0 to use as a catch-all and generic "cast to object"
      const char *id = path->getIdentifier();
      qc = !strcmp(id, "object") ? 0 : getRootNS()->parseFindClass(path->getIdentifier());
   }
   else {
      qc = getRootNS()->parseFindScopedClass(path);
      err = true;
   }

   //printd(5, "QoreCastOperatorNode::parseInit() this=%p resolved %s->%s\n", this, path->getIdentifier(), qc ? qc->getName() : (err ? "<error>" : "<generic object cast>");

   if (exp)
      exp = exp->parseInit(oflag, pflag & ~PF_REFERENCE_OK, lvids, typeInfo);

   if (typeInfo->hasType()) {
      if (!objectTypeInfo->parseAccepts(typeInfo)) {
	 parse_error("cast<>(%s) is invalid; cannot cast from %s to object", qc ? qc->getName() : "object", typeInfo->getName(), typeInfo->getName());
      }
      else if (qc && (qc->getTypeInfo()->parseAccepts(typeInfo) == QTI_NOT_EQUAL)) {
	 parse_error("cannot cast from %s to %s; the classes are not compatible", typeInfo->getName(), path->ostr);
      }
   }

   // free temporary memory
   delete path;
   path = 0;

   // set return type info
   typeInfo = qc ? qc->getTypeInfo() : objectTypeInfo;
   return this;
}
