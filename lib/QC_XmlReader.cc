/*
  QC_XmlReader.cc

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
#include <qore/intern/QC_XmlReader.h>
#include <qore/intern/QC_XmlNode.h>

qore_classid_t CID_XMLREADER;

static void XMLREADER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *n = get_param(params, 0);
   qore_type_t t = n ? n->getType() : NT_NOTHING;
   SimpleRefHolder<QoreXmlReaderData> xr;

   if (t == NT_OBJECT) {
      const QoreObject *obj = reinterpret_cast<const QoreObject *>(n);
      ReferenceHolder<QoreXmlDocData> doc(reinterpret_cast<QoreXmlDocData *>(obj->getReferencedPrivateData(CID_XMLDOC, xsink)), xsink);
      if (!doc) {
	 if (!*xsink)
	    xsink->raiseException("XMLREADER-CONSTRUCTOR-ERROR", "object passed to XmlReader::constructor() is not derived from XmlDoc (got class %s instead)", obj->getClassName());
	 return;
      }
      xr = new QoreXmlReaderData(*doc, xsink);
   }
   else if (t == NT_STRING)
      xr = new QoreXmlReaderData(reinterpret_cast<const QoreStringNode *>(n), xsink);
   else {
      xsink->raiseException("XMLREADER-CONSTRUCTOR-ERROR", "missing required string or XmlDoc argument to XmlReader::constructor()");
      return;
   }

   if (!xr) {
      xsink->raiseException("XMLREADER-CONSTRUCTOR-ERROR", "error parsing XML string");
      return;
   }

   self->setPrivate(CID_XMLREADER, xr.release());
}

static void XMLREADER_copy(QoreObject *self, QoreObject *old, QoreXmlReaderData *xr, ExceptionSink *xsink) {
   ReferenceHolder<QoreXmlReaderData> doc(xr->copy(xsink), xsink);
   if (!*xsink)
      self->setPrivate(CID_XMLREADER, doc.release());
}

static AbstractQoreNode *XMLREADER_read(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   xr->read(xsink);
   return 0;
}

static AbstractQoreNode *XMLREADER_readSkipWhitespace(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   xr->readSkipWhitespace(xsink);
   return 0;
}

static AbstractQoreNode *XMLREADER_elementType(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xr->nodeType());
}

static AbstractQoreNode *XMLREADER_elementTypeName(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = get_xml_element_type_name(xr->nodeType());
   return n ? new QoreStringNode(n) : 0;
}

static AbstractQoreNode *XMLREADER_depth(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xr->depth());
}

static AbstractQoreNode *XMLREADER_name(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->constName();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_value(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->constValue();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_hasAttributes(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->hasAttributes());
}

static AbstractQoreNode *XMLREADER_toQore(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->parseXMLData(QCS_UTF8, false, xsink);
}

static AbstractQoreNode *XMLREADER_toQoreData(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->parseXMLData(QCS_UTF8, true, xsink);
}

QoreClass *initXmlReaderClass() {
   QORE_TRACE("initXmlReaderClass()");

   QoreClass *QC_XMLREADER = new QoreClass("XmlReader");
   CID_XMLREADER = QC_XMLREADER->getID();
   QC_XMLREADER->setConstructor(XMLREADER_constructor);
   QC_XMLREADER->setCopy((q_copy_t)XMLREADER_copy);

   QC_XMLREADER->addMethod("read",                   (q_method_t)XMLREADER_read);
   QC_XMLREADER->addMethod("readSkipWhitespace",     (q_method_t)XMLREADER_readSkipWhitespace);
   QC_XMLREADER->addMethod("elementType",            (q_method_t)XMLREADER_elementType);
   QC_XMLREADER->addMethod("elementTypeName",        (q_method_t)XMLREADER_elementTypeName);
   QC_XMLREADER->addMethod("depth",                  (q_method_t)XMLREADER_depth);
   QC_XMLREADER->addMethod("name",                   (q_method_t)XMLREADER_name);
   QC_XMLREADER->addMethod("value",                  (q_method_t)XMLREADER_value);
   QC_XMLREADER->addMethod("hasAttributes",          (q_method_t)XMLREADER_hasAttributes);
   QC_XMLREADER->addMethod("toQore",                 (q_method_t)XMLREADER_toQore);
   QC_XMLREADER->addMethod("toQoreData",             (q_method_t)XMLREADER_toQoreData);

   return QC_XMLREADER;   
}

