/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreParseHashNode.h

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

#ifndef _QORE_QOREPARSEHASHNODE_H

#define _QORE_QOREPARSEHASHNODE_H

#include <qore/Qore.h>

#include <vector>
#include <map>
#include <string>

class QoreParseHashNode : public ParseNode {
protected:
   typedef std::map<std::string, bool> kmap_t;
   typedef std::vector<AbstractQoreNode*> nvec_t;
   nvec_t keys, values;
   // to detect duplicate values, only stored during parsing
   kmap_t kmap;
   // flag for a hash expression in curly brackets for the hash version of the map operator
   bool curly;

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return hashTypeInfo;
   }

   DLLLOCAL static void doDuplicateWarning(QoreProgramLocation& loc, const char* key);

   DLLLOCAL void checkDup(QoreProgramLocation& loc, const char* key) {
      std::string kstr(key);
      kmap_t::iterator i = kmap.lower_bound(kstr);
      if (i == kmap.end() || i->first != kstr)
	 kmap.insert(i, kmap_t::value_type(kstr, false));
      else if (!i->second) {
	 doDuplicateWarning(loc, key);
	 i->second = true;
      }
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      typeInfo = hashTypeInfo;
      assert(keys.size() == values.size());
      QoreProgramLocation loc = get_parse_location();
      bool needs_eval = false;

      // turn off "return value ignored" flag before performing parse init
      pflag &= ~PF_RETURN_VALUE_IGNORED;

      for (size_t i = 0; i < keys.size(); ++i) {
	 const QoreTypeInfo* argTypeInfo = 0;
	 AbstractQoreNode* p = keys[i];
	 keys[i] = keys[i]->parseInit(oflag, pflag, lvids, argTypeInfo);

	 if (p != keys[i] && (!keys[i] || keys[i]->is_value())) {
	    QoreStringValueHelper key(keys[i]);
	    checkDup(loc, key->getBuffer());
	 }
	 else if (!needs_eval && keys[i] && keys[i]->needs_eval())
	    needs_eval = true;
   
	 if (argTypeInfo->nonStringValue()) {
            QoreStringMaker str("key number %ld (starting from 0) in the hash is ", i);
            argTypeInfo->doNonStringWarning(loc, str.getBuffer());
         }
         
	 argTypeInfo = 0;
	 values[i] = values[i]->parseInit(oflag, pflag, lvids, argTypeInfo);
	 if (!needs_eval && values[i] && values[i]->needs_eval())
	    needs_eval = true;
      }

      kmap.clear();

      if (needs_eval)
	 return this;

      // evaluate immediately
      ValueEvalRefHolder rv(this, 0);
      deref();
      return rv.getReferencedValue();
   }

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const {
      assert(keys.size() == values.size());
      ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);
      for (size_t i = 0; i < keys.size(); ++i) {
	 QoreNodeEvalOptionalRefHolder k(keys[i], xsink);
	 if (*xsink)
	    return QoreValue();

	 QoreNodeEvalOptionalRefHolder v(values[i], xsink);
	 if (*xsink)
	    return QoreValue();

	 QoreStringValueHelper key(*k);
	 h->setKeyValue(key->getBuffer(), v.getReferencedValue(), xsink);
	 if (*xsink)
	    return QoreValue();
      }
      return h.release();
   }

public:
   DLLLOCAL QoreParseHashNode() : ParseNode(NT_PARSE_HASH, true), curly(false) {
   }

   DLLLOCAL ~QoreParseHashNode() {
      assert(keys.size() == values.size());
      for (size_t i = 0; i < keys.size(); ++i) {
	 discard(keys[i], 0);
	 discard(values[i], 0);
      }
      keys.clear();
      values.clear();
   }

   DLLLOCAL void add(AbstractQoreNode* n, AbstractQoreNode* v) {
      keys.push_back(n);
      values.push_back(v);

      if (!n || n->is_value()) {
	 QoreStringValueHelper key(n);
	 QoreProgramLocation loc = get_parse_location();
	 checkDup(loc, key->getBuffer());
      }
   }

   DLLLOCAL size_t size() {
      return keys.size();
   }

   // used when converting to the hash map operator
   DLLLOCAL AbstractQoreNode* takeFirstKeyNode() {
      assert(keys.size() == 1);
      AbstractQoreNode* rv = keys[0];
      keys[0] = 0;
      return rv;
   }

   // used when converting to the hash map operator
   DLLLOCAL AbstractQoreNode* takeFirstValueNode() {
      assert(values.size() == 1);
      AbstractQoreNode* rv = values[0];
      values[0] = 0;
      return rv;
   }

   DLLLOCAL void setCurly() {
      assert(!curly);
      curly = true;
   }
   
   DLLLOCAL bool isCurly() const {
      return curly;
   }
   
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   DLLLOCAL virtual const char* getTypeName() const;
};

#endif
