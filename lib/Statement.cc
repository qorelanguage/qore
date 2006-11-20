/*
  Statement.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/Function.h>
#include <qore/Context.h>
#include <qore/Operator.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/qore_thread.h>
#include <qore/Exception.h>
#include <qore/ParserSupport.h>
#include <qore/QoreClass.h>
#include <qore/QoreWarnings.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// only executed by Statement::exec()
inline int SwitchStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("SwitchStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   class QoreNode *se = sexp->eval(xsink);
   if (!xsink->isEvent())
   {
      // find match
      class CaseNode *w = head;
      while (w)
      {
	 if (!compareHard(se, w->val))
	    break;
	 w = w->next;
      }
      if (!w && deflt)
	 w = deflt;

      while (w && !rc && !xsink->isEvent())
      {
	 if (w->code)
	    rc = w->code->exec(return_value, xsink);

	 w = w->next;
      }
      if (rc == RC_BREAK || rc == RC_CONTINUE)
	 rc = 0;
   }

   if (se)
      se->deref(xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);

   traceout("SwitchStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline int IfStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("IfStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   if (cond->boolEval(xsink))
   {
      if (!xsink->isEvent() && if_code)
	 rc = if_code->exec(return_value, xsink);
   }
   else if (else_code)
      rc = else_code->exec(return_value, xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("IfStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline int ForStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("ForStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // evaluate assignment expression and discard results if any
   if (assignment)
      discard(assignment->eval(xsink), xsink);

   // execute "for" body
   while (!xsink->isEvent())
   {
      // check conditional expression, exit "for" loop if condition is
      // false
      if (cond && (!cond->boolEval(xsink) || xsink->isEvent()))
	 break;

      // otherwise, execute "for" body
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
      
      // evaluate iterator expression and discard results if any
      if (iterator)
	 discard(iterator->eval(xsink), xsink);
   }

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("ForStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline int ForEachStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   if (list->type == NT_REFERENCE)
      return execRef(return_value, xsink);

   int i, rc = 0;

   tracein("ForEachStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // get list evaluation (although may be a single node)
   class QoreNode *tlist = list->eval(xsink);
   if (tlist && is_nothing(tlist))
   {
      tlist->deref(xsink);
      tlist = NULL;
   }

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && ((tlist->type != NT_LIST) || tlist->val.list->size()))
   {
      int i = 0;

      while (true)
      {
	 class VLock vl;
	 class QoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (tlist->type != NT_LIST)
	       tlist->deref(xsink);
	    break;
	 }

	 // dereference old value of variable
	 if (*n)
	 {
	    (*n)->deref(xsink);
	    if (xsink->isEvent())
	    {
	       (*n) = NULL;
	       // unlock lock now
	       vl.del();
	       // dereference single value (because it won't be assigned
	       // to the variable and dereferenced later because an 
	       // exception has been thrown)
	       if (tlist->type != NT_LIST)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (tlist->type == NT_LIST)
	    *n = tlist->val.list->eval_entry(i, xsink);
	 else
	    *n = tlist;

	 // unlock variable
	 vl.del();

	 // execute "for" body
	 if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
	 {
	    rc = 0;
	    break;
	 }

	 if (rc == RC_RETURN)
	    break;
	 else if (rc == RC_CONTINUE)
	    rc = 0;
	 i++;
	 if (tlist->type != NT_LIST)
	    break;
	 if (i == tlist->val.list->size())
	    break;
      }
   }
   // dereference list (but not single values; their reference belongs to the
   // variable assignment
   if (tlist && tlist->type == NT_LIST)
      tlist->deref(xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("ForEachStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline int ForEachStatement::execRef(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("ForEachStatement::exec()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // get list evaluation (although may be a single node)
   class QoreNode *tlist, *vr;
   bool is_self_ref = false;
   vr = doPartialEval(list->val.lvexp, &is_self_ref, xsink);
   if (!xsink->isEvent())
   {
      tlist = vr->eval(xsink);
      if (tlist && is_nothing(tlist))
      {
	 tlist->deref(xsink);
	 tlist = NULL;
      }
   }
   else
      tlist = NULL;

   VLock vl;

   // execute "foreach" body
   if (!xsink->isEvent() && tlist && ((tlist->type != NT_LIST) || tlist->val.list->size()))
   {
      class QoreNode *ln = NULL;
      int i = 0;

      if (tlist->type == NT_LIST)
	 ln = new QoreNode(new List());

      while (true)
      {
	 class QoreNode **n = get_var_value_ptr(var, &vl, xsink);
	 if (xsink->isEvent())
	 {
	    // unlock lock now
	    vl.del();
	    // dereference single value (because it won't be assigned
	    // to the variable and dereferenced later because an 
	    // exception has been thrown)
	    if (tlist->type != NT_LIST)
	       tlist->deref(xsink);
	    break;
	 }

	 // dereference old value of variable
	 if (*n)
	 {
	    (*n)->deref(xsink);
	    if (xsink->isEvent())
	    {
	       (*n) = NULL;
	       // unlock lock now
	       vl.del();
	       // dereference single value (because it won't be assigned
	       // to the variable and dereferenced later because an 
	       // exception has been thrown)
	       if (tlist->type != NT_LIST)
		  tlist->deref(xsink);
	       break;
	    }
	 }

	 // assign variable to current value in list
	 if (tlist->type == NT_LIST)
	    *n = tlist->val.list->eval_entry(i, xsink);
	 else
	    *n = tlist;
	 
	 // unlock variable
	 vl.del();
	 
	 // execute "for" body
	 if (code)
	 {
	    rc = code->exec(return_value, xsink);

	    // assign value of variable to referenced variable
	    n = get_var_value_ptr(var, &vl, xsink);
	    if (xsink->isEvent())
	    {
	       // unlock lock now
	       vl.del();
	       break;
	    }

	    QoreNode *nv;
	    if (*n)
	       nv = (*n)->eval(xsink);
	    else
	       nv = NULL;

	    // assign new value to referenced variable
	    if (tlist->type == NT_LIST)
	       ln->val.list->set_entry(i, nv, NULL);
	    else
	       ln = nv;

	    vl.del();
	 }

	 if (!code || xsink->isEvent() || rc == RC_BREAK)
	 {
	    rc = 0;
	    break;
	 }

	 if (rc == RC_RETURN)
	    break;
	 else if (rc == RC_CONTINUE)
	    rc = 0;
	 i++;

	 // break out of loop if appropriate
	 if (tlist->type != NT_LIST || i == tlist->val.list->size())
	    break;
      }

      if (!xsink->isEvent())
      {
	 // write the value back to the lvalue
	 QoreNode **val = get_var_value_ptr(vr, &vl, xsink);
	 if (!xsink->isEvent())
	 {
	    discard(*val, xsink);
	    *val = ln;
	    vl.del();
	 }
	 else
	 {
	    vl.del();
	    discard(ln, xsink);
	 }
      }
   }

    // dereference list (but not single values; their reference belongs to the
   // variable assignment
   if (tlist && tlist->type == NT_LIST)
      tlist->deref(xsink);

   // dereference partial evaluation for lvalue assignment
   if (vr)
      vr->deref(xsink);
   
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);

   traceout("ForEachStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline void DeleteStatement::exec(ExceptionSink *xsink)
{
   delete_var_node(var, xsink);
}

// only executed by Statement::exec()
inline int TryStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   QoreNode *trv = NULL;

   tracein("TryStatement::exec()");
   int rc = 0;
   if (try_block)
      rc = try_block->exec(&trv, xsink);

   /*
   // if thread_exit has been executed
   if (except == (Exception *)1)
      return rc;
   */

   class Exception *except = xsink->catchException();
   if (except)
   {
      printd(5, "TryStatement::exec() entering catch handler, e=%08p\n", except);

      if (catch_block)
      {
	 // save exception
	 catchSaveException(except);

	 if (param)	 // instantiate exception information parameter
	    instantiateLVar(id, makeExceptionObject(except));

	 rc = catch_block->exec(&trv, xsink);

	 // uninstantiate extra args
	 if (param)
	    uninstantiateLVar(xsink);
      }
      else
	 rc = 0;

      xsink->deleteExceptionChain(except);
   }
   /*
   if (finally)
   {
      printd(5, "TryStatement::exec() now executing finally block...\n");
      rc = finally->exec(return_value, xsink);
   }
   */
   if (trv)
      if (*return_value) // NOTE: double return! (maybe should raise an exception here?)
	 trv->deref(xsink);
      else
	 *return_value = trv;
   traceout("TryStatement::exec()");
   return rc;
}

