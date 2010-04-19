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
   int rc = xr->read(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_readSkipWhitespace(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->readSkipWhitespace(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_nodeType(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xr->nodeType());
}

static AbstractQoreNode *XMLREADER_nodeTypeName(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = get_xml_node_type_name(xr->nodeType());
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

static AbstractQoreNode *XMLREADER_hasValue(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->hasValue());
}

static AbstractQoreNode *XMLREADER_isDefault(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->isDefault());
}

static AbstractQoreNode *XMLREADER_isEmptyElement(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->isEmptyElement());
}

static AbstractQoreNode *XMLREADER_isNamespaceDecl(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->isNamespaceDecl());
}

static AbstractQoreNode *XMLREADER_isValid(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xr->isValid());
}

static AbstractQoreNode *XMLREADER_toQore(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->parseXMLData(QCS_UTF8, false, xsink);
}

static AbstractQoreNode *XMLREADER_toQoreData(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->parseXMLData(QCS_UTF8, true, xsink);
}

static AbstractQoreNode *XMLREADER_attributeCount(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xr->attributeCount());
}

static AbstractQoreNode *XMLREADER_baseUri(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->baseUri();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_encoding(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->encoding();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_localName(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->localName();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_namespaceUri(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->namespaceUri();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_prefix(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->prefix();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_xmlLang(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->xmlLang();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_xmlVersion(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const char *n = xr->xmlVersion();
   return n ? new QoreStringNode(n, QCS_UTF8) : 0;
}

static AbstractQoreNode *XMLREADER_getAttribute(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *attr = test_string_param(params, 0);
   if (!attr) {
      xsink->raiseException("XMLREADER-GETATTRIBUTE-ERROR", "missing attribute name as sole argument to XmlReader::getAttribute()");
      return 0;
   }

   return xr->getAttribute(attr->getBuffer());
}

static AbstractQoreNode *XMLREADER_getAttributeOffset(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->getAttributeOffset(get_bigint_param(params, 0));
}

static AbstractQoreNode *XMLREADER_getAttributeNs(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *lname = test_string_param(params, 0);
   if (!lname) {
      xsink->raiseException("XMLREADER-GETATTRIBUTENS-ERROR", "missing attribute local name as first argument to XmlReader::getAttributeNs()");
      return 0;
   }

   const QoreStringNode *ns = test_string_param(params, 1);
   if (!ns) {
      xsink->raiseException("XMLREADER-GETATTRIBUTENS-ERROR", "missing attribute namespace as second argument to XmlReader::getAttributeNs()");
      return 0;
   }

   return xr->getAttributeNs(lname->getBuffer(), ns->getBuffer());
}

static AbstractQoreNode *XMLREADER_lookupNamespace(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *prefix = test_string_param(params, 0);

   return xr->lookupNamespace(prefix ? prefix->getBuffer() : 0);
}

static AbstractQoreNode *XMLREADER_moveToAttribute(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *attr = test_string_param(params, 0);
   if (!attr) {
      xsink->raiseException("XMLREADER-MOVETOATTRIBUTE-ERROR", "missing attribute name as sole argument to XmlReader::moveToAttribute()");
      return 0;
   }

   int rc = xr->moveToAttribute(attr->getBuffer(), xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToAttributeOffset(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->moveToAttributeOffset(get_bigint_param(params, 0), xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToAttributeNs(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *lname = test_string_param(params, 0);
   if (!lname) {
      xsink->raiseException("XMLREADER-MOVETOATTRIBUTENS-ERROR", "missing attribute local name as first argument to XmlReader::moveToAttributeNs()");
      return 0;
   }

   const QoreStringNode *ns = test_string_param(params, 1);
   if (!ns) {
      xsink->raiseException("XMLREADER-MOVETOATTRIBUTENS-ERROR", "missing attribute namespace as second argument to XmlReader::moveToAttributeNs()");
      return 0;
   }

   int rc = xr->moveToAttributeNs(lname->getBuffer(), ns->getBuffer(), xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToElement(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->moveToElement(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToFirstAttribute(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->moveToFirstAttribute(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToNextAttribute(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->moveToNextAttribute(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_next(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->next(xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_getInnerXml(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->getInnerXml();
}

static AbstractQoreNode *XMLREADER_getOuterXml(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->getOuterXml();
}

static AbstractQoreNode *XMLREADER_relaxNGValidate(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
   const QoreStringNode *rng = test_string_param(params, 0);
   if (!rng) {
      xsink->raiseException("XMLREADER-RELAXNGVALIDATE-ERROR", "missing string giving the RelaxNG schema as sole argument to XmlReader::relaxNGValidate()");
      return 0;
   }

   xr->relaxNGValidate(rng->getBuffer(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderRelaxNGValidate() function, therefore XmlReader::relaxNGValidate() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHRELAXNG to check if this method is implemented before calling");
#endif
   return 0;
}

static AbstractQoreNode *XMLREADER_schemaValidate(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   const QoreStringNode *xsd = test_string_param(params, 0);
   if (!xsd) {
      xsink->raiseException("XMLREADER-SCHEMAVALIDATE-ERROR", "missing string giving the W3C XSD schema as sole argument to XmlReader::schemaValidate()");
      return 0;
   }

   xr->schemaValidate(xsd->getBuffer(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderSchemaValidate() function, therefore XmlReader::schemaValidate() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHSCHEMA to check if this method is implemented before calling");
#endif
   return 0;
}

QoreClass *initXmlReaderClass() {
   QORE_TRACE("initXmlReaderClass()");

   QoreClass *QC_XMLREADER = new QoreClass("XmlReader");
   CID_XMLREADER = QC_XMLREADER->getID();
   QC_XMLREADER->setConstructor(XMLREADER_constructor);
   QC_XMLREADER->setCopy((q_copy_t)XMLREADER_copy);

   QC_XMLREADER->addMethod("read",                      (q_method_t)XMLREADER_read);
   QC_XMLREADER->addMethod("readSkipWhitespace",        (q_method_t)XMLREADER_readSkipWhitespace);
   QC_XMLREADER->addMethod("nodeType",                  (q_method_t)XMLREADER_nodeType);
   QC_XMLREADER->addMethod("nodeTypeName",              (q_method_t)XMLREADER_nodeTypeName);
   QC_XMLREADER->addMethod("depth",                     (q_method_t)XMLREADER_depth);
   QC_XMLREADER->addMethod("name",                      (q_method_t)XMLREADER_name);
   QC_XMLREADER->addMethod("value",                     (q_method_t)XMLREADER_value);
   QC_XMLREADER->addMethod("hasAttributes",             (q_method_t)XMLREADER_hasAttributes);
   QC_XMLREADER->addMethod("hasValue",                  (q_method_t)XMLREADER_hasValue);
   QC_XMLREADER->addMethod("isDefault",                 (q_method_t)XMLREADER_isDefault);
   QC_XMLREADER->addMethod("isEmptyElement",            (q_method_t)XMLREADER_isEmptyElement);
   QC_XMLREADER->addMethod("isNamespaceDecl",           (q_method_t)XMLREADER_isNamespaceDecl);
   QC_XMLREADER->addMethod("isValid",                   (q_method_t)XMLREADER_isValid);
   QC_XMLREADER->addMethod("toQore",                    (q_method_t)XMLREADER_toQore);
   QC_XMLREADER->addMethod("toQoreData",                (q_method_t)XMLREADER_toQoreData);
   QC_XMLREADER->addMethod("attributeCount",            (q_method_t)XMLREADER_attributeCount);
   QC_XMLREADER->addMethod("baseUri",                   (q_method_t)XMLREADER_baseUri);
   QC_XMLREADER->addMethod("encoding",                  (q_method_t)XMLREADER_encoding);
   QC_XMLREADER->addMethod("localName",                 (q_method_t)XMLREADER_localName);
   QC_XMLREADER->addMethod("namespaceUri",              (q_method_t)XMLREADER_namespaceUri);
   QC_XMLREADER->addMethod("prefix",                    (q_method_t)XMLREADER_prefix);
   QC_XMLREADER->addMethod("xmlLang",                   (q_method_t)XMLREADER_xmlLang);
   QC_XMLREADER->addMethod("xmlVersion",                (q_method_t)XMLREADER_xmlVersion);
   QC_XMLREADER->addMethod("getAttribute",              (q_method_t)XMLREADER_getAttribute);
   QC_XMLREADER->addMethod("getAttributeOffset",        (q_method_t)XMLREADER_getAttributeOffset);
   QC_XMLREADER->addMethod("getAttributeNs",            (q_method_t)XMLREADER_getAttributeNs);
   QC_XMLREADER->addMethod("lookupNamespace",           (q_method_t)XMLREADER_lookupNamespace);
   QC_XMLREADER->addMethod("moveToAttribute",           (q_method_t)XMLREADER_moveToAttribute);
   QC_XMLREADER->addMethod("moveToAttributeOffset",     (q_method_t)XMLREADER_moveToAttributeOffset);
   QC_XMLREADER->addMethod("moveToAttributeNs",         (q_method_t)XMLREADER_moveToAttributeNs);
   QC_XMLREADER->addMethod("moveToElement",             (q_method_t)XMLREADER_moveToElement);
   QC_XMLREADER->addMethod("moveToFirstAttribute",      (q_method_t)XMLREADER_moveToFirstAttribute);
   QC_XMLREADER->addMethod("moveToNextAttribute",       (q_method_t)XMLREADER_moveToNextAttribute);
   QC_XMLREADER->addMethod("next",                      (q_method_t)XMLREADER_next);
   QC_XMLREADER->addMethod("getInnerXml",               (q_method_t)XMLREADER_getInnerXml);
   QC_XMLREADER->addMethod("getOuterXml",               (q_method_t)XMLREADER_getOuterXml);
   QC_XMLREADER->addMethod("relaxNGValidate",           (q_method_t)XMLREADER_relaxNGValidate);
   QC_XMLREADER->addMethod("schemaValidate",            (q_method_t)XMLREADER_schemaValidate);

   return QC_XMLREADER;   
}
