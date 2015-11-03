/*
 QC_XmlReader.h
 
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

#ifndef _QORE_QC_XMLREADER_H

#define _QORE_QC_XMLREADER_H

#include <qore/intern/QoreXmlReader.h>

#include <qore/intern/QC_XmlDoc.h>

DLLEXPORT extern qore_classid_t CID_XMLREADER;
DLLLOCAL QoreClass *initXmlReaderClass();

class QoreXmlReaderData : public AbstractPrivateData, public QoreXmlReader {
private:
   QoreXmlDocData *doc;
   QoreStringNode *xmlstr;

   // not implemented
   DLLLOCAL QoreXmlReaderData(const QoreXmlReaderData &orig);

public:
   DLLLOCAL QoreXmlReaderData(const QoreStringNode *n_xml, ExceptionSink *xsink) : QoreXmlReader(xsink, n_xml, QORE_XML_PARSER_OPTIONS), doc(0), xmlstr(n_xml->stringRefSelf()) {
   }

   DLLLOCAL QoreXmlReaderData(QoreXmlDocData *n_doc, ExceptionSink *xsink) : QoreXmlReader(xsink, n_doc->getDocPtr()), doc(n_doc), xmlstr(0) {
      doc->ref();
   }

   DLLLOCAL QoreXmlReaderData *copy(ExceptionSink *xsink) {
      if (doc)
	 return new QoreXmlReaderData(doc, xsink);

      return new QoreXmlReaderData(xmlstr, xsink);
   }

   DLLLOCAL ~QoreXmlReaderData() {
      if (doc) {
	 assert(!xmlstr);
	 doc->deref();
      }
      else {
	 assert(!doc);
	 xmlstr->deref();
      }
   }
};

#endif
