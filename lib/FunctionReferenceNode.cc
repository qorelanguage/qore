/*
 FunctionReferenceNode.cc
 
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

#include <stdlib.h>
#include <string.h>

FunctionReferenceCall::FunctionReferenceCall(class QoreNode *n_exp, class QoreListNode *n_args) : exp(n_exp), args(n_args)
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

   AbstractFunctionReferenceNode *r = dynamic_cast<AbstractFunctionReferenceNode *>(*lv);
   if (!r)
   {
      xsink->raiseException("REFERENCE-CALL-ERROR", "expression does not evaluate to a call reference (evalated to type '%s')", lv ? lv->getTypeName() : "NOTHING");
      return 0;
   }
   return r->exec(args, xsink);
}

int FunctionReferenceCall::parseInit(lvh_t oflag, int pflag)
{
   int lvids = process_node(&exp, oflag, pflag);
   lvids += process_list_node(&args, oflag, pflag);
   return lvids;
}

ParseObjectMethodReferenceNode::ParseObjectMethodReferenceNode(class QoreNode *n_exp, char *n_method) : exp(n_exp), method(n_method)
{
}

ParseObjectMethodReferenceNode::~ParseObjectMethodReferenceNode()
{
   if (exp)
      exp->deref(NULL);
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
QoreNode *ParseObjectMethodReferenceNode::eval(class ExceptionSink *xsink) const
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
   return new RunTimeObjectMethodReferenceNode(o, method);
}

int ParseObjectMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
{
   return process_node(&exp, oflag, pflag);
}

ParseSelfMethodReferenceNode::ParseSelfMethodReferenceNode(char *n_method) : method(n_method)
{
}

ParseSelfMethodReferenceNode::~ParseSelfMethodReferenceNode()
{
   if (method)
      free(method);
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
class QoreNode *ParseSelfMethodReferenceNode::eval(class ExceptionSink *xsink) const
{
   return new RunTimeObjectMethodReferenceNode(getStackObject(), method);
}

int ParseSelfMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
{
   if (!oflag)
      parse_error("reference to object member '%s' out of a class member function definition", method);
   return 0;
}

ParseScopedSelfMethodReferenceNode::ParseScopedSelfMethodReferenceNode(class NamedScope *n_nscope) : nscope(n_nscope), method(0)
{
}

ParseScopedSelfMethodReferenceNode::~ParseScopedSelfMethodReferenceNode()
{
   delete nscope;
}

// returns a RunTimeObjectMethodReference or NULL if there's an exception
class QoreNode *ParseScopedSelfMethodReferenceNode::eval(class ExceptionSink *xsink) const
{
   return new RunTimeObjectScopedMethodReferenceNode(getStackObject(), method);
}

int ParseScopedSelfMethodReferenceNode::parseInit(lvh_t oflag, int pflag)
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

RunTimeObjectScopedMethodReferenceNode::RunTimeObjectScopedMethodReferenceNode(class QoreObject *n_obj, const QoreMethod *n_method) : obj(n_obj), method(n_method)
{
   obj->tRef();
}

RunTimeObjectScopedMethodReferenceNode::~RunTimeObjectScopedMethodReferenceNode()
{
   obj->tDeref();
}

class QoreNode *RunTimeObjectScopedMethodReferenceNode::exec(const QoreListNode *args, class ExceptionSink *xsink) const
{
   return method->eval(obj, args, xsink);
}

/*
AbstractFunctionReference *RunTimeObjectScopedMethodReferenceNode::copy()
{
   return new RunTimeObjectScopedMethodReferenceNode(obj, method);
}
*/

class QoreProgram *RunTimeObjectScopedMethodReferenceNode::getProgram() const
{
   return obj->getProgram();
}

RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode(class QoreObject *n_obj, char *n_method) : obj(n_obj), method(strdup(n_method))
{
   //printd(5, "RunTimeObjectMethodReferenceNode::RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tRef();
}

RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode()
{
   //printd(5, "RunTimeObjectMethodReferenceNode::~RunTimeObjectMethodReferenceNode() this=%08p obj=%08p (method=%s)\n", this, obj, method);
   obj->tDeref();
   free(method);
}

class QoreNode *RunTimeObjectMethodReferenceNode::exec(const QoreListNode *args, class ExceptionSink *xsink) const
{
   return obj->getClass()->evalMethod(obj, method, args, xsink);
}

/*
AbstractFunctionReference *RunTimeObjectMethodReferenceNode::copy()
{
   return new RunTimeObjectMethodReferenceNode(obj, method);
}
*/

class QoreProgram *RunTimeObjectMethodReferenceNode::getProgram() const
{
   return obj->getProgram();
}

FunctionReferenceNode::FunctionReferenceNode(char *n_str) : frtype(FC_UNRESOLVED)
{
   f.str = n_str;
}

FunctionReferenceNode::FunctionReferenceNode(class UserFunction *n_uf) : frtype(FC_USER)
{
   f.user.uf = n_uf;
   f.user.pgm = ::getProgram();
   f.user.pgm->depRef();
}

FunctionReferenceNode::FunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *n_pgm) : frtype(FC_USER)
{
   f.user.uf = n_uf;
   f.user.pgm = n_pgm;
   f.user.pgm->depRef();
}

FunctionReferenceNode::~FunctionReferenceNode()
{
   if (frtype == FC_UNRESOLVED)
   {
      if (f.str)
	 free(f.str);
   }
   else if (frtype == FC_IMPORTED)
      delete f.ifunc;
}

class QoreProgram *FunctionReferenceNode::getProgram() const
{
   if (frtype == FC_IMPORTED)
      return f.ifunc->pgm;
   if (frtype == FC_USER)
      return f.user.pgm;
   return NULL;
}

void FunctionReferenceNode::deref(ExceptionSink *xsink)
{
   if (ROdereference())
   {
      //printd(5, "FunctionReferenceNode::del() this=%08p type=%d (%s)\n", this, type == FC_USER ? "user" : "?");
      if (frtype == FC_USER)
	 f.user.pgm->depDeref(xsink);
      delete this;	 
   }
}

class QoreNode *fr_user_s::eval(const QoreListNode *args, class ExceptionSink *xsink) const
{
   ProgramContextHelper pch(pgm);
   class QoreNode *rv = uf->eval(args, NULL, xsink);
   return rv;
}

class QoreNode *FunctionReferenceNode::exec(const QoreListNode *args, class ExceptionSink *xsink) const
{
   if (frtype == FC_USER)
      return f.user.eval(args, xsink);
   else if (frtype == FC_BUILTIN)
      return f.bf->eval(args, xsink);
   // must be an imported function reference
   return f.ifunc->eval(args, xsink);
}

void FunctionReferenceNode::resolve()
{
   ::getProgram()->resolveFunctionReference(this);
}

/*
AbstractFunctionReference *FunctionReferenceNode::copy()
{
   assert(frtype == FC_USER);
   return new FunctionReference(f.user.uf, f.user.pgm);
}
*/

QoreNode *FunctionReferenceNode::eval(ExceptionSink *xsink) const
{
   if (frtype == FC_STATICUSERREF)
      return new FunctionReferenceNode(f.user.uf, f.user.pgm);

   return RefSelf();
}
