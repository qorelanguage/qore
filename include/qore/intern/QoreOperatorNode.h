/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreOperatorNode.h
 
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

#ifndef _QORE_QOREOPERATORNODE_H
#define _QORE_QOREOPERATORNODE_H

class QoreOperatorNode : public ParseNode {
protected:
   bool ref_rv;

   DLLLOCAL virtual ~QoreOperatorNode() {}

   DLLLOCAL virtual void ignoreReturnValueImpl() {}

public:
   DLLLOCAL QoreOperatorNode(bool n_ref_rv = true) : ParseNode(NT_OPERATOR), ref_rv(n_ref_rv) {
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const = 0;

   DLLLOCAL void ignoreReturnValue() {
      ref_rv = false;
      ignoreReturnValueImpl();
   }
   
   DLLLOCAL virtual bool hasEffect() const = 0;
};

class QoreSingleExpressionOperatorNode : public QoreOperatorNode {
protected:
   AbstractQoreNode *exp;

   DLLLOCAL ~QoreSingleExpressionOperatorNode() {
      if (exp)
         exp->deref(0);
   }

public:
   DLLLOCAL QoreSingleExpressionOperatorNode(AbstractQoreNode *n_exp) : exp(n_exp) {
   }
   DLLLOCAL AbstractQoreNode *getExp() {
      return exp;
   }
   DLLLOCAL const AbstractQoreNode *getExp() const {
      return exp;
   }
};

// include operator headers
#include <qore/intern/QoreDeleteOperatorNode.h>
#include <qore/intern/QoreRemoveOperatorNode.h>
#include <qore/intern/QoreSpliceOperatorNode.h>
#include <qore/intern/QoreExtractOperatorNode.h>
#include <qore/intern/QoreCastOperatorNode.h>
#include <qore/intern/QoreUnaryMinusOperatorNode.h>

#endif
