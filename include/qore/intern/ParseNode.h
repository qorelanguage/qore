/*
  ParseNode.h
  
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

#ifndef _QORE_PARSENODE_H

#define _QORE_PARSENODE_H

class ParseNode : public SimpleQoreNode
{
   private:
      // not implemented
      DLLLOCAL ParseNode& operator=(const ParseNode&);

   public:
      DLLLOCAL ParseNode(const QoreType *t) : SimpleQoreNode(t)
      {
      }
      DLLLOCAL ParseNode(const ParseNode &) : SimpleQoreNode(type)
      {
      }
      // parse types should never be copied
      DLLLOCAL virtual class AbstractQoreNode *realCopy() const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
      {
	 assert(false);
	 return false;
      }
      DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
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
      DLLLOCAL virtual AbstractQoreNode *eval(class ExceptionSink *xsink) const = 0;
      DLLLOCAL virtual AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const
      {
	 ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
	 if (!rv || *xsink) {
	    needs_deref = 0;
	    return 0;
	 }
	 needs_deref = true;
	 return rv.release();
      }
      DLLLOCAL virtual int64 bigIntEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsBigInt() : 0;
      }
      DLLLOCAL virtual int integerEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsInt() : 0;
      }
      DLLLOCAL virtual bool boolEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsBool() : 0;
      }
      DLLLOCAL virtual double floatEval(class ExceptionSink *xsink) const
      {
	 ReferenceHolder<AbstractQoreNode> rv(eval(xsink), xsink);
	 return rv ? rv->getAsFloat() : 0;
      }
};

// these objects will never be copied or referenced therefore they can have 
// public destructors - the deref() functions just call "delete this;"
class ParseNoEvalNode : public ParseNode
{
   private:
      // not implemented
      DLLLOCAL ParseNoEvalNode& operator=(const ParseNoEvalNode&);

   public:
      DLLLOCAL ParseNoEvalNode(const QoreType *t) : ParseNode(t)
      {
      }
      DLLLOCAL ParseNoEvalNode(const ParseNode &) : ParseNode(type)
      {
      }
      DLLLOCAL virtual bool needs_eval() const
      {
	 return false;
      }
      DLLLOCAL virtual AbstractQoreNode *eval(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual AbstractQoreNode *eval(bool &needs_deref, class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual int64 bigIntEval(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual int integerEval(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual bool boolEval(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return false;
      }
      DLLLOCAL virtual double floatEval(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0.0;
      }
      DLLLOCAL virtual void deref()
      {
	 assert(is_unique());
	 delete this;
      }
      DLLLOCAL virtual void deref(class ExceptionSink *xsink)
      {
	 assert(is_unique());
	 delete this;
      }
};

#endif
