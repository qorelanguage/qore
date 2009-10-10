/*
  ql_xml.h

  libxml2-based XML functions

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

#ifndef _QORE_QL_XML_H
#define _QORE_QL_XML_H

#include <libxml/xmlschemas.h>
#include <libxml/relaxng.h>

DLLLOCAL QoreStringNode *makeXMLString(const QoreEncoding *enc, const QoreHashNode &h, bool format, ExceptionSink *xsink);
DLLLOCAL QoreStringNode *makeXMLRPCCallString(const QoreEncoding *ccs, int offset, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL QoreStringNode *makeXMLRPCCallStringArgs(const QoreEncoding *ccs, int offset, const QoreListNode *args, ExceptionSink *xsink);
// ccsid is the output encoding for strings
DLLLOCAL QoreHashNode *parseXMLRPCResponse(const QoreString *msg, const QoreEncoding *ccsid, ExceptionSink *xsink);
DLLLOCAL void init_xml_functions();

// returns the string corresponding to the element type
DLLLOCAL const char *get_xml_element_type_name(int t);

// returns the string corresponding to the node type
DLLLOCAL const char *get_xml_node_type_name(int t);

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
class QoreXmlSchemaContext {
   friend class QoreXmlSchemaValidContext;
protected:
   xmlSchemaPtr schema;

   DLLLOCAL xmlSchemaValidCtxtPtr getValidCtxtPtr() {
      return xmlSchemaNewValidCtxt(schema);
   }

public:
   DLLLOCAL QoreXmlSchemaContext(const char *xsd, int size, ExceptionSink *xsink);
   DLLLOCAL ~QoreXmlSchemaContext() {
      if (schema)
	 xmlSchemaFree(schema);
   }
   DLLLOCAL operator bool() const {
      return schema != 0;
   }
   DLLLOCAL xmlSchemaPtr getSchema() {
      return schema;
   }
};

class QoreXmlSchemaValidContext {
protected:
   xmlSchemaValidCtxtPtr ptr;

   DLLLOCAL QoreXmlSchemaValidContext(xmlSchemaValidCtxtPtr n_ptr) : ptr(n_ptr) {
   }

public:
   DLLLOCAL QoreXmlSchemaValidContext(QoreXmlSchemaContext &c) : ptr(c.getValidCtxtPtr()) {
      assert(ptr);
   }
   DLLLOCAL ~QoreXmlSchemaValidContext() {
      xmlSchemaFreeValidCtxt(ptr);
   }
   DLLLOCAL xmlSchemaValidCtxtPtr getPtr() {
      return ptr;
   }
   DLLLOCAL int validateDoc(xmlDocPtr doc) {
      return xmlSchemaValidateDoc(ptr, doc);
   }
};
#endif

#ifdef HAVE_XMLTEXTREADERRELAXNGSETSCHEMA
class QoreXmlRelaxNGContext {
   friend class QoreXmlRelaxNGValidContext;
protected:
   xmlRelaxNGPtr schema;

   DLLLOCAL xmlRelaxNGValidCtxtPtr getValidCtxtPtr() {
      return xmlRelaxNGNewValidCtxt(schema);
   }

public:
   DLLLOCAL QoreXmlRelaxNGContext(const char *rng, int size, ExceptionSink *xsink);
   DLLLOCAL ~QoreXmlRelaxNGContext() {
      if (schema)
	 xmlRelaxNGFree(schema);
   }
   DLLLOCAL operator bool() const {
      return schema != 0;
   }
   DLLLOCAL xmlRelaxNGPtr getSchema() {
      return schema;
   }
};

class QoreXmlRelaxNGValidContext {
protected:
   xmlRelaxNGValidCtxtPtr ptr;

   DLLLOCAL QoreXmlRelaxNGValidContext(xmlRelaxNGValidCtxtPtr n_ptr) : ptr(n_ptr) {
   }

public:
   DLLLOCAL QoreXmlRelaxNGValidContext(QoreXmlRelaxNGContext &c) : ptr(c.getValidCtxtPtr()) {
      assert(ptr);
   }
   DLLLOCAL ~QoreXmlRelaxNGValidContext() {
      xmlRelaxNGFreeValidCtxt(ptr);
   }
   DLLLOCAL xmlRelaxNGValidCtxtPtr getPtr() {
      return ptr;
   }
   DLLLOCAL int validateDoc(xmlDocPtr doc) {
      return xmlRelaxNGValidateDoc(ptr, doc);
   }
};
#endif

#endif // _QORE_QL_XML_H
