/*
 QoreXPath.h
 
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

#ifndef _QORE_QOREXPATH_H

#define _QORE_QOREXPATH_H

#include <libxml/xpath.h>

#include <qore/intern/QC_XmlDoc.h>
#include <qore/intern/QC_XmlNode.h>

class QoreXPathObject {
private:
   xmlXPathObjectPtr ptr;

public:
   DLLLOCAL QoreXPathObject(xmlXPathObjectPtr n_ptr, ExceptionSink *xsink) : ptr(n_ptr) {
   }
   DLLLOCAL ~QoreXPathObject() {
      if (ptr)
	 xmlXPathFreeObject(ptr);
   }
   DLLLOCAL operator bool() const {
      return ptr;
   }
   DLLLOCAL QoreListNode *getNodeList(QoreXmlDocData *doc) {
      if (!ptr->nodesetval || !ptr->nodesetval->nodeNr)
	 return 0;

      QoreListNode *l = new QoreListNode();
      for (int i = 0, e = ptr->nodesetval->nodeNr; i < e; ++i) {
	 l->push(new QoreObject(QC_XMLNODE, getProgram(), new QoreXmlNodeData(ptr->nodesetval->nodeTab[i], doc)));
      }
      return l;
   }
};

class QoreXPath {
private:
   xmlXPathContextPtr ptr;
   QoreXmlDocData *doc;

public:
   DLLLOCAL QoreXPath(QoreXmlDocData *n_doc, ExceptionSink *xsink) : doc(n_doc) {
      ptr = xmlXPathNewContext(doc->getDocPtr());
      if (!ptr)
	 xsink->raiseException("XPATH-CONSTRUCTOR-ERROR", "failed to create XPath context from XmlDoc object");      
   }
   DLLLOCAL ~QoreXPath() {
      if (ptr)
	 xmlXPathFreeContext(ptr);
   }
   DLLLOCAL QoreListNode *eval(const char *expr, ExceptionSink *xsink) {
      QoreXPathObject xpo(xmlXPathEvalExpression((xmlChar *)expr, ptr), xsink);
      if (!xpo) {
         xsink->raiseException("XPATH-ERROR", "unable to evaluate xpath expression '%s'", expr);
	 return 0;
      }
      return xpo.getNodeList(doc);
   }
   DLLLOCAL operator bool() const {
      return ptr;
   }
};

#endif
