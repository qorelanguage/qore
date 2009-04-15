/*
  lib/ql_xml.cc

  Qore XML functions

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
#include <qore/intern/ql_xml.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemas.h>

#include <string.h>
#include <memory>

#ifdef DEBUG
#define QORE_XML_READER_PARAMS XML_PARSE_NOBLANKS
#else
#define QORE_XML_READER_PARAMS XML_PARSE_NOERROR | XML_PARSE_NOWARNING | XML_PARSE_NOBLANKS
#endif

namespace { // make classes local

class XmlRpcValue {
   private:
      AbstractQoreNode *val;
      AbstractQoreNode **vp;

   public:
      DLLLOCAL inline XmlRpcValue() : val(0), vp(0) {
      }

      DLLLOCAL inline ~XmlRpcValue() {
	 if (val) {
	    val->deref(0);
	    val = 0;
	 }
      }

      DLLLOCAL inline AbstractQoreNode *getValue() {
	 AbstractQoreNode *rv = val;
	 val = 0;
	 return rv;
      }

      DLLLOCAL inline void set(AbstractQoreNode *v) {
	 if (vp)
	    *vp = v;
	 else
	    val = v;
      }

      DLLLOCAL inline void setPtr(AbstractQoreNode **v) {
	 vp = v;
      }
};

class xml_node {
   public:
      AbstractQoreNode **node;
      xml_node *next;
      int depth;
      int vcount;
      int cdcount;

      DLLLOCAL xml_node(AbstractQoreNode **n, int d) 
	 : node(n), next(0), depth(d), vcount(0), cdcount(0) {
      }
};

class xml_stack {
   private:
      xml_node *tail;
      AbstractQoreNode *val;
      
   public:
      DLLLOCAL inline xml_stack() {
	 tail = 0;
	 val = 0;
	 push(&val, -1);
      }
      
      DLLLOCAL inline ~xml_stack() {
	 if (val)
	    val->deref(0);

	 while (tail)
	 {
	    //printd(5, "xml_stack::~xml_stack(): deleting=%08p (%d), next=%08p\n", tail, tail->depth, tail->next);
	    xml_node *n = tail->next;
	    delete tail;
	    tail = n;
	 }
      }
      
      DLLLOCAL inline void checkDepth(int depth) {
	 while (tail && depth && tail->depth >= depth) {
	    //printd(5, "xml_stack::checkDepth(%d): deleting=%08p (%d), new tail=%08p\n", depth, tail, tail->depth, tail->next);
	    xml_node *n = tail->next;
	    delete tail;
	    tail = n;
	 }
      }

      DLLLOCAL inline void push(AbstractQoreNode **node, int depth) {
	 xml_node *sn = new xml_node(node, depth);
	 sn->next = tail;
	 tail = sn;
      }
      DLLLOCAL inline AbstractQoreNode *getNode() {
	 return *tail->node;
      }
      DLLLOCAL inline void setNode(AbstractQoreNode *n) {
	 (*tail->node) = n;
      }
      DLLLOCAL inline AbstractQoreNode *getVal() {
	 AbstractQoreNode *rv = val;
	 val = 0;
	 return rv;
      }
      DLLLOCAL inline int getValueCount() const {
	 return tail->vcount;
      }
      DLLLOCAL inline void incValueCount() {
	 tail->vcount++;
      }
      DLLLOCAL inline int getCDataCount() const {
	 return tail->cdcount;
      }
      DLLLOCAL inline void incCDataCount() {
	 tail->cdcount++;
      }
};

} // anonymous namespace

#if 0
// does not work well, produces ugly,. uninformative output
static void qore_xml_structured_error_func(ExceptionSink *xsink, xmlErrorPtr error) {
   QoreStringNode *desc = new QoreStringNode;

   if (error->line)
      desc->sprintf("line %d: ", error->line);

   if (error->int2)
      desc->sprintf("column %d: ", error->int2);

   desc->concat(error->message);
   desc->chomp();
   
   if (error->str1)
      desc->sprintf(", %s", error->str1);

   if (error->str2)
      desc->sprintf(", %s", error->str1);

   if (error->str3)
      desc->sprintf(", %s", error->str1);

   xsink->raiseException("PARSE-XML-EXCEPTION", desc);
}
#endif

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

class QoreXmlReader {
   protected:
      xmlTextReader *reader;
      const QoreString *xml;

   public:
      DLLLOCAL QoreXmlReader(const QoreString *n_xml, int options, ExceptionSink *xsink) : xml(n_xml) {
	 assert(xml->getEncoding() == QCS_UTF8);
	 reader = xmlReaderForDoc((xmlChar *)xml->getBuffer(), 0, 0, options);
	 if (!reader)
	    xsink->raiseException("XML-READER-ERROR", "could not create XML reader");

	 xmlTextReaderSetErrorHandler(reader, (xmlTextReaderErrorFunc)qore_xml_error_func, xsink);
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
	    xsink->raiseExceptionArg("PARSE-XML-EXCEPTION", new QoreStringNode(*xml), "cannot parse XML string");
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL int read() {
	 return xmlTextReaderRead(reader);
      }

      // gets the node type but skips whitespace
      DLLLOCAL int nodeType() {
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

#ifdef HAVE_XMLTEXTREADERSETSCHEMA
      DLLLOCAL int setSchema(xmlSchemaPtr schema) {
	 return xmlTextReaderSetSchema(reader, schema);
      }
#endif

      DLLLOCAL int readXmlRpc(ExceptionSink *xsink) {
	 int rc = read();
	 if (rc != 1) {
	    if (!*xsink)
	       xsink->raiseException("XML-RPC-PARSE-ERROR", "error parsing XML string");
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL int readXmlRpc(const char *info, ExceptionSink *xsink) {
	 int rc = read();
	 if (rc != 1) {
	    if (!*xsink)
	       xsink->raiseExceptionArg("XML-RPC-PARSE-ERROR", new QoreStringNode(*xml), "error parsing XML string: %s", info);
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL int readXmlRpcNode(ExceptionSink *xsink) {
	 int nt = nodeType();
	 if (nt == -1 && !*xsink)
	       xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string");
	 return nt;
      }

#ifdef DEBUG
      DLLLOCAL const char *getName() {
	 return (const char *)xmlTextReaderConstName(reader);
      }
#endif

      DLLLOCAL int checkXmlRpcMemberName(const char *member, ExceptionSink *xsink) {
	 const char *name = (const char *)xmlTextReaderConstName(reader);
	 if (!name) {
	    xsink->raiseExceptionArg("XML-RPC-PARSE-VALUE-ERROR", new QoreStringNode(*xml), "expecting element '%s', got NOTHING", member);
	    return -1;
	 }
	 
	 if (strcmp(name, member)) {
	    xsink->raiseExceptionArg("XML-RPC-PARSE-VALUE-ERROR", new QoreStringNode(*xml), "expecting element '%s', got '%s'", member, name);
	    return -1;
	 }
	 return 0;
      }

      DLLLOCAL void getXMLRPCArray(XmlRpcValue *v, const QoreEncoding *data_ccsid, ExceptionSink *xsink);
};

static void qore_xml_schema_error_func(ExceptionSink *xsink, const char *msg, ...) {
   if (*xsink)
      return;

   va_list args;
   QoreStringNode *desc = new QoreStringNode;

   while (true) {
      va_start(args, msg);
      int rc = desc->vsprintf(msg, args);
      va_end(args);
      if (!rc)
	 break;
   }
   desc->chomp();

   xsink->raiseException("XML-SCHEMA-PARSE-ERROR", desc);
}

static void qore_xml_schema_warning_func(ExceptionSink *xsink, const char *msg, ...) {
#ifdef DEBUG
   va_list args;
   QoreString buf;

   while (true) {
      va_start(args, msg);
      int rc = buf.vsprintf(msg, args);
      va_end(args);
      if (!rc)
	 break;
   }

   printf("%s", buf.getBuffer());
#endif
}

class QoreXmlSchemaContext {
   protected:
      xmlSchemaPtr schema;

   public:
      DLLLOCAL QoreXmlSchemaContext(const char *xsd, int size, ExceptionSink *xsink) : schema(0) {
	 xmlSchemaParserCtxtPtr scp = xmlSchemaNewMemParserCtxt(xsd, size);
	 if (!scp)
	    return;

	 //xmlSchemaSetParserStructuredErrors(scp, (xmlStructuredErrorFunc)qore_xml_structured_error_func, xsink);

	 xmlSchemaSetParserErrors(scp, (xmlSchemaValidityErrorFunc)qore_xml_schema_error_func, 
				 (xmlSchemaValidityErrorFunc)qore_xml_schema_warning_func , xsink);

	 schema = xmlSchemaParse(scp);

	 xmlSchemaFreeParserCtxt(scp);

	 if (!schema)
	    return;
      }

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

static void makeXMLString(QoreString *str, const QoreHashNode *h, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink);

static void concatSimpleValue(QoreString *str, const AbstractQoreNode *n, ExceptionSink *xsink) {
   //printd(5, "concatSimpleValue() n=%08p (%s) %s\n", n, n->getTypeName(), n->getType() == NT_STRING ? ((QoreStringNode *)n)->getBuffer() : "unknown");

   qore_type_t ntype = n ? n->getType() : 0;

   if (ntype == NT_STRING) {
      const QoreStringNode *qsn = reinterpret_cast<const QoreStringNode *>(n);
      str->concatAndHTMLEncode(qsn, xsink);
      return;
   }

   if (ntype == NT_INT) {
      const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(n);
      str->sprintf("%lld", b->val);
      return;
   }

   if (ntype == NT_FLOAT) {
      str->sprintf("%.9g", reinterpret_cast<const QoreFloatNode *>(n)->f);
      return;
   }

   if (ntype == NT_BOOLEAN) {
      str->sprintf("%d", reinterpret_cast<const QoreBoolNode *>(n)->getValue());
      return;
   }

   if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(n);
      str->concat(date);
      return;
   }

   QoreStringValueHelper temp(n);
   str->concatAndHTMLEncode(*temp, xsink);
}

static void concatSimpleCDataValue(QoreString *str, const AbstractQoreNode *n, ExceptionSink *xsink) {
   //printd(5, "concatSimpleValue() n=%08p (%s) %s\n", n, n->getTypeName(), n->getType() == NT_STRING ? ((QoreStringNode *)n)->getBuffer() : "unknown");

   qore_type_t ntype = n ? n->getType() : 0;

   if (ntype == NT_STRING) {
      const QoreStringNode *qsn = reinterpret_cast<const QoreStringNode *>(n);
      if (strstr(qsn->getBuffer(), "]]>")) {
	 xsink->raiseException("MAKE-XML-ERROR", "CDATA text contains illegal ']]>' sequence");
	 return;
      }
      str->concat(qsn, xsink);
      return;
   }

   if (ntype == NT_INT) {
      const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(n);
      str->sprintf("%lld", b->val);
      return;
   }

   if (ntype == NT_FLOAT) {
      str->sprintf("%.9g", reinterpret_cast<const QoreFloatNode *>(n)->f);
      return;
   }

   if (ntype == NT_BOOLEAN) {
      str->sprintf("%d", reinterpret_cast<const QoreBoolNode *>(n)->getValue());
      return;
   }

   if (ntype == NT_DATE) {
      const DateTimeNode *date = reinterpret_cast<const DateTimeNode *>(n);
      str->concat(date);
      return;
   }

   QoreStringValueHelper temp(n);
   str->concat(*temp, xsink);
}

static void addXMLElement(const char *key, QoreString *str, const AbstractQoreNode *n, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink)
{
   //QORE_TRACE("addXMLElement()");

   if (is_nothing(n)) {
      str->concat('<');
      str->concat(key);
      str->concat("/>");
      return;
   }

   qore_type_t ntype = n->getType();

   if (ntype == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(n);
      // iterate through the list
      int ls = l->size();
      if (ls) {
	 for (int j = 0; j < ls; j++) {
	    const AbstractQoreNode *v = l->retrieve_entry(j);
	    // indent all but first entry if necessary
	    if (j && format) {
	       str->concat('\n');
	       str->addch(' ', indent);
	    }
	    
	    addXMLElement(key, str, v, indent, ccs, format, xsink);
	 }
      }
      else {    // close node
	 str->concat('<');
	 str->concat(key);
	 str->concat("/>");
      }
      return;
   }

   // open node
   str->concat('<');
   str->concat(key);

   if (ntype == NT_HASH) {
      const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(n);
      // inc = ignore node counter, see if special keys exists and increment counter even if they have no value
      qore_size_t inc = 0;
      int vn = 0;
      bool exists;
      const AbstractQoreNode *value = h->getKeyValueExistence("^value^", exists);
      if (!exists)
	 value = 0;
      else {
	 vn++;
	 if (is_nothing(value))
	    inc++;
	 // find all ^value*^ nodes
	 QoreString val;
	 while (true) {
	    val.sprintf("^value%d^", vn);
	    value = h->getKeyValueExistence(val.getBuffer(), exists);
	    if (!exists) {
	       value = 0;
	       break;
	    }
	    else if (is_nothing(value)) // if the node exists but there is no value, then skip
	       inc++;
	    vn++;
	 }
      }
      
      const AbstractQoreNode *attrib = h->getKeyValueExistence("^attributes^", exists);
      if (!exists)
	 attrib = 0;
      else
	 inc++;
      
      // add attributes for objects
      if (attrib && attrib->getType() == NT_HASH) {
	 const QoreHashNode *ah = reinterpret_cast<const QoreHashNode *>(attrib);
	 // add attributes to node
	 ConstHashIterator hi(ah);
	 while (hi.next()) {
	    const char *key = hi.getKey();
	    str->sprintf(" %s=\"", key);
	    const AbstractQoreNode *v = hi.getValue();
	    if (v) {
	       if (v->getType() == NT_STRING) 
		  str->concatAndHTMLEncode(reinterpret_cast<const QoreStringNode *>(v), xsink);
	       else { // convert to string and add
		  QoreStringValueHelper temp(v);
		  str->concat(*temp, xsink);
	       }
	    }
	    str->concat('\"');
	 }
      }
      
      //printd(5, "inc=%d vn=%d\n", inc, vn);
      
      // if there are no more elements, close node immediately
      if (h->size() == inc) {
	 str->concat("/>");
	 return;
      }
      
      // close node
      str->concat('>');

      if (!is_nothing(value) && h->size() == (inc + 1))
	 concatSimpleValue(str, value, xsink);
      else { // add additional elements and formatting only if the additional elements exist 
	 if (format && !vn)
	    str->concat('\n');
	 
	 makeXMLString(str, h, indent + 2, ccs, !vn ? format : 0, xsink);
	 // indent closing entry
	 if (format && !vn) {
	    str->concat('\n');
	    str->addch(' ', indent);
	 }
      }
   }
   else {
      // close node
      str->concat('>');
      
      if (ntype == NT_OBJECT) {
	 const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
	 // get snapshot of data
	 QoreHashNodeHolder h(o->copyData(xsink), xsink);
	 if (!*xsink) {
	    if (format)
	       str->concat('\n');
	    makeXMLString(str, *h, indent + 2, ccs, format, xsink);
	    // indent closing entry
	    if (format)
	       str->addch(' ', indent);
	 }
      }
      else 
	 concatSimpleValue(str, n, xsink);
   }
   
   // close node
   str->concat("</");
   str->concat(key);
   str->concat('>');

}

static void makeXMLString(QoreString *str, const QoreHashNode *h, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink) {
   QORE_TRACE("makeXMLString()");
   ConstHashIterator hi(h);
   bool done = false;
   while (hi.next()) {
      std::auto_ptr<QoreString> keyStr(hi.getKeyString());
      // convert string if needed
      if (keyStr->getEncoding() != ccs) {
	 QoreString *ns = keyStr->convertEncoding(ccs, xsink);
	 if (xsink->isEvent())
	    break;
	 keyStr.reset(ns);
      }

      const char *key = keyStr->getBuffer();
      if (!strcmp(key, "^attributes^"))
	 continue;

      if (!strncmp(key, "^value", 6)) {
	 concatSimpleValue(str, hi.getValue(), xsink);
	 if (xsink->isException())
	    break;
	 continue;
      }

      if (!strncmp(key, "^cdata", 5)) {
	 str->concat("<![CDATA[");
	 concatSimpleCDataValue(str, hi.getValue(), xsink);
	 if (xsink->isException())
	    break;
	 str->concat("]]>");
	 continue;
      }

      // make sure it's a valid XML tag element name
      if (!key || !isalpha(key[0])) {
	 xsink->raiseException("MAKE-XML-ERROR", "tag: \"%s\" is not a valid XML tag element name", key ? key : "");
	 break;
      }

      // process key name - remove ^# from end of key name if present
      qore_size_t l = keyStr->strlen() - 1;
      while (isdigit(key[l]))
	 l--;

      if (l != (keyStr->strlen() - 1) && key[l] == '^')
	 keyStr->terminate(l);

      // indent entry
      if (format) {
	 if (done)
	    str->concat('\n');
         str->addch(' ', indent);
      }
      //printd(5, "makeXMLString() level %d adding member %s\n", indent / 2, node->getBuffer());
      addXMLElement(key, str, hi.getValue(), indent, ccs, format, xsink);
      done = true;
   }

}

// returns top-level key name
static bool hash_ok(const QoreHashNode *h) {
   int count = 0;

   ConstHashIterator hi(h);

   while (hi.next()) {
      const char *k = hi.getKey();
      if (!k[0] || k[0] == '^')
	 continue;

      if (++count > 1)
	 break;
   }

   return count == 1;
}

// usage: makeXMLString(object (with only one top-level element) [, encoding])
// usage: makeXMLString(string (top-level-element), object [, encoding])
static AbstractQoreNode *f_makeXMLString(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *pobj;
   int i;
   const QoreEncoding *ccs;
   const QoreStringNode *pstr = 0;

   QORE_TRACE("f_makeXMLString()");
   if ((pobj = test_hash_param(params, 0)) && hash_ok(pobj)) {
      pstr = 0;
      i = 1;
   }
   else {
      if (!(pstr = test_string_param(params, 0)) || !(pobj = test_hash_param(params, 1))) {
	 xsink->raiseException("MAKE-XML-STRING-PARAMETER-EXCEPTION",
			       "expecting either hash with one member or string, hash as parameters");
	 return 0;
      }
      i = 2;
   }
   const QoreStringNode *pcs;
   if ((pcs = test_string_param(params, i)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNode *str = new QoreStringNode(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>", ccs->getCode());

   if (pstr) {
      TempEncodingHelper key(pstr, QCS_UTF8, xsink);
      if (!key)
	 return 0;
      addXMLElement(key->getBuffer(), str, pobj, 0, ccs, 0, xsink);
   }
   else
      makeXMLString(str, pobj, 0, ccs, 0, xsink);

   //printd(5, "f_makeXMLString() returning %s\n", str->getBuffer());

   return str;
}

// usage: makeFormattedXMLString(object (with only one top-level element) [, encoding])
// usage: makeFormattedXMLString(string (top-level-element), object [, encoding])
static AbstractQoreNode *f_makeFormattedXMLString(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *pobj;
   int i;
   const QoreStringNode *pstr;

   QORE_TRACE("f_makeFormattedXMLString()");
   if ((pobj = test_hash_param(params, 0)) && hash_ok(pobj)) {
      pstr = 0;
      i = 1;
   }
   else {
      if (!(pstr = test_string_param(params, 0)) || !(pobj = test_hash_param(params, 1))) {
	 xsink->raiseException("MAKE-FORMATTED-XML-STRING-PARAMETER-EXCEPTION",
			       "expecting either hash with one member or (string, hash) as parameters");
	 return 0;
      }
      i = 2;
   }

   const QoreEncoding *ccs;
   const QoreStringNode *pcs;
   if ((pcs = test_string_param(params, i)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNode *str = new QoreStringNode(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n", ccs->getCode());
   if (pstr) {
      TempEncodingHelper key(pstr, QCS_UTF8, xsink);
      if (!key)
	 return 0;
      addXMLElement(key->getBuffer(), str, pobj, 0, ccs, 1, xsink);
   }
   else
      makeXMLString(str, pobj, 0, ccs, 1, xsink);

   return str;
}

static AbstractQoreNode *f_makeXMLFragment(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreHashNode *pobj;

   QORE_TRACE("f_makeXMLFragment()");
   pobj = test_hash_param(params, 0);
   if (!pobj)
      return 0;

   const QoreEncoding *ccs;
   const QoreStringNode *pcs;
   if ((pcs = test_string_param(params, 1)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNode *str = new QoreStringNode();
   makeXMLString(str, pobj, 0, ccs, 0, xsink);

   return str;
}

static AbstractQoreNode *f_makeFormattedXMLFragment(const QoreListNode *params, ExceptionSink *xsink) {
   QORE_TRACE("f_makeFormattedXMLFragment()");

   const QoreHashNode *pobj = test_hash_param(params, 0);
   if (!pobj)
      return 0;

   const QoreEncoding *ccs;
   const QoreStringNode *pcs;
   if ((pcs = test_string_param(params, 1)))
      ccs = QEM.findCreate(pcs);
   else
      ccs = QCS_UTF8;

   QoreStringNode *str = new QoreStringNode();
   makeXMLString(str, pobj, 0, ccs, 1, xsink);

   return str;
}

static void addXMLRPCValue(QoreString *str, const AbstractQoreNode *n, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink);

static inline void addXMLRPCValueInternHash(QoreString *str, const QoreHashNode *h, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink) {
   str->concat("<struct>");
   if (format) str->concat('\n');
   ConstHashIterator hi(h);
   while (hi.next()) {
      std::auto_ptr<QoreString> member(hi.getKeyString());
      if (!member->strlen()) {
	 xsink->raiseException("XML-RPC-STRUCT-ERROR", "missing member name in struct");
	 return;
      }
      // convert string if needed
      if (member->getEncoding() != ccs) {
	 QoreString *ns = member->convertEncoding(ccs, xsink);
	 if (xsink->isEvent()) {
	    return;	    
	 }
	 //printd(5, "addXMLRPCValueInternHashInternal() converted %s->%s, \"%s\"->\"%s\"\n", member->getEncoding()->getCode(), ccs->getCode(), member->getBuffer(), ns->getBuffer());
	 member.reset(ns);
      }
      //else printd(5, "addXMLRPCValueInternHashInternal() not converting %sx \"%s\"\n", member->getEncoding()->getCode(), member->getBuffer());
      // indent
      if (format)
         str->addch(' ', indent + 2);
      str->concat("<member>");
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 4);
      }
      str->concat("<name>");
      str->concatAndHTMLEncode(member.get(), xsink);

      member.reset();

      str->concat("</name>");
      if (format) str->concat('\n');
      const AbstractQoreNode *val = hi.getValue();
      addXMLRPCValue(str, val, indent + 4, ccs, format, xsink);
      // indent
      if (format)
         str->addch(' ', indent + 2);
      str->concat("</member>");
      if (format) str->concat('\n');
   }
   // indent
   if (format)
      str->addch(' ', indent);
   str->concat("</struct>");
   //if (format) str->concat('\n');
}

static void addXMLRPCValueIntern(QoreString *str, const AbstractQoreNode *n, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink)
{
   assert(n);
   qore_type_t ntype = n->getType();

   if (ntype == NT_BOOLEAN)
      str->sprintf("<boolean>%d</boolean>", reinterpret_cast<const QoreBoolNode *>(n)->getValue());

   else if (ntype == NT_INT)
      str->sprintf("<i4>%lld</i4>", reinterpret_cast<const QoreBigIntNode *>(n)->val);

   else if (ntype == NT_STRING) {
      str->concat("<string>");
      str->concatAndHTMLEncode(reinterpret_cast<const QoreStringNode *>(n), xsink);
      str->concat("</string>");
   }

   else if (ntype == NT_FLOAT)
      str->sprintf("<double>%f</double>", reinterpret_cast<const QoreFloatNode *>(n)->f);
	
   else if (ntype == NT_DATE) {
      str->concat("<dateTime.iso8601>");
      str->concatISO8601DateTime(reinterpret_cast<const DateTimeNode *>(n));
      str->concat("</dateTime.iso8601>");
   }

   else if (ntype == NT_BINARY) {
      str->concat("<base64>");
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 4);
      }
      str->concatBase64(reinterpret_cast<const BinaryNode *>(n));
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent);
      }
      str->concat("</base64>");
   }

   else if (ntype == NT_HASH)
      addXMLRPCValueInternHash(str, reinterpret_cast<const QoreHashNode *>(n), indent + 2, ccs, format, xsink);

   else if (ntype == NT_LIST) {
      const QoreListNode *l = reinterpret_cast<const QoreListNode *>(n);
      str->concat("<array>");
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 4);
      }
      if (l->size()) {
	 str->concat("<data>");
	 if (format) str->concat('\n');
	 for (unsigned i = 0; i < l->size(); i++)
	    addXMLRPCValue(str, l->retrieve_entry(i), indent + 6, ccs, format, xsink);
	 if (format)
            str->addch(' ', indent + 4);
	 str->concat("</data>");
      }
      else
	 str->concat("<data/>");
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 2);
      }
      str->concat("</array>");
      //if (format) str->concat('\n');
   }

   if (format) {
      str->concat('\n');
      // indent
      str->addch(' ' , indent);
   }
}

static void addXMLRPCValue(QoreString *str, const AbstractQoreNode *n, int indent, const QoreEncoding *ccs, int format, ExceptionSink *xsink)
{
   QORE_TRACE("addXMLRPCValue()");

   // add value node
   // indent
   if (format)
      str->addch(' ', indent);
   
   if (!is_nothing(n)) {
      str->concat("<value>");
      if (format) {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 2);
      }
      
      addXMLRPCValueIntern(str, n, indent, ccs, format, xsink);

      // close value node
      str->concat("</value>");
   }
   else
      str->concat("<value/>"); 
   if (format) str->concat('\n');

}

QoreStringNode *makeXMLRPCCallString(const QoreEncoding *ccs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p;

   const QoreStringNode *p0;
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION", "expecting method name as first parameter");

      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodCall><methodName>", ccs->getCode());
   str->concatAndHTMLEncode(p0, xsink);
   if (*xsink)
      return 0;

   str->concat("</methodName>");

   // now process all params
   int ls = num_params(params);
   if (ls) {
      str->concat("<params>"); 

      for (int i = 1; i < ls; i++) {
	 if ((p = get_param(params, i))) {
	    str->concat("<param>");
	    addXMLRPCValue(*str, p, 0, ccs, 0, xsink);
	    if (*xsink)
	       return 0;
	    str->concat("</param>");
	 }
	 else
	    str->concat("<param/>");
      }
      str->concat("</params>");
   }
   else
      str->concat("<params/>");
   str->concat("</methodCall>");

   return str.release();
}

// makeXMLRPCCallString(string (function name), params, ...)
static AbstractQoreNode *f_makeXMLRPCCallString(const QoreListNode *params, ExceptionSink *xsink)
{
   return makeXMLRPCCallString(QCS_DEFAULT, params, xsink);
}

// makeXMLRPCCallStringArgs(string (function name), list of params)
QoreStringNode *makeXMLRPCCallStringArgs(const QoreEncoding *ccs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;

   QORE_TRACE("makeXMLRPCCallStringArgs()");
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-ARGS-PARAMETER-EXCEPTION", "expecting method name as first parameter");

      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodCall><methodName>", ccs->getCode());
   str->concatAndHTMLEncode(p0, xsink);
   if (*xsink)
      return 0;

   str->concat("</methodName>"); 

   const QoreListNode *l;
   if ((p1 = get_param(params, 1)) && (l = dynamic_cast<const QoreListNode *>(p1)) && l->size()) {
      str->concat("<params>"); 

      // now process all params
      int ls = l->size();
      for (int i = 0; i < ls; i++) {
	 const AbstractQoreNode *p;
	 if ((p = l->retrieve_entry(i))) {
	    str->concat("<param>");
	    addXMLRPCValue(*str, p, 0, ccs, 0, xsink);
	    if (*xsink)
	       return 0;

	    str->concat("</param>");
	 }
	 else
	    str->concat("<param/>");
      }
      str->concat("</params>"); 
   }
   else if (p1 && p1->getType() != NT_LIST) {
      str->concat("<params><param>"); 
      addXMLRPCValue(*str, p1, 0, ccs, 0, xsink);
      if (*xsink)
	 return 0;

      str->concat("</param></params>");       
   }
   else
      str->concat("<params/>");

   str->concat("</methodCall>");
   return str.release();
}

// makeXMLRPCCallStringArgs(string (function name), list of params)
static AbstractQoreNode *f_makeXMLRPCCallStringArgs(const QoreListNode *params, ExceptionSink *xsink)
{
   return makeXMLRPCCallStringArgs(QCS_DEFAULT, params, xsink);
}

static inline QoreStringNode *getXmlString(QoreXmlReader *reader, const QoreEncoding *id, ExceptionSink *xsink)
{
   if (id == QCS_UTF8)
      return new QoreStringNode(reader->constValue(), QCS_UTF8);

   return QoreStringNode::createAndConvertEncoding(reader->constValue(), QCS_UTF8, id, xsink);
}

// returns true if the key names are equal, ignoring any possible "^" suffix in k2
static bool keys_are_equal(const char *k1, const char *k2, bool &get_value)
{
   while (true) {
      if (!(*k1)) {
	 if (!(*k2))
	    return true;
	 if ((*k2) == '^') {
	    get_value = true;
	    return true;
	 }
	 return false;
      }
      if ((*k1) != (*k2))
	 break;
      k1++;
      k2++;
   }
   return false;
}

static int getXMLData(QoreXmlReader *reader, xml_stack *xstack, const QoreEncoding *data_ccsid, bool as_data, ExceptionSink *xsink) {
   QORE_TRACE("getXMLData()");
   int rc = 1;

   while (rc == 1) {
      int nt = reader->nodeType();
      // get node name
      const char *name = reader->constName();
      if (!name)
	 name = "--";

      if (nt == -1) // ERROR
	 break;

      if (nt == XML_READER_TYPE_ELEMENT) {
	 int depth = reader->depth();
	 xstack->checkDepth(depth);

	 AbstractQoreNode *n = xstack->getNode();
	 // if there is no node pointer, then make a hash
	 if (!n) {
	    QoreHashNode *h = new QoreHashNode();
	    xstack->setNode(h);
	    xstack->push(h->getKeyValuePtr(name), depth);
	 }
	 else { // node ptr already exists
	    QoreHashNode *h = dynamic_cast<QoreHashNode *>(n);
	    if (!h) {
	       h = new QoreHashNode();
	       xstack->setNode(h);
	       h->setKeyValue("^value^", n, 0);
	       xstack->incValueCount();
	       xstack->push(h->getKeyValuePtr(name), depth);
	    }
	    else {
	       // see if key already exists
	       AbstractQoreNode *v;
	       if (!(v = h->getKeyValue(name)))
		  xstack->push(h->getKeyValuePtr(name), depth);
	       else {
		  if (as_data) {
		     QoreListNode *vl = v->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>(v) : 0;
		     // if it's not a list, then make into a list with current value as first entry
		     if (!vl) {
			AbstractQoreNode **vp = h->getKeyValuePtr(name);
			vl = new QoreListNode();
			vl->push(v);
			(*vp) = vl;
		     }
		     xstack->push(vl->get_entry_ptr(vl->size()), depth);
		  }
		  else {
		     // see if last key was the same, if so make a list if it's not
		     const char *lk = h->getLastKey();
		     bool get_value = false;
		     if (keys_are_equal(name, lk, get_value)) {
			// get actual key value if there was a suffix 
			if (get_value)
			   v = h->getKeyValue(lk);
			
			QoreListNode *vl = dynamic_cast<QoreListNode *>(v);
			// if it's not a list, then make into a list with current value as first entry
			if (!vl) {
			   AbstractQoreNode **vp = h->getKeyValuePtr(lk);
			   vl = new QoreListNode();
			   vl->push(v);
			   (*vp) = vl;
			}
			xstack->push(vl->get_entry_ptr(vl->size()), depth);
		     }
		     else {
			QoreString ns;
			int c = 1;
			while (true) {
			   ns.sprintf("%s^%d", name, c);
			   AbstractQoreNode *et = h->getKeyValue(ns.getBuffer());
			   if (!et)
			      break;
			   c++;
			   ns.clear();
			}
			xstack->push(h->getKeyValuePtr(ns.getBuffer()), depth);
		     }
		  }
	       }
	    }
	 }
	 // add attributes to structure if possible
	 if (reader->hasAttributes()) {
	    ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);
	    while (reader->moveToNextAttribute() == 1) {
	       const char *name = reader->constName();
	       QoreStringNode *value = getXmlString(reader, data_ccsid, xsink);
	       if (!value)
		  return 0;
	       h->setKeyValue(name, value, xsink);
	    }

	    // make new new a hash and assign "^attributes^" key
	    QoreHashNode *nv = new QoreHashNode();
	    nv->setKeyValue("^attributes^", h.release(), xsink);
	    xstack->setNode(nv);
	 }
	 //printd(5, "%s: type=%d, hasValue=%d, empty=%d, depth=%d\n", name, nt, xmlTextReaderHasValue(reader), xmlTextReaderIsEmptyElement(reader), depth);
      }
      else if (nt == XML_READER_TYPE_TEXT) {
	 int depth = reader->depth();
	 xstack->checkDepth(depth);

	 const char *str = reader->constValue();
	 if (str) {
	    QoreStringNode *val = getXmlString(reader, data_ccsid, xsink);
	    if (!val)
	       return 0;

	    AbstractQoreNode *n = xstack->getNode();
	    if (n) {
	       QoreHashNode *h = dynamic_cast<QoreHashNode *>(n);
	       if (h) {
		  if (!xstack->getValueCount())
		     h->setKeyValue("^value^", val, xsink);
		  else {
		     QoreString kstr;
		     kstr.sprintf("^value%d^", xstack->getValueCount());
		     h->setKeyValue(kstr.getBuffer(), val, xsink);
		  }		  
	       }
	       else { // convert value to hash and save value node
		  h = new QoreHashNode();
		  xstack->setNode(h);
		  h->setKeyValue("^value^", n, 0);
		  xstack->incValueCount();

		  QoreString kstr;
		  kstr.sprintf("^value%d^", 1);
		  h->setKeyValue(kstr.getBuffer(), val, xsink);
	       }
	       xstack->incValueCount();
	    }
	    else
	       xstack->setNode(val);
	 }
      }
      else if (nt == XML_READER_TYPE_CDATA) {
	 int depth = reader->depth();
	 xstack->checkDepth(depth);

	 const char *str = reader->constValue();
	 if (str) {
	    QoreStringNode *val = getXmlString(reader, data_ccsid, xsink);
	    if (!val)
	       return 0;

	    AbstractQoreNode *n = xstack->getNode();
	    if (n && n->getType() == NT_HASH) {
	       QoreHashNode *h = reinterpret_cast<QoreHashNode *>(n);
	       if (!xstack->getCDataCount())
		  h->setKeyValue("^cdata^", val, xsink);
	       else {
		  QoreString kstr;
		  kstr.sprintf("^cdata%d^", xstack->getCDataCount());
		  h->setKeyValue(kstr.getBuffer(), val, xsink);
	       }		  
	    }
	    else { // convert value to hash and save value node
	       QoreHashNode *h = new QoreHashNode();
	       xstack->setNode(h);
	       if (n) {
		  h->setKeyValue("^value^", n, 0);
		  xstack->incValueCount();
	       }

	       h->setKeyValue("^cdata^", val, xsink);
	    }
	    xstack->incCDataCount();
	 }
      }
      rc = reader->read();
   }
   return rc;
}

static void getXMLRPCValueData(QoreXmlReader *reader, class XmlRpcValue *v, const QoreEncoding *data_ccsid, bool read_next, ExceptionSink *xsink);

static void getXMLRPCStruct(QoreXmlReader *reader, class XmlRpcValue *v, const QoreEncoding *data_ccsid, ExceptionSink *xsink) {
   int nt;

   QoreHashNode *h = new QoreHashNode();
   v->set(h);

   int member_depth = reader->depth();
   while (true) {
      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;

      if (nt == XML_READER_TYPE_END_ELEMENT)
	 break;

      if (nt != XML_READER_TYPE_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting 'member' element (got type %d)", nt);
	 return;
      }

      // check for 'member' element
      if (reader->checkXmlRpcMemberName("member", xsink))
	 return;

      // get member name
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->nodeType()) != XML_READER_TYPE_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting struct 'name'");
	 return;
      }

      // check for 'name' element
      if (reader->checkXmlRpcMemberName("name", xsink))
	 return;
   
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;

      if (nt != XML_READER_TYPE_TEXT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "missing member name in struct");
	 return;
      }

      const char *member_name = reader->constValue();
      if (!member_name) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "empty member name in struct");
	 return;
      }

      QoreString member(member_name);
      //printd(5, "DEBUG: got member name '%s'\n", member_name);
      
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_END_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting name close element");
	 return;
      }

      // get value
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting struct 'value' for key '%s'", member.getBuffer());
	 return;
      }

      if (reader->checkXmlRpcMemberName("value", xsink))
	 return;
   
      if (reader->readXmlRpc(xsink))
	 break;

      v->setPtr(h->getKeyValuePtr(member.getBuffer()));

      // if if was not an empty value element
      if (member_depth < reader->depth()) {
	 // check for close value tag
	 if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	    return;
	 if (nt != XML_READER_TYPE_END_ELEMENT) {
	    //printd(5, "struct member='%s', parsing value node\n", member.getBuffer());
	       
	    getXMLRPCValueData(reader, v, data_ccsid, true, xsink);
	    
	    if (xsink->isEvent())
	       return;

	    //printd(5, "struct member='%s', finished parsing value node\n", member.getBuffer());
	    
	    /*
	    if (reader->readXmlRpc(xsink))
	       return;
	    */
	    
	    if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	       return;
	    if (nt != XML_READER_TYPE_END_ELEMENT)
	    {
	       //printd(5, "EXCEPTION close /value: %d: %s\n", nt, (char *)reader->constName());
	       xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting value close element");
	       return;
	    }
	    //printd(5, "close /value: %s\n", (char *)reader->constName());
	 }
	 if (reader->readXmlRpc(xsink))
	    return;
      }
      
      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_END_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting member close element");
	 return;
      }
      //printd(5, "close /member: %s\n", (char *)reader->constName());

      if (reader->readXmlRpc(xsink))
	 return;
   }
}

