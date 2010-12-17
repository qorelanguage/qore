/*
  QC_XmlReader.cpp

  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/ql_xml.h>

qore_classid_t CID_XMLREADER;

static void XMLREADER_constructor_xmldoc(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(doc, QoreXmlDocData, params, 0, CID_XMLDOC, "XmlReader::constructor()", "XmlDoc", xsink);
   if (*xsink)
      return;

   ReferenceHolder<QoreXmlDocData> doc_holder(doc, xsink);

   SimpleRefHolder<QoreXmlReaderData> xr(new QoreXmlReaderData(doc, xsink));
   if (*xsink)
      return;

   self->setPrivate(CID_XMLREADER, xr.release());   
}

static void XMLREADER_constructor_str(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);

   if (p0->getEncoding() == QCS_UTF8)
      p0->ref();
   else {
      p0 = p0->convertEncoding(QCS_UTF8, xsink);
      if (!p0)
	 return;
   }

   SimpleRefHolder<QoreXmlReaderData> xr(new QoreXmlReaderData(const_cast<QoreStringNode *>(p0), xsink));
   if (*xsink)
      return;

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
   const QoreStringNode *attr = HARD_QORE_STRING(params, 0);
   return xr->getAttribute(attr->getBuffer());
}

static AbstractQoreNode *XMLREADER_getAttributeOffset(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->getAttributeOffset((int)HARD_QORE_INT(params, 0));
}

static AbstractQoreNode *XMLREADER_getAttributeNs(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *lname = HARD_QORE_STRING(params, 0);
   const QoreStringNode *ns = HARD_QORE_STRING(params, 1);
   return xr->getAttributeNs(lname->getBuffer(), ns->getBuffer());
}

// *string XmlReader::lookupNamespace()  
static AbstractQoreNode *XMLREADER_lookupNamespace(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   return xr->lookupNamespace(0);
}

// *string XmlReader::lookupNamespace(string $ns)  
static AbstractQoreNode *XMLREADER_lookupNamespace_str(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *prefix = HARD_QORE_STRING(params, 0);
   return xr->lookupNamespace(prefix->getBuffer());
}

static AbstractQoreNode *XMLREADER_moveToAttribute(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *attr = HARD_QORE_STRING(params, 0);
   int rc = xr->moveToAttribute(attr->getBuffer(), xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToAttributeOffset(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = xr->moveToAttributeOffset((int)HARD_QORE_INT(params, 0), xsink);
   return rc == -1 ? 0 : get_bool_node(rc);
}

static AbstractQoreNode *XMLREADER_moveToAttributeNs(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *lname = HARD_QORE_STRING(params, 0);
   const QoreStringNode *ns = HARD_QORE_STRING(params, 1);

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
   const QoreStringNode *rng = HARD_QORE_STRING(params, 0);
   xr->relaxNGValidate(rng->getBuffer(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderRelaxNGValidate() function, therefore XmlReader::relaxNGValidate() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHRELAXNG to check if this method is implemented before calling");
#endif
   return 0;
}

static AbstractQoreNode *XMLREADER_schemaValidate(QoreObject *self, QoreXmlReaderData *xr, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   const QoreStringNode *xsd = HARD_QORE_STRING(params, 0);
   xr->schemaValidate(xsd->getBuffer(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderSchemaValidate() function, therefore XmlReader::schemaValidate() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHSCHEMA to check if this method is implemented before calling");
#endif
   return 0;
}

QoreClass *initXmlReaderClass(QoreClass *XmlDoc) {
   QORE_TRACE("initXmlReaderClass()");

   QoreClass *QC_XMLREADER = new QoreClass("XmlReader");
   CID_XMLREADER = QC_XMLREADER->getID();

   QC_XMLREADER->setConstructorExtended(XMLREADER_constructor_xmldoc, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, XmlDoc->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_XMLREADER->setConstructorExtended(XMLREADER_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_XMLREADER->setCopy((q_copy_t)XMLREADER_copy);

   QC_XMLREADER->addMethodExtended("read",                      (q_method_t)XMLREADER_read, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("readSkipWhitespace",        (q_method_t)XMLREADER_readSkipWhitespace, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("nodeType",                  (q_method_t)XMLREADER_nodeType, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // *string XmlReader::nodeTypeName()  
   QC_XMLREADER->addMethodExtended("nodeTypeName",              (q_method_t)XMLREADER_nodeTypeName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   QC_XMLREADER->addMethodExtended("depth",                     (q_method_t)XMLREADER_depth, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // *string XmlReader::name()  
   QC_XMLREADER->addMethodExtended("name",                      (q_method_t)XMLREADER_name, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::value()  
   QC_XMLREADER->addMethodExtended("value",                     (q_method_t)XMLREADER_value, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   QC_XMLREADER->addMethodExtended("hasAttributes",             (q_method_t)XMLREADER_hasAttributes, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("hasValue",                  (q_method_t)XMLREADER_hasValue, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("isDefault",                 (q_method_t)XMLREADER_isDefault, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("isEmptyElement",            (q_method_t)XMLREADER_isEmptyElement, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("isNamespaceDecl",           (q_method_t)XMLREADER_isNamespaceDecl, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("isValid",                   (q_method_t)XMLREADER_isValid, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // hash XmlReader::toQore()  |string|nothing
   QC_XMLREADER->addMethodExtended("toQore",                    (q_method_t)XMLREADER_toQore, false, QC_RET_VALUE_ONLY);

   // hash XmlReader::toQoreData()  |string|nothing
   QC_XMLREADER->addMethodExtended("toQoreData",                (q_method_t)XMLREADER_toQoreData, false, QC_RET_VALUE_ONLY);

   QC_XMLREADER->addMethodExtended("attributeCount",            (q_method_t)XMLREADER_attributeCount, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // *string XmlReader::baseUri()  
   QC_XMLREADER->addMethodExtended("baseUri",                   (q_method_t)XMLREADER_baseUri, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::encoding()  
   QC_XMLREADER->addMethodExtended("encoding",                  (q_method_t)XMLREADER_encoding, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::localName()  
   QC_XMLREADER->addMethodExtended("localName",                 (q_method_t)XMLREADER_localName, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::namespaceUri()  
   QC_XMLREADER->addMethodExtended("namespaceUri",              (q_method_t)XMLREADER_namespaceUri, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::prefix()  
   QC_XMLREADER->addMethodExtended("prefix",                    (q_method_t)XMLREADER_prefix, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::xmlLang()  
   QC_XMLREADER->addMethodExtended("xmlLang",                   (q_method_t)XMLREADER_xmlLang, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::xmlVersion()  
   QC_XMLREADER->addMethodExtended("xmlVersion",                (q_method_t)XMLREADER_xmlVersion, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::getAttribute(string $attr)  
   QC_XMLREADER->addMethodExtended("getAttribute",              (q_method_t)XMLREADER_getAttribute, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *string XmlReader::getAttribute(softint $offset = 0)  
   QC_XMLREADER->addMethodExtended("getAttributeOffset",        (q_method_t)XMLREADER_getAttributeOffset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // *string XmlReader::getAttributeNs(string $attr, string $ns)  
   QC_XMLREADER->addMethodExtended("getAttributeNs",            (q_method_t)XMLREADER_getAttributeNs, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *string XmlReader::lookupNamespace()  
   QC_XMLREADER->addMethodExtended("lookupNamespace",           (q_method_t)XMLREADER_lookupNamespace, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);
   // *string XmlReader::lookupNamespace(string $ns)  
   QC_XMLREADER->addMethodExtended("lookupNamespace",           (q_method_t)XMLREADER_lookupNamespace_str, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // bool XmlReader::moveToAttribute(string $attr)  
   QC_XMLREADER->addMethodExtended("moveToAttribute",           (q_method_t)XMLREADER_moveToAttribute, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // bool XmlReader::moveToAttributeOffset(softint $offset = 0)  
   QC_XMLREADER->addMethodExtended("moveToAttributeOffset",     (q_method_t)XMLREADER_moveToAttributeOffset, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, softBigIntTypeInfo, zero());

   // bool XmlReader::moveToAttributeNs(string $attr, string $ns)  
   QC_XMLREADER->addMethodExtended("moveToAttributeNs",         (q_method_t)XMLREADER_moveToAttributeNs, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_XMLREADER->addMethodExtended("moveToElement",             (q_method_t)XMLREADER_moveToElement, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("moveToFirstAttribute",      (q_method_t)XMLREADER_moveToFirstAttribute, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("moveToNextAttribute",       (q_method_t)XMLREADER_moveToNextAttribute, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   QC_XMLREADER->addMethodExtended("next",                      (q_method_t)XMLREADER_next, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo);

   // *string XmlReader::getInnerXml()  
   QC_XMLREADER->addMethodExtended("getInnerXml",               (q_method_t)XMLREADER_getInnerXml, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *string XmlReader::getOuterXml()  
   QC_XMLREADER->addMethodExtended("getOuterXml",               (q_method_t)XMLREADER_getOuterXml, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // nothing XmlReader::relaxNGValidate()  
   QC_XMLREADER->addMethodExtended("relaxNGValidate",           (q_method_t)XMLREADER_relaxNGValidate, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // nothing XmlReader::schemaValidate()  
   QC_XMLREADER->addMethodExtended("schemaValidate",            (q_method_t)XMLREADER_schemaValidate, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_XMLREADER;   
}
