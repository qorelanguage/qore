/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreDeleteOperatorNode.h
 
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

#ifndef _QORE_QOREDELETEOPERATORNODE_H

#define _QORE_QOREDELETEOPERATORNODE_H

class QoreDeleteOperatorNode : public QoreSingleExpressionOperatorNode {
protected:
   DLLLOCAL static QoreString delete_str;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

public:
   DLLLOCAL QoreDeleteOperatorNode(AbstractQoreNode *n_exp) : QoreSingleExpressionOperatorNode(n_exp) {
   }
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return delete_str.getBuffer();
   }

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual bool hasEffect() const {
      return true;
   }
};

#endif
