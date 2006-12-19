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

// FIXME: add derefWithObjectDelete() method to QoreNode to do a delete in addition to a deref if the qorenode is an object
//        and use this method instead of calling Object::doDelete directly in places around the library

#ifndef _QORE_NODE_H

#define _QORE_NODE_H

#include <qore/common.h>
#include <qore/ReferenceObject.h>
#include <qore/Tree.h>
#include <qore/node_types.h>

#define TRACK_REFS     0

#define FMT_NONE   -1
#define FMT_NORMAL 0

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

      DLLEXPORT QoreNode();
      DLLEXPORT QoreNode(class QoreType *t);
      DLLEXPORT QoreNode(class QoreType *t, int64 v);
      DLLEXPORT QoreNode(int64 v);
      DLLEXPORT QoreNode(long v);
      DLLEXPORT QoreNode(bool v);
      DLLEXPORT QoreNode(char *str);
      DLLEXPORT QoreNode(const char *str);
      DLLEXPORT QoreNode(double d);
      DLLEXPORT QoreNode(class DateTime *dt);
      DLLEXPORT QoreNode(class BinaryObject *b);
      DLLEXPORT QoreNode(class QoreString *str);
      DLLEXPORT QoreNode(class Object *o);
      DLLEXPORT QoreNode(class Hash *h);
      DLLEXPORT QoreNode(class List *l);

      DLLEXPORT class QoreNode *convert(class QoreType *new_type);
      DLLEXPORT class QoreNode *convert(class QoreType *new_type, class ExceptionSink *xsink);
      DLLEXPORT bool getAsBool();
      DLLEXPORT int getAsInt();
      DLLEXPORT int64 getAsBigInt();
      DLLEXPORT double getAsFloat();
      DLLEXPORT class QoreString *getAsString(int foff, class ExceptionSink *xsink);
      
      DLLLOCAL QoreNode(char *fn, class QoreNode *a);
      DLLLOCAL QoreNode(class QoreNode *a, char *fn);
      DLLLOCAL QoreNode(class QoreNode *a, class NamedScope *n);
      DLLLOCAL QoreNode(class UserFunction *u, class QoreNode *a);
      DLLLOCAL QoreNode(class BuiltinFunction *b, class QoreNode *a);
      DLLLOCAL QoreNode(class NamedScope *n, class QoreNode *a);
      DLLLOCAL QoreNode(class NamedScope *n);
      DLLLOCAL QoreNode(class ClassRef *c);
      DLLLOCAL QoreNode(class VarRef *v);
      DLLLOCAL QoreNode(class QoreNode *l, class Operator *o, class QoreNode *r);
      DLLLOCAL QoreNode(class RegexSubst *rs);
      DLLLOCAL QoreNode(class RegexTrans *rt);
      DLLLOCAL QoreNode(class ComplexContextRef *ccref);
      DLLLOCAL QoreNode(class LVRef *lvref);
      DLLLOCAL QoreNode(class QoreRegex *r);

      DLLLOCAL class QoreNode *realCopy(class ExceptionSink *xsink);
      DLLLOCAL class QoreNode *eval(class ExceptionSink *xsink);
      DLLLOCAL int64 bigIntEval(class ExceptionSink *xsink);
      DLLLOCAL int integerEval(class ExceptionSink *xsink);
      DLLLOCAL bool boolEval(class ExceptionSink *xsink);

      DLLEXPORT class QoreNode *RefSelf();
      DLLEXPORT void ref();
      DLLEXPORT void deref(class ExceptionSink *xsink);
};

// for getting relative time values or integer values
DLLEXPORT int getSecZeroInt(class QoreNode *a);
DLLEXPORT int getSecZeroBigInt(class QoreNode *a);
DLLEXPORT int getSecMinusOneInt(class QoreNode *a);
DLLEXPORT int getSecMinusOneBigInt(class QoreNode *a);
DLLEXPORT int getMsZeroInt(class QoreNode *a);
DLLEXPORT int getMsZeroBigInt(class QoreNode *a);
DLLEXPORT int getMsMinusOneInt(class QoreNode *a);
DLLEXPORT int getMsMinusOneBigInt(class QoreNode *a);
DLLEXPORT bool is_nothing(class QoreNode *n);
DLLEXPORT bool is_value(class QoreNode *node);

DLLLOCAL class QoreNode *copy_and_resolve_lvar_refs(class QoreNode *n, class ExceptionSink *xsink);

static inline void discard(class QoreNode *n, ExceptionSink *xsink)
{
   if (n)
      n->deref(xsink);
}

static inline bool is_null(class QoreNode *n)
{
   if (n && (n->type == NT_NULL))
      return true;
   return false;
}

#endif
