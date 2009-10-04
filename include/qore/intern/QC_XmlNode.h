/*
 QC_XmlNode.h
 
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

#ifndef _QORE_QC_XMLNODE_H

#define _QORE_QC_XMLNODE_H

#include <qore/intern/QC_XmlDoc.h>

DLLEXPORT extern qore_classid_t CID_XMLNODE;
DLLEXPORT extern QoreClass *QC_XMLNODE;
DLLLOCAL QoreNamespace *initXmlNs();

// returns the string corresponding to the node type
DLLLOCAL const char *get_xml_element_type_name(int t);

class QoreXmlNodeData : public AbstractPrivateData {
private:
   xmlNodePtr ptr;
   QoreXmlDocData *doc;
   bool del;

protected:
   DLLLOCAL QoreStringNode *doString(xmlChar *str) {
      if (!str)
	 return 0;
      QoreStringNode *rv = new QoreStringNode((const char *)str);
      xmlFree(str);
      return rv;
   }

   DLLLOCAL QoreXmlNodeData *doNode(xmlNodePtr p) {
      if (!p)
	 return 0;
      return new QoreXmlNodeData(p, doc);
   }

public:
   DLLLOCAL QoreXmlNodeData(xmlNodePtr n_ptr, QoreXmlDocData *n_doc = 0, bool d = false) : ptr(n_ptr), doc(n_doc), del(d) {
      if (doc)
	 doc->ref();
   }
   DLLLOCAL QoreXmlNodeData(const QoreXmlNodeData &orig) {
      ptr = xmlCopyNode(orig.ptr, 1);
      doc = 0;
      del = ptr ? true : false;
   }
   DLLLOCAL ~QoreXmlNodeData() {
      if (ptr) {
	 if (del)
	    xmlFreeNode(ptr);
	 if (doc)
	    doc->deref();
      }
   }
   DLLLOCAL operator bool() const {
      return ptr;
   }
   DLLLOCAL int64 childElementCount() {
      return xmlChildElementCount(ptr);
   }
   DLLLOCAL QoreXmlNodeData *firstElementChild() {
      return doNode(xmlFirstElementChild(ptr));
   }
   DLLLOCAL QoreXmlNodeData *getLastChild() {
      return doNode(xmlGetLastChild(ptr));
   }
   DLLLOCAL QoreStringNode *getPath(ExceptionSink *xsink) {
      xmlChar *np = xmlGetNodePath(ptr);
      if (!np) {
	 xsink->raiseException("XMLNODE-GET-PATH-ERROR", "an error occured retrieving the node's path");
	 return 0;
      }
      return doString(np);
   }
   DLLLOCAL QoreStringNode *getNsProp(const char *prop, const char *ns) {
      return doString(xmlGetNsProp(ptr, (xmlChar *)prop, (xmlChar *)ns));
   }
   DLLLOCAL QoreStringNode *getProp(const char *prop) {
      return doString(xmlGetProp(ptr, (xmlChar *)prop));
   }
   bool isBlank() {
      return xmlIsBlankNode(ptr);
   }
   DLLLOCAL QoreXmlNodeData *lastElementChild() {
      return doNode(xmlLastElementChild(ptr));
   }
   DLLLOCAL QoreXmlNodeData *nextElementSibling() {
      return doNode(xmlNextElementSibling(ptr));
   }
   DLLLOCAL QoreXmlNodeData *previousElementSibling() {
      return doNode(xmlPreviousElementSibling(ptr));
   }
/*
   DLLLOCAL QoreStringNode *getBase() {
      assert(doc);
      return doString(xmlNodeGetBase(doc->getDocPtr(), ptr));
   }
*/
   DLLLOCAL QoreStringNode *getContent() {
      return doString(xmlNodeGetContent(ptr));
   }
   DLLLOCAL QoreStringNode *getLang() {
      return doString(xmlNodeGetLang(ptr));
   }
   DLLLOCAL QoreStringNode *getName() {
      return ptr->name ? new QoreStringNode((const char *)ptr->name) : 0;
   }
   DLLLOCAL int getSpacePreserve() {
      return xmlNodeGetSpacePreserve(ptr);
   }
   DLLLOCAL bool isText() {
      return xmlNodeIsText(ptr);
   }
   DLLLOCAL int64 getLineNumber() {
      return xmlGetLineNo(ptr);
   }
   DLLLOCAL int64 getElementType() const {
      return ptr->type;
   }
};

#endif
