/*
  QoreNode.h
  
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

#ifndef _QORE_NODE_H

#define _QORE_NODE_H

#include <qore/common.h>
#include <qore/ReferenceObject.h>

#define TRACK_REFS     0

#define FMT_NONE   -1
#define FMT_NORMAL 0

class Tree {
   public:
      class Operator *op;
      class QoreNode *left;
      class QoreNode *right;
      
      inline class QoreNode *eval(class ExceptionSink *xsink);
};

class ClassRef {
   private:
      class NamedScope *cscope;
      int cid;

   public:
      inline ClassRef(class NamedScope *n)
      {
	 cscope = n;
      }
      inline ~ClassRef();
      inline void resolve();
      inline int getID() 
      {
	 return cid;
      }
};

union node_u {
      int64 intval;
      // for boolean values
      bool boolval;
      char c_char;
      int c_int;
      double floatval;
      class QoreString *String;
      // to initialize unresolved function calls, backquote expressions, etc
      char *c_str;
      // for dates
      class DateTime *date_time;
      // for binary objects
      class BinaryObject *bin;
      // for Lists
      class List *list;
      // for Hashes (Associative Arrays)
      class Hash *hash;
      // for Objects
      class Object *object;
      // for variable references
      class VarRef *vref;
      // for find expressions
      class Find *find;
      // for function calls
      class FunctionCall *fcall;
      // for Scoped Function Valls
      class ScopedObjectCall *socall;
      // for constant references
      class NamedScope *scoped_ref;
      // for complex context references
      class ComplexContextRef *complex_cref;
      // for a pointer to objects
      void *ptr;
      // for an expression with an operator
      class Tree tree;
      // for references to an lvalue
      class QoreNode *lvexp;
      // for regular expression substitutions
      class RegexSubst *resub;
      // for regular expression matches
      class QoreRegex *regex;
      // for regular expression translations
      class RegexTrans *retrans;
      // for class references
      class ClassRef *classref;
};

class QoreNode : public ReferenceObject
{
   private:

   protected:
      inline ~QoreNode();

   public:
      class QoreType *type;
      union node_u val;

      inline void delete_contents();
      inline QoreNode();
      inline QoreNode(class QoreType *t);
      inline QoreNode(class QoreType *t, int64 v);
      inline QoreNode(int64 v);
      inline QoreNode(long v);
      inline QoreNode(bool v);
      inline QoreNode(char *str);
      inline QoreNode(const char *str);
      inline QoreNode(double d);
      inline QoreNode(class DateTime *dt);
      inline QoreNode(class BinaryObject *b);
      inline QoreNode(class QoreString *str);
      inline QoreNode(class Object *o);
      inline QoreNode(class Hash *h);
      inline QoreNode(class List *l);
      inline QoreNode(char *fn, class QoreNode *a);
      inline QoreNode(class QoreNode *a, char *fn);
      inline QoreNode(class QoreNode *a, class NamedScope *n);
      inline QoreNode(class UserFunction *u, class QoreNode *a);
      inline QoreNode(class BuiltinFunction *b, class QoreNode *a);
      inline QoreNode(class NamedScope *n, class QoreNode *a);
      inline QoreNode(class NamedScope *n);
      inline QoreNode(class ClassRef *c);
      inline QoreNode(class VarRef *v);
      inline QoreNode(class QoreNode *l, class Operator *o, class QoreNode *r);
      inline QoreNode(class RegexSubst *rs);
      inline QoreNode(class RegexTrans *rt);
      inline QoreNode(class ComplexContextRef *ccref);
      inline QoreNode(class LVRef *lvref);
      inline QoreNode(class QoreRegex *r);

      inline class QoreNode *realCopy(class ExceptionSink *xsink);
      inline class QoreNode *eval(class ExceptionSink *xsink);
      inline int64 bigIntEval(class ExceptionSink *xsink);
      inline int integerEval(class ExceptionSink *xsink);
      inline bool boolEval(class ExceptionSink *xsink);
      class QoreNode *convert(class QoreType *new_type);
      class QoreNode *convert(class QoreType *new_type, class ExceptionSink *xsink);
      bool getAsBool();
      int getAsInt();
      int64 getAsBigInt();
      double getAsFloat();
      inline class QoreString *getAsString(int foff, class ExceptionSink *xsink);

      inline class QoreNode *RefSelf();
      inline void ref();
      inline void deref(class ExceptionSink *xsink);
};

class ScopedObjectCall 
{
   public:
      class NamedScope *name;
      class QoreClass *oc;
      class QoreNode *args;
      inline ScopedObjectCall(class NamedScope *n, class QoreNode *a) { name = n; args = a; }
      inline ~ScopedObjectCall();
};

class QoreNode *copy_and_resolve_lvar_refs(class QoreNode *n, class ExceptionSink *xsink);

static inline void discard(class QoreNode *n, ExceptionSink *xsink);
static inline class QoreNode *nothing();
static inline class QoreNode *null();
static inline class QoreNode *zero();
static inline class QoreNode *zero_float();
static inline class QoreNode *zero_date();
static inline class QoreNode *null_string();
static inline bool is_nothing(class QoreNode *n);
static inline bool is_null(class QoreNode *n);
static inline bool is_value(class QoreNode *node);

// for getting relative time values or integer values
static inline int getSecZeroInt(class QoreNode *a);
static inline int getSecZeroBigInt(class QoreNode *a);
static inline int getSecMinusOneInt(class QoreNode *a);
static inline int getSecMinusOneBigInt(class QoreNode *a);
static inline int getMsZeroInt(class QoreNode *a);
static inline int getMsZeroBigInt(class QoreNode *a);
static inline int getMsMinusOneInt(class QoreNode *a);
static inline int getMsMinusOneBigInt(class QoreNode *a);

#include <qore/support.h>
#include <qore/config.h>
#include <qore/Hash.h>
#include <qore/DateTime.h>
#include <qore/QoreString.h>
#include <qore/QoreType.h>
#include <qore/List.h>
#include <qore/Object.h>
#include <qore/QoreClass.h>
#include <qore/Function.h>
#include <qore/Find.h>
#include <qore/Namespace.h>
#include <qore/Operator.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

inline class QoreNode *Tree::eval(class ExceptionSink *xsink)
{
   return op->eval(left, right, xsink);
}

inline ClassRef::~ClassRef()
{
   if (cscope)
      delete cscope;
}

inline void ClassRef::resolve()
{
   if (cscope)
   {
      class QoreClass *qc = getRootNS()->parseFindScopedClass(cscope);
      if (qc)
	 cid = qc->getID();
      delete cscope;
      cscope = NULL;
   }
}

inline ScopedObjectCall::~ScopedObjectCall()
{
   if (name)
      delete name; 
   if (args)
      args->deref(NULL);
}

static inline void discard(class QoreNode *n, ExceptionSink *xsink)
{
   if (n)
      n->deref(xsink);
}

inline class QoreString *QoreNode::getAsString(int foff, class ExceptionSink *xsink)
{
   if (!this)
      return new QoreString("<NOTHING>");

   return type->getAsString(this, foff, xsink);
}

inline class QoreNode *QoreNode::convert(class QoreType *new_type)
{
   // if source and target are the same, then reference and return
   // no exception can be triggered because new_type can never be an 
   // object
   if (type == new_type)
      return RefSelf();

   return new_type->convertTo(this, NULL);
}

inline class QoreNode *QoreNode::convert(class QoreType *new_type, class ExceptionSink *xsink)
{
   // if source and target are the same, then reference and return
   // no exception can be triggered because new_type can never be an 
   // object
   if (type == new_type)
      return RefSelf();

   return new_type->convertTo(this, xsink);
}

static inline class QoreNode *nothing()
{
   Nothing->ref();
   return Nothing;
}

static inline class QoreNode *null()
{
   Null->ref();
   return Null;
}

static inline class QoreNode *boolean_false()
{
   False->ref();
   return False;
}

static inline class QoreNode *boolean_true()
{
   True->ref();
   return True;
}

static inline class QoreNode *zero()
{
   Zero->ref();
   return Zero;
}

static inline class QoreNode *zero_float()
{
   ZeroFloat->ref();
   return ZeroFloat;
}

static inline class QoreNode *zero_date()
{
   ZeroDate->ref();
   return ZeroDate;
}

static inline class QoreNode *null_string()
{
   NullString->ref();
   return NullString;
}

static inline bool is_nothing(class QoreNode *n)
{
   if (!n || (n->type == NT_NOTHING))
      return true;
   
   if (n->type == NT_OBJECT && !n->val.object->isValid())
      return true;

   return false;
}

static inline bool is_null(class QoreNode *n)
{
   if (n && (n->type == NT_NULL))
      return true;
   return false;
}

static inline bool is_value(class QoreNode *node)
{
   // the only container types that can be created at parse time are lists and hashes

   if (node->type == NT_LIST)
      return !node->val.list->needsEval();
   else if (node->type == NT_HASH)
      return !node->val.hash->needsEval();

   return node->type->isValue();
}

inline void QoreNode::ref()
{
#ifdef DEBUG
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (%d->%d)\n", this, type->name, references, references + 1);
#endif
/*
   if (type == NT_OBJECT)
      run_time_error("QoreNode::ref(this=%08p) is an object (class=%s, rc=%d)!", this, val.object->getClass()->name, reference_count());
*/
#endif
   ROreference();
}

