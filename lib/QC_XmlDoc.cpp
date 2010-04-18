/*
  QC_XmlDoc.cpp

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
#include <qore/intern/QC_XmlDoc.h>
#include <qore/intern/QoreXPath.h>
#include <qore/intern/QoreXmlReader.h>
#include <qore/intern/QC_XmlNode.h>
#include <qore/intern/ql_xml.h>

qore_classid_t CID_XMLDOC;

#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
int QoreXmlDoc::validateRelaxNG(const char *rng, int size, ExceptionSink *xsink) {
   QoreXmlRelaxNGContext schema(rng, size, xsink);
   if (!schema) {
      if (!*xsink)
	 xsink->raiseException("RELAXNG-ERROR", "RelaxNG schema passed as argument to XmlDoc::validateRelaxNG() could not be parsed");
      return -1;
   }

   QoreXmlRelaxNGValidContext vcp(schema);
   int rc = vcp.validateDoc(ptr);

   if (!rc)
      return 0;
   if (*xsink)
      return -1;

   if (rc < 0)
      xsink->raiseException("RELAXNG-INTERNAL-ERROR", "an internal error occured validating the document against the RelaxNG schema passed; xmlRelaxNGValidateDoc() returned %d", rc);
   else if (rc)
      xsink->raiseException("RELAXNG-ERROR", "The document failed RelaxNG validation", rc);
   return -1;
}
#endif

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
int QoreXmlDoc::validateSchema(const char *xsd, int size, ExceptionSink *xsink) {
   QoreXmlSchemaContext schema(xsd, size, xsink);
   if (!schema) {
      if (!*xsink)
	 xsink->raiseException("XSD-ERROR", "XSD schema passed as argument to XmlDoc::validateSchema() could not be parsed");
      return -1;
   }

   QoreXmlSchemaValidContext vcp(schema);
   int rc = vcp.validateDoc(ptr);

   if (!rc)
      return 0;
   if (*xsink)
      return -1;

   if (rc < 0)
      xsink->raiseException("XSD-INTERNAL-ERROR", "an internal error occured validating the document against the XSD schema passed; xmlSchemaValidateDoc() returned %d", rc);
   else if (rc)
      xsink->raiseException("XSD-ERROR", "The document failed XSD validation", rc);
   return -1;
}
#endif

QoreXmlNodeData *QoreXmlDocData::getRootElement() {
   xmlNodePtr n = xmlDocGetRootElement(ptr);
   if (!n) return 0;
   return new QoreXmlNodeData(n, this);
}

QoreStringNode *doString(xmlChar *str) {
   if (!str)
      return 0;
   QoreStringNode *rv = new QoreStringNode((const char *)str);
   xmlFree(str);
   return rv;
}

QoreXmlNodeData *doNode(xmlNodePtr p, QoreXmlDocData *doc) {
   if (!p)
      return 0;
   return new QoreXmlNodeData(p, doc);
}

static void XMLDOC_constructor_hash(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *h = HARD_QORE_HASH(params, 0);
   SimpleRefHolder<QoreStringNode> xml(makeXMLString(QCS_UTF8, *h, false, xsink));
   if (!xml)
      return;
   SimpleRefHolder<QoreXmlDocData> xd(new QoreXmlDocData(*xml));
   if (!xd->isValid()) {
      xsink->raiseException("XMLDOC-CONSTRUCTOR-ERROR", "error parsing XML string");
      return;
   }

   self->setPrivate(CID_XMLDOC, xd.release());   
}

static void XMLDOC_constructor_str(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   SimpleRefHolder<QoreXmlDocData> xd(new QoreXmlDocData(str));
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
   const QoreStringNode *expr = HARD_QORE_STRING(params, 0);
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

static AbstractQoreNode *XMLDOC_validateRelaxNG(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
   const QoreStringNode *rng = HARD_QORE_STRING(params, 0);
   // convert to UTF-8 
   TempEncodingHelper nrng(rng, QCS_UTF8, xsink);
   if (!nrng)
      return 0;

   xd->validateRelaxNG(nrng->getBuffer(), nrng->strlen(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderRelaxNGValidate() function, therefore XmlDoc::validateRelaxNG() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHRELAXNG to check if this method is implemented before calling");
#endif
   return 0;
}

static AbstractQoreNode *XMLDOC_validateSchema(QoreObject *self, QoreXmlDocData *xd, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   const QoreStringNode *xsd = HARD_QORE_STRING(params, 0);
   // convert to UTF-8 
   TempEncodingHelper nxsd(xsd, QCS_UTF8, xsink);
   if (!nxsd)
      return 0;

   xd->validateSchema(nxsd->getBuffer(), nxsd->strlen(), xsink);
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderSchemaValidate() function, therefore XmlDoc::validateSchema() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHSCHEMA to check if this method is implemented before calling");
#endif
   return 0;
}

QoreClass *initXmlDocClass() {
   QORE_TRACE("initXmlDocClass()");

   QoreClass *QC_XMLDOC = new QoreClass("XmlDoc");
   CID_XMLDOC = QC_XMLDOC->getID();

   QC_XMLDOC->setConstructorExtended(XMLDOC_constructor_hash, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, hashTypeInfo, QORE_PARAM_NO_ARG);
   QC_XMLDOC->setConstructorExtended(XMLDOC_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_XMLDOC->setCopy((q_copy_t)XMLDOC_copy);

   QC_XMLDOC->addMethodExtended("getVersion",      (q_method_t)XMLDOC_getVersion, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_XMLDOC->addMethodExtended("toQore",          (q_method_t)XMLDOC_toQore, false, QC_RET_VALUE_ONLY);
   QC_XMLDOC->addMethodExtended("toQoreData",      (q_method_t)XMLDOC_toQoreData, false, QC_RET_VALUE_ONLY);
   QC_XMLDOC->addMethodExtended("toString",        (q_method_t)XMLDOC_toString, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);
   QC_XMLDOC->addMethodExtended("evalXPath",       (q_method_t)XMLDOC_evalXPath, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_XMLDOC->addMethodExtended("getRootElement",  (q_method_t)XMLDOC_getRootElement, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, QC_XMLNODE->getTypeInfo());
   QC_XMLDOC->addMethodExtended("validateRelaxNG", (q_method_t)XMLDOC_validateRelaxNG, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_XMLDOC->addMethodExtended("validateSchema",  (q_method_t)XMLDOC_validateSchema, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_XMLDOC;   
}

