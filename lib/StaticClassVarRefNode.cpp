/*
 StaticClassVarRefNode.cpp
 
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
   ReferenceHolder<AbstractQoreNode> rv(StaticClassVarRefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int StaticClassVarRefNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(StaticClassVarRefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool StaticClassVarRefNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(StaticClassVarRefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double StaticClassVarRefNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(StaticClassVarRefNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

AbstractQoreNode *StaticClassVarRefNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   printd(5, "StaticClassVarRefNode::parseInit() '%s::%s'\n", qc.getName(), str.c_str());
   return this;
}

AbstractQoreNode **StaticClassVarRefNode::getValuePtr(AutoVLock &vl, const QoreTypeInfo *&typeInfo) const {
   typeInfo = vi.getTypeInfo();
   vl.set(&vi.l);
   return &vi.val;
}