// only executed by Statement::exec()
inline void exec_rethrow(ExceptionSink *xsink)
{
   xsink->rethrow(catchGetException());
}

// only executed by Statement::exec()
inline void ThrowStatement::exec(ExceptionSink *xsink)
{
   class QoreNode *a;
   if (args)
      a = args->eval(xsink);
   else
      a = NULL;

   xsink->raiseException(a);
   if (a)
      a->deref(NULL);
}

// only executed by Statement::exec()
// FIXME: local vars should only be instantiated if there is a non-null context
inline int ContextStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   tracein("ContextStatement::exec()");
   int rc = 0;
   int i;
   class Context *context;
   class QoreNode *cond = NULL, *sort = NULL;
   int sort_type = CM_NO_SORT;

   // find modifiers node
   if (mods)
   {
      ContextMod *w = mods->getHead();
      while (w)
      {
	 switch (w->type)
	 {
	    case CM_WHERE_NODE:
	       if (!cond)
		  cond = w->c.exp;
	       else
	       {
		  // FIXME: should be a parse exception
		  xsink->raiseException("CONTEXT-CREATION-EXCEPTION", "multiple where conditions found for context statement!");
		  return 0;
	       }
	       break;
	    case CM_SORT_ASCENDING:
	    case CM_SORT_DESCENDING:
	       sort = w->c.exp;
	       sort_type = w->type;
	       break;
	 }
	 w = w->next;
      }
   }
   
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // create the context
   context = new Context(name, xsink, exp, cond, sort_type, sort, NULL);

   // execute the statements
   if (code)
      for (context->pos = 0; context->pos < context->max_pos && !xsink->isEvent(); context->pos++)
      {
	 printd(4, "ContextStatement::exec() iteration %d/%d\n", context->pos, context->max_pos);
	 if (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent())
	 {
	    rc = 0;
	    break;
	 }
	 else if (rc == RC_RETURN)
	    break;
	 else if (rc == RC_CONTINUE)
	    rc = 0;
      }

   // destroy the context
   context->deref(xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);

   traceout("ContextStatement::exec()");
   return rc;

}

