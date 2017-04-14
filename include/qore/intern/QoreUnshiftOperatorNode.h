/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreUnshiftOperatorNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREUNSHIFTOPERATORNODE_H

#define _QORE_QOREUNSHIFTOPERATORNODE_H

class QoreUnshiftOperatorNode : public QoreBinaryLValueOperatorNode {
protected:
   const QoreTypeInfo* returnTypeInfo;

   DLLLOCAL static QoreString unshift_str;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return returnTypeInfo;
   }

public:
   DLLLOCAL QoreUnshiftOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryLValueOperatorNode(n_left, n_right), returnTypeInfo(0) {
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = false;
      return &unshift_str;
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      str.concat(&unshift_str);
      return 0;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return unshift_str.getBuffer();
   }

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }

   DLLLOCAL virtual QoreOperatorNode* copyBackground(ExceptionSink* xsink) const {
      return copyBackgroundExplicit<QoreUnshiftOperatorNode>(xsink);
   }
};

#endif
