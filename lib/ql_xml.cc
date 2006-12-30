/*
  lib/ql_xml.cc

  Qore XML functions

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/QoreNode.h>
#include <qore/Object.h>
#include <qore/support.h>
#include <qore/params.h>
#include <qore/charset.h>
#include <qore/BinaryObject.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/ql_xml.h>
#include <qore/ScopeGuard.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

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
      class QoreNode *val;
      class QoreNode **vp;

   public:
      inline XmlRpcValue()
      : val(0), vp(0)
      {
      }
      inline ~XmlRpcValue()
      {
	 if (val)
	 {
	    val->deref(NULL);
	    val = NULL;
	 }
      }
      inline class QoreNode *getValue()
      {
	 class QoreNode *rv = val;
	 val = NULL;
	 return rv;
      }
      inline void set(class QoreNode *v)
      {
	 if (vp)
	    *vp = v;
	 else
	    val = v;
      }
      inline void setPtr(class QoreNode **v)
      {
	 vp = v;
      }
};

// convenient class to hold a Hash pointer on the stack and delete it if not needed
class hashKeeper {
   private:
      class Hash *h;

   public:
      inline hashKeeper()
      : h(new Hash)
      {
      }
      inline ~hashKeeper()
      {
	 if (h)
	    h->derefAndDelete(NULL);
      }
      inline void setKeyValue(char *k, class QoreNode *v)
      {
         assert(h);
	 h->setKeyValue(k, v, NULL);
      }
      inline class Hash *getHash()
      {
	 class Hash *rv = h;
	 h = NULL;
	 return rv;
      }
};

class xml_node {
   public:
      QoreNode **node;
      xml_node *next;
      int depth;
      int vcount;

      xml_node(QoreNode **n, int d) 
      : node(n), next(0), depth(d), vcount(0)
      {
      }
};

class xml_stack {
   private:
      class xml_node *tail;
      class QoreNode *val;
      
   public:
      inline xml_stack();
      
      inline ~xml_stack()
      {
	 if (val)
	    val->deref(NULL);

	 while (tail)
	 {
	    //printd(5, "xml_stack::~xml_stack(): deleting=%08p (%d), next=%08p\n", tail, tail->depth, tail->next);
	    xml_node *n = tail->next;
	    delete tail;
	    tail = n;
	 }
      }
      inline void checkDepth(int depth)
      {
	 while (tail && depth && tail->depth >= depth)
	 {
	    //printd(5, "xml_stack::checkDepth(%d): deleting=%08p (%d), new tail=%08p\n", depth, tail, tail->depth, tail->next);
	    xml_node *n = tail->next;
	    delete tail;
	    tail = n;
	 }
      }
      inline void push(QoreNode **node, int depth)
      {
	 xml_node *sn = new xml_node(node, depth);
	 sn->next = tail;
	 tail = sn;
      }
      inline class QoreNode *getNode()
      {
	 return *tail->node;
      }
      inline void setNode(class QoreNode *n)
      {
	 (*tail->node) = n;
      }
      inline class QoreNode *getVal()
      {
	 class QoreNode *rv = val;
	 val = NULL;
	 return rv;
      }
      inline int getValueCount()
      {
	 return tail->vcount;
      }
      inline void incValueCount()
      {
	 tail->vcount++;
      }
};

inline xml_stack::xml_stack()
{
   tail = NULL;
   val = NULL;
   push(&val, -1);
}

} // anonymous namespace

static void makeXMLString(QoreString *str, Hash *h, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink);

static void concatSimpleValue(QoreString *str, QoreNode *n, class ExceptionSink *xsink)
{
   //printd(0, "concatSimpleValue() n=%08p (%s) %s\n", n, n->type->getName(), n->type == NT_STRING ? n->val.String->getBuffer() : "unknown");

   if (n->type == NT_STRING)
      str->concatAndHTMLEncode(n->val.String, xsink);
   else if (n->type == NT_INT)
      str->sprintf("%lld", n->val.intval);
   else if (n->type == NT_FLOAT)
      str->sprintf("%.9g", n->val.floatval);
   else if (n->type == NT_BOOLEAN)
      str->sprintf("%d", n->val.boolval);
   else if (n->type == NT_DATE)
      str->concat(n->val.date_time);
   else
   {	 
      QoreNode *nn = n->convert(NT_STRING);
      str->concatAndHTMLEncode(nn->val.String, xsink);
      nn->deref(NULL);
   }
}

static void addXMLElement(char *key, QoreString *str, QoreNode *n, int indent, char *node, class QoreEncoding *ccs, int format, ExceptionSink *xsink)
{
   //tracein("addXMLElement()");

   if (!n)
   {
      str->concat('<');
      str->concat(key);
      str->concat("/>");
      return;
   }

   if (n->type == NT_LIST)
   {
      // iterate through the list
      int ls = n->val.list->size();
      if (ls)
	 for (int j = 0; j < ls; j++)
	 {
	    QoreNode *v = n->val.list->retrieve_entry(j);
	    // indent all but first entry if necessary
	    if (j && format)
	    {
	       str->concat('\n');
               str->addch(' ', indent);
	    }

	    addXMLElement(key, str, v, indent, node, ccs, format, xsink);
	 }
      else    // close node
      {
	 str->concat('<');
	 str->concat(key);
	 str->concat("/>");
      }
      return;
   }
   else
   {
      // open node
      str->concat('<');
      str->concat(key);

      if (n->type == NT_HASH)
      {
	 // inc = ignore node counter, see if special keys exists and increment counter even if they have no value
	 int inc = 0;
	 int vn = 0;
	 QoreNode *value = n->val.hash->getKeyValueExistence("^value^");
	 if (value == (QoreNode *)-1)
	    value = NULL;
	 else
	 {
	    vn++;
	    if (is_nothing(value))
	       inc++;
	    // find all ^value*^ nodes
	    QoreString val;
	    while (true)
	    {
	       val.sprintf("^value%d^", vn);
	       value = n->val.hash->getKeyValueExistence(val.getBuffer());
	       if (value == (QoreNode *)-1)
	       {
		  value = NULL;
		  break;
	       }
	       else if (is_nothing(value)) // if the node exists but there is no value, then skip
		  inc++;
	       vn++;
	    }
	 }

	 QoreNode *attrib = n->val.hash->getKeyValueExistence("^attributes^");
	 if (attrib == (QoreNode *)-1)
	    attrib = NULL;
	 else
	    inc++;

	 // add attributes for objects
	 if (attrib && attrib->type == NT_HASH)
	 {
	    // add attributes to node
	    HashIterator hi(attrib->val.hash);
	    while (hi.next())
	    {
	       char *key = hi.getKey();
	       str->sprintf(" %s=\"", key);
	       class QoreNode *v = hi.getValue();
	       if (v)
		  if (v->type == NT_STRING)
		     str->concatAndHTMLEncode(v->val.String, xsink);
		  else // convert to string and add
		  {
		     QoreNode *t = v->convert(NT_STRING);
		     str->concat(t->val.String);
		     t->deref(xsink);
		  }
	       
	       str->concat('\"');
	    }
	 }

	 //printd(5, "inc=%d vn=%d\n", inc, vn);

	 // if there are no more elements, close node immediately
	 if (n->val.hash->size() == inc)
	 {
	    str->concat("/>");
	    return;
	 }

	 // close node
	 str->concat('>');

	 if (!is_nothing(value) && n->val.hash->size() == (inc + 1))
	    concatSimpleValue(str, value, xsink);
	 else // add additional elements and formatting only if the additional elements exist 
	 {
	    if (format && !vn)
	       str->concat('\n');

	    makeXMLString(str, n->val.hash, indent + 2, ccs, !vn ? format : 0, xsink);
	    // indent closing entry
	    if (format && !vn)
	    {
	       str->concat('\n');
               str->addch(' ', indent);
	    }
	 }
      }
      else
      {
	 // close node
	 str->concat('>');

	 if (n->type == NT_OBJECT)
	 {
	    // get snapshot of data
	    class Hash *h = n->val.object->evalData(xsink);
	    if (!xsink->isEvent())
	    {
	       if (format)
		  str->concat('\n');
	       makeXMLString(str, h, indent + 2, ccs, format, xsink);
	       // indent closing entry
	       if (format)
                  str->addch(' ', indent);
	    }
	    if (h)
	       h->dereference(xsink);
	 }
	 else 
	    concatSimpleValue(str, n, xsink);
      }
   }
   // close node
   str->concat("</");
   str->concat(key);
   str->concat('>');
   //traceout("addXMLElement()");
}

static void makeXMLString(QoreString *str, Hash *h, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink)
{
   tracein("makeXMLString()");
   HashIterator hi(h);
   bool done = false;
   while (hi.next())
   {
      std::auto_ptr<QoreString> keyStr(hi.getKeyString());
      // convert string if needed
      if (keyStr->getEncoding() != ccs)
      {
	 QoreString *ns = keyStr->convertEncoding(ccs, xsink);
	 if (xsink->isEvent())
	 {
	    break;
	 }
	 keyStr.reset(ns);
      }

      char *key = keyStr->getBuffer();
      if (!strcmp(key, "^attributes^"))
      {
	 continue;
      }

      if (!strncmp(key, "^value", 6))
      {
	 concatSimpleValue(str, hi.getValue(), xsink);
	 continue;
      }

      // make sure it's a valid XML tag element name
      if (!key || !isalpha(key[0]))
      {
	 xsink->raiseException("MAKE-XML-ERROR", "tag: \"%s\" is not a valid XML tag element name", key ? key : "");
	 break;
      }

      // process key name - remove ^# from end of key name if present
      int l = keyStr->strlen() - 1;
      while (isdigit(key[l]))
	 l--;

      if (l != (keyStr->strlen() - 1) && key[l] == '^')
	 keyStr->terminate(l);

      // indent entry
      if (format)
      {
	 if (done)
	    str->concat('\n');
         str->addch(' ', indent);
      }
      //printd(5, "makeXMLString() level %d adding member %s\n", indent / 2, node->getBuffer());
      addXMLElement(key, str, hi.getValue(), indent, key, ccs, format, xsink);
      done = true;
   }
   traceout("makeXMLString()");
}

// usage: makeXMLString(object (with only one top-level element) [, encoding])
// usage: makeXMLString(string (top-level-element), object [, encoding])
static class QoreNode *f_makeXMLString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pstr, *pobj, *pcs;
   int i;
   class QoreEncoding *ccs;

   tracein("f_makeXMLString()");
   if ((pobj = test_param(params, NT_HASH, 0)) && (pobj->val.hash->size() == 1))
   {
      pstr = NULL;
      i = 1;
   }
   else
   {
      if (!(pstr = test_param(params, NT_STRING, 0)) || !(pobj = test_param(params, NT_HASH, 1)))
      {
	 xsink->raiseException("MAKE-XML-STRING-PARAMETER-EXCEPTION",
			"expecting either hash with one member or string, hash as parameters");
	 return NULL;
      }
      i = 2;
   }
   if ((pcs = test_param(params, NT_STRING, i)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_UTF8;

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>", ccs->code);
   if (pstr)
   {
      str->concat('<');
      str->concat(pstr->val.String, xsink);
      str->concat('>');
      makeXMLString(str, pobj->val.hash, 0, ccs, 0, xsink);
      str->concat("</");
      str->concat(pstr->val.String, xsink);
      str->concat('>');
   }
   else
      makeXMLString(str, pobj->val.hash, 0, ccs, 0, xsink);
   //printd(0, "f_makeXMLString() returning %s\n", str->getBuffer());
   traceout("f_makeXMLString()");
   return new QoreNode(str);
}

// usage: makeFormattedXMLString(object (with only one top-level element) [, encoding])
// usage: makeFormattedXMLString(string (top-level-element), object [, encoding])
static class QoreNode *f_makeFormattedXMLString(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pstr, *pobj, *pcs;
   int i;

   tracein("f_makeFormattedXMLString()");
   if ((pobj = test_param(params, NT_HASH, 0)) && (pobj->val.hash->size() == 1))
   {
      pstr = NULL;
      i = 1;
   }
   else
   {
      if (!(pstr = test_param(params, NT_STRING, 0)) || !(pobj = test_param(params, NT_HASH, 1)))
      {
	 xsink->raiseException("MAKE-FORMATTED-XML-STRING-PARAMETER-EXCEPTION",
			"expecting either hash with one member or string, hash as parameters");
	 return NULL;
      }
      i = 2;
   }

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, i)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_UTF8;

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n", ccs->code);
   if (pstr)
   {
      str->concat('<');
      str->concat(pstr->val.String, xsink);
      str->concat(">\n");
      makeXMLString(str, pobj->val.hash, 2, ccs, 1, xsink);
      str->concat("</");
      str->concat(pstr->val.String, xsink);
      str->concat('>');
   }
   else
      makeXMLString(str, pobj->val.hash, 0, ccs, 1, xsink);
   traceout("f_makeFormattedXMLString()");
   return new QoreNode(str);
}

static class QoreNode *f_makeXMLFragment(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *pobj, *pcs;

   tracein("f_makeXMLFragment()");
   pobj = test_param(params, NT_HASH, 0);
   if (!pobj)
      return NULL;

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_UTF8;

   QoreNode *rv = new QoreNode(NT_STRING);
   rv->val.String = new QoreString();
   makeXMLString(rv->val.String, pobj->val.hash, 0, ccs, 0, xsink);
   traceout("f_makeXMLFragment()");
   return rv;
}

static class QoreNode *f_makeFormattedXMLFragment(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeFormattedXMLFragment()");

   QoreNode *pobj, *pcs;

   pobj = test_param(params, NT_HASH, 0);
   if (!pobj)
      return NULL;

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String->getBuffer());
   else
      ccs = QCS_UTF8;

   QoreNode *rv = new QoreNode(NT_STRING);
   rv->val.String = new QoreString();
   makeXMLString(rv->val.String, pobj->val.hash, 0, ccs, 1, xsink);
   traceout("f_makeFormattedXMLFragment()");
   return rv;
}

static void addXMLRPCValue(QoreString *str, QoreNode *n, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink);

static inline void addXMLRPCValueInternHash(QoreString *str, Hash *h, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink)
{
   str->concat("<struct>");
   if (format) str->concat('\n');
   HashIterator hi(h);
   while (hi.next())
   {
      std::auto_ptr<QoreString> member(hi.getKeyString());
      // convert string if needed
      if (member->getEncoding() != ccs)
      {
	 QoreString *ns = member->convertEncoding(ccs, xsink);
	 if (xsink->isEvent())
	 {
	    return;	    
	 }
	 //printd(0, "addXMLRPCValueInternHashInternal() converted %s->%s, \"%s\"->\"%s\"\n", member->getEncoding()->code, ccs->code, member->getBuffer(), ns->getBuffer());
	 member.reset(ns);
      }
      //else printd(0, "addXMLRPCValueInternHashInternal() not converting %sx \"%s\"\n", member->getEncoding()->code, member->getBuffer());
      // indent
      if (format)
         str->addch(' ', indent);
      str->concat("<member>");
      if (format)
      {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 6);
      }
      str->concat("<name>");
      str->concatAndHTMLEncode(member.get(), xsink);

      member.reset();

      str->concat("</name>");
      if (format) str->concat('\n');
      QoreNode *val = hi.getValue();
      addXMLRPCValue(str, val, indent + 6, ccs, format, xsink);
      // indent
      if (format)
         str->addch(' ', indent + 4);
      str->concat("</member>");
      if (format) str->concat('\n');
   }
   // indent
   if (format)
      str->addch(' ', indent + 2);
   str->concat("</struct>");
   //if (format) str->concat('\n');
}

static void addXMLRPCValueIntern(QoreString *str, QoreNode *n, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink)
{
   if (n->type == NT_BOOLEAN)
      str->sprintf("<boolean>%d</boolean>", n->val.boolval);

   else if (n->type == NT_INT)
      str->sprintf("<i4>%d</i4>", (int)n->val.intval);

   else if (n->type == NT_STRING)
   {
      str->concat("<string>");
      str->concatAndHTMLEncode(n->val.String, xsink);
      str->concat("</string>");
   }

   else if (n->type == NT_FLOAT)
      str->sprintf("<double>%f</double>", n->val.floatval);
	
   else if (n->type == NT_DATE)
   {
      str->concat("<dateTime.iso8601>");
      str->concatISO8601DateTime(n->val.date_time);
      str->concat("</dateTime.iso8601>");
   }

   else if (n->type == NT_BINARY)
   {
      str->concat("<base64>");
      if (format) 
      {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 4);
      }
      str->concatBase64(n->val.bin);
      if (format)
      {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent);
      }
      str->concat("</base64>");
   }

   else if (n->type == NT_HASH)
      addXMLRPCValueInternHash(str, n->val.hash, indent, ccs, format, xsink);

   else if (n->type == NT_LIST)
   {
      str->concat("<array>");
      if (format) 
      {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 4);
      }
      if (n->val.list->size())
      {
	 str->concat("<data>");
	 if (format) str->concat('\n');
	 for (int i = 0; i < n->val.list->size(); i++)
	    addXMLRPCValue(str, n->val.list->retrieve_entry(i), indent + 6, ccs, format, xsink);
	 if (format)
            str->addch(' ', indent + 4);
	 str->concat("</data>");
      }
      else
	 str->concat("<data/>");
      if (format) 
      {
	 str->concat('\n');
	 // indent
         str->addch(' ', indent + 2);
      }
      str->concat("</array>");
      //if (format) str->concat('\n');
   }

   if (format)
   {
      str->concat('\n');
      // indent
      str->addch(' ' , indent);
   }
}

static void addXMLRPCValue(QoreString *str, QoreNode *n, int indent, class QoreEncoding *ccs, int format, class ExceptionSink *xsink)
{
   tracein("addXMLRPCValue()");

   // add value node
   // indent
   if (format)
      str->addch(' ', indent);
   
   if (!is_nothing(n))
   {
      str->concat("<value>");
      if (format)
      {
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
   traceout("addXMLRPCValue()");
}

// makeXMLRPCCallString(string (function name), params, ...)
static class QoreNode *f_makeXMLRPCCallString(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   tracein("f_makeXMLRPCCallString()");
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
			    "expecting method name as first parameter");
      traceout("f_makeXMLRPCCallString()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodCall><methodName>", ccs->code);
   str->concatAndHTMLEncode(p0->val.String->getBuffer());
   str->concat("</methodName>");

   // now process all params
   int ls = num_params(params);
   if (ls)
   {
      str->concat("<params>"); 

      for (int i = 1; i < ls; i++)
      {
	 if ((p = get_param(params, i)))
	 {
	    str->concat("<param>");
	    addXMLRPCValue(str, p, 0, ccs, 0, xsink);
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
   traceout("f_makeXMLRPCCallString()");
   return new QoreNode(str);
}

// makeXMLRPCCallStringArgs(string (function name), list of params)
static class QoreNode *f_makeXMLRPCCallStringArgs(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   class QoreEncoding *ccs = QCS_DEFAULT;

   tracein("f_makeXMLRPCCallStringArgs()");
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-ARGS-PARAMETER-EXCEPTION",
		     "expecting method name as first parameter");
      traceout("f_makeXMLRPCCallStringArgs()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodCall><methodName>", ccs->code);
   str->concatAndHTMLEncode(p0->val.String->getBuffer());
   str->concat("</methodName>"); 

   if ((p1 = get_param(params, 1)) && (p1->type == NT_LIST) && p1->val.list->size())
   {
      str->concat("<params>"); 

      // now process all params
      int ls = p1->val.list->size();
      for (int i = 0; i < ls; i++)
      {
	 QoreNode *p;
	 if ((p = p1->val.list->retrieve_entry(i)))
	 {
	    str->concat("<param>");
	    addXMLRPCValue(str, p, 0, ccs, 0, xsink);
	    str->concat("</param>");
	 }
	 else
	    str->concat("<param/>");
      }
      str->concat("</params>"); 
   }
   else if (p1 && p1->type != NT_LIST)
   {
      str->concat("<params><param>"); 
      addXMLRPCValue(str, p1, 0, ccs, 0, xsink);
      str->concat("</param></params>");       
   }
   else
      str->concat("<params/>");

   str->concat("</methodCall>");
   traceout("f_makeXMLRPCCallStringArgs()");
   return new QoreNode(str);
}

static inline class QoreString *getXmlString(xmlTextReader *reader, class QoreEncoding *id, class ExceptionSink *xsink)
{
   class QoreString *rv;
   if (id == QCS_UTF8)
      rv = new QoreString((char *)xmlTextReaderConstValue(reader), QCS_UTF8);
   else
   {
      class QoreString temp((char *)xmlTextReaderConstValue(reader), QCS_UTF8);
      rv = temp.convertEncoding(id, xsink);
   }
   return rv;
}

static int getXMLData(xmlTextReader *reader, xml_stack *xstack, class QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   tracein("getXMLData()");
   int rc = 1;

   while (rc == 1) 
   {
      int nt = xmlTextReaderNodeType(reader);
      // get node name
      char *name = (char *)xmlTextReaderConstName(reader);
      if (!name)
	 name = "--";

      if (nt == -1) // ERROR
	 break;

      if (nt == XML_READER_TYPE_ELEMENT)
      {
	 int depth = xmlTextReaderDepth(reader);
	 xstack->checkDepth(depth);

	 class QoreNode *n = xstack->getNode();
	 // if there is no node pointer, then make a hash
	 if (!n)
	 {
	    class Hash *h = new Hash();
	    xstack->setNode(new QoreNode(h));
	    xstack->push(h->getKeyValuePtr(name), depth);
	 }
	 else // node ptr already exists
	 {
	    if (n->type != NT_HASH)
	    {
	       class Hash *h = new Hash();
	       xstack->setNode(new QoreNode(h));
	       h->setKeyValue("^value^", n, NULL);
	       xstack->incValueCount();
	       xstack->push(h->getKeyValuePtr(name), depth);
	    }
	    else
	    {
	       // see if key already exists
	       QoreNode *v;
	       if (!(v = n->val.hash->getKeyValue(name)))
		  xstack->push(n->val.hash->getKeyValuePtr(name), depth);
	       else
	       {
		  // see if last key was the same, if so make a list if it's not
		  char *lk = n->val.hash->getLastKey();
		  if (!strncmp(lk, name, strlen(name)))
		  {
		     // if it's not a list, then make into a list with current value as first entry
		     if (v->type != NT_LIST)
		     {
			QoreNode **vp = n->val.hash->getKeyValuePtr(lk);
			(*vp) = new QoreNode(NT_LIST);
			(*vp)->val.list = new List();
			(*vp)->val.list->push(v);
			xstack->push((*vp)->val.list->get_entry_ptr((*vp)->val.list->size()), depth);
		     }
		     else
			xstack->push(v->val.list->get_entry_ptr(v->val.list->size()), depth);
		  }
		  else
		  {
		     QoreString ns;
		     int c = 1;
		     while (true)
		     {
			ns.sprintf("%s^%d", name, c);
			class QoreNode *et = n->val.hash->getKeyValue(ns.getBuffer());
			if (!et)
			   break;
			c++;
			ns.terminate(0);
		     }
		     xstack->push(n->val.hash->getKeyValuePtr(ns.getBuffer()), depth);
		  }
	       }
	    }
	 }
	 // add attributes to structure if possible
	 if (xmlTextReaderHasAttributes(reader))
	 {
	    Hash *h = new Hash();
	    while (xmlTextReaderMoveToNextAttribute(reader) == 1)
	    {
	       char *name = (char *)xmlTextReaderConstName(reader);
	       class QoreString *value = getXmlString(reader, data_ccsid, xsink);
	       if (!value)
	       {
		  h->derefAndDelete(xsink);
		  return 0;
	       }
	       h->setKeyValue(name, new QoreNode(value), xsink);
	    }

	    // make new new a hash and assign "^attributes^" key
	    class Hash *nv = new Hash();
	    nv->setKeyValue("^attributes^", new QoreNode(h), xsink);
	    xstack->setNode(new QoreNode(nv));
	 }
	 //printd(5, "%s: type=%d, hasValue=%d, empty=%d, depth=%d\n", name, nt, xmlTextReaderHasValue(reader), xmlTextReaderIsEmptyElement(reader), depth);
      }
      else if (nt == XML_READER_TYPE_TEXT)
      {
	 int depth = xmlTextReaderDepth(reader);
	 xstack->checkDepth(depth);

	 char *str = (char *)xmlTextReaderConstValue(reader);
	 if (str)
	 {
	    class QoreString *qstr = getXmlString(reader, data_ccsid, xsink);
	    if (!qstr)
	       return 0;

	    // FIXME: this is wrong
	    class QoreNode *n = xstack->getNode();
	    if (n)
	    {
	       if (n->type == NT_HASH)
	       {
		  if (!xstack->getValueCount())
		     n->val.hash->setKeyValue("^value^", new QoreNode(qstr), xsink);
		  else
		  {
		     QoreString val;
		     val.sprintf("^value%d^", xstack->getValueCount());
		     n->val.hash->setKeyValue(val.getBuffer(), new QoreNode(qstr), xsink);
		  }		  
	       }
	       else // convert value to hash and save value node
	       {
		  class Hash *h = new Hash();
		  xstack->setNode(new QoreNode(h));
		  h->setKeyValue("^value^", n, NULL);
		  xstack->incValueCount();

		  QoreString val;
		  val.sprintf("^value%d^", 1);
		  h->setKeyValue(val.getBuffer(), new QoreNode(qstr), xsink);
	       }
	       xstack->incValueCount();
	    }
	    else
	       xstack->setNode(new QoreNode(qstr));
	 }
      }
      rc = xmlTextReaderRead(reader);
   }
   return rc;
}

static void getXMLRPCValueData(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, bool read_next, ExceptionSink *xsink);

static inline int qore_xmlRead(xmlTextReader *reader, class ExceptionSink *xsink)
{
   int rc = xmlTextReaderRead(reader);
   if (rc != 1)
   {
      xsink->raiseException("XML-RPC-PARSE-ERROR", "error parsing XML string");
      return -1;
   }
   return 0;
}

static inline int qore_xmlRead(xmlTextReader *reader, char *info, class ExceptionSink *xsink)
{
   int rc = xmlTextReaderRead(reader);
   if (rc != 1)
   {
      xsink->raiseException("XML-RPC-PARSE-ERROR", "error parsing XML string: %s", info);
      return -1;
   }
   return 0;
}

// ignores significant whitespace
static inline int qore_xmlTextReaderNodeType(xmlTextReader *reader)
{
   int nt;
   while (true)
   {
      nt = xmlTextReaderNodeType(reader);
      if (nt == XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
      {
	 // get next element
	 if (xmlTextReaderRead(reader) != 1)
	    return -1;
	 continue;
      }
      break;
   }
   return nt;
}

static inline int qore_xmlReadNode(xmlTextReader *reader, class ExceptionSink *xsink)
{
   int nt = qore_xmlTextReaderNodeType(reader);
   if (nt == -1)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string");
   return nt;
}

static inline int qore_xmlCheckName(xmlTextReader *reader, char *member, class ExceptionSink *xsink)
{
   char *name = (char *)xmlTextReaderConstName(reader);
   if (!name)
   {
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "expecting element '%s', got NOTHING", member);
      return -1;
   }
   
   if (strcmp(name, member))
   {
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "expecting element '%s', got '%s'", member, name);
      return -1;
   }
   return 0;
}

static void getXMLRPCStruct(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   int nt;

   class Hash *h = new Hash();
   v->set(new QoreNode(h));

   int member_depth = xmlTextReaderDepth(reader);
   while (true)
   {
      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;

      if (nt == XML_READER_TYPE_END_ELEMENT)
	 break;

      if (nt != XML_READER_TYPE_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting 'member' element (XXX %d)", nt);
	 return;
      }

      // check for 'member' element
      if (qore_xmlCheckName(reader, "member", xsink))
	 return;

      // get member name
      if (qore_xmlRead(reader, xsink))
	 return;

      char *member_name;
      if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting struct 'name'");
	 return;
      }

      // check for 'name' element
      if (qore_xmlCheckName(reader, "name", xsink))
	 return;
   
      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;

      if (nt != XML_READER_TYPE_TEXT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "missing member name in struct");
	 return;
      }

      member_name = (char *)xmlTextReaderConstValue(reader);
      if (!member_name)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "empty member name in struct");
	 return;
      }

      QoreString member(member_name);
      //printd(5, "got member name '%s'\n", member_name);

      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_END_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting name close element");
	 return;
      }

      // get value
      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting struct 'value' for key '%s'", member.getBuffer());
	 return;
      }

      if (qore_xmlCheckName(reader, "value", xsink))
	 return;
   
      if (qore_xmlRead(reader, xsink))
	 break;

      v->setPtr(h->getKeyValuePtr(member.getBuffer()));

      // if if was not an empty value element
      if (member_depth < xmlTextReaderDepth(reader))
      {
	 // check for close value tag
	 if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	    return;
	 if (nt != XML_READER_TYPE_END_ELEMENT)
	 {
	    //printd(5, "struct member='%s', parsing value node\n", member.getBuffer());
	       
	    getXMLRPCValueData(reader, v, data_ccsid, true, xsink);
	    
	    if (xsink->isEvent())
	       return;

	    //printd(5, "struct member='%s', finished parsing value node\n", member.getBuffer());
	    
	    /*
	    if (qore_xmlRead(reader, xsink))
	       return;
	    */
	    
	    if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	       return;
	    if (nt != XML_READER_TYPE_END_ELEMENT)
	    {
	       //printd(5, "EXCEPTION close /value: %d: %s\n", nt, (char *)xmlTextReaderConstName(reader));
	       xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting value close element");
	       return;
	    }
	    //printd(5, "close /value: %s\n", (char *)xmlTextReaderConstName(reader));
	 }
	 if (qore_xmlRead(reader, xsink))
	    return;
      }
      
      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
      if (nt != XML_READER_TYPE_END_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting member close element");
	 return;
      }
      //printd(5, "close /member: %s\n", (char *)xmlTextReaderConstName(reader));

      if (qore_xmlRead(reader, xsink))
	 return;
   }
}

