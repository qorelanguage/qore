/*
  QC_XmlDoc.cc

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
#include <qore/intern/QC_XmlDoc.h>
#include <qore/intern/QoreXPath.h>
#include <qore/intern/QoreXmlReader.h>
#include <qore/intern/QC_XmlNode.h>

qore_classid_t CID_XMLDOC;

QoreXmlNodeData *QoreXmlDocData::getRootElement() {
   xmlNodePtr n = xmlDocGetRootElement(ptr);
   if (!n) return 0;
   return new QoreXmlNodeData(n, this);
}

static void XMLDOC_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *n = get_param(params, 0);
   qore_type_t t = n ? n->getType() : NT_NOTHING;
   SimpleRefHolder<QoreXmlDocData> xd;

   if (t == NT_HASH) {
      SimpleRefHolder<QoreStringNode> xml(makeXMLString(QCS_UTF8, *(reinterpret_cast<const QoreHashNode *>(n)), false, xsink));
      if (!xml)
	 return;
      xd = new QoreXmlDocData(*xml);
   }
   else if (t == NT_STRING)
      xd = new QoreXmlDocData(reinterpret_cast<const QoreStringNode *>(n));
   else {
      xsink->raiseException("XMLDOC-CONSTRUCTOR-ERROR", "missing required string or hash argument to XmlDoc::constructor()");
      return;
   }

   if (!xd->isValid()) {
      xsink->raiseException("XMLDOC-CONSTRUCTOR-ERROR", "error parsing XML string");
      return;
   }

   self->setPrivate(CID_XMLDOC, xd.release());
}

static void XMLDOC_copy(QoreObject *self, QoreObject *old, QoreXmlDocData *xd, ExceptionSink *xsink) {
   self->setPrivate(CID_XMLDOC, new QoreXmlDocData(*xd));
}

static AbstractQoreNode *XMLDOC_getVersion(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode(xd->getVersion());
}

static AbstractQoreNode *XMLDOC_toQore(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   QoreXmlReader reader(xd->getDocPtr(), xsink);
   if (*xsink)
      return 0;
   return reader.parseXMLData(QCS_UTF8, false, xsink);
}

static AbstractQoreNode *XMLDOC_toQoreData(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   QoreXmlReader reader(xd->getDocPtr(), xsink);
   if (*xsink)
      return 0;
   return reader.parseXMLData(QCS_UTF8, true, xsink);
}

static AbstractQoreNode *XMLDOC_toString(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   return xd->toString(xsink);
}

static AbstractQoreNode *XMLDOC_evalXPath(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *expr = test_string_param(params, 0);
   if (!expr) {
      xsink->raiseException("XMLDOC-EVAL-XPATH-ERROR", "missing the XPath expression as the first argument to XmlDoc::evalXPath()");
      return 0;
   }

   QoreXPath xp(xd, xsink);
   if (!xp)
      return 0;

   return xp.eval(expr->getBuffer(), xsink);
}

static AbstractQoreNode *XMLDOC_getRootElement(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
   QoreXmlNodeData *n = xd->getRootElement();
   if (!n) return 0;
   return new QoreObject(QC_XMLNODE, getProgram(), n);
}

QoreClass *initXmlDocClass() {
   QORE_TRACE("initXmlDocClass()");

   QoreClass *QC_XMLDOC = new QoreClass("XmlDoc");
   CID_XMLDOC = QC_XMLDOC->getID();
   QC_XMLDOC->setConstructor(XMLDOC_constructor);
   QC_XMLDOC->setCopy((q_copy_t)XMLDOC_copy);

   QC_XMLDOC->addMethod("getVersion",      (q_method_t)XMLDOC_getVersion);
   QC_XMLDOC->addMethod("toQore",          (q_method_t)XMLDOC_toQore);
   QC_XMLDOC->addMethod("toQoreData",      (q_method_t)XMLDOC_toQoreData);
   QC_XMLDOC->addMethod("toString",        (q_method_t)XMLDOC_toString);
   QC_XMLDOC->addMethod("evalXPath",       (q_method_t)XMLDOC_evalXPath);
   QC_XMLDOC->addMethod("getRootElement",  (q_method_t)XMLDOC_getRootElement);

   return QC_XMLDOC;   
}

