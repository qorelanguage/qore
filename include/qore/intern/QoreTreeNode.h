/*
 QoreTreeNode.h
 
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

#ifndef _QORE_QORETREENODE_H

#define _QORE_QORETREENODE_H

class QoreTreeNode : public ParseNode 
{
   protected:
      bool ref_rv;

      DLLLOCAL virtual ~QoreTreeNode();
      
   public:
      Operator *op;
      QoreNode *left;
      QoreNode *right;

      DLLLOCAL QoreTreeNode(QoreNode *l, Operator *op, QoreNode *r = 0);

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // returns the data type
      DLLLOCAL virtual const QoreType *getType() const;
      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;
      // eval(): return value requires a deref(xsink)
      // default implementation = returns "this" with incremented atomic reference count
      DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *xsink) const;
      // default implementation is getAsBigInt()
      DLLLOCAL virtual int64 bigIntEval(class ExceptionSink *xsink) const;
      // default implementation is getAsInt()
      DLLLOCAL virtual int integerEval(class ExceptionSink *xsink) const;
      // default implementation is getAsBool()
      DLLLOCAL virtual bool boolEval(class ExceptionSink *xsink) const;
      // default implementation is getAsFloat()
      DLLLOCAL virtual double floatEval(class ExceptionSink *xsink) const;

      DLLLOCAL void ignoreReturnValue();
};

#endif
