/*
 FunctionReferenceNode.h
 
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

#ifndef _QORE_FUNCTIONREFERENCE_H

#define _QORE_FUNCTIONREFERENCE_H

class FunctionReferenceCall {
   private:
      class QoreNode *exp;    // must evaluate to an AbstractFunctionReference
      class QoreListNode *args;

   public:
      DLLLOCAL FunctionReferenceCall(class QoreNode *n_exp, class QoreListNode *n_args);
      DLLLOCAL ~FunctionReferenceCall();
      DLLLOCAL QoreNode *eval(class ExceptionSink *xsink) const;
      DLLLOCAL int parseInit(lvh_t oflag, int pflag);
};

// cannot be a ParseNode or SimpleQoreNode because we require deref(xsink)
class AbstractFunctionReferenceNode : public QoreNode
{
   public:
      DLLLOCAL AbstractFunctionReferenceNode() : QoreNode(NT_FUNCREF)
      {
      }

      DLLLOCAL virtual ~AbstractFunctionReferenceNode() {}

      // parse types should never be copied
      DLLLOCAL virtual class QoreNode *realCopy() const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual bool is_equal_soft(const QoreNode *v, ExceptionSink *xsink) const
      {
	 assert(false);
	 return false;
      }
      DLLLOCAL virtual bool is_equal_hard(const QoreNode *v, ExceptionSink *xsink) const
      {
	 assert(false);
	 return false;
      }
      DLLLOCAL virtual bool needs_eval() const
      {
	 return true;
      }
      DLLLOCAL virtual bool is_value() const
      {
	 return false;
      }
      
      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
      {
         str.sprintf("function reference (0x%08p)", this);
         return 0;
      }

      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const
      {
         del = true;
         QoreString *rv = new QoreString();
         getAsString(*rv, foff, xsink);
         return rv;
      }

      // returns the data type
      DLLLOCAL virtual const QoreType *getType() const
      {
         return NT_FUNCREF;
      }

      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const
      {
         return "function reference";
      }
      
      DLLLOCAL virtual int64 bigIntEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<QoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsBigInt() : 0;
      }
      DLLLOCAL virtual int integerEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<QoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsInt() : 0;
      }
      DLLLOCAL virtual bool boolEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<QoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsBool() : 0;
      }
      DLLLOCAL virtual double floatEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<QoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsFloat() : 0;
      }

      DLLLOCAL virtual QoreNode *exec(const QoreListNode *args, ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual void resolve() {}
      //DLLLOCAL virtual AbstractFunctionReferenceNode *copy() = 0;
      DLLLOCAL virtual QoreProgram *getProgram() const = 0;
};

struct fr_user_s {
      class UserFunction *uf;
      class QoreProgram *pgm;

      DLLLOCAL class QoreNode *eval(const class QoreListNode *args, class ExceptionSink *xsink) const;
};

// FIXME: split into separate classes
class FunctionReferenceNode : public AbstractFunctionReferenceNode
{
   private:
      DLLLOCAL virtual ~FunctionReferenceNode();

   public:
      union {
	    struct fr_user_s user;
	    class BuiltinFunction *bf;
	    class ImportedFunctionCall *ifunc;
	    char *str;
      } f;
      int frtype;
      
      DLLLOCAL FunctionReferenceNode(char *n_str);
      DLLLOCAL FunctionReferenceNode(class UserFunction *n_uf);
      DLLLOCAL FunctionReferenceNode(class UserFunction *n_uf, class QoreProgram *pgm);
      DLLLOCAL virtual void deref(class ExceptionSink *xsink);
      DLLLOCAL virtual class QoreNode *exec(const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL virtual void resolve();
      //DLLLOCAL virtual AbstractFunctionReferenceNode *copy();
      DLLLOCAL void set_static(class UserFunction *n_uf, class QoreProgram *n_pgm)
      {
	 frtype = FC_STATICUSERREF;
	 f.user.uf = n_uf;
	 f.user.pgm = n_pgm;
      }
      DLLLOCAL virtual QoreNode *eval(class ExceptionSink *xsink) const;
      DLLLOCAL virtual class QoreProgram *getProgram() const;
};

class RunTimeObjectMethodReferenceNode : public AbstractFunctionReferenceNode
{
   private:
      class QoreObject *obj;
      char *method;

      DLLLOCAL virtual ~RunTimeObjectMethodReferenceNode();
   
   public:
      DLLLOCAL RunTimeObjectMethodReferenceNode(class QoreObject *n_obj, char *n_method);
      DLLLOCAL virtual class QoreNode *exec(const class QoreListNode *args, class ExceptionSink *xsink) const;
      //DLLLOCAL virtual AbstractFunctionReferenceNode *copy();
      DLLLOCAL virtual class QoreProgram *getProgram() const;
};

class RunTimeObjectScopedMethodReferenceNode : public AbstractFunctionReferenceNode
{
   private:
      class QoreObject *obj;
      const class QoreMethod *method;

      DLLLOCAL virtual ~RunTimeObjectScopedMethodReferenceNode();

   public:
      DLLLOCAL RunTimeObjectScopedMethodReferenceNode(class QoreObject *n_obj, const class QoreMethod *n_method);
      DLLLOCAL virtual class QoreNode *exec(const class QoreListNode *args, class ExceptionSink *xsink) const;
      //DLLLOCAL virtual AbstractFunctionReferenceNode *copy();
      DLLLOCAL virtual class QoreProgram *getProgram() const;
};


//FIXME: move to params.h
static inline AbstractFunctionReferenceNode *test_funcref_param(const QoreListNode *n, int i)
{
   if (!n) return 0;
   return dynamic_cast<AbstractFunctionReferenceNode *>(n->retrieve_entry(i));
}

#endif
