/*
 QoreXmlReader.h
 
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

#ifndef _QORE_QOREXMLREADER_H
#define _QORE_QOREXMLREADER_H

#include <libxml/xmlreader.h>

#include <qore/intern/QoreXmlDoc.h>

class QoreXmlReader {
protected:
   xmlTextReader *reader;
   const QoreString *xml;

   static void qore_xml_error_func(ExceptionSink *xsink, const char *msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator) {
      if (severity == XML_PARSER_SEVERITY_VALIDITY_WARNING
	  || severity == XML_PARSER_SEVERITY_WARNING) {
	 printd(1, "XML parser warning: %s", msg);
	 return;
      }
      if (*xsink)
	 return;
      QoreStringNode *desc = new QoreStringNode(msg);
      desc->chomp();
      xsink->raiseException("PARSE-XML-EXCEPTION", desc);
   }

   DLLLOCAL AbstractQoreNode *getXmlData(const QoreEncoding *data_ccsid, bool as_data, ExceptionSink *xsink);

   DLLLOCAL void init(const QoreString *n_xml, int options, ExceptionSink *xsink) {
      xml = n_xml;

      assert(xml->getEncoding() == QCS_UTF8);
      reader = xmlReaderForDoc((xmlChar *)xml->getBuffer(), 0, 0, options);
      if (!reader) {
	 xsink->raiseException("XML-READER-ERROR", "could not create XML reader");
	 return;
      }
	 
      xmlTextReaderSetErrorHandler(reader, (xmlTextReaderErrorFunc)qore_xml_error_func, xsink);
   }

   DLLLOCAL void init(xmlDocPtr doc, ExceptionSink *xsink) {
      xml = 0;
      reader = xmlReaderWalker(doc);
      if (!reader) {
	 xsink->raiseException("XML-READER-ERROR", "could not create XML reader");
	 return;
      }
      // the following call causes a crash - I guess the document has already been parsed anyway
      //xmlTextReaderSetErrorHandler(reader, (xmlTextReaderErrorFunc)qore_xml_error_func, xsink);
   }

public:
   DLLLOCAL QoreXmlReader(const QoreString *n_xml, int options, ExceptionSink *xsink) {
      init(n_xml, options, xsink);
   }

   DLLLOCAL QoreXmlReader(xmlDocPtr doc, ExceptionSink *xsink) {
      init(doc, xsink);
   }

   DLLLOCAL ~QoreXmlReader() {
      if (reader)
	 xmlFreeTextReader(reader);
   }

   DLLLOCAL operator bool() const {
      return reader != 0;
   }

   // returns 0=OK, -1=error
   DLLLOCAL int read(ExceptionSink *xsink) {
      if (xmlTextReaderRead(reader) != 1) {
	 xsink->raiseExceptionArg("PARSE-XML-EXCEPTION", xml ? new QoreStringNode(*xml) : 0, "cannot parse XML string");
	 return -1;
      }
      return 0;
   }

   DLLLOCAL int read() {
      return xmlTextReaderRead(reader);
   }

   DLLLOCAL int readSkipWhitespace(ExceptionSink *xsink) {
      while (true) {
	 if (read(xsink))
	    return -1;
 	 int nt = xmlTextReaderNodeType(reader);
	 if (nt != XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
	    break;
      }
      return 0;
   }

   DLLLOCAL int nodeType() {
      return xmlTextReaderNodeType(reader);
   }

   // gets the node type but skips whitespace
   DLLLOCAL int nodeTypeSkipWhitespace() {
      int nt;
      while (true) {
	 nt = xmlTextReaderNodeType(reader);
	 if (nt == XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
	    // get next element
	    if (read() != 1)
	       return -1;
	    continue;
	 }
	 break;
      }
      return nt;
   }

   DLLLOCAL int depth() {
      return xmlTextReaderDepth(reader);
   }

   DLLLOCAL const char *constName() {
      return (const char*)xmlTextReaderConstName(reader);
   }
      
   DLLLOCAL const char *constValue() {
      return (const char*)xmlTextReaderConstValue(reader);
   }

   DLLLOCAL bool hasAttributes() {
      return xmlTextReaderHasAttributes(reader);
   }

   DLLLOCAL int moveToNextAttribute() {
      return xmlTextReaderMoveToNextAttribute(reader);
   }

   DLLLOCAL QoreStringNode *getValue(const QoreEncoding *id, ExceptionSink *xsink) {
      if (id == QCS_UTF8)
	 return new QoreStringNode(constValue(), QCS_UTF8);
      
      return QoreStringNode::createAndConvertEncoding(constValue(), QCS_UTF8, id, xsink);
   }

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   DLLLOCAL int setSchema(xmlSchemaPtr schema) {
      return xmlTextReaderSetSchema(reader, schema);
   }
#endif

   DLLLOCAL AbstractQoreNode *parseXMLData(const QoreEncoding *data_ccsid, bool as_data, ExceptionSink *xsink);
};

#endif