inline class QoreNode *QoreNode::RefSelf()
{
   ref();
   return this;
}

inline void QoreNode::deref(ExceptionSink *xsink)
{
   //tracein("QoreNode::deref()");
#ifdef DEBUG
#if TRACK_REFS
   printd(5, "QoreNode::deref() %08p type=%s (%d->%d)\n", this, type->name, references, references - 1);
#endif
   if (references <= 0)
      run_time_error("QoreNode::deref(): %08p has references = %d (type=%s)!",
		     this, references, type->name);
   if (references > 51200)
      if (type == NT_INT)
	 printd(0, "QoreNode::deref() WARNING, node %08p references=%d (type=%s) (val=%d)\n",
		this, references, type->name, val.intval);
      else if (type == NT_STRING)
	 printd(0, "QoreNode::deref() WARNING, node %08p references=%d (type=%s) (val=\"%s\")\n",
		this, references, type->name, val.String->getBuffer());
      else
	 printd(0, "QoreNode::deref() WARNING, node %08p references=%d (type=%s)\n",
		this, references, type->name);
#endif

   if (ROdereference())
   {
      if (type == NT_LIST)
	 val.list->dereference(xsink);
      else if (type == NT_HASH)
	 val.hash->dereference(xsink);
      else if (type == NT_OBJECT)
	 val.object->dereference(xsink);

      // now delete this QoreNode
      delete this;
   }

   //traceout("QoreNode::deref()");
}

