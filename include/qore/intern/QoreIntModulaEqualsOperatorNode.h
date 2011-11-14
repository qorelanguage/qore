/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreIntModulaEqualsOperatorNode.h

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

#ifndef _QORE_QOREINTMODULAEQUALSOPERATORNODE_H
#define _QORE_QOREINTMODULAEQUALSOPERATORNODE_H

class QoreIntModulaEqualsOperatorNode : public QoreModulaEqualsOperatorNode {
protected:
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;

public:
   DLLLOCAL QoreIntModulaEqualsOperatorNode(AbstractQoreNode *n_left, AbstractQoreNode *n_right) : QoreModulaEqualsOperatorNode(n_left, n_right) {
   }
};

#endif

