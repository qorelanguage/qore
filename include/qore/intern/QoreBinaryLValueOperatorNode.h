/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreBinaryLValueOperatorNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2011 David Nichols
 
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

#ifndef _QORE_QOREBINARYLVALUEOPERATORNODE_H

#define _QORE_QOREBINARYLVALUEOPERATORNODE_H

class QoreBinaryLValueOperatorNode : public QoreBinaryOperatorNode<LValueOperatorNode> {
protected:
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return ti;
   }

public:
   const QoreTypeInfo *ti;         // typeinfo of lhs

   DLLLOCAL QoreBinaryLValueOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreBinaryOperatorNode<LValueOperatorNode>(n_left, n_right), ti(0) {
   }

   DLLLOCAL void parseInitIntLValue(const char *name, LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      // turn off "reference ok" and "return value ignored" flags
      pflag &= ~(PF_REFERENCE_OK | PF_RETURN_VALUE_IGNORED);

      typeInfo = bigIntTypeInfo;

      const QoreTypeInfo *mti = 0;
      left = left->parseInit(oflag, pflag | PF_FOR_ASSIGNMENT, lvids, mti);
      
      // make sure left side can take an integer value
      check_lvalue_int(mti, name);

      mti = 0;
      // FIXME: check for invalid operation - type cannot be converted to integer
      right = right->parseInit(oflag, pflag, lvids, mti);
   }
};

#endif
