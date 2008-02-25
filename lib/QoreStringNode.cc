/*
  QoreStringNode.cc

  QoreStringNode Class Definition

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/intern/qore_string_private.h>

QoreStringNode::QoreStringNode() : SimpleValueQoreNode(NT_STRING)
{
}

QoreStringNode::~QoreStringNode()
{
}

QoreStringNode::QoreStringNode(const char *str, const class QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(str, enc)
{
}

// FIXME: remove this function
// takes ownership of str
/*
QoreStringNode::QoreStringNode(QoreString *str) : SimpleValueQoreNode(NT_STRING), QoreString(str->priv)
{
   str->priv = 0;
   delete str;
}
*/

// copies str
QoreStringNode::QoreStringNode(const QoreString &str) : SimpleValueQoreNode(NT_STRING), QoreString(str)
{
}

// copies str
QoreStringNode::QoreStringNode(const QoreStringNode &str) : SimpleValueQoreNode(NT_STRING), QoreString(str)
{
}

// copies str
QoreStringNode::QoreStringNode(const std::string &str, const class QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(str, enc)
{
}

QoreStringNode::QoreStringNode(char c) : SimpleValueQoreNode(NT_STRING), QoreString(c)
{
}

QoreStringNode::QoreStringNode(const class BinaryNode *b) : SimpleValueQoreNode(NT_STRING), QoreString(b)
{
}

QoreStringNode::QoreStringNode(struct qore_string_private *p) : SimpleValueQoreNode(NT_STRING), QoreString(p)
{
}

QoreStringNode::QoreStringNode(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc) : SimpleValueQoreNode(NT_STRING), QoreString(nbuf, nlen, nallocated, enc)
{
}

QoreStringNode::QoreStringNode(const char *str, int len, const class QoreEncoding *new_qorecharset) : SimpleValueQoreNode(NT_STRING), QoreString(str, len, new_qorecharset)
{
}

// virtual function
int QoreStringNode::getAsIntImpl() const
{
   return strtoll(getBuffer(), 0, 10);
}

int64 QoreStringNode::getAsBigIntImpl() const
{
   return strtoll(getBuffer(), 0, 10);   
}

double QoreStringNode::getAsFloatImpl() const
{
   return atof(getBuffer());
}

QoreString *QoreStringNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   QoreString *str = new QoreString(getEncoding());
   str->concat('"');
   str->concat(this);
   str->concat('"');
   return str;
}

int QoreStringNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   str.concat('"');
   str.concat(this, xsink);
   if (*xsink)
      return -1;
   str.concat('"');
   return 0;
}

bool QoreStringNode::getAsBoolImpl() const
{
   return strtoll(getBuffer(), 0, 10) ? true : false;
}

// get the value of the type in a string context, empty string for complex types (default implementation)
QoreString *QoreStringNode::getStringRepresentation(bool &del) const
{
   del = false;
   return const_cast<QoreStringNode *>(this);
}
 
QoreStringNode *QoreStringNode::convertEncoding(const class QoreEncoding *nccs, ExceptionSink *xsink) const
{
   printd(5, "QoreStringNode::convertEncoding() from '%s' to '%s'\n", getEncoding()->getCode(), nccs->getCode());

   if (nccs == priv->charset) {
      ref();
      return const_cast<QoreStringNode *>(this);
   }
   if (!priv->len)
      return new QoreStringNode(nccs);

   QoreStringNode *targ = new QoreStringNode(nccs);

   if (convert_encoding_intern(priv->buf, priv->len, priv->charset, *targ, nccs, xsink)) {
      targ->deref();
      return 0;
   }
   return targ;
}

// DLLLOCAL constructor
QoreStringNode::QoreStringNode(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink) : SimpleValueQoreNode(NT_DATE), QoreString(to)
{
   convert_encoding_intern(str, ::strlen(str), from, *this, to, xsink);
}

// static function
QoreStringNode *QoreStringNode::createAndConvertEncoding(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink)
{
   QoreStringNode *rv = new QoreStringNode(str, from, to, xsink);
   if (!*xsink)
      return rv;

   rv->deref();
   return 0;
}

AbstractQoreNode *QoreStringNode::realCopy() const
{
   return copy();
}

QoreStringNode *QoreStringNode::copy() const
{
   return new QoreStringNode(*this);
}

QoreStringNode *QoreStringNode::substr(int offset) const
{
   QoreStringNode *str = new QoreStringNode(priv->charset);

   int rc;
   if (!getEncoding()->isMultiByte())
      rc = substr_simple(str, offset);
   else
      rc = substr_complex(str, offset);

   if (!rc)
      return str;

   str->deref();
   return 0;
}

QoreStringNode *QoreStringNode::substr(int offset, int length) const
{
   QoreStringNode *str = new QoreStringNode(priv->charset);

   int rc;
   if (!getEncoding()->isMultiByte())
      rc = substr_simple(str, offset, length);
   else
      rc = substr_complex(str, offset, length);

   if (!rc)
      return str;

   str->deref();
   return 0;
}

QoreStringNode *QoreStringNode::reverse() const
{
   QoreStringNode *str = new QoreStringNode(priv->charset);
   concat_reverse(str);
   return str;
}

QoreStringNode *QoreStringNode::parseBase64ToString(ExceptionSink *xsink) const
{
   SimpleRefHolder<BinaryNode> b(::parseBase64(priv->buf, priv->len, xsink));
   if (!b)
      return 0;

   qore_string_private *p = new qore_string_private;
   p->len = b->size() - 1;
   p->buf = (char *)b->giveBuffer();
   p->charset = QCS_DEFAULT;

   // free memory allocated to binary object
   b = 0;

   // check for null termination
   if (p->buf[p->len])
   {
      ++p->len;
      p->buf = (char *)realloc(p->buf, p->len + 1);
      p->buf[p->len] = '\0';
   }

   p->allocated = p->len + 1;
   return new QoreStringNode(p);
}

/*
// returns 0 if the value is not immediately returnable as a QoreString (without conversion)
class QoreString *QoreStringNode::getQoreStringValue() const
{
   return this;
}

// returns 0 if the value is not immediately returnable as a const char * (without conversion)
const char *QoreStringNode::getStringValue() const
{
   return getBuffer();
}
*/

void QoreStringNode::getStringRepresentation(QoreString &str) const
{
   str.concat(static_cast<const QoreString *>(this));
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
class DateTime *QoreStringNode::getDateTimeRepresentation(bool &del) const
{
   del = true;
   return new DateTime(getBuffer());
}

// assign date representation to a DateTime * (no action for complex types = default implementation)
void QoreStringNode::getDateTimeRepresentation(DateTime &dt) const
{
   dt.setDate(getBuffer());
}

bool QoreStringNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   QoreStringValueHelper str(v, getEncoding(), xsink);
   if (*xsink)
      return false;
   return !compare(*str);
}

bool QoreStringNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(v);
   if (!str)
      return false;

   if (getEncoding() != str->getEncoding())
      return false;

   return !compare(str);
}

const char *QoreStringNode::getTypeName() const
{
   return getStaticTypeName();
}
