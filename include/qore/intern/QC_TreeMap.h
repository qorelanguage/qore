/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TreeMap.h

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

#ifndef _QORE_QC_TREEMAP_H

#define _QORE_QC_TREEMAP_H

#include <qore/Qore.h>
#include <map>
#include <string>

DLLEXPORT extern qore_classid_t CID_TREEMAP;
DLLLOCAL extern QoreClass* QC_TREEMAP;

DLLLOCAL QoreClass *initTreeMapClass(QoreNamespace& ns);

class TreeMapData : public AbstractPrivateData {

public:
   DLLLOCAL TreeMapData() {
   }

   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         for (Map::iterator i = data.begin(), e = data.end(); i != e; ++i) {
            i->second->deref(xsink);
         }
         delete this;
      }
   }

   DLLLOCAL void put(const QoreStringNode *key, const AbstractQoreNode *value, ExceptionSink *xsink) {
      TempEncodingHelper keyStr(key, QCS_DEFAULT, xsink);
      if (keyStr) {
         data[keyStr->getBuffer()] = value->refSelf();
      }
   }

   DLLLOCAL AbstractQoreNode *get(const QoreStringNode *key, ExceptionSink *xsink) const {
      TempEncodingHelper keyStr(key, QCS_DEFAULT, xsink);

//TODO implement actual prefix search
      Map::const_iterator it = data.find(keyStr->getBuffer());
      if (it == data.end()) {
         return nothing();
      }
      return it->second->refSelf();
   }

private:
   typedef std::map<std::string, AbstractQoreNode *> Map;
   Map data;
};

#endif
