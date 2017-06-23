/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TreeMap.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologoes, s.r.o.

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

#ifndef _QORE_QC_TREEMAP_H
#define _QORE_QC_TREEMAP_H

#include <qore/Qore.h>
#include <map>
#include <string>

DLLEXPORT extern qore_classid_t CID_TREEMAP;
DLLLOCAL extern QoreClass* QC_TREEMAP;

DLLLOCAL QoreClass *initTreeMapClass(QoreNamespace& ns);

static inline bool isPathEnd(char c) {
   return c == '/' || c == '?';
}

static inline size_t getFirstPathSegmentLength(const std::string &path) {
   size_t prefixLen = path.find_first_of("/?");
   return prefixLen == std::string::npos ? path.length() : prefixLen;
}

static inline bool isPrefix(const std::string &prefix, const std::string &str) {
   return str.length() >= prefix.length() && !str.compare(0, prefix.length(), prefix);
}

static inline bool isPathPrefix(const std::string &prefix, const std::string &path) {
   return isPrefix(prefix, path) && (path.length() == prefix.length() || isPathEnd(path[prefix.length()]));
}

class TreeMapData : public AbstractPrivateData {
public:
   DLLLOCAL TreeMapData() {
   }

   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         for (Map::iterator i = data.begin(), e = data.end(); i != e; ++i) {
            i->second.discard(xsink);
         }
         delete this;
      }
   }

   DLLLOCAL void put(const QoreStringNode* key, const QoreValue value, ExceptionSink* xsink) {
      TempEncodingHelper keyStr(key, QCS_DEFAULT, xsink);
      if (keyStr) {
         QoreAutoRWWriteLocker al(rwl);

         Map::mapped_type &refToMap = data[keyStr->getBuffer()];
         ::discard(refToMap.assignAndSanitize(value.refSelf()), xsink);
      }
   }

   // return value must be dereferenced by the caller
   DLLLOCAL QoreValue get(const QoreStringNode* key, const ReferenceNode* unmatched, ExceptionSink* xsink) const {
      TempEncodingHelper keyStr(key, QCS_DEFAULT, xsink);
      if (!keyStr) {
         return QoreValue();
      }

      QoreAutoRWReadLocker al(rwl);
      if (!data.empty()) {
         std::string path(keyStr->getBuffer());

         Map::const_iterator b = data.begin();
         Map::const_iterator it = data.upper_bound(path);

         size_t prefixLen = getFirstPathSegmentLength(path);
         while (it != b && !(--it)->first.compare(0, prefixLen, path, 0, prefixLen)) {
            if (isPathPrefix(it->first, path)) {
               if (unmatched) {
                  size_t l = it->first.length();
                  if (isPathEnd(path[l])) {
                     l++;
                  }
                  QoreTypeSafeReferenceHelper ref(unmatched, xsink);
                  if (!ref)
                     return QoreValue();
                  if (ref.assign(key->substr(l, xsink)))
                     return QoreValue();
               }
               return it->second.refSelf();
            }
         }
      }
      if (unmatched) {
         QoreTypeSafeReferenceHelper ref(unmatched, xsink);
         if (!ref)
            return QoreValue();
         if (ref.assign(&Nothing))
            return QoreValue();
      }
      return QoreValue();
   }

   DLLLOCAL QoreHashNode* getAll() const {
      QoreAutoRWReadLocker al(rwl);

      if (data.empty())
         return nullptr;

      ReferenceHolder<QoreHashNode> h(new QoreHashNode, nullptr);
      for (Map::const_iterator i = data.begin(), e = data.end(); i != e; ++i)
         h->setKeyValue(i->first.c_str(), i->second.getReferencedValue(), nullptr);
      return h.release();
   }

   // return value must be dereferenced
   DLLLOCAL QoreValue take(const QoreStringNode* key, ExceptionSink* xsink) {
      TempEncodingHelper keyStr(key, QCS_DEFAULT, xsink);
      if (!keyStr) {
         return QoreValue();
      }

      QoreAutoRWWriteLocker al(rwl);
      Map::iterator i = data.find(keyStr->getBuffer());
      if (i == data.end())
         return QoreValue();

      QoreValue rv = i->second;
      data.erase(i);
      return rv;
   }

private:
   typedef std::map<std::string, QoreValue> Map;
   Map data;
   mutable QoreRWLock rwl;
};

#endif
