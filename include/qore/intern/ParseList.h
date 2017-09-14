/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ParseList.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_PARSELIST_H

#define _QORE_PARSELIST_H

typedef std::vector<QoreValue> val_vec_t;

class ParseList : public ParseNode {
   friend class ParseListIterator;
private:
   bool finalized = false;

protected:
   // vector of values
   val_vec_t vvec;

public:
   DLLLOCAL ParseList(const QoreProgramLocation& loc, bool needs_eval = true) : ParseNode(loc, NT_PARSE_LIST, needs_eval) {
   }

   DLLLOCAL ~QorParseList() {
      xxx;
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      xxx;
   }

   DLLLOCAL QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
      xxx;
   }

   DLLLOCAL QoreValueList* evalValueList(ExceptionSink* xsink) const {
      xxx;
   }

   DLLLOCAL size_t size() const {
      return vvec.size();
   }

   DLLLOCAL void push(QoreValue v) {
      vvec.push_back(v);
   }

   DLLLOCAL bool isFinalized() const {
      return finalized;
   }

   DLLLOCAL void setFinalized() {
      assert(!finalized);
      finalized = true;
   }
};

class ParseListIterator {
protected:
   val_vec_t& vvec;
   val_vec_t::iterator i;

public:
   DLLLOCAL ParseListIterator(ParseList& l) : vvec(l.vvec) {
      i = vvec.end();
   }

   DLLLOCAL bool next() {
      if (i == vvec.end())
         i = vvec.begin();
      else
         ++i;
      return i != vvec.end();
   }

   DLLLOCAL QoreValue operator->() {
      return *i;
   }

   DLLLOCAL QoreValue operator*() {
      return *i;
   }

   DLLLOCAL QoreValue getValue() {
      return *i;
   }
}

#endif
