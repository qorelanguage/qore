/*
  QoreNode.h
  
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

// FIXME: add derefWithObjectDelete() method to QoreNode to do a delete in addition to a deref if the qorenode is an object
//        and use this method instead of calling QoreObject::doDelete directly in places around the library

#ifndef _QORE_NODE_H

#define _QORE_NODE_H

#include <qore/common.h>
#include <qore/ReferenceObject.h>
#include <qore/node_types.h>

#include <string>

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
      class QoreList *list;
      // for Hashes (Associative Arrays)
      class QoreHash *hash;
      // for Objects
      class QoreObject *object;
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
      class Tree *tree;
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
      // for object method references
      class AbstractParseObjectMethodReference *objmethref;
      // for function references
      class AbstractFunctionReference *funcref;
      // for function reference calls
      class FunctionReferenceCall *funcrefcall;
};

class QoreNode : public ReferenceObject
{
   private:
      // not implemented
      DLLLOCAL QoreNode(const QoreNode&);
      DLLLOCAL QoreNode& operator=(const QoreNode&);

   protected:
      inline ~QoreNode();

   public:
      union node_u val;
      class QoreType *type;

      DLLEXPORT QoreNode();
      DLLEXPORT QoreNode(class QoreType *t);
      DLLEXPORT QoreNode(class QoreType *t, int64 v);
      DLLEXPORT QoreNode(long v);
      DLLEXPORT QoreNode(int64 v);
      DLLEXPORT QoreNode(bool v);
      //DLLEXPORT QoreNode(char *str);
      DLLEXPORT QoreNode(const char *str);
      DLLEXPORT QoreNode(const std::string &str);
      DLLEXPORT QoreNode(double d);
      DLLEXPORT QoreNode(class DateTime *dt);
      DLLEXPORT QoreNode(class BinaryObject *b);
      DLLEXPORT QoreNode(class QoreString *str);
      DLLEXPORT QoreNode(class QoreObject *o);
      DLLEXPORT QoreNode(class QoreHash *h);
      DLLEXPORT QoreNode(class QoreList *l);

      DLLEXPORT class QoreNode *convert(const class QoreType *new_type) const;
      DLLEXPORT class QoreNode *convert(const class QoreType *new_type, class ExceptionSink *xsink) const;
      DLLEXPORT bool getAsBool() const;
      DLLEXPORT int getAsInt() const;
      DLLEXPORT int64 getAsBigInt() const;
      DLLEXPORT double getAsFloat() const;
      DLLEXPORT class QoreString *getAsString(int foff, class ExceptionSink *xsink) const;
      
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
      DLLLOCAL QoreNode(class Tree *t);
      DLLLOCAL QoreNode(class FunctionReferenceCall *frc);
      DLLLOCAL QoreNode(class AbstractFunctionReference *afr);
      DLLLOCAL QoreNode(class AbstractParseObjectMethodReference *objmethref);
      DLLLOCAL QoreNode(class FunctionCall *fc);
      
      DLLLOCAL bool needs_eval() const;
      DLLLOCAL class QoreNode *realCopy(class ExceptionSink *xsink) const;
      DLLLOCAL class QoreNode *eval(class ExceptionSink *xsink) const;
      DLLLOCAL class QoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const;
      DLLLOCAL int64 bigIntEval(class ExceptionSink *xsink) const;
      DLLLOCAL int integerEval(class ExceptionSink *xsink) const;
      DLLLOCAL bool boolEval(class ExceptionSink *xsink) const;

      DLLEXPORT class QoreNode *RefSelf() const;
      DLLEXPORT void ref() const;
      DLLEXPORT void deref(class ExceptionSink *xsink);
};

// for getting relative time values or integer values
DLLEXPORT int getSecZeroInt(const QoreNode *a);
DLLEXPORT int getSecZeroBigInt(const QoreNode *a);
DLLEXPORT int getSecMinusOneInt(const QoreNode *a);
DLLEXPORT int getSecMinusOneBigInt(const QoreNode *a);
DLLEXPORT int getMsZeroInt(const QoreNode *a);
DLLEXPORT int getMsZeroBigInt(const QoreNode *a);
DLLEXPORT int getMsMinusOneInt(const QoreNode *a);
DLLEXPORT int getMsMinusOneBigInt(const QoreNode *a);
DLLEXPORT int getMicroSecZeroInt(const QoreNode *a);
DLLEXPORT bool is_nothing(const QoreNode *n);
DLLEXPORT bool is_value(const QoreNode *node);

DLLLOCAL class QoreNode *copy_and_resolve_lvar_refs(const class QoreNode *n, class ExceptionSink *xsink);

static inline void discard(class QoreNode *n, ExceptionSink *xsink)
{
   if (n)
      n->deref(xsink);
}

static inline bool is_null(const class QoreNode *n)
{
   if (n && (n->type == NT_NULL))
      return true;
   return false;
}

class QoreNodeTypeHelper {
   private:
      const QoreNode *node;
      bool temp;

      // not implemented
      DLLLOCAL QoreNodeTypeHelper(const QoreNodeTypeHelper&);
      DLLLOCAL QoreNodeTypeHelper& operator=(const QoreNodeTypeHelper&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLEXPORT QoreNodeTypeHelper(const QoreNode *n, const QoreType *t, class ExceptionSink *xsink);
      DLLEXPORT ~QoreNodeTypeHelper()
      {
         if (node && temp)
            (const_cast<QoreNode *>(node))->deref(NULL);
      }
      // to check for an exception in the constructor
      DLLEXPORT operator bool() const 
      { 
         return !temp || node;
      }
      DLLEXPORT const class QoreNode *operator*()
      {
	 return node;
      }
      // FIXME: eliminate when the QoreNode union is eliminated
      // this is dangerous, but in order for the QoreNode union to be accessible from this
      // helper class, this function is necessary to implement with the const_cast
      // we trust that nobody will change a QoreNodeTypeHelper object anyway
      DLLEXPORT class QoreNode *operator->()
      { 
	 return const_cast<QoreNode *>(node); 
      }
      DLLEXPORT bool is_temp() const
      {
	 return temp;
      }
};

/*
class ConstQoreNodeTypeHelper {
   private:
      const class QoreNode *node;
      bool temp;

      // not implemented
      DLLLOCAL ConstQoreNodeTypeHelper(const ConstQoreNodeTypeHelper&);
      DLLLOCAL ConstQoreNodeTypeHelper& operator=(const ConstQoreNodeTypeHelper&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLEXPORT ConstQoreNodeTypeHelper(const QoreNode *n, const QoreType *t, class ExceptionSink *xsink);
      DLLEXPORT ~ConstQoreNodeTypeHelper()
      {
         if (node && temp)
            node->deref(NULL);
      }
      // to check for an exception in the constructor
      DLLEXPORT operator bool() const 
      { 
         return !temp || node;
      }
      DLLEXPORT const class QoreNode *operator*()
      {
	 return node;
      }
      DLLEXPORT bool is_temp() const
      {
	 return temp;
      }
};
*/

