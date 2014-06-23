/*
  StaticClassVarRefNode.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
 
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

#include <qore/Qore.h>
#include <qore/intern/QoreClassIntern.h>

StaticClassVarRefNode::StaticClassVarRefNode(const char *c_str, const QoreClass &n_qc, QoreVarInfo &n_vi) : ParseNode(NT_CLASS_VARREF), qc(n_qc), vi(n_vi), str(c_str) {
}

StaticClassVarRefNode::~StaticClassVarRefNode() {
}

int StaticClassVarRefNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("reference to static class variable %s::%s", qc.getName(), str.c_str());
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *StaticClassVarRefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *StaticClassVarRefNode::getTypeName() const {
   return "static class variable reference";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *StaticClassVarRefNode::evalImpl(ExceptionSink *xsink) const {
   return vi.getReferencedValue();
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *StaticClassVarRefNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return vi.getReferencedValue();
}

int64 StaticClassVarRefNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return vi.getAsBigInt();
}

int StaticClassVarRefNode::integerEvalImpl(ExceptionSink *xsink) const {
   return (int)vi.getAsBigInt();
}

bool StaticClassVarRefNode::boolEvalImpl(ExceptionSink *xsink) const {
   return vi.getAsBool();
}

double StaticClassVarRefNode::floatEvalImpl(ExceptionSink *xsink) const {
   return vi.getAsFloat();
}

AbstractQoreNode *StaticClassVarRefNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   printd(5, "StaticClassVarRefNode::parseInit() '%s::%s'\n", qc.getName(), str.c_str());
   typeInfo = vi.getTypeInfo();
   return this;
}

void StaticClassVarRefNode::getLValue(LValueHelper& lvh) const {
   vi.getLValue(lvh);
}

void StaticClassVarRefNode::remove(LValueRemoveHelper& lvrh) {
   QoreAutoRWWriteLocker sl(vi.rwl);
   lvrh.doRemove((QoreLValueGeneric&)vi.val, vi.getTypeInfo());
}

const QoreTypeInfo *StaticClassVarRefNode::getTypeInfo() const {
   return vi.getTypeInfo();
}