static void getXMLRPCParams(QoreXmlReader *reader, class XmlRpcValue *v, const QoreEncoding *data_ccsid, ExceptionSink *xsink) {
   int nt;
   int index = 0;

   QoreListNode *l = new QoreListNode();
   v->set(l);

   int array_depth = reader->depth();

   while (true) {
      // expecting param open element
      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;

      //printd(5, "getXMLRPCParams() nt=%d name=%s\n", nt, reader->getName());
      
      // if higher-level "params" element closed, then return
      if (nt == XML_READER_TYPE_END_ELEMENT)
	 return;
      
      if (nt != XML_READER_TYPE_ELEMENT) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting 'param' open element");
	 return;
      }
      
      if (reader->checkXmlRpcMemberName("param", xsink))
	 return;
      
      // get next value tag or param close tag
      if (reader->readXmlRpc(xsink))
	 return;
      
      int value_depth = reader->depth();
      // if param was not an empty node
      if (value_depth > array_depth) {
	 if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	    return;
	 
	 // if we got a "value" element
	 if (nt == XML_READER_TYPE_ELEMENT) {
	    if (reader->checkXmlRpcMemberName("value", xsink))
	       return;
      
	    v->setPtr(l->get_entry_ptr(index++));
	    
	    if (reader->readXmlRpc(xsink))
	       return;

	    //printd(5, "just read <value>, now value_depth=%d, reader->depth=%d, nt=%d\n", value_depth, reader->depth(), reader->readXmlRpcNode(xsink));

	    // if this was <value/>, then skip
	    if (value_depth <= reader->depth()) {
	       if ((nt = reader->readXmlRpcNode(xsink)) == -1)
		  return;
	       
	       // if ! </value>
	       if (nt != XML_READER_TYPE_END_ELEMENT) {
		  getXMLRPCValueData(reader, v, data_ccsid, true, xsink);
		  
		  if (xsink->isEvent())
		     return;

		  /*
		  // read value close tag
		  if (reader->readXmlRpc(xsink))
		     return;
		  */
		  
		  if ((nt = reader->readXmlRpcNode(xsink)) == -1)
		     return;
		  
		  if (nt != XML_READER_TYPE_END_ELEMENT) {
		     xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting value close tag");
		     return;
		  }
	       }
	       // get param close tag
	       if (reader->readXmlRpc(xsink))
		  return;
	    }

	    if ((nt = reader->nodeType()) != XML_READER_TYPE_END_ELEMENT) {
	       xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting param close tag");
	       return;
	    }	    
	 }
	 else if (nt != XML_READER_TYPE_END_ELEMENT) {
	    xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting value element");
	    return;
	 }
	 // just read a param close tag, position reader at next element
	 if (reader->readXmlRpc(xsink))
	    return;
      }
   }
}

