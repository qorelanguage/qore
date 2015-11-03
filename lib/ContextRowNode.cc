/*
 ContextRowNode.cc
 
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

ContextRowNode::ContextRowNode() : ParseNode(NT_CONTEXT_ROW)
{
}

ContextRowNode::~ContextRowNode()
{
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ContextRowNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const
{
   qstr.sprintf("context row reference '%%' (0x%08p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ContextRowNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *ContextRowNode::getTypeName() const
{
   return "context row reference";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *ContextRowNode::evalImpl(ExceptionSink *xsink) const
{
   return evalContextRow(xsink);
}

// evalImpl(): return value requires a deref(xsink) if not 0
AbstractQoreNode *ContextRowNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const
{
   needs_deref = true;
   return ContextRowNode::evalImpl(xsink);
}

int64 ContextRowNode::bigIntEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int ContextRowNode::integerEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool ContextRowNode::boolEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double ContextRowNode::floatEvalImpl(ExceptionSink *xsink) const
{
   ReferenceHolder<AbstractQoreNode> rv(ContextRowNode::evalImpl(xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}
