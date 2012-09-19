/*
 QoreDotEvalOperatorNode.cpp

 Qore Programming Language

 Copyright 2003 - 2012 David Nichols

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
#include <qore/intern/qore_program_private.h>
#include <qore/intern/QoreClassIntern.h>

QoreString QoreDotEvalOperatorNode::name("dot eval expression");

static const AbstractQoreNode *check_call_ref(const AbstractQoreNode *op, const char *name) {
   // FIXME: this is an ugly hack!
   const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(op);
   // see if the hash member is a call reference
   const AbstractQoreNode *ref = h->getKeyValue(name);
   return (ref && (ref->getType() == NT_FUNCREF || ref->getType() == NT_RUNTIME_CLOSURE)) ? ref : 0;
}

AbstractQoreNode *QoreDotEvalOperatorNode::evalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   if (get_node_type(*op) == NT_HASH) {
      const AbstractQoreNode *ref = check_call_ref(*op, m->getName());
      if (ref)
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->exec(m->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      if (m->isPseudo())
	 return m->execPseudo(*op, xsink);

      return pseudo_classes_eval(*op, m->getName(), m->getArgs(), xsink);
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return m->exec(o, xsink);
}

int64 QoreDotEvalOperatorNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   if (get_node_type(*op) == NT_HASH) {
      const AbstractQoreNode *ref = check_call_ref(*op, m->getName());
      if (ref)
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->bigIntExec(m->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      if (m->isPseudo())
	 return m->bigIntExecPseudo(*op, xsink);

      return pseudo_classes_int64_eval(*op, m->getName(), m->getArgs(), xsink);
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return m->bigIntExec(o, xsink);
}

int QoreDotEvalOperatorNode::integerEvalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   if (get_node_type(*op) == NT_HASH) {
      const AbstractQoreNode *ref = check_call_ref(*op, m->getName());
      if (ref)
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->intExec(m->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      if (m->isPseudo())
	 return m->intExecPseudo(*op, xsink);

      return pseudo_classes_int_eval(*op, m->getName(), m->getArgs(), xsink);
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return m->intExec(o, xsink);
}

bool QoreDotEvalOperatorNode::boolEvalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   if (get_node_type(*op) == NT_HASH) {
      const AbstractQoreNode *ref = check_call_ref(*op, m->getName());
      if (ref)
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->boolExec(m->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      if (m->isPseudo())
	 return m->boolExecPseudo(*op, xsink);

      return pseudo_classes_bool_eval(*op, m->getName(), m->getArgs(), xsink);
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return m->boolExec(o, xsink);
}

double QoreDotEvalOperatorNode::floatEvalImpl(ExceptionSink *xsink) const {
   QoreNodeEvalOptionalRefHolder op(left, xsink);
   if (*xsink)
      return 0;

   if (get_node_type(*op) == NT_HASH) {
      const AbstractQoreNode *ref = check_call_ref(*op, m->getName());
      if (ref)
	 return reinterpret_cast<const ResolvedCallReferenceNode *>(ref)->floatExec(m->getArgs(), xsink);
   }

   if (!(*op) || (*op)->getType() != NT_OBJECT) {
      if (m->isPseudo())
	 return m->floatExecPseudo(*op, xsink);

      return pseudo_classes_double_eval(*op, m->getName(), m->getArgs(), xsink);
   }

   QoreObject *o = const_cast<QoreObject *>(reinterpret_cast<const QoreObject *>(*op));
   return m->floatExec(o, xsink);
}

AbstractQoreNode *QoreDotEvalOperatorNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return evalImpl(xsink);
}

AbstractQoreNode *QoreDotEvalOperatorNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
   assert(!returnTypeInfo);
   const QoreTypeInfo *typeInfo = 0;
   left = left->parseInit(oflag, pflag, lvids, typeInfo);

   QoreClass *qc = const_cast<QoreClass *>(typeInfo->getUniqueReturnClass());

   const QoreMethod *meth = 0;

   const char *mname = m->getName();

   if (!qc) {
      // if the left side has a type and it's not an object, then we try to match pseudo-methods
      if (typeInfo->hasType()
	  && !objectTypeInfo->parseAccepts(typeInfo)) {
	 // check for pseudo-methods
	 bool possible_match;
	 meth = pseudo_classes_find_method(typeInfo, mname, qc, possible_match);

	 if (meth) {
	    m->setPseudo();
	    // save method for optimizing calls later
	    m->parseSetClassAndMethod(qc, meth);

	    // check parameters, if any
	    lvids += m->parseArgs(oflag, pflag, meth->getFunction(), returnTypeInfo);

	    return this;
	 }
	 else if (!possible_match && !hashTypeInfo->parseAccepts(typeInfo)) {
	    // issue an error if there was no match and it's not a hash
	    QoreStringNode *edesc = new QoreStringNode;
	    edesc->sprintf("no pseudo-method <%s>.%s() can be found", typeInfo->getName(), mname);
	    qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
	 }
      }

#ifdef DEBUG
      typeInfo = 0;
      AbstractQoreNode *n = m->parseInit(oflag, pflag, lvids, typeInfo);
      assert(n == m);
#else
      m->parseInit(oflag, pflag, lvids, typeInfo);
#endif
      return this;
   }

   // make sure method arguments and return types are resolved
   qore_class_private::parseInitPartial(*qc);

   if (!m)
      return this;

   meth = qc->parseFindMethodTree(mname);

   //printd(5, "check_op_object_func_ref() %s::%s() method=%p (%s) (private=%s)\n", qc->getName(), mname, meth, meth ? meth->getClassName() : "n/a", meth && meth->parseIsPrivate() ? "true" : "false" );

   const QoreListNode *args = m->getArgs();
   if (!strcmp(mname, "copy")) {
      if (args && args->size())
	 parse_error("no arguments may be passed to copy methods (%d argument%s given in call to %s::copy())", args->size(), args->size() == 1 ? "" : "s", qc->getName());

      if (m && meth->parseIsPrivate() && (!oflag || !qore_class_private::parseCheckCompatibleClass(*qc, *(getParseClass()))))
	 parse_error("illegal call to private %s::copy() method", qc->getName());

      // do not save method pointer for copy methods
      returnTypeInfo = qc->getTypeInfo();
#ifdef DEBUG
      typeInfo = 0;
      AbstractQoreNode *n = m->parseInit(oflag, pflag, lvids, typeInfo);
      assert(n == m);
#else
      m->parseInit(oflag, pflag, lvids, typeInfo);
#endif
      return this;
   }

   // if a normal method is not found, then look for a static method
   if (!meth)
      meth = qc->parseFindStaticMethodTree(mname);

   if (!meth) {
      if (!qc->parseHasMethodGate()) {
	 // check if it could be a pseudo-method call
	 meth = pseudo_classes_find_method(NT_OBJECT, mname, qc);
	 if (meth)
	    m->setPseudo();
	 else
	    raiseNonExistentMethodCallWarning(qc, mname);
      }

      if (!meth) {
#ifdef DEBUG
         typeInfo = 0;
	 AbstractQoreNode *n = m->parseInit(oflag, pflag, lvids, typeInfo);
	 assert(n == m);
#else
	 m->parseInit(oflag, pflag, lvids, typeInfo);
#endif
	 return this;
      }
   }

   if (meth->parseIsPrivate() && !qore_class_private::parseCheckCompatibleClass(*qc, *(getParseClass())))
      parse_error("illegal call to private method %s::%s()", qc->getName(), mname);

   // save method for optimizing calls later
   m->parseSetClassAndMethod(qc, meth);

   // check parameters, if any
   lvids += m->parseArgs(oflag, pflag, meth->getFunction(), returnTypeInfo);

   printd(5, "QoreDotEvalOperatorNode::parseInitImpl() %s::%s() method=%p (%s::%s()) (private=%s, static=%s) rv=%s\n", qc->getName(), mname, meth, meth ? meth->getClassName() : "n/a", mname, meth && meth->parseIsPrivate() ? "true" : "false", meth->isStatic() ? "true" : "false", returnTypeInfo->getName());

   return this;
}

AbstractQoreNode *QoreDotEvalOperatorNode::makeCallReference() {
   if (m->getArgs()) {
      parse_error("argument given to call reference");
      return this;
   }

   assert(is_unique());

   // rewrite as a call reference
   AbstractQoreNode *exp = left;
   left = 0;
   char *meth = m->takeName();
   this->deref();

   //printd(5, "made parse object method reference: exp=%p meth=%s\n", exp, meth);

   return new ParseObjectMethodReferenceNode(exp, meth);
}

QoreOperatorNode *QoreDotEvalOperatorNode::copyBackground(ExceptionSink *xsink) const {
   return new QoreDotEvalOperatorNode(copy_and_resolve_lvar_refs(left, xsink), reinterpret_cast<MethodCallNode *>(copy_and_resolve_lvar_refs(m, xsink)));
}
