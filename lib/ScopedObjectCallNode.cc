/*
 ScopedObjectCall.cc
 
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

ScopedObjectCallNode::ScopedObjectCallNode(NamedScope *n, QoreListNode *a) : ParseNoEvalNode(NT_SCOPE_REF)
{ 
   name = n; 
   args = a; 
}

ScopedObjectCallNode::~ScopedObjectCallNode()
{
   if (name)
      delete name; 
   if (args)
      args->deref(0);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ScopedObjectCallNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.sprintf("new operator expression (class '%s')", oc ? oc->getName() : name ? name->ostr : "<null>", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ScopedObjectCallNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
qore_type_t ScopedObjectCallNode::getType() const
{
   return NT_SCOPE_REF;
}

// returns the type name as a c string
const char *ScopedObjectCallNode::getTypeName() const
{
   return "new object call";
}