static void getXMLRPCArray(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   int nt;
   int index = 0;

   class List *l = new List();
   v->set(new QoreNode(l));

   int array_depth = xmlTextReaderDepth(reader);

   // expecting data open element
   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;
      
   // if higher-level element closed, then return
   if (nt == XML_READER_TYPE_END_ELEMENT)
      return;
      
   if (nt != XML_READER_TYPE_ELEMENT)
   {
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting data open element");
      return;
   }
      
   if (qore_xmlCheckName(reader, "data", xsink))
      return;

   //printd(5, "getXMLRPCArray() before str=%s\n", (char *)xmlTextReaderConstName(reader));

   // get next value tag or data close tag
   if (qore_xmlRead(reader, xsink))
      return;

   int value_depth = xmlTextReaderDepth(reader);

   // if we just read an empty tag, then don't try to read to data close tag
   if (value_depth > array_depth)
   {
      while (true)
      { 
	 if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	    return;
	 
	 if (nt == XML_READER_TYPE_END_ELEMENT)
	    break;
	 
	 // get "value" element
	 if (nt != XML_READER_TYPE_ELEMENT)
	 {
	    xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in array, expecting value element");
	    return;
	 }
	 
	 if (qore_xmlCheckName(reader, "value", xsink))
	    return;
	 
	 v->setPtr(l->get_entry_ptr(index++));
	 
	 if (qore_xmlRead(reader, xsink))
	    return;
	 
	 // if this was <value/>, then skip
	 if (value_depth < xmlTextReaderDepth(reader))
	 {
	    if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	       return;

	    if (nt != XML_READER_TYPE_END_ELEMENT)
	    {
	       getXMLRPCValueData(reader, v, data_ccsid, true, xsink);

	       if (xsink->isEvent())
		  return;
	       
	       // check for </value> close tag
	       if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
		  return;

	       if (nt != XML_READER_TYPE_END_ELEMENT)
	       {
		  xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in array, expecting value close tag");
		  return;
	       }
	    }
	    // read </data> close tag element
	    if (qore_xmlRead(reader, "expecting data close tag", xsink))
	       return;
	 }
      }
      // read </array> close tag element
      if (qore_xmlRead(reader, "error reading array close tag", xsink))
	 return;
   }
   else if (value_depth == array_depth && qore_xmlRead(reader, xsink))
      return;

   // check for array close tag
   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-ARRAY-ERROR", "extra data in array, expecting array close tag");
}

