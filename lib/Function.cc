/*
  Function.cc

  Qore Programming language

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
#include <qore/Function.h>
#include <qore/QoreNode.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/qore_thread.h>
#include <qore/Exception.h>
#include <qore/params.h>
#include <qore/CallStack.h>
#include <qore/QoreClass.h>

#include <stdio.h>
#include <ctype.h>

Paramlist::Paramlist(class QoreNode *params)
{
   ids = NULL;
   if (!params)
   {
      num_params = 0;
      names = NULL;
   }
   else if (params->type == NT_VARREF)
   {
      num_params = 1;
      names = new char *[1];
      names[0] = strdup(params->val.vref->name);
      params->deref(NULL);
   }
   else if (params->type != NT_LIST)
   {
      param_error();
      num_params = 0;
      names = NULL;
   }
   else
   {
      num_params = params->val.list->size();
      names = new char *[params->val.list->size()];
      for (int i = 0; i < params->val.list->size(); i++)
      {
         if (params->val.list->retrieve_entry(i)->type != NT_VARREF)
         {
            param_error();
            num_params = 0;
            delete [] names;
            names = NULL;
            break;
         }
         else
            names[i] = strdup(params->val.list->retrieve_entry(i)->val.vref->name);
      }
      params->deref(NULL);
   }
}

// calls a user function
class QoreNode *UserFunction::eval(QoreNode *args, Object *self, class ExceptionSink *xsink)
{
   tracein("UserFunction::eval()");
   printd(2, "UserFunction::eval(): function='%s' args=%08p (size=%d)\n", 
          name, args, args ? args->val.list->size() : 0);

   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::eval() %d: instantiating param lvar %d (%08p %s)\n", i, params->ids[i], n, n ? n->type->name : "(null)");
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
	    //printd(5, "UserFunction::eval() ref self_ref=%d, self=%08p (%s) so=%08p (%s)\n", is_self_ref, self, self ? self->getClass()->name : "NULL", getStackObject(), getStackObject() ? getStackObject()->getClass()->name : "NULL");
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::eval() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      List *l = new List();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      pushCall(name, CT_USER, self);

      // push call on stack
      if (self)
         self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack
      instantiateLVar(params->argvid, argv);
      push_argv(params->argvid);

      // enter gate if necessary
      if (synchronized)
         gate->enter();

      // execute function
      val = statements->exec(xsink);

      // exit gate if necessary
      if (synchronized)
	 gate->exit();

      // pop argv from stack and uninstantiate
      pop_argv();
      uninstantiateLVar(xsink);

      // if self then uninstantiate
      if (self)
	 self->uninstantiateLVar(xsink);

      popCall(xsink);   
   }
   else
      discard(argv, xsink);

   if (num_params)
   {
      printd(5, "UserFunction::eval() about to uninstantiate %d vars\n", 
	     params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::eval()");
   return val;
}


// this function will set up user copy constructor calls
void UserFunction::evalCopy(Object *oold, Object *self, ExceptionSink *xsink)
{
   tracein("UserFunction::evalCopy()");
   printd(2, "UserFunction::evalCopy(): function='%s', num_params=%d, oldobj=%08p\n", name, params->num_params, oold);

   // create QoreNode for "old" for either param or argv list
   oold->ref();
   QoreNode *old = new QoreNode(oold);

   // instantiate local vars from param list
   for (int i = 0; i < params->num_params; i++)
   {
      class QoreNode *n = (i ? NULL : old);
      printd(5, "UserFunction::evalCopy(): instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      instantiateLVar(params->ids[i], n);
   }

   class QoreNode *argv;

   if (!params->num_params)
   {
      List *l = new List();
      l->push(old);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   if (statements)
   {
      // push call on stack
      pushCall(name, CT_USER, self);

      // instantiate self
      self->instantiateLVar(params->selfid);
   
      // instantiate argv and push id on stack (for shift)
      instantiateLVar(params->argvid, argv);
      push_argv(params->argvid);
   
      // execute function
      discard(statements->exec(xsink), xsink);

      // pop argv from stack
      pop_argv();
      uninstantiateLVar(xsink);
   
      // uninstantiate self
      self->uninstantiateLVar(xsink);

      popCall(xsink);
   }
   else
      discard(argv, xsink);

   if (params->num_params)
   {
      printd(5, "UserFunction::evalCopy() about to uninstantiate %d vars\n", 
	     params->num_params);

      // uninstantiate local vars from param list
      for (int i = 0; i < params->num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::evalCopy()");
}

// calls a user constructor method
class QoreNode *UserFunction::evalConstructor(QoreNode *args, Object *self, class BCList *bcl, class BCEAList *bceal, class ExceptionSink *xsink)
{
   tracein("UserFunction::evalConstructor()");
   printd(2, "UserFunction::evalConstructor(): method='%s:%s' args=%08p (size=%d)\n", 
          self->getClass()->getName(), name, args, args ? args->val.list->size() : 0);

   int i = 0;
   class QoreNode *val = NULL;
   int num_args, num_params;

   if (args)
      num_args = args->val.list->size();
   else
      num_args = 0;

   // instantiate local vars from param list
   num_params = params->num_params;
   for (i = 0; i < num_params; i++)
   {
      QoreNode *n = args ? args->val.list->retrieve_entry(i) : NULL;
      printd(4, "UserFunction::evalConstructor() %d: instantiating param lvar %d (%08p)\n", i, params->ids[i], n);
      if (n)
      {
         if (n->type == NT_REFERENCE)
         {
	    bool is_self_ref = false;
            n = doPartialEval(n->val.lvexp, &is_self_ref, xsink);
            if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n, is_self_ref ? getStackObject() : NULL);
         }
         else
         {
            n = n->eval(xsink);
	    if (!xsink->isEvent())
	       instantiateLVar(params->ids[i], n);
         }
	 // the above if block will only instantiate the local variable if no
	 // exceptions have occurred. therefore here we do the cleanup the rest
	 // of any already instantiated local variables if an exception does occur
         if (xsink->isEvent())
         {
            if (n)
               n->deref(xsink);
            for (int j = i; j; j--)
               uninstantiateLVar(xsink);
	    traceout("UserFunction::evalConstructor()");
            return NULL;
         }
      }
      else
         instantiateLVar(params->ids[i], NULL);
   }

   // if there are more arguments than parameters
   printd(5, "UserFunction::evalConstructor() params=%d arg=%d\n", num_params, num_args);
   class QoreNode *argv;
   
   if (num_params < num_args)
   {
      List *l = new List();
      
      for (i = 0; i < (num_args - num_params); i++)
         if (args->val.list->retrieve_entry(i + num_params))
         {
            QoreNode *v = args->val.list->eval_entry(i + num_params, xsink);
            if (xsink->isEvent())
            {
	       if (v)
		  v->deref(xsink);
               l->derefAndDelete(xsink);
               // uninstantiate local vars from param list
               for (int j = 0; j < num_params; j++)
                  uninstantiateLVar(xsink);
               return NULL;
            }
            l->push(v);
         }
         else
            l->push(NULL);
      argv = new QoreNode(l);
   }
   else
      argv = NULL;

   // evaluate base class constructors (if any)
   if (bcl)
      bcl->execConstructorsWithArgs(self, bceal, xsink);

   if (!xsink->isEvent())
   {
      // switch to new program for imported objects
      QoreProgram *cpgm;
      QoreProgram *opgm = self->getProgram();
      if (opgm)
      {
	 cpgm = getProgram();
	 if (opgm && cpgm != opgm)
	    pushProgram(opgm);
      }
      else
	 cpgm = NULL;

      // execute constructor
      if (statements)
      {
	 // push call on stack
	 pushCall(name, CT_USER, self);

	 // instantiate "$self" variable
	 self->instantiateLVar(params->selfid);
	 
	 // instantiate argv and push id on stack
	 instantiateLVar(params->argvid, argv);
	 push_argv(params->argvid);
	 
	 // enter gate if necessary
	 if (synchronized)
	    gate->enter();
	 
	 // execute function
	 val = statements->exec(xsink);
	 
	 // exit gate if necessary
	 if (synchronized)
	    gate->exit();
	 
	 // pop argv from stack and uninstantiate
	 pop_argv();
	 uninstantiateLVar(xsink);
	    
	 // uninstantiate "$self" variable
	 self->uninstantiateLVar(xsink);
	 
	 popCall(xsink);   
      }
      else
	 discard(argv, xsink);
      
      // switch back to original program if necessary
      if (opgm && cpgm != opgm)
	 popProgram();
   }

   if (num_params)
   {
      printd(5, "UserFunction::evalConstructor() about to uninstantiate %d vars\n", params->num_params);

      // uninstantiate local vars from param list
      for (i = 0; i < num_params; i++)
	 uninstantiateLVar(xsink);
   }
   traceout("UserFunction::evalConstructor()");
   return val;
}

// this will only be called with lvalue expressions
class QoreNode *doPartialEval(class QoreNode *n, bool *is_self_ref, class ExceptionSink *xsink)
{
   QoreNode *rv;
   if (n->type == NT_TREE)
   {
      class QoreNode *nn = n->val.tree.right->eval(xsink);
      if (xsink->isEvent())
      {
	 discard(nn, xsink);
	 return NULL;
      }
      rv = new QoreNode(NT_TREE);
      rv->val.tree.right = nn ? nn : nothing();
      rv->val.tree.op = n->val.tree.op;
      if (!(rv->val.tree.left = doPartialEval(n->val.tree.left, is_self_ref, xsink)))
      {
	 rv->deref(xsink);
	 rv = NULL;
      }
   }
   else
   {
      rv = n->RefSelf();
      if (n->type == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}

void print_node(FILE *fp, class QoreNode *node)
{
   class QoreNode *n_node;

   printd(5, "print_node() node=%08p (%s)\n", node, node ? node->type->name : "(null)");
   if (!node)
      return;
   if (node->type != NT_STRING)
   {
      n_node = node->convert(NT_STRING);
      fputs(n_node->val.String->getBuffer(), fp);
      n_node->deref(NULL);
      return;
   }
   fputs(node->val.String->getBuffer(), fp);
}