static void getXMLRPCString(QoreXmlReader *reader, class XmlRpcValue *v, const QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_END_ELEMENT) {
      // save an empty string
      v->set(null_string());
      return;
   }

   if (nt != XML_READER_TYPE_TEXT && nt != XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
      //printd(5, "getXMLRPCString() unexpected node type %d (expecting text %s)\n", nt, reader->constName());
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in string");
   }

   QoreStringNode *qstr = getXmlString(reader, data_ccsid, xsink);
   if (!qstr)
      return;

   //printd(5, "** got string '%s'\n", str);
   v->set(qstr);
   
   if (reader->readXmlRpc(xsink))
      return;
   
   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;
   
   if (nt != XML_READER_TYPE_END_ELEMENT) {
      printd(5, "getXMLRPCString() unexpected node type %d (expecting end element %s)\n", nt, reader->constName());
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in string (%d)", nt);
   }
}

static void getXMLRPCBoolean(QoreXmlReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT) {
      const char *str = reader->constValue();
      if (str) {
	 //printd(5, "** got boolean '%s'\n", str);
	 v->set(strtoll(str, 0, 10) ? boolean_true() : boolean_false());
      }

      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
   }
   else
      v->set(boolean_false());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in boolean (%d)", nt);
}

static void getXMLRPCInt(QoreXmlReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT) {
      const char *str = reader->constValue();
      if (str) {
	 //printd(5, "** got int '%s'\n", str);
	 // note that we can parse 64-bit integers here, which is not conformant to the standard
	 v->set(new QoreBigIntNode(strtoll(str, 0, 10)));
      }

      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
   }
   else
      v->set(new QoreBigIntNode());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in int (%d)", nt);
}