static void getXMLRPCParams(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   int nt;
   int index = 0;

   class List *l = new List();
   v->set(new QoreNode(l));

   int array_depth = xmlTextReaderDepth(reader);

   while (true)
   {
      // expecting param open element
      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
      
      // if higher-level "params" element closed, then return
      if (nt == XML_READER_TYPE_END_ELEMENT)
	 return;
      
      if (nt != XML_READER_TYPE_ELEMENT)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string, expecting 'param' open element");
	 return;
      }
      
      if (qore_xmlCheckName(reader, "param", xsink))
	 return;
      
      // get next value tag or param close tag
      if (qore_xmlRead(reader, xsink))
	 return;
      
      int value_depth = xmlTextReaderDepth(reader);
      // if param was not an empty node
      if (value_depth > array_depth)
      {
	 if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	    return;
	 
	 // if we got a "value" element
	 if (nt == XML_READER_TYPE_ELEMENT)
	 {
	    if (qore_xmlCheckName(reader, "value", xsink))
	       return;
      
	    v->setPtr(l->get_entry_ptr(index++));
	    
	    if (qore_xmlRead(reader, xsink))
	       return;

	    // if this was <value/>, then skip
	    if (value_depth < xmlTextReaderDepth(reader))
	    {
	       if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
		  return;
	       
	       // if ! </value>
	       if (nt != XML_READER_TYPE_END_ELEMENT)
	       {
		  getXMLRPCValueData(reader, v, data_ccsid, true, xsink);
		  
		  if (xsink->isEvent())
		     return;

		  /*
		  // read value close tag
		  if (qore_xmlRead(reader, xsink))
		     return;
		  */
		  
		  if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
		     return;
		  
		  if (nt != XML_READER_TYPE_END_ELEMENT)
		  {
		     xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting value close tag");
		     return;
		  }
	       }
	       // get param close tag
	       if (qore_xmlRead(reader, xsink))
		  return;
	    }

	    if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
	    {
	       xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting param close tag");
	       return;
	    }	    
	 }
	 else if (nt != XML_READER_TYPE_END_ELEMENT)
	 {
	    xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra data in params, expecting value element");
	    return;
	 }
	 // just read a param close tag, position reader at next element
	 if (qore_xmlRead(reader, xsink))
	    return;
      }
   }
}

