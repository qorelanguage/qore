/*
 ClassRefNode.cc
 
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

ClassRefNode::ClassRefNode(char *str) : ParseNoEvalNode(NT_CLASSREF), cscope(new NamedScope(str))
{
}

ClassRefNode::~ClassRefNode()
{
   delete cscope;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ClassRefNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   if (cscope)
      str.sprintf("reference to Qore class '%s' (unresolved, 0x%08p)", cscope->ostr, this);
   else
      str.sprintf("reference to Qore class id: %d (resolved, 0x%08p)", cid, this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ClassRefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   return rv;
}

// returns the data type
qore_type_t ClassRefNode::getType() const
{
   return NT_CLASSREF;
}

// returns the type name as a c string
const char *ClassRefNode::getTypeName() const
{
   return "reference to Qore class";
}

void ClassRefNode::resolve()
{
   if (cscope)
   {
      class QoreClass *qc = getRootNS()->parseFindScopedClass(cscope);
      if (qc)
	 cid = qc->getID();
      delete cscope;
      cscope = 0;
   }
}

int ClassRefNode::getID() const
{
   return cid;
}
