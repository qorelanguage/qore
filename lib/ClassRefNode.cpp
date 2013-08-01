/*
 ClassRefNode.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2013 David Nichols
 
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
#include <qore/intern/QoreNamespaceIntern.h>

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int ClassRefNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   if (cscope)
      str.sprintf("reference to Qore class '%s' (unresolved, %p)", cscope->ostr, this);
   else
      str.sprintf("reference to Qore class '%s' (resolved, %p)", qc->getName(), this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *ClassRefNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   return rv;
}

AbstractQoreNode *ClassRefNode::parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   // FIXME: implement a type for this
   typeInfo = 0;
   if (cscope) {
      qc = qore_root_ns_private::parseFindScopedClass(loc, *cscope);
      delete cscope;
      cscope = 0;
   }
   return this;
}