static void getXMLRPCString(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_END_ELEMENT)
   {
      // save an empty string
      v->set(null_string());
      return;
   }

   if (nt != XML_READER_TYPE_TEXT && nt != XML_READER_TYPE_SIGNIFICANT_WHITESPACE)
   {
      //printd(5, "getXMLRPCString() unexpected node type %d (expecting text %s)\n", nt, xmlTextReaderConstName(reader));
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in string");
   }

   class QoreString *qstr = getXmlString(reader, data_ccsid, xsink);
   if (!qstr)
      return;

   //printd(5, "** got string '%s'\n", str);
   v->set(new QoreNode(qstr));
   
   if (qore_xmlRead(reader, xsink))
      return;
   
   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
   {
      printd(5, "getXMLRPCString() unexpected node type %d (expecting end element %s)\n", nt, xmlTextReaderConstName(reader));
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in string (%d)", nt);
   }
}

static void getXMLRPCBoolean(xmlTextReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT)
   {
      char *str = (char *)xmlTextReaderConstValue(reader);
      if (str)
      {
	 //printd(5, "** got boolean '%s'\n", str);
	 v->set(strtoll(str, NULL, 10) ? boolean_true() : boolean_false());
      }

      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
   }
   else
      v->set(boolean_false());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in boolean (%d)", nt);
}

static void getXMLRPCInt(xmlTextReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT)
   {
      char *str = (char *)xmlTextReaderConstValue(reader);
      if (str)
      {
	 //printd(5, "** got int '%s'\n", str);
	 // note that we can parse 64-bit integers here, which is not conformant to the standard
	 v->set(new QoreNode((int64)strtoll(str, NULL, 10)));
      }

      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
   }
   else
      v->set(zero());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in int (%d)", nt);
}

static void getXMLRPCDouble(xmlTextReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT)
   {
      char *str = (char *)xmlTextReaderConstValue(reader);
      if (str)
      {
	 //printd(5, "** got float '%s'\n", str);
	 v->set(new QoreNode(atof(str)));
      }

      // advance to next position
      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
   }
   else
      v->set(zero_float());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in float (%d)", nt);
}

static void getXMLRPCDate(xmlTextReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT)
   {
      char *str = (char *)xmlTextReaderConstValue(reader);
      if (str)
      {
	 // printd(5, "** got date '%s'\n", str);
	 QoreString qstr(str);
	 // ex: 20060414T12:48:14
	 // remove T
	 if (qstr.strlen() > 8) 
	 {
	    qstr.replace(8, 1, (char *)NULL);
	    // remove first :
	    if (qstr.strlen() > 10)
	    {
	       qstr.replace(10, 1, (char *)NULL);
	       // remove second :
	       if (qstr.strlen() > 12)
		  qstr.replace(12, 1, (char *)NULL);
	    }
	 }
	 // pad with zeros in case incomplete date passed
	 while (qstr.strlen() < 14)
	    qstr.concat('0');

	 v->set(new QoreNode(new DateTime(qstr.getBuffer())));
      }

      // advance to next position
      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
   }
   else
      v->set(zero_date());
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in float (%d)", nt);
}

static void getXMLRPCBase64(xmlTextReader *reader, class XmlRpcValue *v, ExceptionSink *xsink)
{
   int nt;

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return;

   if (nt == XML_READER_TYPE_TEXT)
   {
      char *str = (char *)xmlTextReaderConstValue(reader);
      if (str)
      {
	 //printd(5, "** got base64 '%s'\n", str);
	 class BinaryObject *b = parseBase64(str, strlen(str), xsink);
	 if (!b)
	    return;

	 v->set(new QoreNode(b));
      }

      // advance to next position
      if (qore_xmlRead(reader, xsink))
	 return;

      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return;
   }
   else
      v->set(new QoreNode(new BinaryObject()));
   
   if (nt != XML_READER_TYPE_END_ELEMENT)
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "extra information in base64 (%d)", nt);
}

static void doEmptyValue(class XmlRpcValue *v, char *name, int depth, class ExceptionSink *xsink)
{
   if (!strcmp(name, "string"))
      v->set(null_string());
   else if (!strcmp(name, "i4") || !strcmp(name, "int"))
      v->set(zero());
   else if (!strcmp(name, "boolean"))
      v->set(boolean_false());
   else if (!strcmp(name, "struct"))
      v->set(new QoreNode(new Hash()));
   else if (!strcmp(name, "array"))
      v->set(new QoreNode(new List()));
   else if (!strcmp(name, "double"))
      v->set(zero_float());
   else if (!strcmp(name, "dateTime.iso8601"))
      v->set(zero_date());
   else if (!strcmp(name, "base64"))
      v->set(new QoreNode(new BinaryObject()));
   else
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "unknown XML-RPC type '%s' at level %d", name, depth);
}

