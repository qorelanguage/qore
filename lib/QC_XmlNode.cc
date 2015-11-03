/*
 QC_XmlNode.cc
 
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
#include <qore/intern/QC_XmlNode.h>
#include <qore/intern/QC_XmlReader.h>

qore_classid_t CID_XMLNODE;
QoreClass *QC_XMLNODE;

static void XMLNODE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   xsink->raiseException("XMLNODE-CONSTRUCTOR-ERROR", "this class cannot be constructed directly");
}

static void XMLNODE_copy(QoreObject *self, QoreObject *old, QoreXmlNodeData *xn, ExceptionSink *xsink) {
   self->setPrivate(CID_XMLNODE, new QoreXmlNodeData(*xn));
}

static AbstractQoreNode *XMLNODE_childElementCount(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xn->childElementCount());
}

static AbstractQoreNode *XMLNODE_getSpacePreserve(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xn->getSpacePreserve());
}

static AbstractQoreNode *XMLNODE_getLineNumber(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xn->getLineNumber());
}

static AbstractQoreNode *XMLNODE_getElementType(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(xn->getElementType());
}

static AbstractQoreNode *XMLNODE_getElementTypeName(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   const char *nt = get_xml_element_type_name(xn->getElementType());
   return nt ? new QoreStringNode(nt) : 0;
}

static QoreObject *doObject(QoreXmlNodeData *data) {
   return data ? new QoreObject(QC_XMLNODE, getProgram(), data) : 0;
}

static AbstractQoreNode *XMLNODE_firstElementChild(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return doObject(xn->firstElementChild());
}

static AbstractQoreNode *XMLNODE_getLastChild(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return doObject(xn->getLastChild());
}

static AbstractQoreNode *XMLNODE_lastElementChild(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return doObject(xn->lastElementChild());
}

static AbstractQoreNode *XMLNODE_nextElementSibling(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return doObject(xn->nextElementSibling());
}

static AbstractQoreNode *XMLNODE_previousElementSibling(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return doObject(xn->previousElementSibling());
}

static AbstractQoreNode *XMLNODE_getPath(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getPath(xsink);
}

static AbstractQoreNode *XMLNODE_getNsProp(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *prop = test_string_param(params, 0);
   if (!prop) {
      xsink->raiseException("XMLNODE-GETNSPROP-ERROR", "missing property name as first parameter to XmlNode::getNsProp()");
      return 0;
   }
   const QoreStringNode *ns = test_string_param(params, 1);
   if (!ns) {
      xsink->raiseException("XMLNODE-GETNSPROP-ERROR", "missing namespace name as second parameter to XmlNode::getNsProp()");
      return 0;
   }
   return xn->getNsProp(prop->getBuffer(), ns->getBuffer());
}

static AbstractQoreNode *XMLNODE_getProp(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *prop = test_string_param(params, 0);
   if (!prop) {
      xsink->raiseException("XMLNODE-GETPROP-ERROR", "missing property name as sole parameter to XmlNode::getProp()");
      return 0;
   }
   return xn->getProp(prop->getBuffer());
}

/*
static AbstractQoreNode *XMLNODE_getBase(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getBase();
}
*/

static AbstractQoreNode *XMLNODE_getContent(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getContent();
}

static AbstractQoreNode *XMLNODE_getName(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getName();
}

static AbstractQoreNode *XMLNODE_getLang(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getLang();
}

static AbstractQoreNode *XMLNODE_isText(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xn->isText());
}

static AbstractQoreNode *XMLNODE_isBlank(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node(xn->isBlank());
}

static AbstractQoreNode *XMLNODE_getXML(QoreObject *self, QoreXmlNodeData *xn, const QoreListNode *params, ExceptionSink *xsink) {
   return xn->getXML();
}

