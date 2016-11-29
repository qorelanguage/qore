/*
  QoreStringNode.cpp

  QoreStringNode Class Definition

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>

#include "qore/intern/qore_string_private.h"

#include <stdarg.h>

QoreStringNodeMaker::QoreStringNodeMaker(const char* fmt, ...) {
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
}

QoreStringNode::QoreStringNode() : SimpleValueQoreNode(NT_STRING) {
   //sset.add(this);
}

QoreStringNode::~QoreStringNode() {
   //sset.del(this);
}

QoreStringNode::QoreStringNode(const char *str, const QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(str, enc) {
   //sset.add(this);
}

// copies str
QoreStringNode::QoreStringNode(const QoreString &str) : SimpleValueQoreNode(NT_STRING), QoreString(str) {
   //sset.add(this);
}

// copies str
QoreStringNode::QoreStringNode(const QoreStringNode &str) : SimpleValueQoreNode(NT_STRING), QoreString(str) {
   //sset.add(this);
}

// copies str
QoreStringNode::QoreStringNode(const std::string &str, const QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(str, enc) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(char c) : SimpleValueQoreNode(NT_STRING), QoreString(c) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(const BinaryNode *b) : SimpleValueQoreNode(NT_STRING), QoreString(b) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(const BinaryNode* b, qore_size_t maxlinelen) : SimpleValueQoreNode(NT_STRING), QoreString(b, maxlinelen) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(struct qore_string_private *p) : SimpleValueQoreNode(NT_STRING), QoreString(p) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(char *nbuf, qore_size_t nlen, qore_size_t nallocated, const QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(nbuf, nlen, nallocated, enc) {
   //sset.add(this);
}

QoreStringNode::QoreStringNode(const char *str, qore_size_t len, const QoreEncoding *new_qorecharset) : SimpleValueQoreNode(NT_STRING), QoreString(str, len, new_qorecharset) {
   //sset.add(this);
}

// virtual function
int QoreStringNode::getAsIntImpl() const {
   return (int)strtoll(getBuffer(), 0, 10);
}

int64 QoreStringNode::getAsBigIntImpl() const {
   return strtoll(getBuffer(), 0, 10);
}

double QoreStringNode::getAsFloatImpl() const {
   return strtod(getBuffer(), 0);
}

QoreString *QoreStringNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   TempString str(getEncoding());
   str->concat('"');
   str->concatEscape(this, '\"', '\\', xsink);
   if (*xsink)
      return 0;
   str->concat('"');
   return str.release();
}

int QoreStringNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   str.concat('"');
   str.concatEscape(this, '\"', '\\', xsink);
   if (*xsink)
      return -1;
   str.concat('"');
   return 0;
}

bool QoreStringNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return atof(getBuffer());
   if (priv->len == 1 && priv->buf[0] == '0')
      return false;
   return !empty();
}

// get the value of the type in a string context, empty string for complex types (default implementation)
QoreString *QoreStringNode::getStringRepresentation(bool &del) const {
   del = false;
   return const_cast<QoreStringNode *>(this);
}

QoreStringNode *QoreStringNode::convertEncoding(const QoreEncoding *nccs, ExceptionSink *xsink) const {
   printd(5, "QoreStringNode::convertEncoding() from '%s' to '%s'\n", getEncoding()->getCode(), nccs->getCode());

   if (nccs == priv->charset) {
      ref();
      return const_cast<QoreStringNode *>(this);
   }
   if (!priv->len)
      return new QoreStringNode(nccs);

   QoreStringNode *targ = new QoreStringNode(nccs);

   if (qore_string_private::convert_encoding_intern(priv->buf, priv->len, priv->charset, *targ, nccs, xsink)) {
      targ->deref();
      return 0;
   }
   return targ;
}

// DLLLOCAL constructor
QoreStringNode::QoreStringNode(const char *str, const QoreEncoding *from, const QoreEncoding *to, ExceptionSink *xsink) : SimpleValueQoreNode(NT_STRING), QoreString(to) {
   qore_string_private::convert_encoding_intern(str, ::strlen(str), from, *this, to, xsink);
}

// static function
QoreStringNode *QoreStringNode::createAndConvertEncoding(const char *str, const QoreEncoding *from, const QoreEncoding *to, ExceptionSink *xsink) {
   QoreStringNodeHolder rv(new QoreStringNode(str, from, to, xsink));
   return *xsink ? 0 : rv.release();
}

AbstractQoreNode *QoreStringNode::realCopy() const {
   return copy();
}

QoreStringNode *QoreStringNode::copy() const {
   return new QoreStringNode(*this);
}

QoreStringNode *QoreStringNode::substr(qore_offset_t offset, ExceptionSink *xsink) const {
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(priv->charset));

   int rc;
   if (!getEncoding()->isMultiByte())
      rc = substr_simple(*str, offset);
   else
      rc = substr_complex(*str, offset, xsink);

   return rc ? 0 : str.release();
}

QoreStringNode *QoreStringNode::substr(qore_offset_t offset, qore_offset_t length, ExceptionSink *xsink) const {
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(priv->charset));

   int rc;
   if (!getEncoding()->isMultiByte())
      rc = substr_simple(*str, offset, length);
   else
      rc = substr_complex(*str, offset, length, xsink);

   return rc ? 0 : str.release();
}

QoreStringNode *QoreStringNode::reverse() const {
   QoreStringNode *str = new QoreStringNode(priv->charset);
   concat_reverse(str);
   return str;
}

QoreStringNode *QoreStringNode::parseBase64ToString(const QoreEncoding* qe, ExceptionSink* xsink) const {
   SimpleRefHolder<BinaryNode> b(::parseBase64(priv->buf, priv->len, xsink));
   if (!b)
      return 0;

   if (b->empty())
      return new QoreStringNode;

   qore_string_private *p = new qore_string_private;
   p->len = b->size() - 1;
   p->buf = (char *)b->giveBuffer();
   p->charset = qe;

   // free memory allocated to binary object
   b = 0;

   // check for null termination
   if (p->buf[p->len]) {
      ++p->len;
      p->buf = (char *)realloc(p->buf, p->len + 1);
      p->buf[p->len] = '\0';
   }

   p->allocated = p->len + 1;
   return new QoreStringNode(p);
}

QoreStringNode *QoreStringNode::parseBase64ToString(ExceptionSink *xsink) const {
   return parseBase64ToString(QCS_DEFAULT, xsink);
}

void QoreStringNode::getStringRepresentation(QoreString &str) const {
   str.concat(static_cast<const QoreString *>(this));
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *QoreStringNode::getDateTimeRepresentation(bool &del) const {
   del = true;
   return new DateTime(getBuffer());
}

// assign date representation to a DateTime * (no action for complex types = default implementation)
void QoreStringNode::getDateTimeRepresentation(DateTime &dt) const {
   dt.setDate(getBuffer());
}

bool QoreStringNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   if (get_node_type(v) == NT_STRING)
      return equalSoft(*reinterpret_cast<const QoreStringNode*>(v), xsink);
   QoreStringValueHelper str(v, getEncoding(), xsink);
   if (*xsink)
      return false;
   return equal(*str);
}

bool QoreStringNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   // note that "hard" equality checks now do encoding conversions if necessary
   if (get_node_type(v) == NT_STRING)
      return equalSoft(*reinterpret_cast<const QoreStringNode*>(v), xsink);
   return false;
}

QoreStringNode *QoreStringNode::stringRefSelf() const {
   ref();
   return const_cast<QoreStringNode *>(this);
}

const char *QoreStringNode::getTypeName() const {
   return getStaticTypeName();
}

AbstractQoreNode *QoreStringNode::parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = stringTypeInfo;
   return this;
}

QoreStringNode *QoreStringNode::extract(qore_offset_t offset, ExceptionSink *xsink) {
   QoreStringNode *str = new QoreStringNode(priv->charset);
   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset = priv->check_offset(offset);
      if (n_offset != priv->len)
	 splice_simple(n_offset, priv->len - n_offset, str);
   }
   else
      splice_complex(offset, xsink, str);
   return str;
}

QoreStringNode *QoreStringNode::extract(qore_offset_t offset, qore_offset_t num, ExceptionSink *xsink) {
   QoreStringNode *str = new QoreStringNode(priv->charset);
   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset != priv->len && n_num)
	 splice_simple(n_offset, n_num, str);
   }
   else
      splice_complex(offset, num, xsink, str);
   return str;
}

QoreStringNode *QoreStringNode::extract(qore_offset_t offset, qore_offset_t num, const AbstractQoreNode *strn, ExceptionSink *xsink) {
   if (!strn || strn->getType() != NT_STRING)
      return extract(offset, num, xsink);

   const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(strn);
   TempEncodingHelper tmp(str, priv->charset, xsink);
   if (!tmp)
       return 0;

   QoreStringNode *rv = new QoreStringNode(priv->charset);
   if (!priv->charset->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset == priv->len) {
	 if (!tmp->strlen())
	    return rv;
	 n_num = 0;
      }
      splice_simple(n_offset, n_num, tmp->getBuffer(), tmp->strlen(), rv);
   }
   else
      splice_complex(offset, num, *tmp, xsink, rv);
   return rv;
}

QoreNodeAsStringHelper::QoreNodeAsStringHelper(const AbstractQoreNode *n, int format_offset, ExceptionSink *xsink) {
   if (n)
      str = n->getAsString(del, format_offset, xsink);
   else {
      str = format_offset == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString;
      del = false;
   }
}

QoreNodeAsStringHelper::QoreNodeAsStringHelper(const QoreValue n, int format_offset, ExceptionSink *xsink) {
   str = n.getAsString(del, format_offset, xsink);
   /*
   if (n.isNothing()) {
      str = format_offset == FMT_YAML_SHORT ? &YamlNullString : &NothingTypeString;
      del = false;
      return;
   }
   switch (n.type) {
      case QV_Int: str = new QoreStringMaker(QLLD, n.v.i); del = true; break;
      case QV_Bool: str = n.v.b ? &TrueString : &FalseString; del = false; break;
      case QV_Float: str = new QoreStringMaker("%.9g", n.v.f); del = true; break;
      case QV_Node: str = n.getAsString(del, format_offset, xsink); break;
      default:
	 assert(false);
	 // no break;
   }
   */
}