static void getXMLRPCDouble(QoreXmlReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT) {
      const char *str = reader->constValue();
      if (str) {
	 //printd(5, "** got float '%s'\n", str);
	 v->set(new QoreFloatNode(atof(str)));
      }

      // advance to next position
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
   }
   else
      v->set(zero_float());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in float (%d)", nt);
}

static void getXMLRPCDate(QoreXmlReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT) {
      const char *str = reader->constValue();
      if (str) {
	 // printd(5, "** got date '%s'\n", str);
	 QoreString qstr(str);
	 // ex: 20060414T12:48:14
	 // remove T
	 if (qstr.strlen() > 8) {
	    qstr.replace(8, 1, (char *)NULL);
	    // remove first :
	    if (qstr.strlen() > 10) {
	       qstr.replace(10, 1, (char *)NULL);
	       // remove second :
	       if (qstr.strlen() > 12)
		  qstr.replace(12, 1, (char *)NULL);
	    }
	 }
	 // pad with zeros in case incomplete date passed
	 while (qstr.strlen() < 14)
	    qstr.concat('0');

	 v->set(new DateTimeNode(qstr.getBuffer()));
      }

      // advance to next position
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
   }
   else
      v->set(zero_date());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in float (%d)", nt);
}

static void getXMLRPCBase64(QoreXmlReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = reader->readXmlRpcNode(xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT) {
      const char *str = reader->constValue();
      if (str) {
	 //printd(5, "** got base64 '%s'\n", str);
	 BinaryNode *b = parseBase64(str, strlen(str), xsink);
	 if (!b)
	    return;

	 v->set(b);
      }

      // advance to next position
      if (reader->readXmlRpc(xsink))
	 return;

      if ((nt = reader->readXmlRpcNode(xsink)) == -1)
	 return;
   }
   else
      v->set(new BinaryNode());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in base64 (%d)", nt);
}

