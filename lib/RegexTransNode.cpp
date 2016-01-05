/*
  RegexTransNode.cpp
 
  regex-like transliteration class definition
 
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

#include <qore/Qore.h>
#include <qore/intern/RegexTransNode.h>

#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

// constructor used when parsing
RegexTransNode::RegexTransNode() : ParseNoEvalNode(NT_REGEX_TRANS) {
   //printd(5, "RegexTransNode::RegexTransNode() this=%p\n", this);
   source = new QoreString;
   target = new QoreString;
   sr = tr = false;
}

RegexTransNode::~RegexTransNode() {
   //printd(5, "RegexTransNode::~RegexTransNode() this=%p\n", this);
   if (source)
      delete source;
   if (target)
      delete target;
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
// returns -1 for exception raised, 0 = OK
int RegexTransNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.sprintf("transliteration expression (%p)", this);
   return 0;
}

// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
QoreString *RegexTransNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

// returns the data type
qore_type_t RegexTransNode::getType() const {
   return NT_REGEX_TRANS;
}

// returns the type name as a c string
const char *RegexTransNode::getTypeName() const {
   return "transliteration expression";
}

void RegexTransNode::setTargetRange() { 
   tr = true; 
}

void RegexTransNode::setSourceRange() { 
   sr = true; 
}

void RegexTransNode::finishSource() {
   if (sr)
      parse_error("no end character for character range at end of source pattern in transliteration");
}

void RegexTransNode::finishTarget() {
   if (tr)
      parse_error("no end character for character range at end of target pattern in transliteration");
}

void RegexTransNode::doRange(class QoreString *str, char end) {
   if (!str->strlen()) {
      parse_error("no start character for character range in transliteration");
      return;
   }
   char start = str->getBuffer()[str->strlen() - 1];
   str->terminate(str->strlen() - 1);
   if (start > end) {
      parse_error("invalid range '%c' - '%c' in transliteration operator", start, end);
      return;
   }
   do
      str->concat(start++);
   while (start <= end);
}

void RegexTransNode::concatSource(char c) {
   if (sr) {
      doRange(source, c);
      sr = false;
   }
   else
      source->concat(c);
}

void RegexTransNode::concatTarget(char c) {
   if (tr) {
      doRange(target, c);
      tr = false;
   }
   else
      target->concat(c);
}

QoreStringNode *RegexTransNode::exec(const QoreString *str, ExceptionSink *xsink) const {
   //printd(5, "source='%s' target='%s' ('%s')\n", source->getBuffer(), target->getBuffer(), str->getBuffer());
   TempEncodingHelper tstr(str, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   QoreStringNode *ns = new QoreStringNode;
   for (qore_size_t i = 0; i < tstr->strlen(); i++) {
      char c = tstr->getBuffer()[i];
      const char *p = strchr(source->getBuffer(), c);
      if (p) {
	 qore_size_t pos = p - source->getBuffer();
	 if (target->strlen() <= pos)
	    pos = target->strlen() - 1;
	 ns->concat(target->getBuffer()[pos]);
      }
      else
	 ns->concat(c);
   }

   return ns;
}
