/*
 FunctionReference.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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
#include <qore/intern/FunctionReference.h>
#include <qore/intern/ObjectMethodReference.h>

#include <stdlib.h>
#include <string.h>

class QoreNode *AbstractFunctionReference::eval(const QoreNode *n)
{
   return n->RefSelf();
}

FunctionReferenceCall::FunctionReferenceCall(class QoreNode *n_exp, class QoreList *n_args) : exp(n_exp), args(n_args)
{
   //printd(0, "FunctionReferenceCall");
}

FunctionReferenceCall::~FunctionReferenceCall()
{
   if (exp)
      exp->deref(0);
   if (args)
      args->deref(0);
}

class QoreNode *FunctionReferenceCall::eval(class ExceptionSink *xsink) const
{
   ReferenceHolder<QoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;
   
   if (!lv || lv->type != NT_FUNCREF)
   {
      xsink->raiseException("REFERENCE-CALL-ERROR", "expression does not evaluate to a call reference");
      return 0;
   }
   return lv->val.funcref->exec(args, xsink);
}

int FunctionReferenceCall::parseInit(lvh_t oflag, int pflag)
{
   int lvids = process_node(&exp, oflag, pflag);
   lvids += process_list_node(&args, oflag, pflag);
   return lvids;
}

ParseObjectMethodReference::ParseObjectMethodReference(class QoreNode *n_exp, char *n_method) : exp(n_exp), method(n_method)
{
}

ParseObjectMethodReference::~ParseObjectMethodReference()
{
   if (exp)
      exp->deref(NULL);
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
class QoreNode *ParseObjectMethodReference::eval(class ExceptionSink *xsink) const
{
   // evaluate lvalue expression
   ReferenceHolder<QoreNode> lv(exp->eval(xsink), xsink);
   if (*xsink)
      return 0;

   QoreObject *o = dynamic_cast<QoreObject *>(*lv);
   if (!o)
   {
      xsink->raiseException("OBJECT-METHOD-REFERENCE-ERROR", "expression does not evaluate to an object");
      return 0;
   }
   return new QoreNode(new RunTimeObjectMethodReference(o, method));
}

int ParseObjectMethodReference::parseInit(lvh_t oflag, int pflag)
{
   return process_node(&exp, oflag, pflag);
}

ParseSelfMethodReference::ParseSelfMethodReference(char *n_method) : method(n_method)
{
}

ParseSelfMethodReference::~ParseSelfMethodReference()
{
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
class QoreNode *ParseSelfMethodReference::eval(class ExceptionSink *xsink) const
{
   return new QoreNode(new RunTimeObjectMethodReference(getStackObject(), method));
}

int ParseSelfMethodReference::parseInit(lvh_t oflag, int pflag)
{
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   return 0;
}

ParseScopedSelfMethodReference::ParseScopedSelfMethodReference(class NamedScope *n_nscope) : nscope(n_nscope), method(0)
{
}

ParseScopedSelfMethodReference::~ParseScopedSelfMethodReference()
{
   delete nscope;
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
class QoreNode *ParseScopedSelfMethodReference::eval(class ExceptionSink *xsink) const
{
   return new QoreNode(new RunTimeObjectScopedMethodReference(getStackObject(), method));
}

int ParseScopedSelfMethodReference::parseInit(lvh_t oflag, int pflag)
{
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   else
   {
      method = getParseClass()->resolveSelfMethod(nscope);
      delete nscope;
      nscope = 0;
   }

   return 0;
}

RunTimeObjectScopedMethodReference::RunTimeObjectScopedMethodReference(class QoreObject *n_obj, const QoreMethod *n_method) : obj(n_obj), method(n_method)
{
   obj->tRef();
}

RunTimeObjectScopedMethodReference::~RunTimeObjectScopedMethodReference()
{
   obj->tDeref();
}

class QoreNode *RunTimeObjectScopedMethodReference::exec(const QoreList *args, class ExceptionSink *xsink) const
{
   return method->eval(obj, args, xsink);
}

AbstractFunctionReference *RunTimeObjectScopedMethodReference::copy()
{
   return new RunTimeObjectScopedMethodReference(obj, method);
}


class QoreProgram *RunTimeObjectScopedMethodReference::getProgram() const
{
   return obj->getProgram();
}

RunTimeObjectMethodReference::RunTimeObjectMethodReference(class QoreObject *n_obj, char *n_method) : obj(n_obj), method(strdup(n_method))
{
   //printd(5, "RunTimeObjectMethodReference::RunTimeObjectMethodReference() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tRef();
}

RunTimeObjectMethodReference::~RunTimeObjectMethodReference()
{
   //printd(5, "RunTimeObjectMethodReference::~RunTimeObjectMethodReference() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tDeref();
   free(method);
}

class QoreNode *RunTimeObjectMethodReference::exec(const QoreList *args, class ExceptionSink *xsink) const
{
   return obj->getClass()->evalMethod(obj, method, args, xsink);
}

AbstractFunctionReference *RunTimeObjectMethodReference::copy()
{
   return new RunTimeObjectMethodReference(obj, method);
}

class QoreProgram *RunTimeObjectMethodReference::getProgram() const
{
   return obj->getProgram();
}

FunctionReference::FunctionReference(char *n_str) : type(FC_UNRESOLVED)
{
   f.str = n_str;
}

FunctionReference::FunctionReference(class UserFunction *n_uf) : type(FC_USER)
{
   f.user.uf = n_uf;
   f.user.pgm = ::getProgram();
   f.user.pgm->depRef();
}

FunctionReference::FunctionReference(class UserFunction *n_uf, class QoreProgram *n_pgm) : type(FC_USER)
{
   f.user.uf = n_uf;
   f.user.pgm = n_pgm;
   f.user.pgm->depRef();
}

FunctionReference::~FunctionReference()
{
   if (type == FC_UNRESOLVED)
   {
      if (f.str)
	 free(f.str);
   }
   else if (type == FC_IMPORTED)
      delete f.ifunc;
}

class QoreProgram *FunctionReference::getProgram() const
{
   if (type == FC_IMPORTED)
      return f.ifunc->pgm;
   if (type == FC_USER)
      return f.user.pgm;
   return NULL;
}

void FunctionReference::del(class ExceptionSink *xsink)
{
   //printd(5, "FunctionReference::del() this=%08p type=%d (%s)\n", this, type == FC_USER ? "user" : "?");
   if (type == FC_USER)
      f.user.pgm->depDeref(xsink);
    delete this;
}

class QoreNode *fr_user_s::eval(const QoreList *args, class ExceptionSink *xsink) const
{
   ProgramContextHelper pch(pgm);
   class QoreNode *rv = uf->eval(args, NULL, xsink);
   return rv;
}

class QoreNode *FunctionReference::exec(const QoreList *args, class ExceptionSink *xsink) const
{
   if (type == FC_USER)
      return f.user.eval(args, xsink);
   else if (type == FC_BUILTIN)
      return f.bf->eval(args, xsink);
   // must be an imported function reference
   return f.ifunc->eval(args, xsink);
}

void FunctionReference::resolve()
{
   ::getProgram()->resolveFunctionReference(this);
}

AbstractFunctionReference *FunctionReference::copy()
{
   assert(type == FC_USER);
   return new FunctionReference(f.user.uf, f.user.pgm);
}

class QoreNode *FunctionReference::eval(const QoreNode *n)
{
   if (type == FC_STATICUSERREF)
      return new QoreNode(new FunctionReference(f.user.uf, f.user.pgm));

   return n->RefSelf();
}
