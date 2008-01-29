/*
 ComplexContextrefNode.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

ComplexContextrefNode::ComplexContextrefNode(char *str) : ParseNode(NT_COMPLEXCONTEXTREF)
{
   char *c = strchr(str, ':');
   *c = '\0';
   name = strdup(str);
   member = strdup(c + 1);
   free(str);
}

ComplexContextrefNode::~ComplexContextrefNode()
{
   if (name)
      free(name); 
   if (member)
      free(member); 
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ComplexContextrefNode::getAsString(QoreString &qstr, int foff, class ExceptionSink *xsink) const
{
   qstr.sprintf("complex context reference '%s:%s' (0x%08p)", name ? name : "<null>", member ? member : "<null>", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ComplexContextrefNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
const QoreType *ComplexContextrefNode::getType() const
{
   return NT_COMPLEXCONTEXTREF;
}

// returns the type name as a c string
const char *ComplexContextrefNode::getTypeName() const
{
   return "complex context reference";
}

// eval(): return value requires a deref(xsink)
QoreNode *ComplexContextrefNode::eval(class ExceptionSink *xsink) const
{
   int count = 0;

   Context *cs = get_context_stack();
   while (count != stack_offset)
   {
      count++;
      cs = cs->next;
   }
   return cs->evalValue(member, xsink);
}