// only executed by Statement::exec()
inline int ContextStatement::execSummary(class QoreNode **return_value, class ExceptionSink *xsink)
{
   tracein("ContextStatement::execSummary()");
   int rc = 0;
   int i;
   class Context *context;
   class QoreNode *cond = NULL, *sort = NULL, *summarize = NULL;
   int sort_type = CM_NO_SORT;

   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   // find modifiers node
   if (mods)
   {
      ContextMod *w = mods->getHead();
      while (w)
      {
	 switch (w->type)
	 {
	    case CM_WHERE_NODE:
	       if (cond)
	       {
		  // FIXME: should be a parse exception
		  xsink->raiseException("CONTEXT-CREATION-EXCEPTION", "multiple where conditions found for context statement!");
		  return 0;
	       }
	       cond = w->c.exp;
	       break;
	    case CM_SORT_ASCENDING:
	    case CM_SORT_DESCENDING:
	       sort = w->c.exp;
	       sort_type = w->type;
	       break;
	    case CM_SUMMARIZE_BY:
	       summarize = w->c.exp;
	       break;
	 }
	 w = w->next;
      }
   }

   // create the context
   context = new Context(name, xsink, exp, cond, sort_type, sort, summarize);
   
   // execute the statements
   if (code)
   {
      if (context->max_group_pos && !xsink->isEvent())
	 do
	 {
	    if (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent())
	    {
	       rc = 0;
	       break;
	    }
	    else if (rc == RC_RETURN)
	       break;
	    else if (rc == RC_CONTINUE)
	       rc = 0;
	 }
	 while (!xsink->isEvent() && context->next_summary());
   }
   
   // destroy the context
   context->deref(xsink);

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("ContextStatement::execSummary()");
   return rc;
}

