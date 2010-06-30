/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreTreeNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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

class QoreTreeNode : public ParseNode {
protected:
   Operator *op;

   DLLLOCAL virtual ~QoreTreeNode();

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   // evalImpl(): return value requires a deref(xsink)
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
      
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

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL void leftParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      if (left) {
         left = left->parseInit(oflag, pflag, lvids, typeInfo);
         checkArg(left);
         //printd(5, "QoreTreeNode::leftParseInit() this=%p new left=%p (%s)\n", this, left, get_type_name(left));
      }
   }

   DLLLOCAL void rightParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      if (right) {
         right = right->parseInit(oflag, pflag, lvids, typeInfo);
         checkArg(right);
         //printd(5, "QoreTreeNode::rightParseInit() this=%p new right=%p (%s)\n", this, right, get_type_name(right));
      }
   }

   DLLLOCAL void checkArg(AbstractQoreNode *arg) {
      if (!is_const_ok())
         return;

      ParseNode *pn = dynamic_cast<ParseNode *>(arg);
      if (pn && !pn->is_const_ok())
         set_const_ok(false);
   }

   DLLLOCAL AbstractQoreNode *defaultParseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&returnTypeInfo) {
      const QoreTypeInfo *typeInfo = 0;
      if (left) {
         left = left->parseInit(oflag, pflag, lvids, typeInfo);
         checkArg(left);
      }
      if (right) {
         right = right->parseInit(oflag, pflag, lvids, typeInfo);
         checkArg(right);
      }

      if (constArgs())
         return evalSubst(returnTypeInfo);

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
   DLLLOCAL AbstractQoreNode *evalSubst(const QoreTypeInfo *&returnTypeInfo) {
      SimpleRefHolder<QoreTreeNode> rh(this);
      ExceptionSink xsink;
      AbstractQoreNode *rv = op->eval(left, right, true, &xsink);
      returnTypeInfo = rv ? getTypeInfoForType(rv->getType()) : nothingTypeInfo;
      return rv ? rv : nothing();
   }
};

#endif
