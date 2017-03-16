/*
  ForEachStatement.cpp

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

#include <qore/Qore.h>
#include "qore/intern/ForEachStatement.h"
#include "qore/intern/StatementBlock.h"
#include "qore/intern/AbstractIteratorHelper.h"

#include <memory>

ForEachStatement::ForEachStatement(int start_line, int end_line, AbstractQoreNode* v, AbstractQoreNode* l, StatementBlock *cd) : AbstractStatement(start_line, end_line), var(v), list(l), code(cd), lvars(0), iterator_func(0), is_ref(false) {
}

ForEachStatement::~ForEachStatement() {
   if (var)
      var->deref(0);
   if (list)
      list->deref(0);
   delete code;
   delete lvars;
}

int ForEachStatement::execImpl(QoreValue& return_value, ExceptionSink* xsink) {
   if (is_ref)
      return execRef(return_value, xsink);

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   // get iterator object
   FunctionalOperator::FunctionalValueType value_type;
   std::unique_ptr<FunctionalOperatorInterface> f(iterator_func ? iterator_func->getFunctionalIterator(value_type, xsink) : FunctionalOperatorInterface::getFunctionalIterator(value_type, list, true, "foreach statement", xsink));
   if (*xsink || value_type == FunctionalOperator::nothing || !code)
      return 0;

   // execute "foreach" body
   unsigned i = 0;

   int rc = 0;

   while (true) {
      {
	 // get first value
	 ValueOptionalRefHolder iv(xsink);
	 if (f->getNext(iv, xsink))
	    break;
	 if (*xsink)
	    break;

	 LValueHelper n(var, xsink);
	 if (!n)
	    break;

	 // assign variable to current value
	 if (n.assign(iv.takeReferencedValue(), "<foreach lvalue assignment>"))
	    break;
      }

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(i++);

      // execute "foreach" body
      if (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || *xsink) {
	 rc = 0;
	 break;
      }

      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }

   return rc;
}

int ForEachStatement::execRef(QoreValue& return_value, ExceptionSink* xsink) {
   int rc = 0;

   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);

   ParseReferenceNode* r = reinterpret_cast<ParseReferenceNode*>(list);

   // here we get the runtime reference
   ReferenceHolder<ReferenceNode> vr(r->evalToRef(xsink), xsink);
   if (*xsink)
      return 0;

   // get the current value of the lvalue expression
   ReferenceHolder<AbstractQoreNode> tlist(vr->eval(xsink), xsink);
   if (!code || *xsink || is_nothing(*tlist))
      return 0;

   QoreListNode* l_tlist = tlist->getType() == NT_LIST ? reinterpret_cast<QoreListNode*>(*tlist) : 0;
   if (l_tlist && l_tlist->empty())
      return 0;

   // execute "foreach" body
   ReferenceHolder<AbstractQoreNode> ln(0, xsink);
   unsigned i = 0;

   if (l_tlist)
      ln = new QoreListNode;

   while (true) {
      {
	 LValueHelper n(var, xsink);
	 if (!n)
	    return 0;

	 // assign variable to current value in list
	 if (n.assign(l_tlist ? l_tlist->get_referenced_entry(i) : tlist.release()))
	    return 0;
      }

      // set offset in thread-local data for "$#"
      ImplicitElementHelper eh(l_tlist ? (int)i : 0);

      // execute "for" body
      rc = code->execImpl(return_value, xsink);
      if (*xsink)
	 return 0;

      // get value of foreach variable
      AbstractQoreNode* nv = var->eval(xsink);
      if (*xsink)
	 return 0;

      // assign new value to temporary variable for later assignment to referenced lvalue
      if (l_tlist)
	 reinterpret_cast<QoreListNode*>(*ln)->push(nv);
      else
	 ln = nv;

      if (rc == RC_BREAK) {
	 // assign remaining values to list unchanged
	 if (l_tlist)
	    while (++i < l_tlist->size())
	       reinterpret_cast<QoreListNode*>(*ln)->push(l_tlist->get_referenced_entry(i));

	 rc = 0;
	 break;
      }

      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
      i++;

      // break out of loop if appropriate
      if (!l_tlist || i == l_tlist->size())
	 break;
   }

   // write the value back to the lvalue
   LValueHelper val(**vr, xsink);
   if (!val)
      return 0;

   if (val.assign(ln.release()))
      return 0;

   return rc;
}

int ForEachStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;

   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   const QoreTypeInfo *argTypeInfo = 0;
   if (var)
      var = var->parseInit(oflag, pflag, lvids, argTypeInfo);

   qore_type_t t = get_node_type(var);
   if (t != NT_VARREF && t != NT_SELF_VARREF)
      parse_error("foreach variable expression is not a variable reference (got type '%s' instead)", get_type_name(var));

   if (list) {
      argTypeInfo = 0;
      list = list->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (code)
      code->parseInitImpl(oflag, pflag | PF_BREAK_OK | PF_CONTINUE_OK);

   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   qore_type_t typ = list->getType();

   is_ref = (typ == NT_PARSEREFERENCE);

   // use lazy evaluation if the iterator expression supports it
   iterator_func = dynamic_cast<FunctionalOperator*>(list);

   return 0;
}