// only executed by Statement::exec()
inline int WhileStatement::execWhile(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("WhileStatement::execWhile()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

   while (cond->boolEval(xsink) && !xsink->isEvent())
   {
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }
   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("WhileStatement::execWhile()");
   return rc;
}

// only executed by Statement::exec()
inline int WhileStatement::execDoWhile(class QoreNode **return_value, class ExceptionSink *xsink)
{
   int i, rc = 0;

   tracein("WhileStatement::execDoWhile()");
   // instantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);
      
   do
   {
      if (code && (((rc = code->exec(return_value, xsink)) == RC_BREAK) || xsink->isEvent()))
      {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   } while (cond->boolEval(xsink) && !xsink->isEvent());

   // uninstantiate local variables
   for (i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("WhileStatement::execDoWhile()");
   return rc;
}

Statement::~Statement()
{
   switch (Type)
   {
      case S_RETURN:
	 if (s.node)
	    s.node->deref(NULL);
	 break;
      case S_EXPRESSION:
	 s.node->deref(NULL);
	 break;
      case S_SUBCONTEXT:
      case S_SUMMARY:
      case S_CONTEXT:
	 delete s.SContext;
	 break;
      case S_IF:
	 delete s.If;
	 break;
      case S_WHILE:
      case S_DO_WHILE:
	 delete s.While;
	 break;
      case S_FOR:
	 delete s.For;
	 break;
      case S_FOREACH:
	 delete s.ForEach;
	 break;
      case S_DELETE:
	 delete s.Delete;
	 break;
      case S_SUB_BLOCK:
	 if (s.sub_block)
	    delete s.sub_block;
	 break;
      case S_THROW:
	 delete s.Throw;
	 break;
      case S_TRY:
	 delete s.Try;
	 break;
      case S_SWITCH:
	 delete s.Switch;
	 break;
#ifdef DEBUG
      case S_RETHROW:
      case S_TEMP:
      case S_BREAK:
      case S_CONTINUE:
      case S_THREAD_EXIT:
	 break;
      default:
	 run_time_error("don't know how to delete statement type %d\n", Type);
#endif
   }
}

int Statement::exec(class QoreNode **return_value, ExceptionSink *xsink)
{
   class QoreNode *rv;

   printd(1, "Statement::exec() file=%s line=%d type=%d\n", FileName, LineNumber, Type);
   update_pgm_counter_pgm_file(LineNumber, FileName);
   switch (Type)
   {
      case S_EXPRESSION:
	 if ((rv = s.node->eval(xsink)))
	    rv->deref(xsink);
	 break;
      case S_CONTEXT:
      case S_SUBCONTEXT:
	 return s.SContext->exec(return_value, xsink);
      case S_SUMMARY:
	 return s.SContext->execSummary(return_value, xsink);
      case S_IF:
	 return s.If->exec(return_value, xsink);
      case S_WHILE:
	 return s.While->execWhile(return_value, xsink);
      case S_DO_WHILE:
	 return s.While->execDoWhile(return_value, xsink);
      case S_FOR:
	 return s.For->exec(return_value, xsink);
      case S_FOREACH:
	 return s.ForEach->exec(return_value, xsink);
      case S_DELETE:
	 s.Delete->exec(xsink);
	 return 0;
      case S_SUB_BLOCK:
	 return s.sub_block->exec(return_value, xsink);
      case S_RETURN:
	 if (s.node)
	    (*return_value) = s.node->eval(xsink);
	 return RC_RETURN;
      case S_BREAK:
	 return RC_BREAK;
      case S_CONTINUE:
	 return RC_CONTINUE;
      case S_TRY:
	 return s.Try->exec(return_value, xsink);
      case S_RETHROW:
	 exec_rethrow(xsink);
	 return 0;
      case S_THROW:
	 s.Throw->exec(xsink);
	 return 0;
      case S_THREAD_EXIT:
	 xsink->raiseThreadExit();
	 return 0;
      case S_SWITCH:
	 return s.Switch->exec(return_value, xsink);
   }
   return 0;
}

int StatementBlock::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   tracein("StatementBlock::exec()");
   int rc = 0;
   // instantiate local variables
   for (int i = 0; i < lvars->num_lvars; i++)
      instantiateLVar(lvars->ids[i], NULL);

#ifdef DEBUG
   if (!xsink)
      run_time_error("StatementBlock::exec() xsink=NULL");
#endif

   // execute block
   class Statement *where = head;
   while (where && !xsink->isEvent())
   {
      if ((rc = where->exec(return_value, xsink)))
	 break;
      where = where->next;
   }

   // delete all variables local to this block
   for (int i = 0; i < lvars->num_lvars; i++)
      uninstantiateLVar(xsink);
   traceout("StatementBlock::exec()");
   return rc;
}

// top-level block (program) execution member function
void StatementBlock::exec()
{
   ExceptionSink xsink;
   exec(&xsink);
}

static inline void push_cvar(char *name)
{
   class CVNode *cvn = new CVNode(name);
   cvn->next = getCVarStack();
   updateCVarStack(cvn);
}

static inline void pop_cvar()
{
   class CVNode *cvn = getCVarStack();
   updateCVarStack(cvn->next);
   delete cvn;
}

static inline lvh_t push_local_var(char *name)
{
   class VNode *vnode;

   // check stack for duplicate entries
   if (getProgram()->checkWarning(QP_WARN_DUPLICATE_LOCAL_VARS))
   {
      vnode = getVStack();
      while (vnode)
      {
	 if (!strcmp(vnode->name, name))
	 {
	    getProgram()->makeParseWarning(QP_WARN_DUPLICATE_LOCAL_VARS, "DUPLICATE-LOCAL-VARIABLE", "local variable '%s' was already declared in this lexical scope", name);
	    break;
	 }
	 vnode = vnode->next;
      }
   }
   
   //printd(5, "push_local_var(): pushing var %s\n", name);
   vnode = new class VNode(name);
   vnode->next = getVStack();
   updateVStack(vnode);
   return vnode->name;
}

static inline lvh_t pop_local_var()
{
   class VNode *vnode = getVStack();
   lvh_t rc = vnode->name;

#ifdef DEBUG
   if (!vnode)
      run_time_error("pop_local_var(): empty VStack!");
#endif
   printd(5, "pop_local_var(): popping var %s\n", vnode->name);
   updateVStack(vnode->next);
   delete vnode;
   return rc;
}

static inline lvh_t find_local_var(char *name)
{
   class VNode *vnode = getVStack();

   while (vnode)
   {
      if (!strcmp(vnode->name, name))
	 return vnode->name;
      vnode = vnode->next;
   }
   return 0;
}

inline void VarRef::resolve()
{
   lvh_t id;
   if ((id = find_local_var(name)))
   {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRef::resolve(): local var %s resolved (id=%08p)\n", name, ref.id);
   }
   else
   {
      ref.var = getProgram()->checkVar(name);
      type = VT_GLOBAL;
      printd(5, "VarRef::resolve(): global var %s resolved (var=%08p)\n", name, ref.var);
   }
}

// returns 0 for OK, 1 for would be a new variable
int VarRef::resolveExisting()
{
   lvh_t id;
   if ((id = find_local_var(name)))
   {
      type = VT_LOCAL;
      ref.id = id;
      printd(5, "VarRef::resolveExisting(): local var %s resolved (id=%08p)\n", name, ref.id);
      return 0;
   }

   ref.var = getProgram()->findVar(name);
   type = VT_GLOBAL;
   printd(5, "VarRef::resolveExisting(): global var %s resolved (var=%08p)\n", name, ref.var);
   return !ref.var;
}

// checks for illegal $self assignments in an object context
static inline void checkSelf(class QoreNode *n, lvh_t selfid)
{
   // if it's a variable reference
   if (n->type == NT_VARREF)
   {
      if (n->val.vref->type == VT_LOCAL && n->val.vref->ref.id == selfid)
	 parse_error("illegal assignment to $self in an object context");
      return;
   }
   
   if (n->type != NT_TREE)
      return;

   // otherwise it's a tree: go to root expression 
   while (n->val.tree.left->type == NT_TREE)
      n = n->val.tree.left;

   if (n->val.tree.left->type != NT_VARREF)
      return;

   // left must be variable reference, check if the tree is
   // a list reference; if so, it's invalid
   if (n->val.tree.left->val.vref->type == VT_LOCAL && n->val.tree.left->val.vref->ref.id == selfid
       && n->val.tree.op == OP_LIST_REF)
      parse_error("illegal conversion of $self to a list");
}

static inline void checkLocalVariableChange(class QoreNode *n)
{
   if (n->type == NT_VARREF && n->val.vref->type == VT_LOCAL)
      parse_error("illegal local variable modification in background expression");
}
	    


#define PF_BACKGROUND   1
#define PF_REFERENCE_OK 2
#define PF_RETHROW_OK   4

static inline int getBaseLVType(class QoreNode *n)
{
   while (true)
   {
      if (n->type == NT_SELF_VARREF)
	 return VT_OBJECT;
      if (n->type == NT_VARREF)
	 return n->val.vref->type;
      n = n->val.tree.left;
   }
}

// this function will put variables on the local stack but will not pop them
static int process_node(class QoreNode **node, lvh_t oflag, int pflag)
{
   int lvids = 0, i;
   int current_pflag = pflag;
   pflag &= ~PF_REFERENCE_OK;  // unset "reference ok" for next call

   printd(4, "process_node() %08p type=%s cp=%d, p=%d\n", *node, *node ? (*node)->type->name : "(null)", current_pflag, pflag);
   if (!(*node))
      return 0;

   if ((*node)->type == NT_REFERENCE)
   {
       // otherwise throw a parse exception if an illegal reference is used
       if (!(current_pflag & PF_REFERENCE_OK))
	  parse_error("the reference operator can only be used in function and method call argument lists and in foreach statements");
       else
       {
	  lvids = process_node(&((*node)->val.lvexp), oflag, pflag);
	  // if a background expression is being parsed, then check that no references to local variables
	  // or object members are being used
	  if (pflag & PF_BACKGROUND)
	  {
	     int vtype = getBaseLVType((*node)->val.lvexp);

	     if (vtype == VT_LOCAL)
		parse_error("the reference operator cannot be used with local variables in a background expression");
	     //else if (vtype == VT_OBJECT)
	     //parse_error("the reference operator cannot be used with object members in a background expression");
	  }
       }
       return lvids;
   }
   
   if ((*node)->type == NT_VARREF)
   {
      // if it is a new variable being declared
      if ((*node)->val.vref->type == VT_LOCAL)
      {
	 (*node)->val.vref->ref.id = push_local_var((*node)->val.vref->name);
	 lvids++;
	 //printd(5, "process_node(): local var %s declared (id=%08p)\n", (*node)->val.vref->name, (*node)->val.vref->ref.id);
      }
      else if ((*node)->val.vref->type == VT_GLOBAL)
	 (*node)->val.vref->ref.var = getProgram()->createVar((*node)->val.vref->name);
      else // otherwise reference must be resolved
	 (*node)->val.vref->resolve();

      return lvids;
   }

   if ((*node)->type == NT_BAREWORD)
   {
      // resolve simple constant
      printd(5, "process_node() resolving simple constant \"%s\"\n", (*node)->val.c_str);
      getRootNS()->resolveSimpleConstant(node, 1);
      return lvids;
   }

   if ((*node)->type == NT_CONSTANT)
   {
      printd(5, "process_node() resolving scoped constant \"%s\"\n", (*node)->val.scoped_ref->ostr);
      getRootNS()->resolveScopedConstant(node, 1);
      return lvids;
   }

   if ((*node)->type == NT_COMPLEXCONTEXTREF)
   {
      if (!getCVarStack())
      {
	 parse_error("complex context reference \"%s.%s\" encountered out of context", 
		     (*node)->val.complex_cref->name, (*node)->val.complex_cref->member);
	 return lvids;
      }
      
      int stack_offset = 0;
      int found = 0;
      class CVNode *cvn = getCVarStack();
      while (cvn)
      {
	 if (cvn->name && !strcmp((*node)->val.complex_cref->name, cvn->name))
	 {
	    found = 1;
	    break;
	 }
	 cvn = cvn->next;
	 stack_offset++;
      }
      if (!found)
	 parse_error("\"%s\" does not match any current context", (*node)->val.complex_cref->name);
      else
	 (*node)->val.complex_cref->stack_offset = stack_offset;

      return lvids;
   }

   if ((*node)->type == NT_CONTEXTREF)
   {
      if (!getCVarStack())
	 parse_error("context reference \"%s\" out of context", (*node)->val.c_str);
      return lvids;
   }

   if ((*node)->type == NT_CONTEXT_ROW)
   {
      if (!getCVarStack())
	 parse_error("context row reference \"%%\" encountered out of context");
      return lvids;
   }

   if ((*node)->type == NT_TREE)
   {
      // set "parsing background" flag if the background operator is being parsed
      if ((*node)->val.tree.op == OP_BACKGROUND)
	 pflag |= PF_BACKGROUND;

      // process left branch of tree
      lvids += process_node(&((*node)->val.tree.left), oflag, pflag);
      // process right branch if it exists
      if ((*node)->val.tree.right)
	 lvids += process_node(&((*node)->val.tree.right), oflag, pflag);
      
      // check for illegal changes to local variables in background expressions
      if (pflag & PF_BACKGROUND && (*node)->val.tree.op->needsLValue())
	 checkLocalVariableChange((*node)->val.tree.left);	 

      // throw a parse exception if an assignment is attempted on $self
      if (((*node)->val.tree.op == OP_ASSIGNMENT || (*node)->val.tree.op == OP_SINGLE_ASSIGN) && oflag)
	    checkSelf((*node)->val.tree.left, oflag);
      return lvids;
   }

   if ((*node)->type == NT_FUNCTION_CALL)
   {
      if ((*node)->val.fcall->type == FC_SELF)
      {
	 if (!oflag)
	    parse_error("cannot call member function \"%s\" out of an object member function definition", 
			(*node)->val.fcall->f.sfunc->name);
	 else
	    (*node)->val.fcall->f.sfunc->resolve();
      }
      else if ((*node)->val.fcall->type == FC_UNRESOLVED)
	 getProgram()->resolveFunction((*node)->val.fcall);
      
      if ((*node)->val.fcall->args)
	 for (i = 0; i < (*node)->val.fcall->args->val.list->size(); i++)
	 {
	    class QoreNode **n = (*node)->val.fcall->args->val.list->get_entry_ptr(i);
	    if ((*n)->type == NT_REFERENCE)
	    {
	       if (!(*node)->val.fcall->existsUserParam(i))
		  parse_error("not enough parameters in \"%s\" to accept reference expression", (*node)->val.fcall->getName());
	       lvids += process_node(n, oflag, pflag | PF_REFERENCE_OK);
	    }
	    else
	       lvids += process_node(n, oflag, pflag);
	 }

      return lvids;
   }

   // for the "new" operator
   if ((*node)->type == NT_SCOPE_REF)
   {
      // find object class
      if (((*node)->val.socall->oc = getRootNS()->parseFindScopedClass((*node)->val.socall->name)))
      {
	 // check if parse options allow access to this class
	 if ((*node)->val.socall->oc->getDomain() & getProgram()->getParseOptions())
	    parseException("ILLEGAL-CLASS-INSTANTIATION", "parse options do not allow access to the '%s' class", (*node)->val.socall->oc->getName());
      }
      delete (*node)->val.socall->name;
      (*node)->val.socall->name = NULL;
      if ((*node)->val.socall->args)
	 lvids += process_node(&((*node)->val.socall->args), oflag, pflag);
      
      return lvids;
   }

   if ((*node)->type == NT_FLIST)
      (*node)->type = NT_LIST;

   if ((*node)->type == NT_LIST || (*node)->type == NT_VLIST)
   {
      for (i = 0; i < (*node)->val.list->size(); i++)
	 lvids += process_node((*node)->val.list->get_entry_ptr(i), oflag, pflag);

      return lvids;
   }

   if ((*node)->type == NT_HASH)
   {
      List *keys = (*node)->val.hash->getKeys();
      for (i = 0; i < keys->size(); i++)
      {
	 char *k = keys->retrieve_entry(i)->val.String->getBuffer();
	 class QoreNode **value = (*node)->val.hash->getKeyValuePtr(k);
	 // resolve constant references in keys
	 if (k[0] == HE_TAG_CONST || k[0] == HE_TAG_SCOPED_CONST)
	 {
	    QoreNode *rv;
	    if (k[0] == HE_TAG_CONST)
	       rv = getRootNS()->findConstantValue(k + 1, 1);
	    else
	    {
	       class NamedScope *nscope = new NamedScope(strdup(k + 1));
	       rv = getRootNS()->findConstantValue(nscope, 1);
	       delete nscope;
	    }
	    if (rv)
	    {
	       QoreNode *t;
	       if (rv->type != NT_STRING)
		  t = rv->convert(NT_STRING);
	       else
		  t = rv;
	       
	       // reference value for new hash key
	       (*value)->ref();
	       // not possible to have an exception here
	       (*node)->val.hash->setKeyValue(t->val.String->getBuffer(), *value, NULL);

	       // now reget new value ptr
	       value = (*node)->val.hash->getKeyValuePtr(t->val.String->getBuffer());

	       // or here
	       if (rv != t)
		  t->deref(NULL);
	    }
	    else
	       value = NULL;
	    // delete the old key (not possible to have an exception here)
	    (*node)->val.hash->deleteKey(k, NULL);
	 }
	 if (value)
	    lvids += process_node(value, oflag, pflag);
      }
      keys->derefAndDelete(NULL);
      return lvids;
   }

   if ((*node)->type == NT_FIND)
   {
      push_cvar(NULL);
      lvids += process_node(&((*node)->val.find->find_exp), oflag, pflag);
      lvids += process_node(&((*node)->val.find->where),    oflag, pflag);
      lvids += process_node(&((*node)->val.find->exp),      oflag, pflag);
      pop_cvar();
      return lvids;
   }

   if ((*node)->type == NT_SELF_VARREF)
   {
      //printd(0, "process_node() SELF_REF '%s'  oflag=%d\n", (*node)->val.c_str, oflag);
      if (!oflag)
	 parse_error("cannot reference member \"%s\" out of an object member function definition", 
		     (*node)->val.c_str);
      
      return lvids;
   }
   
   if ((*node)->type == NT_CLASSREF)
      (*node)->val.classref->resolve();

   return lvids;
}

inline void ContextStatement::parseInit(lvh_t oflag, int pflag)
{
   tracein("ContextStatement::parseInit()");

   int i, lvids = 0;

   if (!exp && !getCVarStack())
      parse_error("subcontext statement out of context");

   // initialize context expression
   if (exp)
      lvids += process_node(&exp, oflag, pflag);

   // need to push something on the stack even if the context is not named
   push_cvar(name);

   if (mods)
   {
      ContextMod *w = mods->getHead();
      while (w)
      {
	 //printd(0, "%08p: i=%d/%d\n", this, i, mods->num_mods);
	 switch (w->type)
	 {
	    case CM_SORT_ASCENDING:
	    case CM_SORT_DESCENDING:
	       if (process_node(&(w->c.exp), oflag, pflag))
		  parse_error("local variable declarations not allowed in sort expressions!");
	       break;
	    case CM_WHERE_NODE:
	       lvids += process_node(&(w->c.exp), oflag, pflag);
	       break;
	    case CM_SUMMARIZE_BY:
	       if (process_node(&(w->c.exp), oflag, pflag))
		  parse_error("local variable declarations not allowed in \"summarize by\" expressions!");
	       break;
#ifdef DEBUG
	    default:
	       parse_error("type %d not handled", w->type);
	       traceout("ContextStatement::parseInit()");
	       leave(2);
#endif
	 }
	 w = w->next;
      }
   }

   // initialize statement block
   if (code)
      code->parseInit(oflag, pflag);

   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();

   pop_cvar();
   traceout("ContextStatement::parseInit()");
}

inline void IfStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   lvids += process_node(&cond, oflag, pflag);
   if (if_code)
      if_code->parseInit(oflag, pflag);
   if (else_code)
      else_code->parseInit(oflag, pflag);
   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

inline void WhileStatement::parseWhileInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   lvids += process_node(&cond, oflag, pflag);
   if (code)
      code->parseInit(oflag, pflag);

   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

/* do ... while statements can have variables local to the statement
 * however, it doesn't do much good :-) */
inline void WhileStatement::parseDoWhileInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   if (code)
      code->parseInit(oflag, pflag);
   lvids += process_node(&cond, oflag, pflag);

   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

inline void ForStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   if (assignment)
      lvids += process_node(&assignment, oflag, pflag);
   if (cond)
      lvids += process_node(&cond, oflag, pflag);
   if (iterator)
      lvids += process_node(&iterator, oflag, pflag);
   if (code)
      code->parseInit(oflag, pflag);

   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

inline void ForEachStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   lvids += process_node(&var, oflag, pflag);
   lvids += process_node(&list, oflag, pflag | PF_REFERENCE_OK);
   if (code)
      code->parseInit(oflag, pflag);

   // save local variables 
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

inline void TryStatement::parseInit(lvh_t oflag, int pflag)
{
   if (try_block)
      try_block->parseInit(oflag, pflag);

   // prepare catch block and params
   if (param)
   {
      id = push_local_var(param);
      printd(3, "TryStatement::parseInit() reg. local var %s (id=%08p)\n", param, id);
   }
   else
      id = NULL;
   
   // initialize code block
   if (catch_block)
      catch_block->parseInit(oflag, pflag | PF_RETHROW_OK);

   // pop local param from stack
   if (param)
      pop_local_var();
}

inline int DeleteStatement::parseInit(lvh_t oflag, int pflag)
{
   return process_node(&var, oflag, pflag);
}

inline int ThrowStatement::parseInit(lvh_t oflag, int pflag)
{
   if (args)
      return process_node(&args, oflag, pflag);
   return 0;
}

inline void SwitchStatement::parseInit(lvh_t oflag, int pflag)
{
   int i, lvids = 0;

   lvids += process_node(&sexp, oflag, pflag);

   class CaseNode *w = head;
   while (w)
   {
      if (w->val)
      {
	 getRootNS()->parseInitConstantValue(&w->val, 0);

	 // check for duplicate values
	 class CaseNode *cw = head;
	 while (cw != w)
	 {
	    if (!compareHard(w->val, cw->val))
	       parse_error("duplicate case values in switch");
	    cw = cw->next;
	 }
      }

      if (w->code)
	 w->code->parseInit(oflag, pflag);
      w = w->next;
   }

   // save local variables
   lvars = new LVList(lvids);
   for (i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();
}

// processes a single statement; does not pop variables off stack
int Statement::parseInit(lvh_t oflag, int pflag)
{
   int lvids = 0;

   tracein("Statement::parseInit()");
   printd(2, "Statement::parseInit() %08p type=%d line %d file %s\n", this, Type, LineNumber, FileName);
   // set pgm position in case of errors
   update_pgm_counter_pgm_file(LineNumber, FileName);
   switch (Type)
   {
      case S_SUBCONTEXT:
      case S_SUMMARY:
      case S_CONTEXT:
	 s.SContext->parseInit(oflag, pflag);
	 break;
      case S_EXPRESSION:
	 lvids += process_node(&(s.node), oflag, pflag);
	 break;
      case S_IF:
	 s.If->parseInit(oflag, pflag);
	 break;
      case S_WHILE:
	 s.While->parseWhileInit(oflag, pflag);
	 break;
      case S_DO_WHILE:
	 s.While->parseDoWhileInit(oflag, pflag);
	 break;
      case S_FOR:
	 s.For->parseInit(oflag, pflag);
	 break;
      case S_FOREACH:
	 s.ForEach->parseInit(oflag, pflag);
	 break;
      case S_TRY:
	 s.Try->parseInit(oflag, pflag);
	 break;
      case S_DELETE:
	 lvids += s.Delete->parseInit(oflag, pflag);
	 break;
      case S_SUB_BLOCK:
	 s.sub_block->parseInit(oflag, pflag);
	 break;
      case S_RETURN:
	 if (s.node)
	    lvids += process_node(&(s.node), oflag, pflag);
	 break;
      case S_THROW:
	 lvids += s.Throw->parseInit(oflag, pflag);
	 break;
      case S_RETHROW:
	 if (!(pflag & PF_RETHROW_OK))
	    parse_error("rethrow statements only legal in catch block");
	 break;
      case S_SWITCH:
	 s.Switch->parseInit(oflag, pflag);
	 break;
   }
   traceout("Statement::parseInit()");
   return lvids;
}

void StatementBlock::parseInit(lvh_t oflag, int pflag)
{
   int lvids = 0;

   tracein("StatementBlock::parseInit()");
   printd(4, "StatementBlock::parseInit(b=%08p, oflag=%d) head=%08p tail=%08p\n", this, oflag, head, tail);

   class Statement *where = head, *ret = NULL;
   while (where)
   {
      lvids += where->parseInit(oflag, pflag);
      if (!ret && where->next && where->Type == S_RETURN)
      {
	 // return statement not found at end of block
	 getProgram()->makeParseWarning(QP_WARN_UNREACHABLE_CODE, "UNREACHABLE-CODE", "code after this statement can never be reached");
	 ret = where;
      }
      where = where->next;
   }

   lvars = new LVList(lvids);
   for (int i = 0; i < lvids; i++)
      lvars->ids[i] = pop_local_var();

   printd(4, "StatementBlock::parseInit(): done (lvars = %d, vstack = %08p)\n", lvids, getVStack());
   traceout("StatementBlock::parseInit()");
}

// can also be called with this=NULL
void StatementBlock::parseInit(class Paramlist *params)
{
   tracein("StatementBlock::parseInit()");
   if (params->num_params)
      params->ids = new lvh_t[params->num_params];
   else
      params->ids = NULL;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv");
   printd(5, "StatementBlock::parseInit() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++)
   {
      params->ids[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInit() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->ids[i]);
   }

   // initialize code block
   if (this)
      parseInit((lvh_t)NULL);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();

   traceout("StatementBlock::parseInit()");
}

// can also be called with this=NULL
void StatementBlock::parseInit(class Paramlist *params, class BCList *bcl)
{
   tracein("StatementBlock::parseInit()");
   if (params->num_params)
      params->ids = new lvh_t[params->num_params];
   else
      params->ids = NULL;

   // this is a class constructor method, push local $self variable
   params->selfid = push_local_var("self");
   // set oflag to selfid
   lvh_t oflag = params->selfid;

   // push $argv var on stack and save id
   params->argvid = push_local_var("argv");
   printd(5, "StatementBlock::parseInit() params=%08p argvid=%08p\n", params, params->argvid);

   // init param ids and push local param vars on stack
   for (int i = 0; i < params->num_params; i++)
   {
      params->ids[i] = push_local_var(params->names[i]);
      printd(3, "StatementBlock::parseInit() reg. local var %s (id=%08p)\n", 
	     params->names[i], params->ids[i]);
   }

   // initialize base class constructor arguments
   if (bcl)
   {
      int tlvids = 0;
      class BCNode *w = bcl->head;
      while (w)
      {
	 if (w->args)
	 {
	    for (int i = 0; i < w->args->val.list->size(); i++)
	    {
	       class QoreNode **n = w->args->val.list->get_entry_ptr(i);
	       tlvids += process_node(n, oflag, PF_REFERENCE_OK);
	    }
	 }
	 w = w->next;
      }
      if (tlvids)
      {
	 parse_error("illegal local variable declaration in base class constructor argument");
	 for (int i = 0; i < tlvids; i++)
	    pop_local_var();
      }
   }

   // initialize code block
   if (this)
      parseInit(oflag);

   // pop local param vars from stack
   for (int i = 0; i < params->num_params; i++)
      pop_local_var();

   // pop argv param off stack
   pop_local_var();

   // pop $self id off stack
   pop_local_var();

   traceout("StatementBlock::parseInit()");
}