inline class QoreNode *QoreNode::realCopy(ExceptionSink *xsink)
{
   //tracein("QoreNode::realCopy()");
#ifdef DEBUG
   if (!this) 
      run_time_error("Node::realCopy() ERROR NULL ptr passed!"); 
#endif
   //traceout("QoreNode::realCopy()");
   return type->copy(this, xsink);
}

/*
  QoreNode::eval(): return value requires a dereference
 */
inline class QoreNode *QoreNode::eval(ExceptionSink *xsink)
{
   return type->eval(this, xsink);
}

inline void QoreNode::delete_contents()
{
#if 0
   printd(5, "delete_contents() type=%s\n", type->name);
#endif
   return type->deleteContents(this);
}

inline int64 QoreNode::bigIntEval(ExceptionSink *xsink)
{
   class QoreNode *new_node = eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_node, xsink);
      return 0;
   }
   // assign to zero if null
   if (!new_node)
      return (int64)0;

   // convert value to integer if necessary
   int64 rv = new_node->getAsBigInt();
   new_node->deref(xsink);
   return rv;
}

inline int QoreNode::integerEval(ExceptionSink *xsink)
{
   class QoreNode *new_node = eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_node, xsink);
      return 0;
   }
   // assign to zero if null
   if (!new_node)
      return 0;

   // convert value to integer if necessary
   int rv = new_node->getAsInt();
   new_node->deref(xsink);
   return rv;
}

