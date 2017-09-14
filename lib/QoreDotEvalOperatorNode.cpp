/*
  QoreDotEvalOperatorNode.cpp

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

QoreString QoreDotEvalOperatorNode::name("dot eval expression");

static const AbstractQoreNode *check_call_ref(const AbstractQoreNode *op, const char *name) {
   // FIXME: this is an ugly hack!
   const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(op);
   // see if the hash member is a call reference
   const AbstractQoreNode *ref = h->getKeyValue(name);
   return (ref && (ref->getType() == NT_FUNCREF || ref->getType() == NT_RUNTIME_CLOSURE)) ? ref : 0;
}

QoreValue QoreDotEvalOperatorNode::evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
   ValueEvalRefHolder op(left, xsink);
   if (*xsink)
      return QoreValue();

   switch (op->getType()) {
      case NT_WEAKREF: {
         // FIXME: inefficient
         return m->exec(op->get<WeakReferenceNode>()->get(), xsink);
      }

      case NT_OBJECT: {
         QoreObject* o = const_cast<QoreObject*>(reinterpret_cast<const QoreObject*>(op->getInternalNode()));
         // FIXME: inefficient
         return m->exec(o, xsink);
      }

      case NT_HASH: {
         const AbstractQoreNode* ref = check_call_ref(op->getInternalNode(), m->getName());
         if (ref)
            return reinterpret_cast<const ResolvedCallReferenceNode*>(ref)->execValue(m->getArgs(), xsink);
         // drop down to default
      }

      default:
         break;
   }

   // FIXME: inefficient
   ReferenceHolder<> nop(op.getReferencedValue(), xsink);
   if (m->isPseudo())
      return m->execPseudo(*nop, xsink);

   return pseudo_classes_eval(*nop, m->getName(), m->getArgs(), xsink);
}

AbstractQoreNode* QoreDotEvalOperatorNode::parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& expTypeInfo) {
   assert(!expTypeInfo);
   const QoreTypeInfo* typeInfo = 0;
   left = left->parseInit(oflag, pflag, lvids, typeInfo);

   QoreClass* qc = const_cast<QoreClass*>(QoreTypeInfo::getUniqueReturnClass(typeInfo));

   const QoreMethod* meth = 0;

   const char* mname = m->getName();

   if (!qc) {
      // if the left side has a type and it's not an object, then we try to match pseudo-methods
      if (QoreTypeInfo::hasType(typeInfo)
         && !QoreTypeInfo::parseAccepts(objectTypeInfo, typeInfo)) {
         // check for pseudo-methods
         bool possible_match;
         meth = pseudo_classes_find_method(typeInfo, mname, qc, possible_match);

         if (meth) {
            m->setPseudo(typeInfo);
            // save method for optimizing calls later
            m->parseSetClassAndMethod(qc, meth);

            // check parameters, if any
            lvids += m->parseArgs(oflag, pflag, meth->getFunction(), returnTypeInfo);
            expTypeInfo = returnTypeInfo;

            return this;
         }
         else if (!possible_match && !QoreTypeInfo::parseAccepts(hashTypeInfo, typeInfo)) {
            // issue an error if there was no match and it's not a hash
            QoreStringNode* edesc = new QoreStringNode;
            edesc->sprintf("no pseudo-method <%s>.%s() can be found", QoreTypeInfo::getName(typeInfo), mname);
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", edesc);
         }
      }

#ifdef DEBUG
      typeInfo = 0;
      AbstractQoreNode* n = m->parseInit(oflag, pflag, lvids, typeInfo);
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

   qore_class_private* class_ctx = parse_get_class_priv();
   if (class_ctx && !qore_class_private::parseCheckPrivateClassAccess(*qc, class_ctx))
      class_ctx = 0;
   // method access is already checked here
   meth = qore_class_private::get(*qc)->parseFindAnyMethod(mname, class_ctx);

   //printd(5, "QoreDotEvalOperatorNode::parseInitImpl() %s::%s() method: %p (%s) class_ctx: %p (%s)\n", qc->getName(), mname, meth, meth ? meth->getClassName() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

   const QoreListNode* args = m->getArgs();
   if (!strcmp(mname, "copy")) {
      if (args && args->size())
         parse_error(loc, "no arguments may be passed to copy methods (%d argument%s given in call to %s::copy())", args->size(), args->size() == 1 ? "" : "s", qc->getName());

      // do not save method pointer for copy methods
      expTypeInfo = returnTypeInfo = qc->getTypeInfo();
#ifdef DEBUG
      typeInfo = 0;
      AbstractQoreNode *n = m->parseInit(oflag, pflag, lvids, typeInfo);
      assert(n == m);
#else
      m->parseInit(oflag, pflag, lvids, typeInfo);
#endif
      return this;
   }

   if (!meth) {
      if (!qc->parseHasMethodGate()) {
         // check if it could be a pseudo-method call
         meth = pseudo_classes_find_method(NT_OBJECT, mname, qc);
         if (meth)
            m->setPseudo(qc->getTypeInfo());
         else
            raise_nonexistent_method_call_warning(loc, qc, mname);
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

   // save method for optimizing calls later
   m->parseSetClassAndMethod(qc, meth);

   // check parameters, if any
   lvids += m->parseArgs(oflag, pflag, meth->getFunction(), returnTypeInfo);
   expTypeInfo = returnTypeInfo;

   printd(5, "QoreDotEvalOperatorNode::parseInitImpl() %s::%s() method=%p (%s::%s()) (private=%s, static=%s) rv=%s\n", qc->getName(), mname, meth, meth ? meth->getClassName() : "n/a", mname, meth && (qore_method_private::parseGetAccess(*meth) > Public) ? "true" : "false", meth->isStatic() ? "true" : "false", QoreTypeInfo::getName(returnTypeInfo));

   return this;
}

AbstractQoreNode *QoreDotEvalOperatorNode::makeCallReference() {
   if (m->getArgs()) {
      parse_error(loc, "argument given to call reference");
      return this;
   }

   if (!strcmp(m->getName(), "copy")) {
      parse_error(loc, "cannot make a call reference to a copy() method");
      return this;
   }

   assert(is_unique());

   // rewrite as a call reference
   AbstractQoreNode *exp = left;
   left = 0;
   char *meth = m->takeName();
   QoreProgramLocation nloc = loc;
   this->deref();

   //printd(5, "made parse object method reference: exp=%p meth=%s\n", exp, meth);

   return new ParseObjectMethodReferenceNode(nloc, exp, meth);
}

QoreOperatorNode *QoreDotEvalOperatorNode::copyBackground(ExceptionSink *xsink) const {
   return new QoreDotEvalOperatorNode(loc, copy_and_resolve_lvar_refs(left, xsink), reinterpret_cast<MethodCallNode*>(copy_and_resolve_lvar_refs(m, xsink)));
}
