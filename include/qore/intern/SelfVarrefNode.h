/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 SelfVarrefNode.h
 
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

#ifndef _QORE_SELFVARREFNODE_H

#define _QORE_SELFVARREFNODE_H

class SelfVarrefNode : public ParseNode  {
protected:
   const QoreTypeInfo *returnTypeInfo;

   DLLLOCAL virtual AbstractQoreNode *evalImpl(class ExceptionSink *xsink) const;
      
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;      

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return returnTypeInfo;
   }

public:
   char *str;

   DLLLOCAL SelfVarrefNode(char *c_str) : ParseNode(NT_SELF_VARREF), returnTypeInfo(0), str(c_str) {
   }

   DLLLOCAL virtual ~SelfVarrefNode() {
      if (str)
         free(str);
   }

   // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   // the ExceptionSink is only needed for QoreObject where a method may be executed
   // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   // returns -1 for exception raised, 0 = OK
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;

   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;

   // returns the string, caller owns the memory
   DLLLOCAL char *takeString();
};

#endif
