/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTreeNode.h
 
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

#ifndef _QORE_QORETREENODE_H

#define _QORE_QORETREENODE_H

class QoreTreeNode : public ParseNode {
protected:
   Operator *op;
   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL virtual ~QoreTreeNode();

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return returnTypeInfo;
   }
      
public:
   AbstractQoreNode *left;
   AbstractQoreNode *right;

   DLLLOCAL QoreTreeNode(AbstractQoreNode *l, Operator *op, AbstractQoreNode *r = 0);

   // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   // the ExceptionSink is only needed for QoreObject where a method may be executed
   // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   // returns -1 for exception raised, 0 = OK
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;

   DLLLOCAL void ignoreReturnValue();

   DLLLOCAL void leftParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      assert(!typeInfo);
      if (left) {
         bool for_assignment = pflag & PF_FOR_ASSIGNMENT;
         if (for_assignment && left->getType() == NT_TREE) {
            QoreTreeNode *t = reinterpret_cast<QoreTreeNode *>(left);
            if (t->getOp() != OP_LIST_REF && t->getOp() != OP_OBJECT_REF) {
               parse_error("expression used for assignment requires an lvalue but an expression with the %s operator is used instead", t->getOp()->getName());
               return;
            }

         }

         left = left->parseInit(oflag, pflag, lvids, typeInfo);

         // throw an exception if we are at the bottom left element of a tree being used for assignment
         // and the value is not an lvalue
         if (left && for_assignment && check_lvalue(left))
            parse_error("expression used for assignment requires an lvalue, got '%s' instead", left->getTypeName());
 
         //printd(5, "QoreTreeNode::leftParseInit() this=%p new left=%p (%s)\n", this, left, get_type_name(left));
      }
   }

   DLLLOCAL void rightParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      if (right) {
         typeInfo = 0;
         right = right->parseInit(oflag, pflag & ~PF_FOR_ASSIGNMENT, lvids, typeInfo);
         //printd(0, "QoreTreeNode::rightParseInit() this=%p new right=%p (%s, type: %s)\n", this, right, get_type_name(right), typeInfo->getName());
      }
   }

   DLLLOCAL AbstractQoreNode *defaultParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&rtTypeInfo) {
      const QoreTypeInfo *typeInfo = 0;
      leftParseInit(oflag, pflag, lvids, typeInfo);
      rightParseInit(oflag, pflag, lvids, typeInfo);

      if (constArgs())
         return evalSubst(rtTypeInfo);

      return this;
   }

   DLLLOCAL Operator *getOp() const {
      return op;
   }

   DLLLOCAL void setOp(Operator *n_op) {
      op = n_op;
      set_effect(op->hasEffect());
   }

   // see if args are values
   DLLLOCAL bool constArgs() {
      return left && left->is_value() && (op->numArgs() == 1 || (right && right->is_value()));
   }

   DLLLOCAL AbstractQoreNode* evalSubst(const QoreTypeInfo*& rtTypeInfo) {
      SimpleRefHolder<QoreTreeNode> rh(this);
      ExceptionSink xsink;

      ValueEvalRefHolder v(this, &xsink);
      assert(!xsink);
      AbstractQoreNode* rv = v.getReferencedValue();
      rtTypeInfo = rv ? getTypeInfoForType(rv->getType()) : nothingTypeInfo;
      xsink.clear();
      return rv ? rv : nothing();
   }
};

#endif