QoreNodeAsStringHelper::~QoreNodeAsStringHelper() {
   if (del)
      delete str;
}

QoreString* QoreNodeAsStringHelper::giveString() {
   if (!str)
      return 0;
   if (!del)
      return str->copy();

   QoreString* rv = str;
   del = false;
   str = 0;
   return rv;
}

void QoreStringValueHelper::setup(ExceptionSink* xsink, const QoreValue n, const QoreEncoding* enc) {
   switch (n.type) {
      case QV_Bool:
      case QV_Int:
	 str = new QoreStringMaker(QLLD, n.getAsBigInt());
	 del = true;
	 break;

      case QV_Float:
	 str = new QoreStringMaker("%.9g", n.getAsFloat());
	 del = true;
	 break;

      case QV_Node:
	 if (n.v.n) {
	    //optimization to remove the need for a virtual function call in the most common case
	    if (n.v.n->getType() == NT_STRING) {
	       del = false;
	       str = const_cast<QoreStringNode*>(n.get<QoreStringNode>());
	    }
	    else
	       str = n.get<AbstractQoreNode>()->getStringRepresentation(del);
	    if (enc && str->getEncoding() != enc) {
	       QoreString* t = str->convertEncoding(enc, xsink);
	       if (!t)
		  break;
	       if (del)
		  delete str;
	       str = t;
	       del = true;
	    }
	 }
	 else {
	    str = NullString;
	    del = false;
	 }
	 break;

      default:
	 assert(false);
	 // no break
   }
}

