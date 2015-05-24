/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSpliceOperatorNode.h
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols
 
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QORESPLICEOPERATORNODE_H

#define _QORE_QORESPLICEOPERATORNODE_H

class QoreSpliceOperatorNode : public LValueOperatorNode {
protected:
   AbstractQoreNode *lvalue_exp, *offset_exp, *length_exp, *new_exp;
   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL static QoreString splice_str;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL ~QoreSpliceOperatorNode() {
      discard(lvalue_exp, 0);
      discard(offset_exp, 0);
      discard(length_exp, 0);
      discard(new_exp, 0);
   }

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return returnTypeInfo;
   }

public:
   DLLLOCAL QoreSpliceOperatorNode(AbstractQoreNode *n_lvalue_exp, AbstractQoreNode *n_offset_exp,
                                   AbstractQoreNode *n_length_exp, AbstractQoreNode *n_new_exp) : lvalue_exp(n_lvalue_exp),
                                                                                                  offset_exp(n_offset_exp),
                                                                                                  length_exp(n_length_exp),
                                                                                                  new_exp(n_new_exp), 
                                                                                                  returnTypeInfo(0) {
   }
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return splice_str.getBuffer();
   }

   //DLLLOCAL AbstractQoreNode *splice(ExceptionSink *xsink) const;
};

#endif
