/*
 BackquoteNode.cc
 
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

#include <errno.h>

BackquoteNode::BackquoteNode(char *c_str) : ParseNode(NT_BACKQUOTE), str(c_str) {
}

BackquoteNode::~BackquoteNode() {
   if (str)
      free(str);
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int BackquoteNode::getAsString(QoreString &qstr, int foff, ExceptionSink *xsink) const {
   qstr.sprintf("backquote '%s' (0x%08p)", str ? str : "<null>", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *BackquoteNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString();
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the type name as a c string
const char *BackquoteNode::getTypeName() const {
   return "backquote expression";
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *BackquoteNode::evalImpl(ExceptionSink *xsink) const {
   return backquoteEval(str, xsink);
}

// eval(): return value requires a deref(xsink)
AbstractQoreNode *BackquoteNode::evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
   needs_deref = true;
   return backquoteEval(str, xsink);
}

int64 BackquoteNode::bigIntEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(backquoteEval(str, xsink), xsink);
   return rv ? rv->getAsBigInt() : 0;
}

int BackquoteNode::integerEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(backquoteEval(str, xsink), xsink);
   return rv ? rv->getAsInt() : 0;
}

bool BackquoteNode::boolEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(backquoteEval(str, xsink), xsink);
   return rv ? rv->getAsBool() : 0;
}

double BackquoteNode::floatEvalImpl(ExceptionSink *xsink) const {
   ReferenceHolder<AbstractQoreNode> rv(backquoteEval(str, xsink), xsink);
   return rv ? rv->getAsFloat() : 0;
}

#ifndef READ_BLOCK
#define READ_BLOCK 1024
#endif

AbstractQoreNode *backquoteEval(const char *cmd, ExceptionSink *xsink) {
   // execute command in a new process and read stdout in parent
   FILE *p = popen(cmd, "r");
   if (!p) {
      // could not fork or create pipe
      xsink->raiseException("BACKQUOTE-ERROR", strerror(errno));
      return 0;
   }

   // allocate buffer for return value
   QoreStringNodeHolder s(new QoreStringNode());

   // read in result string
   while (true) {
      char buf[READ_BLOCK];
      int size = fread(buf, 1, READ_BLOCK, p);

      // break if no data is available or an error occurred
      if (!size || size == -1)
         break;

      s->concat(buf, size);

      // break if there is no more data
      if (size != READ_BLOCK)
         break;
   }

   // wait for child process to terminate and close pipe
   pclose(p);

   if (!s->strlen())
      return 0;

   return s.release();
}
