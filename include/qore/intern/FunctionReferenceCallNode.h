/*
 FunctionReferenceCallNode.h
 
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

#ifndef _QORE_FUNCTIONREFERENCECALLNODE_H

#define _QORE_FUNCTIONREFERENCECALLNODE_H

class FunctionReferenceCallNode : public ParseNode
{
   private:
      AbstractQoreNode *exp;    // must evaluate to an AbstractFunctionReference
      QoreListNode *args;

      //! evaluates the value and returns the result
      /** if a qore-language exception occurs, then the result returned must be 0.
	  the result of evaluation can also be 0 (equivalent to NOTHING) as well
	  without an exception.
	  @param xsink if an error occurs, the Qore-language exception information will be added here
	  @return the result of the evaluation (can be 0)
       */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const;

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
      DLLLOCAL FunctionReferenceCallNode(class AbstractQoreNode *n_exp, class QoreListNode *n_args);

      DLLLOCAL virtual ~FunctionReferenceCallNode();

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      //! returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

      DLLLOCAL int parseInit(lvh_t oflag, int pflag);
};

#endif