static void doEmptyValue(class XmlRpcValue *v, const char *name, int depth, ExceptionSink *xsink)
{
   if (!strcmp(name, "string"))
      v->set(null_string());
   else if (!strcmp(name, "i4") || !strcmp(name, "int"))
      v->set(zero());
   else if (!strcmp(name, "boolean"))
      v->set(get_bool_node(false));
   else if (!strcmp(name, "struct"))
      v->set(new QoreHashNode());
   else if (!strcmp(name, "array"))
      v->set(new QoreListNode());
   else if (!strcmp(name, "double"))
      v->set(zero_float());
   else if (!strcmp(name, "dateTime.iso8601"))
      v->set(zero_date());
   else if (!strcmp(name, "base64"))
      v->set(new BinaryNode());
   else
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "unknown XML-RPC type '%s' at level %d", name, depth);
}

void QoreXmlReader::getXMLRPCArray(XmlRpcValue *v, const QoreEncoding *data_ccsid, ExceptionSink *xsink) {
   int nt;
   int index = 0;

   QoreListNode *l = new QoreListNode();
   v->set(l);

   int array_depth = depth();

   // expecting data open element
   if ((nt = readXmlRpcNode(xsink)) == -1)
      return;
      
   // if higher-level element closed, then return
   if (nt == XML_READER_TYPE_END_ELEMENT)
      return;
      
   if (nt != XML_READER_TYPE_ELEMENT) {
      xsink->raiseExceptionArg("XML-RPC-PARSE-VALUE-ERROR", new QoreStringNode(*xml), "error parsing XML string, expecting data open element");
      return;
   }
      
   if (checkXmlRpcMemberName("data", xsink))
      return;

   //printd(5, "getXMLRPCArray() before str=%s\n", (char *)constName());

   // get next value tag or data close tag
   if (readXmlRpc(xsink))
      return;

   int value_depth = depth();

   // if we just read an empty tag, then don't try to read to data close tag
   if (value_depth > array_depth) {
      while (true) {
	 if ((nt = readXmlRpcNode(xsink)) == -1)
	    return;
	 
	 if (nt == XML_READER_TYPE_END_ELEMENT)
	    break;
	 
	 // get "value" element
	 if (nt != XML_READER_TYPE_ELEMENT) {
	    xsink->raiseExceptionArg("XML-RPC-PARSE-VALUE-ERROR", new QoreStringNode(*xml), "extra data in array, expecting value element");
	    return;
	 }
	 
	 if (checkXmlRpcMemberName("value", xsink))
	    return;
	 
	 v->setPtr(l->get_entry_ptr(index++));
	 
	 if (readXmlRpc(xsink))
	    return;

	 //printd(5, "DEBUG: vd=%d, d=%d\n", value_depth, depth());

	 // if this was <value/>, then skip
	 if (value_depth <= depth()) {
	    if ((nt = readXmlRpcNode(xsink)) == -1)
	       return;

	    if (nt == XML_READER_TYPE_END_ELEMENT)
	       v->set(0);
	    else {
	       getXMLRPCValueData(this, v, data_ccsid, true, xsink);

	       if (xsink->isEvent())
		  return;

	       // check for </value> close tag
	       if ((nt = readXmlRpcNode(xsink)) == -1)
		  return;

	       if (nt != XML_READER_TYPE_END_ELEMENT) {
		  xsink->raiseExceptionArg("XML-RPC-PARSE-VALUE-ERROR", new QoreStringNode(*xml), "extra data in array, expecting value close tag");
		  return;
	       }
	    }
	    // read </data> close tag element
	    if (readXmlRpc("expecting data close tag", xsink))
	       return;
	 }
      }
      // read </array> close tag element
      if (readXmlRpc("error reading array close tag", xsink))
	 return;
   }
   else if (value_depth == array_depth && readXmlRpc(xsink))
      return;

   //printd(5, "vd=%d ad=%d\n", value_depth, array_depth);
   
   // check for array close tag
   if ((nt = nodeType()) != XML_READER_TYPE_END_ELEMENT) {
      if (nt == XML_READER_TYPE_ELEMENT)
	 xsink->raiseExceptionArg("XML-RPC-PARSE-ARRAY-ERROR", new QoreStringNode(*xml), "expecting array close tag, got element '%s' instead", constName());
      else
	 xsink->raiseExceptionArg("XML-RPC-PARSE-ARRAY-ERROR", new QoreStringNode(*xml), "extra data in array, expecting array close tag, got node type %d", nt);
   }
}

