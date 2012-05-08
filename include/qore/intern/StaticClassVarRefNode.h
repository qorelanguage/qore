/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 StaticClassVarRefNode.h
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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

#ifndef _QORE_STATICCLASSVARREFNODE_H

#define _QORE_STATICCLASSVARREFNODE_H

#include <string>

class QoreVarInfo;
class LValueHelper;

class StaticClassVarRefNode : public ParseNode {
protected:
   DLLLOCAL virtual AbstractQoreNode* evalImpl(class ExceptionSink* xsink) const;
      
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool &needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const;

   DLLLOCAL AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo*& typeInfo);
   DLLLOCAL const QoreTypeInfo* getTypeInfo() const;

public:
   const QoreClass& qc;
   QoreVarInfo& vi;
   std::string str;

   DLLLOCAL StaticClassVarRefNode(const char* c_str, const QoreClass& n_qc, QoreVarInfo& n_vi);

   DLLLOCAL virtual ~StaticClassVarRefNode();

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   // if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const;

   DLLLOCAL void getLValue(LValueHelper& lvh) const;
};

#endif
