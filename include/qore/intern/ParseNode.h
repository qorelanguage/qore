/*
  ParseNode.h
  
  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

   protected:

   public:
      DLLLOCAL ParseNode(qore_type_t t, bool n_needs_eval = true) : SimpleQoreNode(t, false, n_needs_eval)
      {
      }
      DLLLOCAL ParseNode(const ParseNode &) : SimpleQoreNode(type, false, needs_eval_flag)
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
};

// these objects will never be copied or referenced therefore they can have 
// public destructors - the deref() functions just call "delete this;"
class ParseNoEvalNode : public ParseNode
{
   private:
      // not implemented
      DLLLOCAL ParseNoEvalNode& operator=(const ParseNoEvalNode&);

   protected:
      DLLLOCAL virtual int64 bigIntEvalImpl(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual int integerEvalImpl(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual bool boolEvalImpl(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return false;
      }
      DLLLOCAL virtual double floatEvalImpl(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0.0;
      }
      DLLLOCAL virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const
      {
	 assert(false);
	 return 0;
      }

   public:
      DLLLOCAL ParseNoEvalNode(qore_type_t t) : ParseNode(t, false)
      {
      }
      DLLLOCAL ParseNoEvalNode(const ParseNode &) : ParseNode(type, false)
      {
      }
};

#endif