static void getXMLRPCValueData(QoreXmlReader *reader, XmlRpcValue *v, const QoreEncoding *data_ccsid, bool read_next, ExceptionSink *xsink) {
   int nt = reader->nodeType();
   if (nt == -1) {
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string");
      return;
   }

   if (nt == XML_READER_TYPE_ELEMENT) {
      int depth = reader->depth();
      
      // get xmlrpc type name
      const char *name = reader->constName();
      if (!name) {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "expecting type name, got NOTHING at level %d", depth);
	 return;
      }

      //printd(5, "DEBUG: getXMLRPCValueData() parsing type '%s'\n", name);

      int rc = reader->read();
      if (rc != 1) {
	 if (!read_next)
	    doEmptyValue(v, name, depth, xsink);
	 else
	    xsink->raiseException("XML-RPC-PARSE-ERROR", "error parsing XML string");
	 return;
      }

      // if this was an empty element, assign an empty value
      if (depth > reader->depth()) {
	 doEmptyValue(v, name, depth, xsink);
	 return;
      }

      if (!strcmp(name, "string"))
	 getXMLRPCString(reader, v, data_ccsid, xsink);
      else if (!strcmp(name, "i4") || !strcmp(name, "int"))
	 getXMLRPCInt(reader, v, xsink);
      else if (!strcmp(name, "boolean"))
	 getXMLRPCBoolean(reader, v, xsink);
      else if (!strcmp(name, "struct"))
	 getXMLRPCStruct(reader, v, data_ccsid, xsink);
      else if (!strcmp(name, "array"))
	 reader->getXMLRPCArray(v, data_ccsid, xsink);
      else if (!strcmp(name, "double"))
	 getXMLRPCDouble(reader, v, xsink);
      else if (!strcmp(name, "dateTime.iso8601"))
	 getXMLRPCDate(reader, v, xsink);
      else if (!strcmp(name, "base64"))
	 getXMLRPCBase64(reader, v, xsink);
      else {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "unknown XML-RPC type '%s' at level %d", name, depth);
	 return;
      }
      
      printd(5, "getXMLRPCValueData() finished parsing type '%s' element depth=%d\n", name, depth);
      if (xsink->isEvent())
	 return;
   }
   else if (nt == XML_READER_TYPE_TEXT) { // without type defaults to string
      QoreStringNode *qstr = getXmlString(reader, data_ccsid, xsink);
      if (qstr)
	 v->set(qstr);
   }

   if (read_next)
      reader->readXmlRpc(xsink);
}

// NOTE: the libxml2 library requires all input to be in UTF-8 encoding
// syntax: parseXML(xml string [, output encoding])
static AbstractQoreNode *parseXMLIntern(bool as_data, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)))
      return 0;

   printd(5, "parseXML(%s)\n", p0->getBuffer());

   const QoreEncoding *ccsid;
   if ((p1 = test_string_param(params, 1)))
      ccsid = QEM.findCreate(p1);
   else
      ccsid = QCS_DEFAULT;

   // convert to UTF-8 
   TempEncodingHelper str(p0, QCS_UTF8, xsink);
   if (!str)
      return 0;

   QoreXmlReader reader(*str, QORE_XML_READER_PARAMS, xsink);
   if (!reader)
      return 0;

   if (reader.read(xsink))
      return 0;

   xml_stack xstack;
   int rc = getXMLData(&reader, &xstack, ccsid, as_data, xsink);

   if (rc) {
      if (!*xsink)
	 xsink->raiseException("PARSE-XML-EXCEPTION", "parse error parsing XML string");
      return 0;
   }

   return xstack.getVal();
}

static AbstractQoreNode *f_parseXML(const QoreListNode *params, ExceptionSink *xsink) {
   return parseXMLIntern(false, params, xsink);
}

static AbstractQoreNode *f_parseXMLAsData(const QoreListNode *params, ExceptionSink *xsink) {
   return parseXMLIntern(true, params, xsink);
}

// makeXMLRPCFaultResponseString(param)
static AbstractQoreNode *f_makeXMLRPCFaultResponseString(const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("f_makeXMLRPCFaultResponseString()");

   const AbstractQoreNode *p0;
   const QoreStringNode *p1;
   p0 = get_param(params, 0);
   if (!(p1 = test_string_param(params, 1))) {
      xsink->raiseException("MAKE-XML-RPC-FAULT-RESPONSE-STRING-PARAMETER-ERROR", "expecting fault code, fault string as parameters to makeXMLRPCFaultResponseString()");
      return 0;
   }
   int code = p0 ? p0->getAsInt() : 0;
   const QoreEncoding *ccsid = p1->getEncoding();

   // for speed, the XML is created directly here
   QoreStringNode *str = new QoreStringNode(ccsid);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%d</int></value></member><member><name>faultString</name><value><string>",
		ccsid->getCode(), code);
   str->concatAndHTMLEncode(p1->getBuffer());
   str->concat("</string></value></member></struct></value></fault></methodResponse>");

   return str;
}

// makeXMLRPCFormattedFaultResponseString(param)
static AbstractQoreNode *f_makeFormattedXMLRPCFaultResponseString(const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("f_makeFormattedXMLRPCFaultResponseString()");

   const AbstractQoreNode *p0;
   const QoreStringNode *p1;
   p0 = get_param(params, 0);
   if (!(p1 = test_string_param(params, 1))) {
      xsink->raiseException("MAKE-XML-RPC-FAULT-RESPONSE-STRING-PARAMETER-ERROR", "expecting fault code, fault string as parameters to makeXMLRPCFaultResponseString()");
      return 0;
   }
   int code = p0 ? p0->getAsInt() : 0;
   const QoreEncoding *ccsid = p1->getEncoding();
   //printd(5, "ccsid=%016x (%s) (%s) code=%d\n", ccsid, ccsid->getCode(), ((QoreStringNode *)p1)->getBuffer(), code);

   // for speed, the XML is created directly here
   QoreStringNode *str = new QoreStringNode(ccsid);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodResponse>\n  <fault>\n    <value>\n      <struct>\n        <member>\n          <name>faultCode</name>\n          <value><int>%d</int></value>\n        </member>\n        <member>\n          <name>faultString</name>\n          <value><string>",
		ccsid->getCode(), code);
   str->concatAndHTMLEncode(p1->getBuffer());
   str->concat("</string></value>\n        </member>\n      </struct>\n    </value>\n  </fault>\n</methodResponse>");

   return str;
}

// makeXMLRPCResponseString(params, ...)
static AbstractQoreNode *f_makeXMLRPCResponseString(const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("f_makeXMLRPCResponseString()");

   const AbstractQoreNode *p;
   const QoreEncoding *ccs = QCS_DEFAULT;

   if (!num_params(params))
      return 0;

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodResponse><params>", ccs->getCode());

   // now loop through the params
   int ls = num_params(params);
   for (int i = 0; i < ls; i++)
      if ((p = get_param(params, i))) {
	 str->concat("<param>");
	 addXMLRPCValue(*str, p, 0, ccs, 0, xsink);
	 if (*xsink)
	    return 0;

	 str->concat("</param>");
      }
      else
	 str->concat("<param/>");

   str->concat("</params></methodResponse>");

   return str.release();
}

// makeXMLRPCResponseString(params, ...)
static AbstractQoreNode *f_makeXMLRPCValueString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p;
   const QoreEncoding *ccs = QCS_DEFAULT;

   QORE_TRACE("f_makeXMLRPCValueString()");
   if (!(p = get_param(params, 0)))
      return 0;

   QoreStringNode *str = new QoreStringNode(ccs);
   addXMLRPCValueIntern(str, p, 0, ccs, 0, xsink);

   return str;
}

