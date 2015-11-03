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

class QoreXmlNodeData : public AbstractPrivateData {
private:
   xmlNodePtr ptr;
   QoreXmlDocData *doc;
   bool del;

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
#ifdef HAVE_XMLCHILDELEMENTCOUNT
      return xmlChildElementCount(ptr);
#else
      int64 ret = 0;
      xmlNodePtr cur = 0;

      switch (ptr->type) {
	 case XML_ELEMENT_NODE:
	 case XML_ENTITY_NODE:
	 case XML_DOCUMENT_NODE:
	 case XML_HTML_DOCUMENT_NODE:
            cur = ptr->children;
            break;
	 default:
            return 0;
      }
      while (cur) {
	 if (cur->type == XML_ELEMENT_NODE)
            ++ret;
	 cur = cur->next;
      }
      return ret;
#endif
   }
   DLLLOCAL QoreXmlNodeData *firstElementChild() {
#ifdef HAVE_XMLFIRSTELEMENTCHILD
      return doNode(xmlFirstElementChild(ptr), doc);
#else
      xmlNodePtr cur = 0;

      switch (ptr->type) {
	 case XML_ELEMENT_NODE:
	 case XML_ENTITY_NODE:
	 case XML_DOCUMENT_NODE:
	 case XML_HTML_DOCUMENT_NODE:
            cur = ptr->children;
            break;
	 default:
            return 0;
      }
      while (cur) {
	 if (cur->type == XML_ELEMENT_NODE)
            return doNode(cur, doc);
	 cur = cur->next;
      }
      return 0;
#endif
   }
   DLLLOCAL QoreXmlNodeData *getLastChild() {
#ifdef HAVE_XMLGETLASTCHILD
      return doNode(xmlGetLastChild(ptr), doc);
#else
      return doNode(ptr->last, doc);
#endif
   }
   DLLLOCAL QoreXmlNodeData *lastElementChild() {
#ifdef HAVE_XMLLASTELEMENTCHILD
      return doNode(xmlLastElementChild(ptr), doc);
#else
      xmlNodePtr cur = 0;

      switch (ptr->type) {
	 case XML_ELEMENT_NODE:
	 case XML_ENTITY_NODE:
	 case XML_DOCUMENT_NODE:
	 case XML_HTML_DOCUMENT_NODE:
            cur = ptr->last;
            break;
	 default:
            return 0;
      }
      while (cur) {
	 if (cur->type == XML_ELEMENT_NODE)
            return doNode(cur, doc);
	 cur = cur->prev;
      }
      return 0;
#endif
   }
   DLLLOCAL QoreXmlNodeData *nextElementSibling() {
#ifdef HAVE_XMLNEXTELEMENTSIBLING
      return doNode(xmlNextElementSibling(ptr), doc);
#else
      xmlNodePtr cur = ptr;

      switch (cur->type) {
	 case XML_ELEMENT_NODE:
	 case XML_TEXT_NODE:
	 case XML_CDATA_SECTION_NODE:
	 case XML_ENTITY_REF_NODE:
	 case XML_ENTITY_NODE:
	 case XML_PI_NODE:
	 case XML_COMMENT_NODE:
	 case XML_DTD_NODE:
	 case XML_XINCLUDE_START:
	 case XML_XINCLUDE_END:
	    cur = cur->next;
            break;
	 default:
            return 0;
      }
      while (cur) {
	 if (cur->type == XML_ELEMENT_NODE)
            return doNode(cur, doc);
	 cur = cur->next;
      }
      return 0;
#endif
   }
   DLLLOCAL QoreXmlNodeData *previousElementSibling() {
#ifdef HAVE_XMLPREVIOUSELEMENTSIBLING
      return doNode(xmlPreviousElementSibling(ptr), doc);
#else
      xmlNodePtr cur = ptr;

      switch (cur->type) {
	 case XML_ELEMENT_NODE:
	 case XML_TEXT_NODE:
	 case XML_CDATA_SECTION_NODE:
	 case XML_ENTITY_REF_NODE:
	 case XML_ENTITY_NODE:
	 case XML_PI_NODE:
	 case XML_COMMENT_NODE:
	 case XML_DTD_NODE:
	 case XML_XINCLUDE_START:
	 case XML_XINCLUDE_END:
	    cur = cur->prev;
            break;
	 default:
            return 0;
      }
      while (cur) {
	 if (cur->type == XML_ELEMENT_NODE)
            return doNode(cur, doc);
	 cur = cur->prev;
      }
      return 0;
#endif
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
   DLLLOCAL QoreStringNode *getXML() {
      if (!doc)
	 return 0;
      xmlBufferPtr buf = xmlBufferCreate();
      assert(buf);
      int rc = xmlNodeDump(buf, doc->getDocPtr(), ptr, 0, 0);
      QoreStringNode *str = (rc != -1 && buf->size) ? new QoreStringNode((const char *)buf->content, buf->size, QCS_UTF8) : 0;
      xmlBufferFree(buf);
      return str;
   }
};

#endif