static void getXMLRPCValueData(xmlTextReader *reader, class XmlRpcValue *v, class QoreEncoding *data_ccsid, bool read_next, ExceptionSink *xsink)
{
   int nt = qore_xmlTextReaderNodeType(reader);
   if (nt == -1)
   {
      xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "error parsing XML string");
      return;
   }

   if (nt == XML_READER_TYPE_ELEMENT)
   {
      int depth = xmlTextReaderDepth(reader);
      
      // get xmlrpc type name
      char *name = (char *)xmlTextReaderConstName(reader);
      if (!name)
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "expecting type name, got NOTHING at level %d", depth);
	 return;
      }

      int rc = xmlTextReaderRead(reader);
      if (rc != 1)
      {
	 if (!read_next)
	    doEmptyValue(v, name, depth, xsink);
	 else
	    xsink->raiseException("XML-RPC-PARSE-ERROR", "error parsing XML string");
	 return;
      }

      // if this was an empty element, assign an empty value
      if (depth > xmlTextReaderDepth(reader))
      {
	 doEmptyValue(v, name, depth, xsink);
	 return;
      }

      //printd(5, "getXMLRPCValueData() parsing type '%s'\n", name);

      if (!strcmp(name, "string"))
	 getXMLRPCString(reader, v, data_ccsid, xsink);
      else if (!strcmp(name, "i4") || !strcmp(name, "int"))
	 getXMLRPCInt(reader, v, xsink);
      else if (!strcmp(name, "boolean"))
	 getXMLRPCBoolean(reader, v, xsink);
      else if (!strcmp(name, "struct"))
	 getXMLRPCStruct(reader, v, data_ccsid, xsink);
      else if (!strcmp(name, "array"))
	 getXMLRPCArray(reader, v, data_ccsid, xsink);
      else if (!strcmp(name, "double"))
	 getXMLRPCDouble(reader, v, xsink);
      else if (!strcmp(name, "dateTime.iso8601"))
	 getXMLRPCDate(reader, v, xsink);
      else if (!strcmp(name, "base64"))
	 getXMLRPCBase64(reader, v, xsink);
      else
      {
	 xsink->raiseException("XML-RPC-PARSE-VALUE-ERROR", "unknown XML-RPC type '%s' at level %d", name, depth);
	 return;
      }
      
      printd(5, "getXMLRPCValueData() finished parsing type '%s' element depth=%d\n", name, depth);
      if (xsink->isEvent())
	 return;
   }
   else if (nt == XML_READER_TYPE_TEXT)  // without type defaults to string
   {
      class QoreString *qstr = getXmlString(reader, data_ccsid, xsink);
      if (qstr)
	 v->set(new QoreNode(qstr));
   }

   if (read_next)
      qore_xmlRead(reader, xsink);
}

static void makeXMLStringNew(xmlTextWriterPtr writer, Hash *h, class ExceptionSink *xsink);

static void addXMLElementNew(xmlTextWriterPtr writer, QoreNode *n, xmlChar *key, ExceptionSink *xsink)
{
   //tracein("addXMLElementNew()");
   QoreNode *attrib;
   // add attributes for objects
   if (n->type == NT_HASH && (attrib = n->val.hash->getKeyValue("^attributes^")) && attrib->type == NT_HASH)
   {
      // add attributes to node
      HashIterator hi(attrib->val.hash);
      while (hi.next())
      {
	 class QoreNode *v = hi.getValue();
	 if (v)
	 {
	    std::auto_ptr<QoreString> akey(hi.getKeyString());
	    if (akey->getEncoding() != QCS_UTF8)
	    {
	       QoreString *t = akey->convertEncoding(QCS_UTF8, xsink);
	       if (xsink->isEvent())
		  return;
	       akey.reset(t);
	    }

	    QoreNode *t;
	    if (v->type == NT_STRING)
	       t = v;
	    else // convert to string and add
	       t = v->convert(NT_STRING);
	    
	    QoreString *val;
	    if (t->val.String->getEncoding() != QCS_UTF8)
	    {
	       val = t->val.String->convertEncoding(QCS_UTF8, xsink);
	       if (xsink->isEvent())
	       {
		  if (t != v)
		     t->deref(xsink);
		  return;
	       }
	    }
	    else
	       val = t->val.String;

	    if (xmlTextWriterWriteAttribute(writer, (xmlChar *)akey->getBuffer(), (xmlChar *)val->getBuffer()) < 0)
	    {
	       if (val != t->val.String)
		  delete val;
	       if (t != v)
		  t->deref(xsink);
	       
	       xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteAttribute() returned an error");
	       return;
            }
	    if (val != t->val.String)
	       delete val;
	    if (t != v)
	       t->deref(xsink);
	 }
      }
   }

   if (n->type == NT_HASH)
   {
      makeXMLStringNew(writer, n->val.hash, xsink);
      return;
   }

   if (n->type == NT_LIST)
   {
      bool output = false;
      // iterate through the list
      for (int j = 0; j < n->val.list->size(); j++)
      {
	 QoreNode *v = n->val.list->retrieve_entry(j);
	 if (v)
	 {
	    if (j && output)
	    {
	       if (xmlTextWriterEndElement(writer) < 0)
	       {
		  xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
		  return;
	       }		  
	       if (xmlTextWriterStartElement(writer, key) < 0)
	       {
		  xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
		  return;
	       }		  
	    }
	    addXMLElementNew(writer, v, key, xsink);
	    output = true;
	 }
      }
      return;
   }

   if (n->type == NT_STRING)
   {
      QoreString *t = n->val.String;
      if (t->getEncoding() != QCS_UTF8)
      {
	 t = t->convertEncoding(QCS_UTF8, xsink);
	 if (xsink->isEvent())
	    return;
      }
      xmlTextWriterWriteString(writer, (xmlChar *)t->getBuffer());
      if (t != n->val.String)
	 delete t;

      return;
   }

   if (n->type == NT_INT)
   {
      if (xmlTextWriterWriteFormatString(writer, "%lld", n->val.intval) < 0)
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteFormatString() returned an error");
      return;
   }

   if (n->type == NT_FLOAT)
   {
      if (xmlTextWriterWriteFormatString(writer, "%.9g", n->val.floatval) < 0)
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteFormatString() returned an error");	 
      return;
   }

   if (n->type == NT_BOOLEAN)
   {
      if (xmlTextWriterWriteFormatString(writer, "%d", n->val.boolval) < 0)
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteFormatString() returned an error");	 
      return;
   }

   QoreNode *nn = n->convert(NT_STRING);
   xmlTextWriterWriteString(writer, (xmlChar *)nn->val.String->getBuffer());
   nn->deref(NULL);
   //traceout("addXMLElementNew()");
}

static void makeXMLStringNew(xmlTextWriterPtr writer, Hash *h, class ExceptionSink *xsink)
{
   tracein("makeXMLStringNew()");
   HashIterator hi(h);
   while (hi.next())
   {
      std::auto_ptr<QoreString> keyStr(hi.getKeyString());
      // convert string if needed
      if (keyStr->getEncoding() != QCS_UTF8)
      {
	 QoreString *ns = keyStr->convertEncoding(QCS_UTF8, xsink);
	 if (xsink->isEvent())
	 {
	    break;
	 }
	 keyStr.reset(ns);
      }

      char *key = keyStr->getBuffer();
      if (!strcmp(key, "^attributes^") || !strncmp(key, "^value", 6))
      {
	 continue;
      }

      // make sure it's a valid XML tag element name
      // FIXME: This is far too restrictive - does not follow the XML spec
      if (!key || !isalpha(key[0]))
      {
	 xsink->raiseException("MAKE-XML-ERROR", "tag: \"%s\" is not a valid XML tag element name", key ? key : "");
	 break;
      }

      QoreNode *value = hi.getValue();
      if (xmlTextWriterStartElement(writer, (xmlChar *)key) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 break;
      }

      if (value)
	 addXMLElementNew(writer, value, (xmlChar *)key, xsink);

      if (xmlTextWriterFullEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterFullEndElement() returned an error");
	 break;
      }
   }
   traceout("makeXMLStringNew()");
}

// NOTE: the libxml2 library will convert all characters to UTF-8, assuming that the
// input encoding is correctly specified
// syntax: parseXML(xml string [, output encoding])
static class QoreNode *f_parseXML(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   tracein("f_parseXML()");
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      traceout("f_parseXML()");
      return NULL;
   }

   printd(5, "parseXML(%s)\n", p0->val.String->getBuffer());

   class QoreEncoding *ccsid;
   if ((p1 = test_param(params, NT_STRING, 1)))
      ccsid = QEM.findCreate(p1->val.String);
   else
      ccsid = QCS_DEFAULT;

   xmlTextReader *reader = xmlReaderForDoc((xmlChar *)p0->val.String->getBuffer(), NULL, NULL, QORE_XML_READER_PARAMS);
   if (!reader)
   {
      traceout("f_parseXML()");
      return NULL;
   }
   ON_BLOCK_EXIT(xmlFreeTextReader, reader);

   int rc = xmlTextReaderRead(reader);
   if (rc != 1) 
   {
      xsink->raiseException("PARSE-XML-EXCEPTION", "cannot parse XML string");
      return NULL;
   }
   xml_stack xstack;
   rc = getXMLData(reader, &xstack, ccsid, xsink);

   if (rc) 
   {
      xsink->raiseException("PARSE-XML-EXCEPTION", "parse error parsing XML string");
      return NULL;
   }

   traceout("f_parseXML()");
   return xstack.getVal();
}

class QoreString *makeXMLQoreString(QoreNode *pstr, QoreNode *pobj, int format, class QoreEncoding *ccsid, bool fragment, class ExceptionSink *xsink)
{
   xmlBufferPtr buf = xmlBufferCreate();
   if (!buf)
   {
      xsink->raiseException("OUT-OF-MEMORY", "cannot create XML buffer");
      return NULL;
   }
   ON_BLOCK_EXIT(xmlBufferFree, buf);
   
   xmlTextWriterPtr writer = xmlNewTextWriterMemory(buf, 0);
   if (!writer) 
   {
      xsink->raiseException("OUT-OF-MEMORY", "cannot create XML buffer");
      return NULL;
   }
   ON_BLOCK_EXIT(xmlFreeTextWriter, writer);

   if (xmlTextWriterSetIndent(writer, format) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterSetIndent() returned an error");
      return NULL;
   }

   //if (format)
   //   xmlTextWriterSetIndentString(writer, (xmlChar *)"  ");

   if (!fragment && (xmlTextWriterStartDocument(writer, NULL, ccsid->code, NULL) < 0))
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartDocument() returned an error");
      return NULL;
   }
   
   if (pstr)
   {
      if (xmlTextWriterStartElement(writer, (xmlChar *)pstr->val.String->getBuffer()) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return NULL;
      }
      makeXMLStringNew(writer, pobj->val.hash, xsink);

      if (xmlTextWriterEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return NULL;
      }
   }
   else
      makeXMLStringNew(writer, pobj->val.hash, xsink);

   if (xmlTextWriterEndDocument(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndDocument() returned an error");
      return NULL;
   }

   std::auto_ptr<QoreString> str(new QoreString());
   str->take((char *)buf->content, ccsid);

   buf->content = NULL;
   buf->size = 0;
   buf->use = 0;
   // xmlBufferFree(buf); - the scope guard should handle this idiom correctly

   if (fragment && ccsid != QCS_UTF8)
   {
      QoreString *t = str->convertEncoding(ccsid, xsink);
      if (xsink->isEvent())
	 return NULL;
      str.reset(t);     // here was a bug: t = str
   }

   return str.release();
}