// makeFormattedXMLRPCCallStringArgs(string (function name), params, ...)
static AbstractQoreNode *f_makeFormattedXMLRPCCallStringArgs(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;
   const QoreEncoding *ccs = QCS_DEFAULT;

   QORE_TRACE("f_makeFormattedXMLRPCCallStringArgs()");
   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
			    "expecting method name as first parameter");
      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodCall>\n  <methodName>", ccs->getCode());
   str->concatAndHTMLEncode(p0->getBuffer());
   str->concat("</methodName>\n  <params>\n");

   if ((p1 = get_param(params, 1))) {
      const QoreListNode *l = dynamic_cast<const QoreListNode *>(p1);
      if (l) {
	 // now process all params
	 int ls = l->size();
	 for (int i = 0; i < ls; i++) {
	    const AbstractQoreNode *p;
	    if ((p = l->retrieve_entry(i))) {
	       str->concat("    <param>\n");
	       addXMLRPCValue(*str, p, 6, ccs, 1, xsink);
	       if (*xsink)
		  return 0;

	       str->concat("    </param>\n");
	    }
	    else
	       str->concat("    <param/>\n");
	 }
      }
      else {
	 str->concat("    <param>\n");
	 addXMLRPCValue(*str, p1, 6, ccs, 1, xsink);
	 if (*xsink)
	    return 0;

	 str->concat("    </param>\n");
      }
   }

   str->concat("  </params>\n</methodCall>");
   return str.release();
}

// make_formatted_xml_rpc_call_string(string (function name), params, ...)
static AbstractQoreNode *f_makeFormattedXMLRPCCallString(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const QoreEncoding *ccs = QCS_DEFAULT;

   QORE_TRACE("f_makeFormattedXMLRPCCallString()");
   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
		     "expecting method name as first parameter");

      return 0;
   }

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodCall>\n  <methodName>", ccs->getCode());
   str->concatAndHTMLEncode(p0->getBuffer());
   str->concat("</methodName>\n  <params>\n");

   // now loop through the params
   int ls = num_params(params);
   for (int i = 1; i < ls; i++) {
      const AbstractQoreNode *p;
      if ((p = get_param(params, i))) 
      {
	 str->concat("    <param>\n");
	 addXMLRPCValue(*str, p, 6, ccs, 1, xsink);
	 if (*xsink)
	    return 0;

	 str->concat("    </param>\n");
      }
      else
	 str->concat("<param/>");
   }
   str->concat("  </params>\n</methodCall>");

   return str.release();
}

// makeFormattedXMLRPCResponseString(params, ...)
static AbstractQoreNode *f_makeFormattedXMLRPCResponseString(const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("f_makeFormattedXMLRPCResponseString()");

   const AbstractQoreNode *p;
   const QoreEncoding *ccs = QCS_DEFAULT;

   int ls = num_params(params);
   if (!ls)
      return 0;

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodResponse>\n  <params>\n", ccs->getCode());

   // now loop through the params
   for (int i = 0; i < ls; i++)
      if ((p = get_param(params, i))) 
      {
	 str->concat("    <param>\n");
	 addXMLRPCValue(*str, p, 6, ccs, 1, xsink);
	 if (*xsink)
	    return 0;

	 str->concat("    </param>\n");
      }
      else
	 str->concat("    <param/>\n");

   str->concat("  </params>\n</methodResponse>");
 

   return str.release();
}

// makeFormattedXMLRPCValueString(params, ...)
static AbstractQoreNode *f_makeFormattedXMLRPCValueString(const QoreListNode *params, ExceptionSink *xsink)
{
   QORE_TRACE("f_makeFormattedXMLRPCValueString()");

   const AbstractQoreNode *p;
   const QoreEncoding *ccs = QCS_DEFAULT;

   if (!(p = get_param(params, 0)))
      return 0;

   QoreStringNodeHolder str(new QoreStringNode(ccs));
   addXMLRPCValue(*str, p, 0, ccs, 1, xsink);
   if (*xsink)
      return 0;

   return str.release();
}

static AbstractQoreNode *f_parseXMLRPCValue(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   printd(5, "parseXMLRPCValue(%s)\n", p0->getBuffer());

   const QoreEncoding *ccsid;
   if ((p1 = test_string_param(params, 1)))
      ccsid = QEM.findCreate(p1->getBuffer());
   else
      ccsid = QCS_DEFAULT;

   TempEncodingHelper str(p0, QCS_UTF8, xsink);
   if (!str)
      return 0;

   QoreXmlReader reader(*str, QORE_XML_READER_PARAMS, xsink);
   if (!reader)
      return 0;

   if (reader.read(xsink))
      return 0;

   XmlRpcValue v;
   getXMLRPCValueData(&reader, &v, ccsid, false, xsink);
   if (xsink->isEvent())
      return 0;

   return v.getValue();
}

static inline AbstractQoreNode *qore_xml_exception(const char *ex, const char *info, ExceptionSink *xsink)
{
   if (!*xsink)
      xsink->raiseException(ex, "error parsing XML string: %s", info);
   return 0;
}

static inline AbstractQoreNode *qore_xml_exception(const char *ex, ExceptionSink *xsink)
{
   if (!*xsink)
      xsink->raiseException(ex, "error parsing XML string");
   return 0;
}

static inline QoreHashNode *qore_xml_hash_exception(const char *ex, const char *info, ExceptionSink *xsink, const QoreString *xml = 0) {
   if (!*xsink)
      xsink->raiseExceptionArg(ex, xml ? new QoreStringNode(*xml) : 0, "error parsing XML string: %s", info);
   return 0;
}

static inline QoreHashNode *qore_xml_hash_exception(const char *ex, ExceptionSink *xsink, const QoreString *xml = 0) {
   if (!*xsink)
      xsink->raiseExceptionArg(ex, xml ? new QoreStringNode(*xml) : 0, "error parsing XML string");
   return 0;
}

static AbstractQoreNode *f_parseXMLRPCCall(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   //printd(5, "parseXMLRPCCall() c=%d str=%s\n", p0->getBuffer()[0], p0->getBuffer());

   const QoreEncoding *ccsid;
   if ((p1 = test_string_param(params, 1)))
      ccsid = QEM.findCreate(p1);
   else
      ccsid = QCS_DEFAULT;

   TempEncodingHelper str(p0, QCS_UTF8, xsink);
   if (!str)
      return 0;

   QoreXmlReader reader(*str, QORE_XML_READER_PARAMS, xsink);
   if (!reader)
      return 0;

   if (reader.read(xsink))
      return 0;

   int nt;
   // get "methodCall" element
   if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodCall' element", xsink);

   if (reader.checkXmlRpcMemberName("methodCall", xsink))
      return 0;

   // get "methodName" element
   if (reader.readXmlRpc("expecting methodName element", xsink))
      return 0;

   if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodName' element", xsink);

   if (reader.checkXmlRpcMemberName("methodName", xsink))
      return 0;

   // get method name string
   if (reader.readXmlRpc("expecting method name", xsink))
      return 0;

   if ((nt = reader.nodeType()) != XML_READER_TYPE_TEXT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting method name", xsink);

   const char *method_name = reader.constValue();
   if (!method_name)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting method name", xsink);

   ReferenceHolder<QoreHashNode> h(new QoreHashNode(), xsink);
   h->setKeyValue("methodName", new QoreStringNode(method_name), 0);

   // get methodName close tag
   if (reader.readXmlRpc("expecting methodName close element", xsink))
      return 0;

   if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodName' close element", xsink);

   // get "params" element
   if (reader.readXmlRpc("expecting params element", xsink))
      return 0;

   if ((nt = reader.readXmlRpcNode(xsink)) == -1)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", xsink);

   // if the methodCall end element was not found
   if (nt != XML_READER_TYPE_END_ELEMENT) {
      if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'params' element", xsink);

      if (reader.checkXmlRpcMemberName("params", xsink))
	 return 0;

      // get 'param' element or close params
      if (reader.readXmlRpc("expecting param element", xsink))
	 return 0;
      
      if ((nt = reader.readXmlRpcNode(xsink)) == -1)
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", xsink);
      
      XmlRpcValue v;
      if (reader.depth()) {
	 if (nt != XML_READER_TYPE_END_ELEMENT) {
	    if (nt != XML_READER_TYPE_ELEMENT)
	       return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'params' element", xsink);
	    
	    getXMLRPCParams(&reader, &v, ccsid, xsink);

	    if (*xsink)
	       return 0;
	 }

	 // get methodCall close tag
	 if (reader.readXmlRpc("expecting methodCall close tag", xsink))
	    return 0;
      }

      if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT) {
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodCall' close element", xsink);
      }

      h->setKeyValue("params", v.getValue(), xsink);
   }

   return h.release();
}