inline bool QoreNode::boolEval(ExceptionSink *xsink)
{
   class QoreNode *new_node = eval(xsink);
   if (xsink->isEvent())
   {
      discard(new_node, xsink);
      return false;
   }
   // assign to zero if null
   if (!new_node)
      return false;

   bool rv = new_node->getAsBool();
   new_node->deref(xsink);
   return rv;
}

inline QoreNode::QoreNode() 
{
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=unknown (0->1)\n", this);
#endif
}

inline QoreNode::QoreNode(class QoreType *t) 
{
   type = t; 
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(int64 v) 
{ 
   type = NT_INT;
   val.intval = v; 
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(long v) 
{ 
   type = NT_INT;
   val.intval = (int64)v; 
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class QoreType *t, int64 v) 
{ 
   type = t;
   val.intval = v; 
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(bool v)
{
   type = NT_BOOLEAN;
   val.boolval = v;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class Hash *h)
{
   type = NT_HASH;
   val.hash = h;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(Object *o)
{
   type = NT_OBJECT;
   val.object = o;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(List *l)
{
   type = NT_LIST;
   val.list = l;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(double f)
{
   type = NT_FLOAT;
   val.floatval = f;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class BinaryObject *b)
{
   type = NT_BINARY;
   val.bin = b;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(DateTime *dt)
{
   type = NT_DATE;
   val.date_time = dt;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class QoreString *str)
{
   type = NT_STRING;
   val.String = str;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(char *str)
{
   //fprintf(stderr,"QoreNode::QoreNode(char *= %s)\n", str);
   type = NT_STRING;
   val.String = new QoreString(str);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(const char *str)
{
   //fprintf(stderr,"QoreNode::QoreNode(char *= %s)\n", str);
   type = NT_STRING;
   val.String = new QoreString((char *)str);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(char *name, class QoreNode *a)
{
   type = NT_FUNCTION_CALL;
   val.fcall = new FunctionCall(name, a);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

// for non-scoped in-object calls
inline QoreNode::QoreNode(class QoreNode *a, char *name)
{
   type = NT_FUNCTION_CALL;
   val.fcall = new FunctionCall(a, name);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

// for in-object base class calls
inline QoreNode::QoreNode(class QoreNode *a, class NamedScope *n)
{
   type = NT_FUNCTION_CALL;
   val.fcall = new FunctionCall(a, n);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class UserFunction *u, class QoreNode *a)
{
   type = NT_FUNCTION_CALL;
   val.fcall = new FunctionCall(u, a);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class BuiltinFunction *b, class QoreNode *a)
{
   type = NT_FUNCTION_CALL;
   val.fcall = new FunctionCall(b, a);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class NamedScope *n, class QoreNode *a)
{
   type = NT_SCOPE_REF;
   val.socall = new ScopedObjectCall(n, a);
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class NamedScope *n)
{
   type = NT_CONSTANT;
   val.scoped_ref = n;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class ClassRef *c)
{
   type = NT_CLASSREF;
   val.classref = c;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class VarRef *v)
{
   type = NT_VARREF;
   val.vref = v;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class QoreNode *l, class Operator *o, class QoreNode *r)
{
   type = NT_TREE;
   val.tree.left = l;
   val.tree.op = o;
   val.tree.right = r;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class RegexSubst *rs)
{
   type = NT_REGEX_SUBST;
   val.resub = rs;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class RegexTrans *rt)
{
   type = NT_REGEX_TRANS;
   val.retrans = rt;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class QoreRegex *r)
{
   type = NT_REGEX;
   val.regex = r;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::QoreNode(class ComplexContextRef *ccref)
{
   type = NT_COMPLEXCONTEXTREF;
   val.complex_cref = ccref;
#if TRACK_REFS
   printd(5, "QoreNode::ref() %08p type=%s (0->1)\n", this, type->name);
#endif
}

inline QoreNode::~QoreNode()
{
   delete_contents(); 
}

inline bool QoreNode::getAsBool()
{
   if (type == NT_BOOLEAN)
      return val.boolval;
   else if (type == NT_FLOAT)
      return (bool)val.floatval;
   else if (type == NT_INT)
      return (bool)val.intval;

   QoreNode *t = this->convert(NT_BOOLEAN);
   bool rc = t->val.boolval;
   t->deref(NULL);
   return rc;
}

inline int QoreNode::getAsInt()
{
   if (type == NT_INT)
      return val.intval;
   else if (type == NT_BOOLEAN)
      return (int)val.boolval;
   else if (type == NT_FLOAT)
      return (int)val.floatval;

   QoreNode *t = this->convert(NT_INT);
   int rc = t->val.intval;
   t->deref(NULL);
   return rc;
}

inline int64 QoreNode::getAsBigInt()
{
   if (type == NT_INT)
      return val.intval;
   else if (type == NT_BOOLEAN)
      return (int64)val.boolval;
   else if (type == NT_FLOAT)
      return (int64)val.floatval;

   QoreNode *t = this->convert(NT_INT);
   int64 rc = t->val.intval;
   t->deref(NULL);
   return rc;
}

inline double QoreNode::getAsFloat()
{
   if (type == NT_FLOAT)
      return val.floatval;
   else if (type == NT_BOOLEAN)
      return (double)val.boolval;
   else if (type == NT_INT)
      return (double)val.intval;

   QoreNode *t = this->convert(NT_FLOAT);
   double rc = t->val.floatval;
   t->deref(NULL);
   return rc;
}

// call to get a node with reference count 1
inline void ensure_unique(class QoreNode **v, class ExceptionSink *xsink)
{
   if ((*v)->reference_count() > 1)
   {
      QoreNode *old = *v;
      (*v) = old->realCopy(xsink);
      old->deref(xsink);
   }
}

// call to get a node with the specified type
inline void ensure_type(class QoreNode **v, class QoreType *type, class ExceptionSink *xsink)
{
   if ((*v)->type != type)
   {
      class QoreNode *n = (*v)->convert(type);
      (*v)->deref(xsink);
      (*v) = n;
   }
}

// for getting relative time values or integer values
static inline int getSecZeroInt(class QoreNode *a)
{
   if (is_nothing(a))
      return 0;
   if (a->type == NT_DATE)
      return (int)a->val.date_time->getRelativeSeconds();
   return a->getAsInt();
}
static inline int getSecZeroBigInt(class QoreNode *a)
{
   if (is_nothing(a))
      return 0;
   if (a->type == NT_DATE)
      return a->val.date_time->getRelativeSeconds();
   return a->getAsBigInt();
}
// for getting relative time values or integer values
static inline int getSecMinusOneInt(class QoreNode *a)
{
   if (is_nothing(a))
      return -1;
   if (a->type == NT_DATE)
      return (int)a->val.date_time->getRelativeSeconds();
   return a->getAsInt();
}
static inline int getSecMinusOneBigInt(class QoreNode *a)
{
   if (is_nothing(a))
      return -1;
   if (a->type == NT_DATE)
      return a->val.date_time->getRelativeSeconds();
   return a->getAsBigInt();
}
static inline int getMsZeroInt(class QoreNode *a)
{
   if (is_nothing(a))
      return 0;
   if (a->type == NT_DATE)
      return (int)a->val.date_time->getRelativeMilliseconds();
   return a->getAsInt();
}
static inline int getMsZeroBigInt(class QoreNode *a)
{
   if (is_nothing(a))
      return 0;
   if (a->type == NT_DATE)
      return a->val.date_time->getRelativeMilliseconds();
   return a->getAsBigInt();
}
// for getting relative time values or integer values
static inline int getMsMinusOneInt(class QoreNode *a)
{
   if (is_nothing(a))
      return -1;
   if (a->type == NT_DATE)
      return (int)a->val.date_time->getRelativeMilliseconds();
   return a->getAsInt();
}
static inline int getMsMinusOneBigInt(class QoreNode *a)
{
   if (is_nothing(a))
      return -1;
   if (a->type == NT_DATE)
      return a->val.date_time->getRelativeMilliseconds();
   return a->getAsBigInt();
}
static inline int getMicroSecZeroInt(class QoreNode *a)
{
   if (is_nothing(a))
      return 0;
   if (a->type == NT_DATE)
      return (int)a->val.date_time->getRelativeMilliseconds() * 1000;
   return a->getAsInt();
}

#endif
