/*
  FindNode.h

  Qore Programming Language

  Copyright (C) David Nichols 2003, 2004, 2005, 2006

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

#ifndef QORE_FINDNODE_H

#define QORE_FINDNODE_H

class FindNode : public ParseNode
{
   protected:
      // eval(): return value requires a deref(xsink)
      DLLLOCAL virtual class AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @see AbstractQoreNode::eval()
      */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const;

      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      AbstractQoreNode *exp;
      AbstractQoreNode *where;
      AbstractQoreNode *find_exp;
      
      DLLLOCAL FindNode(class AbstractQoreNode *expr, class AbstractQoreNode *FindNode_expr, class AbstractQoreNode *w);
      DLLLOCAL ~FindNode();

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

};


#endif