QoreStringValueHelper::QoreStringValueHelper(const QoreValue& n) {
   setup(0, n);
}

QoreStringValueHelper::QoreStringValueHelper(const QoreValue& n, const QoreEncoding* enc, ExceptionSink* xsink) {
   setup(xsink, n, enc);
}

QoreStringValueHelper::QoreStringValueHelper(const AbstractQoreNode* n) {
   setup(0, const_cast<AbstractQoreNode*>(n));
}

QoreStringValueHelper::QoreStringValueHelper(const AbstractQoreNode* n, const QoreEncoding* enc, ExceptionSink* xsink) {
   setup(xsink, const_cast<AbstractQoreNode*>(n), enc);
}

void QoreStringNodeValueHelper::setup(ExceptionSink* xsink, const QoreValue n, const QoreEncoding* enc) {
   switch (n.type) {
      case QV_Bool:
      case QV_Int:
	 str = new QoreStringNodeMaker(QLLD, n.getAsBigInt());
	 del = true;
	 break;

      case QV_Float:
	 str = new QoreStringNodeMaker("%.9g", n.getAsFloat());
	 del = true;
	 break;

      case QV_Node:
	 if (n.v.n) {
	    //optimization to remove the need for a virtual function call in the most common case
	    if (n.v.n->getType() == NT_STRING) {
	       del = false;
	       str = const_cast<QoreStringNode*>(n.get<QoreStringNode>());
	    }
	    else {
	       del = true;
	       str = new QoreStringNode;
	       n.get<AbstractQoreNode>()->getStringRepresentation(*str);
	    }
	    if (enc && str->getEncoding() != enc) {
	       QoreStringNode* t = str->convertEncoding(enc, xsink);
	       if (!t)
		  break;
	       if (del)
		  str->deref();
	       str = t;
	       del = true;
	    }
	 }
	 else {
	    str = NullString;
	    del = false;
	 }
	 break;

      default:
	 assert(false);
	 // no break
   }
}

QoreStringNodeValueHelper::QoreStringNodeValueHelper(const QoreValue& n) {
   setup(0, n);
}

QoreStringNodeValueHelper::QoreStringNodeValueHelper(const QoreValue& n, const QoreEncoding* enc, ExceptionSink* xsink) {
   setup(xsink, n, enc);
}

QoreStringNodeValueHelper::QoreStringNodeValueHelper(const AbstractQoreNode* n) {
   setup(0, const_cast<AbstractQoreNode*>(n));
}

QoreStringNodeValueHelper::QoreStringNodeValueHelper(const AbstractQoreNode* n, const QoreEncoding* enc, ExceptionSink* xsink) {
   setup(xsink, const_cast<AbstractQoreNode*>(n), enc);
}

QoreStringNodeValueHelper::~QoreStringNodeValueHelper() {
   if (del)
      str->deref();
}

QoreStringNode* QoreStringNodeValueHelper::getReferencedValue() {
   if (del)
      del = false;
   else if (str)
      str->ref();
   return str;
}
