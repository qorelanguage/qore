/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ClassRefNode.h
 
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

#ifndef _QORE_CLASSREFNODE_H

#define _QORE_CLASSREFNODE_H

class ClassRefNode : public ParseNoEvalNode {
protected:
   // populated on creation
   QoreProgramLocation loc;
   NamedScope* cscope;
   const QoreClass* qc;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      // FIXME: implement a type for this
      return 0;
   }
      
public:
   DLLLOCAL ClassRefNode(char* str) : ParseNoEvalNode(NT_CLASSREF), loc(ParseLocation), cscope(new NamedScope(str)), qc(0) {
   }

   DLLLOCAL ~ClassRefNode() {
      delete cscope;
   }

   // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
   // the ExceptionSink is only needed for QoreObject where a method may be executed
   // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
   // returns -1 for exception raised, 0 = OK
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;
   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   // returns the data type
   DLLLOCAL virtual qore_type_t getType() const {
      return NT_CLASSREF;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return "reference to Qore class";
   }

   DLLLOCAL const QoreClass* getClass() const {
      return qc;
   }
};

#endif