// usage: makeXMLStringNew(object (with only one top-level element) [, encoding])
// usage: makeXMLString(string (top-level-element), object [, encoding])
static class QoreNode *f_makeXMLStringNew(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *pstr, *pobj, *pcs;
   int i;

   tracein("f_makeXMLString()");
   if ((pobj = test_param(params, NT_HASH, 0)) && (pobj->val.hash->size() == 1))
   {
      pstr = NULL;
      i = 1;
   }
   else
   {
      if (!(pstr = test_param(params, NT_STRING, 0)) || !(pobj = test_param(params, NT_HASH, 1)))
      {
	 xsink->raiseException("MAKE-XML-STRING-PARAMETER-EXCEPTION",
			"expecting either hash with one member or string, hash as parameters");
	 return NULL;
      }
      i = 2;
   }

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, i)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   //xmlChar *tmp;
   
   QoreString *str = makeXMLQoreString(pstr, pobj, 0, ccs, false, xsink);

   traceout("f_makeXMLString()");
   return new QoreNode(str);
}

// usage: makeFormattedXMLStringNew(object (with only one top-level element) [, encoding])
// usage: makeFormattedXMLStringNew(string (top-level-element), object [, encoding])
static class QoreNode *f_makeFormattedXMLStringNew(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pstr, *pobj, *pcs;
   int i;

   tracein("f_makeFormattedXMLStringNew()");
   if ((pobj = test_param(params, NT_HASH, 0)) && (pobj->val.hash->size() == 1))
   {
      pstr = NULL;
      i = 1;
   }
   else
   {
      if (!(pstr = test_param(params, NT_STRING, 0)) || !(pobj = test_param(params, NT_HASH, 1)))
      {
	 xsink->raiseException("MAKE-XML-STRING-PARAMETER-EXCEPTION",
			"expecting either hash with one member or string, hash as parameters");
	 return NULL;
      }
      i = 2;
   }
   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, i)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   //xmlChar *tmp;
   
   QoreString *str = makeXMLQoreString(pstr, pobj, 1, ccs, false, xsink);

   traceout("f_makeFormattedXMLStringNew()");
   return new QoreNode(str);
}

static void addXMLRPCValueNew(xmlTextWriterPtr writer, QoreNode *n, class ExceptionSink *xsink);

static inline void addXMLRPCValueInternHashNew(xmlTextWriterPtr writer, Hash *h, class ExceptionSink *xsink)
{
   if (xmlTextWriterStartElement(writer, (xmlChar *)"struct") < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return;
   }

   HashIterator hi(h);
   while (hi.next())
   {
      std::auto_ptr<QoreString> member(hi.getKeyString());
      // convert string if needed
      if (member->getEncoding() != QCS_UTF8)
      {
	 QoreString *ns = member->convertEncoding(QCS_UTF8, xsink);
	 if (xsink->isEvent())
	 {
	    return;	    
	 }
	 //printd(0, "addXMLRPCValueInternHash() converted %s->%s, \"%s\"->\"%s\"\n", member->getEncoding()->code, ccs->code, member->getBuffer(), ns->getBuffer());
	 member.reset(ns);
      }
      //else printd(0, "addXMLRPCValueInternHash() not converting %s \"%s\"\n", member->getEncoding()->code, member->getBuffer());
      if (xmlTextWriterStartElement(writer, (xmlChar *)"member") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return;
      }
      if (xmlTextWriterStartElement(writer, (xmlChar *)"name") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return;
      }
      if (xmlTextWriterWriteString(writer, (xmlChar *)member->getBuffer()) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteString() returned an error");
	 return;
      }

      // close "name"
      if (xmlTextWriterEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
	 return;
      }
      QoreNode *val = hi.getValue();
      addXMLRPCValueNew(writer, val, xsink);

      // close "member"
      if (xmlTextWriterEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
	 return;
      }
   }
   // close "struct"
   if (xmlTextWriterEndElement(writer) < 0)
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
}

static void addXMLRPCValueHelper(xmlTextWriterPtr writer, char *type, char *val, class ExceptionSink *xsink)
{
   if (xmlTextWriterStartElement(writer, (xmlChar *)type) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return;
   }
   if (xmlTextWriterWriteString(writer, (xmlChar *)val) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteString() returned an error");
      return;
   }
   if (xmlTextWriterEndElement(writer) < 0)
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
}

static void addXMLRPCValueInternNew(xmlTextWriterPtr writer, QoreNode *n, class ExceptionSink *xsink)
{
   if (n->type == NT_STRING)
   {
      QoreString *t;
      if (n->val.String->getEncoding() != QCS_UTF8)
      {
	 t = n->val.String->convertEncoding(QCS_UTF8, xsink);
	 if (xsink->isEvent())
	    return;
      }
      else
	 t = n->val.String;
      
      addXMLRPCValueHelper(writer, "string", t->getBuffer(), xsink);
      
      if (t != n->val.String)
	 delete t;
      
      return;
   }

   if (n->type == NT_BINARY)
   {
      if (xmlTextWriterStartElement(writer, (xmlChar *)"base64") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return;
      }
      if (xmlTextWriterWriteBase64(writer, (const char *)n->val.bin->getPtr(), 0, n->val.bin->size()) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteBase64() returned an error");
	 return;
      }
      if (xmlTextWriterEndElement(writer) < 0)
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
      return;
   }

   if (n->type == NT_HASH)
   {
      addXMLRPCValueInternHashNew(writer, n->val.hash, xsink);
      return;
   }

   if (n->type == NT_LIST)
   {
      if (xmlTextWriterStartElement(writer, (xmlChar *)"array") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return;
      }
      if (xmlTextWriterStartElement(writer, (xmlChar *)"data") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return;
      }
      for (int i = 0, cnt = n->val.list->size(); i != cnt; ++i)
	 addXMLRPCValueNew(writer, n->val.list->retrieve_entry(i), xsink);
      
      // close "data"
      if (xmlTextWriterEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
	 return;
      }
      
      // close "array"
      if (xmlTextWriterEndElement(writer) < 0)
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
      
      return;
   }

   if (n->type == NT_BOOLEAN)
   {
      QoreString str(n->val.boolval);
      addXMLRPCValueHelper(writer, "boolean", str.getBuffer(), xsink);
      return;
   }

   if (n->type == NT_INT)
   {
      QoreString str(n->val.intval);
      addXMLRPCValueHelper(writer, "i4", str.getBuffer(), xsink);
      return;
   }

   if (n->type == NT_FLOAT)
   {
      QoreString str(n->val.floatval);
      addXMLRPCValueHelper(writer, "double", str.getBuffer(), xsink);
      return;
   }

   if (n->type == NT_DATE)
   {
      QoreString str;
      str.concatISO8601DateTime(n->val.date_time);
      addXMLRPCValueHelper(writer, "dateTime.iso8601", str.getBuffer(), xsink);
      return;
   }
}

static void addXMLRPCValueNew(xmlTextWriterPtr writer, QoreNode *n, class ExceptionSink *xsink)
{
   tracein("addXMLRPCValue()");

   // add value node
   if (xmlTextWriterStartElement(writer, (xmlChar *)"value") < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return;
   }

   // add empty value node for no vlaue
   if (!is_nothing(n))
      addXMLRPCValueInternNew(writer, n, xsink);

   if (xmlTextWriterFullEndElement(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterFullEndElement() returned an error");
      return;
   }
   traceout("addXMLRPCValue()");
}

// makeXMLRPCCallString(string (function name), params, ...)
static class QoreNode *f_makeXMLRPCCallStringNew(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
		     "expecting method name as first parameter");
      return NULL;
   }
   class QoreEncoding *ccs = QCS_DEFAULT;

   xmlBufferPtr buf = xmlBufferCreate();
   if (!buf)
   {
      xsink->raiseException("OUT-OF-MEMORY", "cannot create XML buffer");
      return NULL;
   }
   ON_BLOCK_EXIT(xmlBufferFree, buf);
   
   xmlTextWriterPtr writer = xmlNewTextWriterMemory(buf, 0);
   if (!writer) 
   {
      xsink->raiseException("OUT-OF-MEMORY", "cannot create XML buffer");
      return NULL;
   }
   ON_BLOCK_EXIT(xmlFreeTextWriter, writer);

   if (xmlTextWriterSetIndent(writer, 0) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterSetIndent() returned an error");
      return NULL;
   }
   
   if (xmlTextWriterStartDocument(writer, NULL, ccs->code, NULL) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartDocument() returned an error");
      return NULL;
   }
   
   if (xmlTextWriterStartElement(writer, (xmlChar *)"methodCall") < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return NULL;
   }

   if (xmlTextWriterStartElement(writer, (xmlChar *)"methodName") < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return NULL;
   }

   class QoreString *t;
   if (p0->val.String->getEncoding() != QCS_UTF8)
   {
      t = p0->val.String->convertEncoding(QCS_UTF8, xsink);
      if (xsink->isEvent())
      {
	 return NULL;
      }
   }
   else
      t = p0->val.String;

   if (xmlTextWriterWriteString(writer, (xmlChar *)t->getBuffer()) < 0)
   {
      if (t != p0->val.String)
	 delete t;
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterWriteString() returned an error");
      return NULL;
   }
   if (t != p0->val.String)
      delete t;

   // close methodName
   if (xmlTextWriterEndElement(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
      return NULL;
   }

   // add params
   if (xmlTextWriterStartElement(writer, (xmlChar *)"params") < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
      return NULL;
   }

   int ls = num_params(params);
   for (int i = 0; i < ls; i++)
   {
      // add param
      if (xmlTextWriterStartElement(writer, (xmlChar *)"param") < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterStartElement() returned an error");
	 return NULL;
      }
      if ((p = get_param(params, i)))
	 addXMLRPCValueNew(writer, p, xsink);

      // close param
      if (xmlTextWriterEndElement(writer) < 0)
      {
	 xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
	 return NULL;
      }
   }

   // close params
   if (xmlTextWriterEndElement(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
      return NULL;
   }

   // close methodCall
   if (xmlTextWriterEndElement(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndElement() returned an error");
      return NULL;
   }

   if (xmlTextWriterEndDocument(writer) < 0)
   {
      xsink->raiseException("XML-WRITER-ERROR", "xmlTextWriterEndDocument() returned an error");
      return NULL;
   }

   std::auto_ptr<QoreString> str(new QoreString());
   str->take((char *)buf->content, ccs);

   buf->content = NULL;
   buf->size = 0;
   buf->use = 0;
   // xmlBufferFree(buf); - scope guard should handle this idiom correctly

   return new QoreNode(str.release());
}

// makeXMLRPCFaultResponseString(param)
static class QoreNode *f_makeXMLRPCFaultResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeXMLRPCFaultResponseString()");

   QoreNode *p0, *p1;
   p0 = get_param(params, 0);
   if (!(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("MAKE-XML-RPC-FAULT-RESPONSE-STRING-PARAMETER-ERROR", "expecting fault code, fault string as parameters to makeXMLRPCFaultResponseString()");
      return NULL;
   }
   int code = p0 ? p0->getAsInt() : 0;
   class QoreEncoding *ccsid = p1->val.String->getEncoding();

   // for speed, the XML is created directly here
   QoreString *str = new QoreString(ccsid);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodResponse><fault><value><struct><member><name>faultCode</name><value><int>%d</int></value></member><member><name>faultString</name><value><string>",
		ccsid->code, code);
   str->concatAndHTMLEncode(p1->val.String->getBuffer());
   str->concat("</string></value></member></struct></value></fault></methodResponse>");
   traceout("f_makeXMLRPCFaultResponseString()");
   return new QoreNode(str);
}

// makeXMLRPCFormattedFaultResponseString(param)
static class QoreNode *f_makeFormattedXMLRPCFaultResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeFormattedXMLRPCFaultResponseString()");

   QoreNode *p0, *p1;
   p0 = get_param(params, 0);
   if (!(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("MAKE-XML-RPC-FAULT-RESPONSE-STRING-PARAMETER-ERROR", "expecting fault code, fault string as parameters to makeXMLRPCFaultResponseString()");
      return NULL;
   }
   int code = p0 ? p0->getAsInt() : 0;
   class QoreEncoding *ccsid = p1->val.String->getEncoding();
   //printd(0, "ccsid=%016x (%s) (%s) code=%d\n", ccsid, ccsid->code, p1->val.String->getBuffer(), code);

   // for speed, the XML is created directly here
   QoreString *str = new QoreString(ccsid);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodResponse>\n  <fault>\n    <value>\n      <struct>\n        <member>\n          <name>faultCode</name>\n          <value><int>%d</int></value>\n        </member>\n        <member>\n          <name>faultString</name>\n          <value><string>",
		ccsid->code, code);
   str->concatAndHTMLEncode(p1->val.String->getBuffer());
   str->concat("</string></value>\n        </member>\n      </struct>\n    </value>\n  </fault>\n</methodResponse>");
   traceout("f_makeFormattedXMLRPCFaultResponseString()");
   return new QoreNode(str);
}