class QoreNodeCStringHelper {
   private:
      class QoreNode *node;
      bool temp;

      // not implemented
      DLLLOCAL QoreNodeCStringHelper(const QoreNodeCStringHelper&);
      DLLLOCAL QoreNodeCStringHelper& operator=(const QoreNodeCStringHelper&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLEXPORT QoreNodeCStringHelper(const QoreNode *n, const class QoreEncoding *enc, class ExceptionSink *xsink);
      DLLEXPORT ~QoreNodeCStringHelper()
      {
	 if (node && temp)
	    node->deref(NULL);
      }
      // to check for an exception in the constructor
      DLLEXPORT operator bool() const 
      { 
	 return !temp || node;
      }
      DLLEXPORT const char *operator*();
      DLLEXPORT bool is_temp() const
      {
	 return temp;
      }
      DLLEXPORT int strlen() const;
};

class QoreNodeEvalOptionalRefHolder {
   private:
      QoreNode *val;
      ExceptionSink *xsink;
      bool needs_deref;

      DLLLOCAL void discard_intern()
      {
	 if (needs_deref && val)
	    val->deref(xsink);
      }

      // not implemented
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const QoreNodeEvalOptionalRefHolder&);
      DLLLOCAL QoreNodeEvalOptionalRefHolder& operator=(const QoreNodeEvalOptionalRefHolder&);
      DLLLOCAL void *operator new(size_t);

   public:
      DLLLOCAL QoreNodeEvalOptionalRefHolder(ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 needs_deref = false;
	 val = 0;
      }
      DLLLOCAL QoreNodeEvalOptionalRefHolder(const QoreNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink)
      {
	 val = exp ? exp->eval(needs_deref, xsink) : 0;
      }
      DLLLOCAL ~QoreNodeEvalOptionalRefHolder()
      {
	 discard_intern();
      }
      DLLLOCAL void discard()
      {
	 discard_intern();
	 needs_deref = false;
	 val = 0;
      }
      DLLLOCAL void assign(bool n_needs_deref, QoreNode *n_val)
      {
	 discard_intern();
	 needs_deref = n_needs_deref;
	 val = n_val;
      }
      // returns a referenced value - the caller will own the reference
      DLLLOCAL QoreNode *getReferencedValue()
      {
	 if (needs_deref)
	    needs_deref = false;
	 else if (val)
	    val->ref();
	 return val;
      }
      DLLLOCAL QoreNode *takeReferencedValue()
      {
	 QoreNode *rv = val && !needs_deref ? val->RefSelf() : val;
	 val = 0;
	 needs_deref = false;
	 return rv;
      }
      DLLLOCAL QoreNode* operator->() { return val; }
      DLLLOCAL QoreNode* operator*() { return val; }
      DLLLOCAL operator bool() const { return val != 0; }
};

#endif