static QoreClass *initXmlNodeClass() {
   QORE_TRACE("initXmlNodeClass()");

   QC_XMLNODE = new QoreClass("XmlNode");
   CID_XMLNODE = QC_XMLNODE->getID();
   QC_XMLNODE->setConstructor(XMLNODE_constructor);
   QC_XMLNODE->setCopy((q_copy_t)XMLNODE_copy);

   QC_XMLNODE->addMethod("childElementCount",      (q_method_t)XMLNODE_childElementCount);
   QC_XMLNODE->addMethod("getSpacePreserve",       (q_method_t)XMLNODE_getSpacePreserve);
   QC_XMLNODE->addMethod("getLineNumber",          (q_method_t)XMLNODE_getLineNumber);
   QC_XMLNODE->addMethod("getElementType",         (q_method_t)XMLNODE_getElementType);
   QC_XMLNODE->addMethod("getElementTypeName",     (q_method_t)XMLNODE_getElementTypeName);
   QC_XMLNODE->addMethod("firstElementChild",      (q_method_t)XMLNODE_firstElementChild);
   QC_XMLNODE->addMethod("getLastChild",           (q_method_t)XMLNODE_getLastChild);
   QC_XMLNODE->addMethod("lastElementChild",       (q_method_t)XMLNODE_lastElementChild);
   QC_XMLNODE->addMethod("nextElementSibling",     (q_method_t)XMLNODE_nextElementSibling);
   QC_XMLNODE->addMethod("previousElementSibling", (q_method_t)XMLNODE_previousElementSibling);
   QC_XMLNODE->addMethod("getPath",                (q_method_t)XMLNODE_getPath);
   QC_XMLNODE->addMethod("getNsProp",              (q_method_t)XMLNODE_getNsProp);
   QC_XMLNODE->addMethod("getProp",                (q_method_t)XMLNODE_getProp);
   //QC_XMLNODE->addMethod("getBase",                (q_method_t)XMLNODE_getBase);
   QC_XMLNODE->addMethod("getContent",             (q_method_t)XMLNODE_getContent);
   QC_XMLNODE->addMethod("getLang",                (q_method_t)XMLNODE_getLang);
   QC_XMLNODE->addMethod("getName",                (q_method_t)XMLNODE_getName);
   QC_XMLNODE->addMethod("isText",                 (q_method_t)XMLNODE_isText);
   QC_XMLNODE->addMethod("isBlank",                (q_method_t)XMLNODE_isBlank);
   QC_XMLNODE->addMethod("getXML",                 (q_method_t)XMLNODE_getXML);

   return QC_XMLNODE;   
}