QoreHashNode *parseXMLRPCResponse(const QoreString *msg, const QoreEncoding *ccsid, ExceptionSink *xsink) {
    //printd(5, "parseXMLRPCCall() %s\n", msg->getBuffer());

   TempEncodingHelper str(msg, QCS_UTF8, xsink);
   if (!str)
      return 0;

   QoreXmlReader reader(*str, QORE_XML_READER_PARAMS, xsink);
   if (!reader)
      return 0;

   if (reader.read(xsink))
      return 0;

   int nt;
   // get "methodResponse" element
   if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
       return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'methodResponse' element", xsink, *str);

   if (reader.checkXmlRpcMemberName("methodResponse", xsink))
      return 0;

   // check for params or fault element
   if (reader.readXmlRpc("expecting 'params' or 'fault' element", xsink))
      return 0;

   if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
       return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'params' or 'fault' element", xsink, *str);

   const char *name = reader.constName();
   if (!name) {
      xsink->raiseExceptionArg("PARSE-XML-RPC-RESPONSE-ERROR", new QoreStringNode(*str), "missing 'params' or 'fault' element tag");
      return 0;
   }

   XmlRpcValue v;
   bool fault = false;
   if (!strcmp(name, "params")) {
      int depth = reader.depth();

      // get "params" element
      if (reader.readXmlRpc("expecting 'params' element", xsink))
	 return 0;

      int params_depth = reader.depth();

      // if params was not an empty element
      if (depth < params_depth) {
	 if ((nt = reader.readXmlRpcNode(xsink)) == -1)
	    return 0;

	 if (nt != XML_READER_TYPE_END_ELEMENT) {
	    if (nt != XML_READER_TYPE_ELEMENT)
	       return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'param' element", xsink, *str);
	       
	    if (reader.checkXmlRpcMemberName("param", xsink))
	       return 0;
	       
	    // get "value" element
	    if (reader.readXmlRpc("expecting 'value' element", xsink))
	       return 0;

	    // if param was not an empty element
	    depth = reader.depth();
	    if (params_depth < depth) {
	       if ((nt = reader.readXmlRpcNode(xsink)) == -1)
		  return 0;
	    
	       if (nt != XML_READER_TYPE_END_ELEMENT) {
		  if (nt != XML_READER_TYPE_ELEMENT)
		     return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'value' element", xsink, *str);
	       
		  if (reader.checkXmlRpcMemberName("value", xsink))
		     return 0;
	       		  
		  // position at next element
		  if (reader.readXmlRpc("expecting XML-RPC value element", xsink))
		     return 0;
		  
		  // if value was not an empty element
		  if (depth < reader.depth()) {
		     getXMLRPCValueData(&reader, &v, ccsid, true, xsink);
		     
		     if (xsink->isEvent())
			return 0;
		  }
		  if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT)
		     return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'param' end element", xsink, *str);
	       }

	       // get "params" end element
	       if (reader.readXmlRpc("expecting 'params' end element", xsink))
		  return 0;
	    }
	    if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT)
	       return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'params' end element", xsink, *str);
	 }
	 // get "methodResponse" end element
	 if (reader.readXmlRpc("expecting 'methodResponse' end element", xsink))
	    return 0;
      }
   }
   else if (!strcmp(name, "fault")) {
      fault = true;

      // get "value" element
      if (reader.readXmlRpc("expecting 'value' element", xsink))
	 return 0;
      
      if ((nt = reader.nodeType()) != XML_READER_TYPE_ELEMENT)
	 return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting fault 'value' element", xsink, *str);

      if (reader.checkXmlRpcMemberName("value", xsink))
	 return 0;

      // position at next element
      if (reader.readXmlRpc("expecting XML-RPC value element", xsink))
	 return 0;
      
      // get fault structure
      getXMLRPCValueData(&reader, &v, ccsid, true, xsink);

      if (xsink->isEvent())
	 return 0;

      if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT)
	 return qore_xml_hash_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'fault' end element", xsink, *str);

      // get "methodResponse" end element
      if (reader.readXmlRpc("expecting 'methodResponse' end element", xsink))
	 return 0;
   }
   else {
      xsink->raiseException("PARSE-XML-RPC-RESPONSE-ERROR", "unexpected element '%s', expecting 'params' or 'fault'", name, *str);
      return 0;      
   }

   if ((nt = reader.nodeType()) != XML_READER_TYPE_END_ELEMENT)
      return qore_xml_hash_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodResponse' end element", xsink, *str);

   QoreHashNode *h = new QoreHashNode();
   if (fault)
      h->setKeyValue("fault", v.getValue(), 0);
   else
      h->setKeyValue("params", v.getValue(), 0);
   return h;
}

static AbstractQoreNode *f_parseXMLRPCResponse(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;
   if (!(p0 = test_string_param(params, 0)))
      return 0;

   printd(5, "parseXMLRPCCall(%s)\n", p0->getBuffer());

   const QoreEncoding *ccsid;
   if ((p1 = test_string_param(params, 1)))
      ccsid = QEM.findCreate(p1);
   else
      ccsid = QCS_DEFAULT;

   return parseXMLRPCResponse(p0, ccsid, xsink);
}

// NOTE: the libxml2 library requires all input to be in UTF-8 encoding
// syntax: parseXML(xml_string, xsd_string [, output encoding])
static AbstractQoreNode *parseXMLWithSchemaIntern(bool as_data, const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_XMLTEXTREADERSETSCHEMA
   const QoreStringNode *p0, *p1, *p2;

   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException(as_data ? "PARSE-XML-AS-DATA-WITH-SCHEMA-ERROR" : "PARSE-XML-WITH-SCHEMA-ERROR", "expecting XML string as first argument to parseXMLWithSchema()");
      return 0;
   }

   if (!(p1 = test_string_param(params, 1))) {
      xsink->raiseException(as_data ? "PARSE-XML-AS-DATA-WITH-SCHEMA-ERROR" : "PARSE-XML-WITH-SCHEMA-ERROR", "expecting XML schema string (xsd) as second argument to parseXMLWithSchema()");
      return 0;
   }

   printd(5, "parseXMLWithSchema() xml=%s\n xsd=%s\n", p0->getBuffer(), p1->getBuffer());

   const QoreEncoding *ccsid;
   if ((p2 = test_string_param(params, 2)))
      ccsid = QEM.findCreate(p1);
   else
      ccsid = QCS_DEFAULT;

   // convert to UTF-8 
   TempEncodingHelper str(p0, QCS_UTF8, xsink);
   if (!str)
      return 0;

   TempEncodingHelper xsd(p1, QCS_UTF8, xsink);
   if (!xsd)
      return 0;

   QoreXmlSchemaContext schema(xsd->getBuffer(), xsd->strlen(), xsink);
   if (!schema) {
      if (!*xsink)
	 xsink->raiseException("XML-SCHEMA-ERROR", "XML schema passed as second argument to parseXMLWithSchema() could not be parsed");
      return 0;
   }

   QoreXmlReader reader(*str, QORE_XML_READER_PARAMS, xsink);
   if (!reader)
      return 0;

   int rc = reader.setSchema(schema.getSchema());
   if (rc < 0) {
      if (!*xsink)
	 xsink->raiseException("XML-SCHEMA-ERROR", "XML schema passed as second argument to parseXMLWithSchema() could not be validated");      
      return 0;
   }

   if (reader.read(xsink))
      return 0;

   xml_stack xstack;
   rc = getXMLData(&reader, &xstack, ccsid, as_data, xsink);

   if (rc) {
      if (!*xsink)
	 xsink->raiseException("PARSE-XML-EXCEPTION", "parse error parsing XML string");
      return 0;
   }

   return xstack.getVal();
#else
   xsink->raiseException("MISSING-FEATURE-ERROR", "the libxml2 version used to compile the qore library did not support the xmlTextReaderSetSchema() function, therefore parseXMLWithSchema() is not available in Qore; for maximum portability, use the constant Option::HAVE_PARSEXMLWITHSCHEMA to check if this function is implemented before calling");
   return 0;
#endif
}

static AbstractQoreNode *f_parseXMLWithSchema(const QoreListNode *params, ExceptionSink *xsink) {
   return parseXMLWithSchemaIntern(false, params, xsink);
}

static AbstractQoreNode *f_parseXMLAsDataWithSchema(const QoreListNode *params, ExceptionSink *xsink) {
   return parseXMLWithSchemaIntern(true, params, xsink);
}

void init_xml_functions() {
   builtinFunctions.add("parseXML",                               f_parseXML);

   builtinFunctions.add("makeFormattedXMLString",                 f_makeFormattedXMLString);
   builtinFunctions.add("makeFormattedXMLFragment",               f_makeFormattedXMLFragment);

   builtinFunctions.add("makeXMLString",                          f_makeXMLString);
   builtinFunctions.add("makeXMLFragment",                        f_makeXMLFragment);

   builtinFunctions.add("makeXMLRPCCallString",                   f_makeXMLRPCCallString);
   builtinFunctions.add("makeXMLRPCCallStringArgs",               f_makeXMLRPCCallStringArgs);

   builtinFunctions.add("makeXMLRPCResponseString",               f_makeXMLRPCResponseString);
   builtinFunctions.add("makeXMLRPCFaultResponseString",          f_makeXMLRPCFaultResponseString);
   builtinFunctions.add("makeXMLRPCValueString",                  f_makeXMLRPCValueString);
   builtinFunctions.add("makeFormattedXMLRPCCallString",          f_makeFormattedXMLRPCCallString);
   builtinFunctions.add("makeFormattedXMLRPCCallStringArgs",      f_makeFormattedXMLRPCCallStringArgs);
   builtinFunctions.add("makeFormattedXMLRPCResponseString",      f_makeFormattedXMLRPCResponseString);
   builtinFunctions.add("makeFormattedXMLRPCFaultResponseString", f_makeFormattedXMLRPCFaultResponseString);
   builtinFunctions.add("makeFormattedXMLRPCValueString",         f_makeFormattedXMLRPCValueString);

   builtinFunctions.add("parseXMLRPCValue",                       f_parseXMLRPCValue);
   builtinFunctions.add("parseXMLRPCCall",                        f_parseXMLRPCCall);
   builtinFunctions.add("parseXMLRPCResponse",                    f_parseXMLRPCResponse);

   builtinFunctions.add("parseXMLWithSchema",                     f_parseXMLWithSchema);

   builtinFunctions.add("parseXMLAsData",                         f_parseXMLAsData);
   builtinFunctions.add("parseXMLAsDataWithSchema",               f_parseXMLAsDataWithSchema);
}
