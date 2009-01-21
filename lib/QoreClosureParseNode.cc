/*
   QoreClosureParseNode.cc

   Qore Programming Language

   Copyright 2003 - 2009 David Nichols

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

QoreClosureParseNode::QoreClosureParseNode(UserFunction *n_uf, bool n_lambda) : ParseNode(NT_CLOSURE), uf(n_uf), lambda(n_lambda), in_method(false) {
}

QoreClosureNode *QoreClosureParseNode::evalClosure() const {
   return new QoreClosureNode(this);
}

QoreObjectClosureNode *QoreClosureParseNode::evalObjectClosure() const {
   return new QoreObjectClosureNode(getStackObject(), this);
}

AbstractQoreNode *QoreClosureParseNode::evalImpl(ExceptionSink *xsink) const {
   return in_method ? (AbstractQoreNode *)evalObjectClosure() : (AbstractQoreNode *)evalClosure();
}

AbstractQoreNode *QoreClosureParseNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return in_method ? (AbstractQoreNode *)evalObjectClosure() : (AbstractQoreNode *)evalClosure();
}

int64 QoreClosureParseNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

int QoreClosureParseNode::integerEvalImpl(ExceptionSink *xsink) const {
   return 0;
}

bool QoreClosureParseNode::boolEvalImpl(ExceptionSink *xsink) const {
   return false;
}

double QoreClosureParseNode::floatEvalImpl(ExceptionSink *xsink) const {
   return 0.0;
}

int QoreClosureParseNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("function closure (%slambda, 0x%08p)", lambda ? "" : "non-", this);
   return 0;
}

QoreString *QoreClosureParseNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

AbstractQoreNode *QoreClosureParseNode::parseInit(LocalVar *oflag, int pflag, int &lvids) {
   in_method = (bool)oflag;
   uf->statements->parseInitClosure(uf->params, in_method, &vlist);
   return this;
}

const char *QoreClosureParseNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreClosureParseNode::exec(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink) const {
   return uf->eval(args, self, xsink);
}