// makeXMLRPCResponseString(params, ...)
static class QoreNode *f_makeXMLRPCResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeXMLRPCResponseString()");

   QoreNode *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   if (!num_params(params))
   {
      traceout("f_makeXMLRPCResponseString()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?><methodResponse><params>", ccs->code);

   // now loop through the params
   int ls = num_params(params);
   for (int i = 0; i < ls; i++)
      if ((p = get_param(params, i))) 
      {
	 str->concat("<param>");
	 addXMLRPCValue(str, p, 0, ccs, 0, xsink);
	 str->concat("</param>");
      }
      else
	 str->concat("<param/>");

   str->concat("</params></methodResponse>");
   traceout("f_makeXMLRPCResponseString()");
   return new QoreNode(str);
}

// makeXMLRPCResponseString(params, ...)
static class QoreNode *f_makeXMLRPCValueString(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   tracein("f_makeXMLRPCValueString()");
   if (!(p = get_param(params, 0)))
   {
      traceout("f_makeXMLRPCValueString()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   addXMLRPCValueIntern(str, p, 0, ccs, 0, xsink);

   traceout("f_makeXMLRPCValueString()");
   return new QoreNode(str);
}

// makeFormattedXMLRPCCallStringArgs(string (function name), params, ...)
static class QoreNode *f_makeFormattedXMLRPCCallStringArgs(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   class QoreEncoding *ccs = QCS_DEFAULT;

   tracein("f_makeFormattedXMLRPCCallStringArgs()");
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
		     "expecting method name as first parameter");
      traceout("f_makeFormattedXMLRPCCallStringArgs()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodCall>\n  <methodName>", ccs->code);
   str->concatAndHTMLEncode(p0->val.String->getBuffer());
   str->concat("</methodName>\n  <params>\n");

   if ((p1 = get_param(params, 1)))
   {
      if (p1->type == NT_LIST)
      {
	 // now process all params
	 int ls = p1->val.list->size();
	 for (int i = 0; i < ls; i++)
	 {
	    QoreNode *p;
	    if ((p = p1->val.list->retrieve_entry(i)))
	    {
	       str->concat("    <param>\n");
	       addXMLRPCValue(str, p, 6, ccs, 1, xsink);
	       str->concat("    </param>\n");
	    }
	    else
	       str->concat("    <param/>\n");
	 }
      }
      else
      {
	 str->concat("    <param>\n");
	 addXMLRPCValue(str, p1, 6, ccs, 1, xsink);
	 str->concat("    </param>\n");
      }
   }

   str->concat("  </params>\n</methodCall>");
   traceout("f_makeFormattedXMLRPCCallStringArgs()");
   return new QoreNode(str);
}

// make_formatted_xml_rpc_call_string(string (function name), params, ...)
static class QoreNode *f_makeFormattedXMLRPCCallString(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   tracein("f_makeFormattedXMLRPCCallString()");
   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("MAKE-XML-RPC-CALL-STRING-PARAMETER-EXCEPTION",
		     "expecting method name as first parameter");
      traceout("f_makeFormattedXMLRPCCallString()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   str->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodCall>\n  <methodName>", ccs->code);
   str->concatAndHTMLEncode(p0->val.String->getBuffer());
   str->concat("</methodName>\n  <params>\n");

   // now loop through the params
   int ls = num_params(params);
   for (int i = 1; i < ls; i++)
      if ((p = get_param(params, i))) 
      {
	 str->concat("    <param>\n");
	 addXMLRPCValue(str, p, 6, ccs, 1, xsink);
	 str->concat("    </param>\n");
      }
      else
	 str->concat("<param/>");

   str->concat("  </params>\n</methodCall>");
   traceout("f_makeFormattedXMLRPCCallString()");
   return new QoreNode(str);
}

// makeFormattedXMLRPCResponseString(params, ...)
static class QoreNode *f_makeFormattedXMLRPCResponseString(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeFormattedXMLRPCResponseString()");

   QoreNode *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   int ls = num_params(params);
   if (!ls)
   {
      traceout("f_makeXMLRPCResponseString()");
      return NULL;
   }

   QoreNode *rv = new QoreNode(NT_STRING);
   rv->val.String = new QoreString(ccs);
   rv->val.String->sprintf("<?xml version=\"1.0\" encoding=\"%s\"?>\n<methodResponse>\n  <params>\n", ccs->code);

   // now loop through the params
   for (int i = 0; i < ls; i++)
      if ((p = get_param(params, i))) 
      {
	 rv->val.String->concat("    <param>\n");
	 addXMLRPCValue(rv->val.String, p, 6, ccs, 1, xsink);
	 rv->val.String->concat("    </param>\n");
      }
      else
	 rv->val.String->concat("    <param/>\n");

   rv->val.String->concat("  </params>\n</methodResponse>");
   traceout("f_makeFormattedXMLRPCResponseString()");
   return rv;
}

// makeFormattedXMLRPCValueString(params, ...)
static class QoreNode *f_makeFormattedXMLRPCValueString(class QoreNode *params, ExceptionSink *xsink)
{
   tracein("f_makeFormattedXMLRPCValueString()");

   QoreNode *p;
   class QoreEncoding *ccs = QCS_DEFAULT;

   if (!(p = get_param(params, 0)))
   {
      traceout("f_makeFormattedXMLRPCValueString()");
      return NULL;
   }

   QoreString *str = new QoreString(ccs);
   addXMLRPCValue(str, p, 0, ccs, 1, xsink);

   traceout("f_makeFormattedXMLRPCValueString()");
   return new QoreNode(str);
}

static class QoreNode *f_makeXMLFragmentNew(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *pobj, *pcs;

   tracein("f_makeXMLFragmentNew()");
   pobj = test_param(params, NT_HASH, 0);
   if (!pobj)
      return NULL;

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   class QoreString *str = makeXMLQoreString(NULL, pobj, 0, ccs, true, xsink);
   traceout("f_makeXMLFragmentNew()");
   return new QoreNode(str);
}

static class QoreNode *f_makeFormattedXMLFragmentNew(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *pobj, *pcs;

   tracein("f_makeFormattedXMLFragmentNew()");
   pobj = test_param(params, NT_HASH, 0);
   if (!pobj)
      return NULL;

   class QoreEncoding *ccs;
   if ((pcs = test_param(params, NT_STRING, 1)))
      ccs = QEM.findCreate(pcs->val.String);
   else
      ccs = QCS_DEFAULT;

   class QoreString *str = makeXMLQoreString(NULL, pobj, 1, ccs, true, xsink);
   traceout("f_makeFormattedXMLFragmentNew()");
   return new QoreNode(str);
}

static class QoreNode *f_parseXMLRPCValue(class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   printd(5, "parseXMLRPCValue(%s)\n", p0->val.String->getBuffer());

   class QoreEncoding *ccsid;
   if ((p1 = test_param(params, NT_STRING, 1)))
      ccsid = QEM.findCreate(p1->val.String->getBuffer());
   else
      ccsid = QCS_DEFAULT;

   xmlTextReader *reader = xmlReaderForDoc((xmlChar *)p0->val.String->getBuffer(), NULL, NULL, QORE_XML_READER_PARAMS);
   if (!reader)
      return NULL;
   ON_BLOCK_EXIT(xmlFreeTextReader, reader);

   int rc = xmlTextReaderRead(reader);
   if (rc != 1) 
   {
      xsink->raiseException("PARSE-XML-RPC-VALUE-READER-ERROR", "cannot parse XML string");
      return NULL;
   }

   class XmlRpcValue v;
   getXMLRPCValueData(reader, &v, ccsid, false, xsink);

   if (xsink->isEvent())
      return NULL;
   return v.getValue();
}

static inline class QoreNode *qore_xml_exception(char *ex, char *info, class ExceptionSink *xsink)
{
   xsink->raiseException(ex, "error parsing XML string: %s", info);
   return NULL;
}

static inline class QoreNode *qore_xml_exception(char *ex, class ExceptionSink *xsink)
{
   xsink->raiseException(ex, "error parsing XML string");
   return NULL;
}

static class QoreNode *f_parseXMLRPCCall(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   printd(5, "parseXMLRPCCall(%s)\n", p0->val.String->getBuffer());

   class QoreEncoding *ccsid;
   if ((p1 = test_param(params, NT_STRING, 1)))
      ccsid = QEM.findCreate(p1->val.String);
   else
      ccsid = QCS_DEFAULT;

   xmlTextReader *reader = xmlReaderForDoc((xmlChar *)p0->val.String->getBuffer(), NULL, NULL, QORE_XML_READER_PARAMS);
   if (!reader)
      return NULL;
   ON_BLOCK_EXIT(xmlFreeTextReader, reader);

   if (xmlTextReaderRead(reader) != 1)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", xsink);

   int nt;
   // get "methodCall" element
   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodCall' element", xsink);

   if (qore_xmlCheckName(reader, "methodCall", xsink))
   {
      return NULL;
   }

   // get "methodName" element
   if (qore_xmlRead(reader, "expecting methodName element", xsink))
   {
      return NULL;
   }

   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodName' element", xsink);

   if (qore_xmlCheckName(reader, "methodName", xsink))
   {
      return NULL;
   }

   // get method name string
   if (qore_xmlRead(reader, "expecting method name", xsink))
   {
      return NULL;
   }

   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_TEXT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting method name", xsink);

   char *method_name = (char *)xmlTextReaderConstValue(reader);
   if (!method_name)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting method name", xsink);

   class hashKeeper h;
   h.setKeyValue("methodName", new QoreNode(method_name));

   // get methodName close tag
   if (qore_xmlRead(reader, "expecting methodName close element", xsink))
   {
      return NULL;
   }

   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodName' close element", xsink);

   // get "params" element
   if (qore_xmlRead(reader, "expecting params element", xsink))
   {
      return NULL;
   }

   if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", xsink);

   // if the methodCall end element was not found
   if (nt != XML_READER_TYPE_END_ELEMENT)
   {
      if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'params' element", xsink);

      if (qore_xmlCheckName(reader, "params", xsink))
      {
	 return NULL;
      }

      // get 'param' element or close params
      if (qore_xmlRead(reader, "expecting param element", xsink))
      {
	 return NULL;
      }
      
      if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", xsink);
      
      class XmlRpcValue v;
      if (xmlTextReaderDepth(reader))
      {
	 if (nt != XML_READER_TYPE_END_ELEMENT)
	 {
	    if (nt != XML_READER_TYPE_ELEMENT)
	       return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'params' element", xsink);
	    
	    getXMLRPCParams(reader, &v, ccsid, xsink);

	    if (xsink->isEvent())
	    {
	       return NULL;
	    }
	 }

	 // get methodCall close tag
	 if (qore_xmlRead(reader, "expecting methodCall close tag", xsink))
	 {
	    return NULL;
	 }
      }

      if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
	 return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodCall' close element", xsink);

      h.setKeyValue("params", v.getValue());
   }

   return new QoreNode(h.getHash());
}

static class QoreNode *f_parseXMLRPCResponse(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   printd(5, "parseXMLRPCCall(%s)\n", p0->val.String->getBuffer());

   class QoreEncoding *ccsid;
   if ((p1 = test_param(params, NT_STRING, 1)))
      ccsid = QEM.findCreate(p1->val.String);
   else
      ccsid = QCS_DEFAULT;

   xmlTextReader *reader = xmlReaderForDoc((xmlChar *)p0->val.String->getBuffer(), NULL, NULL, QORE_XML_READER_PARAMS);
   if (!reader)
      return NULL;
   ON_BLOCK_EXIT(xmlFreeTextReader, reader);

   if (xmlTextReaderRead(reader) != 1)
      return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", xsink);

   int nt;
   // get "methodResponse" element
   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'methodResponse' element", xsink);

   if (qore_xmlCheckName(reader, "methodResponse", xsink))
   {
      return NULL;
   }

   // check for params or fault element
   if (qore_xmlRead(reader, "expecting 'params' or 'fault' element", xsink))
   {
      return NULL;
   }

   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'params' or 'fault' element", xsink);

   char *name = (char *)xmlTextReaderConstName(reader);
   if (!name)
   {
      xsink->raiseException("PARSE-XML-RPC-RESPONSE-ERROR", "missing 'params' or 'fault' element tag");
      return NULL;
   }

   class XmlRpcValue v;
   bool fault = false;
   if (!strcmp(name, "params"))
   {
      int depth = xmlTextReaderDepth(reader);

      // get "params" element
      if (qore_xmlRead(reader, "expecting 'params' element", xsink))
      {
	 return NULL;
      }

      int params_depth = xmlTextReaderDepth(reader);

      // if params was not an empty element
      if (depth < params_depth)
      {
	 if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	 {
	    return NULL;
	 }

	 if (nt != XML_READER_TYPE_END_ELEMENT)
	 {
	    if (nt != XML_READER_TYPE_ELEMENT)
	       return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'param' element", xsink);
	       
	    if (qore_xmlCheckName(reader, "param", xsink))
	    {
	       return NULL;
	    }
	       
	    // get "value" element
	    if (qore_xmlRead(reader, "expecting 'value' element", xsink))
	    {
	       return NULL;
	    }

	    // if param was not an empty element
	    depth = xmlTextReaderDepth(reader);
	    if (params_depth < depth)
	    {
	       if ((nt = qore_xmlReadNode(reader, xsink)) == -1)
	       {
		  return NULL;
	       }
	    
	       if (nt != XML_READER_TYPE_END_ELEMENT)
	       {
		  if (nt != XML_READER_TYPE_ELEMENT)
		     return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'value' element", xsink);
	       
		  if (qore_xmlCheckName(reader, "value", xsink))
		  {
		     return NULL;
		  }
	       		  
		  // position at next element
		  if (qore_xmlRead(reader, "expecting XML-RPC value element", xsink))
		  {
		     return NULL;
		  }
		  
		  // if value was not an empty element
		  if (depth < xmlTextReaderDepth(reader))
		  {
		     getXMLRPCValueData(reader, &v, ccsid, true, xsink);
		     
		     if (xsink->isEvent())
		     {
			return NULL;
		     }
		     /*
		     // get "param" end element
		     if (qore_xmlRead(reader, "expecting 'param' end element", xsink))
		     {
			return NULL;
		     }
		     */
		  }
		  if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
		     return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'param' end element", xsink);
	       }

	       // get "params" end element
	       if (qore_xmlRead(reader, "expecting 'params' end element", xsink))
	       {
		  return NULL;
	       }
	    }
	    if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
	       return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'params' end element", xsink);
	 }
	 // get "methodResponse" end element
	 if (qore_xmlRead(reader, "expecting 'methodResponse' end element", xsink))
	 {
	    return NULL;
	 }
      }
   }
   else if (!strcmp(name, "fault"))
   {
      fault = true;

      // get "value" element
      if (qore_xmlRead(reader, "expecting 'value' element", xsink))
      {
	 return NULL;
      }
      
      if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_ELEMENT)
	 return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting fault 'value' element", xsink);

      if (qore_xmlCheckName(reader, "value", xsink))
      {
	 return NULL;
      }

      // position at next element
      if (qore_xmlRead(reader, "expecting XML-RPC value element", xsink))
      {
	 return NULL;
      }
      
      // get fault structure
      getXMLRPCValueData(reader, &v, ccsid, true, xsink);

      if (xsink->isEvent())
      {
	 return NULL;
      }

      /*
      // get "fault" end element
      if (qore_xmlRead(reader, "expecting 'fault' end element", xsink))
      {
	 return NULL;
      }
      */
      
      if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
	 return qore_xml_exception("PARSE-XML-RPC-RESPONSE-ERROR", "expecting 'fault' end element", xsink);

      // get "methodResponse" end element
      if (qore_xmlRead(reader, "expecting 'methodResponse' end element", xsink))
      {
	 return NULL;
      }
   }
   else
   {
      xsink->raiseException("PARSE-XML-RPC-RESPONSE-ERROR", "unexpected element '%s', expecting 'params' or 'fault'", name);
      return NULL;      
   }

   if ((nt = qore_xmlTextReaderNodeType(reader)) != XML_READER_TYPE_END_ELEMENT)
      return qore_xml_exception("PARSE-XML-RPC-CALL-ERROR", "expecting 'methodResponse' end element", xsink);


   class Hash *h = new Hash();
   if (fault)
      h->setKeyValue("fault", v.getValue(), NULL);
   else
      h->setKeyValue("params", v.getValue(), NULL);
   return new QoreNode(h);
}

