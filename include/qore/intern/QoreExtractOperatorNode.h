/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 QoreExtractOperatorNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2013 David Nichols
 
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

#ifndef _QORE_QOREEXTRACTOPERATORNODE_H

#define _QORE_QOREEXTRACTOPERATORNODE_H

class QoreExtractOperatorNode : public LValueOperatorNode {
protected:
   AbstractQoreNode *lvalue_exp, *offset_exp, *length_exp, *new_exp;
   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL static QoreString extract_str;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL ~QoreExtractOperatorNode() {
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
   DLLLOCAL QoreExtractOperatorNode(AbstractQoreNode *n_lvalue_exp, AbstractQoreNode *n_offset_exp,
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
      return extract_str.getBuffer();
   }

   DLLLOCAL AbstractQoreNode *extract(ExceptionSink *xsink) const;
};

#endif