QoreNamespace *initXmlNs() {
   QoreNamespace *xns = new QoreNamespace("Xml");
   xns->addConstant("XML_ELEMENT_NODE",         new QoreBigIntNode(XML_ELEMENT_NODE));
   xns->addConstant("XML_ATTRIBUTE_NODE",       new QoreBigIntNode(XML_ATTRIBUTE_NODE));
   xns->addConstant("XML_TEXT_NODE",            new QoreBigIntNode(XML_TEXT_NODE));
   xns->addConstant("XML_CDATA_SECTION_NODE",   new QoreBigIntNode(XML_CDATA_SECTION_NODE));
   xns->addConstant("XML_ENTITY_REF_NODE",      new QoreBigIntNode(XML_ENTITY_REF_NODE));
   xns->addConstant("XML_ENTITY_NODE",          new QoreBigIntNode(XML_ENTITY_NODE));
   xns->addConstant("XML_PI_NODE",              new QoreBigIntNode(XML_PI_NODE));
   xns->addConstant("XML_COMMENT_NODE",         new QoreBigIntNode(XML_COMMENT_NODE));
   xns->addConstant("XML_DOCUMENT_NODE",        new QoreBigIntNode(XML_DOCUMENT_NODE));
   xns->addConstant("XML_DOCUMENT_TYPE_NODE",   new QoreBigIntNode(XML_DOCUMENT_TYPE_NODE));
   xns->addConstant("XML_DOCUMENT_FRAG_NODE",   new QoreBigIntNode(XML_DOCUMENT_FRAG_NODE));
   xns->addConstant("XML_NOTATION_NODE",        new QoreBigIntNode(XML_NOTATION_NODE));
   xns->addConstant("XML_HTML_DOCUMENT_NODE",   new QoreBigIntNode(XML_HTML_DOCUMENT_NODE));
   xns->addConstant("XML_DTD_NODE",             new QoreBigIntNode(XML_DTD_NODE));
   xns->addConstant("XML_ELEMENT_DECL",         new QoreBigIntNode(XML_ELEMENT_DECL));
   xns->addConstant("XML_ATTRIBUTE_DECL",       new QoreBigIntNode(XML_ATTRIBUTE_DECL));
   xns->addConstant("XML_ENTITY_DECL",          new QoreBigIntNode(XML_ENTITY_DECL));
   xns->addConstant("XML_NAMESPACE_DECL",       new QoreBigIntNode(XML_NAMESPACE_DECL));
   xns->addConstant("XML_XINCLUDE_START",       new QoreBigIntNode(XML_XINCLUDE_START));
   xns->addConstant("XML_XINCLUDE_END",         new QoreBigIntNode(XML_XINCLUDE_END));
   xns->addConstant("XML_DOCB_DOCUMENT_NODE",   new QoreBigIntNode(XML_DOCB_DOCUMENT_NODE));

   xns->addConstant("XML_NODE_TYPE_NONE",                    new QoreBigIntNode(XML_READER_TYPE_NONE));
   xns->addConstant("XML_NODE_TYPE_ELEMENT",                 new QoreBigIntNode(XML_READER_TYPE_ELEMENT));
   xns->addConstant("XML_NODE_TYPE_ATTRIBUTE",               new QoreBigIntNode(XML_READER_TYPE_ATTRIBUTE));
   xns->addConstant("XML_NODE_TYPE_TEXT",                    new QoreBigIntNode(XML_READER_TYPE_TEXT));
   xns->addConstant("XML_NODE_TYPE_CDATA",                   new QoreBigIntNode(XML_READER_TYPE_CDATA));
   xns->addConstant("XML_NODE_TYPE_ENTITY_REFERENCE",        new QoreBigIntNode(XML_READER_TYPE_ENTITY_REFERENCE));
   xns->addConstant("XML_NODE_TYPE_ENTITY",                  new QoreBigIntNode(XML_READER_TYPE_ENTITY));
   xns->addConstant("XML_NODE_TYPE_PROCESSING_INSTRUCTION",  new QoreBigIntNode(XML_READER_TYPE_PROCESSING_INSTRUCTION));
   xns->addConstant("XML_NODE_TYPE_COMMENT",                 new QoreBigIntNode(XML_READER_TYPE_COMMENT));
   xns->addConstant("XML_NODE_TYPE_DOCUMENT",                new QoreBigIntNode(XML_READER_TYPE_DOCUMENT));
   xns->addConstant("XML_NODE_TYPE_DOCUMENT_TYPE",           new QoreBigIntNode(XML_READER_TYPE_DOCUMENT_TYPE));
   xns->addConstant("XML_NODE_TYPE_DOCUMENT_FRAGMENT",       new QoreBigIntNode(XML_READER_TYPE_DOCUMENT_FRAGMENT));
   xns->addConstant("XML_NODE_TYPE_NOTATION",                new QoreBigIntNode(XML_READER_TYPE_NOTATION));
   xns->addConstant("XML_NODE_TYPE_WHITESPACE",              new QoreBigIntNode(XML_READER_TYPE_WHITESPACE));
   xns->addConstant("XML_NODE_TYPE_SIGNIFICANT_WHITESPACE",  new QoreBigIntNode(XML_READER_TYPE_SIGNIFICANT_WHITESPACE));
   xns->addConstant("XML_NODE_TYPE_END_ELEMENT",             new QoreBigIntNode(XML_READER_TYPE_END_ELEMENT));
   xns->addConstant("XML_NODE_TYPE_END_ENTITY",              new QoreBigIntNode(XML_READER_TYPE_END_ENTITY));
   xns->addConstant("XML_NODE_TYPE_XML_DECLARATION",         new QoreBigIntNode(XML_READER_TYPE_XML_DECLARATION));

   // set up element map
   QoreHashNode *xm = new QoreHashNode();
   xm->setKeyValue("1",  new QoreStringNode("XML_ELEMENT_NODE"), 0);
   xm->setKeyValue("2",  new QoreStringNode("XML_ATTRIBUTE_NODE"), 0);
   xm->setKeyValue("3",  new QoreStringNode("XML_TEXT_NODE"), 0);
   xm->setKeyValue("4",  new QoreStringNode("XML_CDATA_SECTION_NODE"), 0);
   xm->setKeyValue("5",  new QoreStringNode("XML_ENTITY_REF_NODE"), 0);
   xm->setKeyValue("6",  new QoreStringNode("XML_ENTITY_NODE"), 0);
   xm->setKeyValue("7",  new QoreStringNode("XML_PI_NODE"), 0);
   xm->setKeyValue("8",  new QoreStringNode("XML_COMMENT_NODE"), 0);
   xm->setKeyValue("9",  new QoreStringNode("XML_DOCUMENT_NODE"), 0);
   xm->setKeyValue("10", new QoreStringNode("XML_DOCUMENT_TYPE_NODE"), 0);
   xm->setKeyValue("11", new QoreStringNode("XML_DOCUMENT_FRAG_NODE"), 0);
   xm->setKeyValue("12", new QoreStringNode("XML_NOTATION_NODE"), 0);
   xm->setKeyValue("13", new QoreStringNode("XML_HTML_DOCUMENT_NODE"), 0);
   xm->setKeyValue("14", new QoreStringNode("XML_DTD_NODE"), 0);
   xm->setKeyValue("15", new QoreStringNode("XML_ELEMENT_DECL"), 0);
   xm->setKeyValue("16", new QoreStringNode("XML_ATTRIBUTE_DECL"), 0);
   xm->setKeyValue("17", new QoreStringNode("XML_ENTITY_DECL"), 0);
   xm->setKeyValue("18", new QoreStringNode("XML_NAMESPACE_DECL"), 0);
   xm->setKeyValue("19", new QoreStringNode("XML_XINCLUDE_START"), 0);
   xm->setKeyValue("20", new QoreStringNode("XML_XINCLUDE_END"), 0);
   xm->setKeyValue("21", new QoreStringNode("XML_DOCB_DOCUMENT_NODE"), 0);

   xns->addConstant("ElementTypeMap", xm);

   // now set up node map
   xm = new QoreHashNode();
   xm->setKeyValue("0",  new QoreStringNode("XML_NODE_TYPE_NONE"), 0);
   xm->setKeyValue("1",  new QoreStringNode("XML_NODE_TYPE_ELEMENT"), 0);
   xm->setKeyValue("2",  new QoreStringNode("XML_NODE_TYPE_ATTRIBUTE"), 0);
   xm->setKeyValue("3",  new QoreStringNode("XML_NODE_TYPE_TEXT"), 0);
   xm->setKeyValue("4",  new QoreStringNode("XML_NODE_TYPE_CDATA"), 0);
   xm->setKeyValue("5",  new QoreStringNode("XML_NODE_TYPE_ENTITY_REFERENCE"), 0);
   xm->setKeyValue("6",  new QoreStringNode("XML_NODE_TYPE_ENTITY"), 0);
   xm->setKeyValue("7",  new QoreStringNode("XML_NODE_TYPE_PROCESSING_INSTRUCTION"), 0);
   xm->setKeyValue("8",  new QoreStringNode("XML_NODE_TYPE_COMMENT"), 0);
   xm->setKeyValue("9",  new QoreStringNode("XML_NODE_TYPE_DOCUMENT"), 0);
   xm->setKeyValue("10", new QoreStringNode("XML_NODE_TYPE_DOCUMENT_TYPE"), 0);
   xm->setKeyValue("11", new QoreStringNode("XML_NODE_TYPE_DOCUMENT_FRAGMENT"), 0);
   xm->setKeyValue("12", new QoreStringNode("XML_NODE_TYPE_NOTATION"), 0);
   xm->setKeyValue("13", new QoreStringNode("XML_NODE_TYPE_WHITESPACE"), 0);
   xm->setKeyValue("14", new QoreStringNode("XML_NODE_TYPE_SIGNIFICANT_WHITESPACE"), 0);
   xm->setKeyValue("15", new QoreStringNode("XML_NODE_TYPE_END_ELEMENT"), 0);
   xm->setKeyValue("16", new QoreStringNode("XML_NODE_TYPE_END_ENTITY"), 0);
   xm->setKeyValue("17", new QoreStringNode("XML_NODE_TYPE_XML_DECLARATION"), 0);

   xns->addConstant("NodeTypeMap", xm);

   xns->addSystemClass(initXmlNodeClass());
   xns->addSystemClass(initXmlDocClass());
   xns->addSystemClass(initXmlReaderClass());

   return xns;
}