void init_xml_functions()
{
   builtinFunctions.add("parseXML",                               f_parseXML);

   builtinFunctions.add("makeFormattedXMLString",                 f_makeFormattedXMLString);
   builtinFunctions.add("makeFormattedXMLFragment",               f_makeFormattedXMLFragment);

   builtinFunctions.add("makeXMLString",                          f_makeXMLString);
   builtinFunctions.add("makeXMLFragment",                        f_makeXMLFragment);

   builtinFunctions.add("makeXMLRPCCallString",                   f_makeXMLRPCCallString);
   builtinFunctions.add("makeXMLRPCCallStringArgs",               f_makeXMLRPCCallStringArgs);

   builtinFunctions.add("makeXMLStringNew",                       f_makeXMLStringNew);
   builtinFunctions.add("makeXMLFragmentNew",                     f_makeXMLFragmentNew);

   // the following 3 functions use the libxml2 writer to generate the XML output, but the formatting is funny sometimes
   builtinFunctions.add("makeFormattedXMLStringNew",              f_makeFormattedXMLStringNew);
   builtinFunctions.add("makeFormattedXMLFragmentNew",            f_makeFormattedXMLFragmentNew);
   builtinFunctions.add("makeXMLRPCCallStringNew",                f_makeXMLRPCCallStringNew);

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
}
