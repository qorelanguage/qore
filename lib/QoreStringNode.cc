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

QoreStringNode::QoreStringNode() : SimpleQoreNode(NT_STRING)
{
}

QoreStringNode::~QoreStringNode()
{
}

QoreStringNode::QoreStringNode(const char *str, const class QoreEncoding *enc) : SimpleQoreNode(NT_STRING), QoreString(str, enc)
{
}

// FIXME: remove this function
// takes ownership of str
/*
QoreStringNode::QoreStringNode(QoreString *str) : SimpleQoreNode(NT_STRING), QoreString(str->priv)
{
   str->priv = 0;
   delete str;
}
*/

// copies str
QoreStringNode::QoreStringNode(const QoreString &str) : SimpleQoreNode(NT_STRING), QoreString(str)
{
}

// copies str
QoreStringNode::QoreStringNode(const QoreStringNode &str) : SimpleQoreNode(NT_STRING), QoreString(str)
{
}

// copies str
QoreStringNode::QoreStringNode(const std::string &str, const class QoreEncoding *enc) : SimpleQoreNode(NT_STRING), QoreString(str, enc)
{
}

QoreStringNode::QoreStringNode(char c) : SimpleQoreNode(NT_STRING), QoreString(c)
{
}

QoreStringNode::QoreStringNode(const class BinaryObject *b) : SimpleQoreNode(NT_STRING), QoreString(b)
{
}

QoreStringNode::QoreStringNode(struct qore_string_private *p) : SimpleQoreNode(NT_STRING), QoreString(p)
{
}

QoreStringNode::QoreStringNode(char *nbuf, int nlen, int nallocated, const class QoreEncoding *enc) : SimpleQoreNode(NT_STRING), QoreString(nbuf, nlen, nallocated, enc)
{
}

QoreStringNode::QoreStringNode(const char *str, int len, const class QoreEncoding *new_qorecharset) : SimpleQoreNode(NT_STRING), QoreString(str, len, new_qorecharset)
{
}

// virtual function
int QoreStringNode::getAsInt() const
{
   return strtoll(getBuffer(), 0, 10);
}

bool QoreStringNode::needs_eval() const
{
   return false;
}

int64 QoreStringNode::getAsBigInt() const
{
   return strtoll(getBuffer(), 0, 10);   
}

double QoreStringNode::getAsFloat() const
{
   return atof(getBuffer());
}

QoreString *QoreStringNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   del = true;
   QoreString *str = new QoreString(getEncoding());
   str->concat('"');
   str->concat(static_cast<const QoreString *>(this));
   str->concat('"');
   return str;
}

bool QoreStringNode::getAsBool() const
{
   return strtoll(getBuffer(), 0, 10) ? true : false;
}

// get the value of the type in a string context, empty string for complex types (default implementation)
QoreString *QoreStringNode::getStringRepresentation(bool &del) const
{
   del = false;
   return const_cast<QoreStringNode *>(this);
}
 
class QoreStringNode *QoreStringNode::convertEncoding(const class QoreEncoding *nccs, class ExceptionSink *xsink) const
{
   printd(5, "QoreStringNode::convertEncoding() from '%s' to '%s'\n", getEncoding()->getCode(), nccs->getCode());

   if (nccs == priv->charset) {
      ref();
      return const_cast<QoreStringNode *>(this);
   }
   if (!priv->len)
      return new QoreStringNode(nccs);

   class QoreStringNode *targ = new QoreStringNode(nccs);

   if (convert_encoding_intern(priv->buf, priv->len, priv->charset, *targ, nccs, xsink)) {
      targ->deref();
      return 0;
   }
   return targ;
}

// DLLLOCAL constructor
QoreStringNode::QoreStringNode(const char *str, const class QoreEncoding *from, const class QoreEncoding *to, ExceptionSink *xsink) : SimpleQoreNode(NT_DATE), QoreString(to)
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

class QoreNode *QoreStringNode::realCopy(class ExceptionSink *xsink) const
{
   return copy();
}

QoreStringNode *QoreStringNode::copy() const
{
   return new QoreStringNode(*this);
}

class QoreStringNode *QoreStringNode::substr(int offset) const
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

class QoreStringNode *QoreStringNode::substr(int offset, int length) const
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

class QoreStringNode *QoreStringNode::reverse() const
{
   class QoreStringNode *str = new QoreStringNode(priv->charset);
   concat_reverse(str);
   return str;
}

class QoreStringNode *QoreStringNode::parseBase64ToString(class ExceptionSink *xsink) const
{
   class BinaryObject *b = ::parseBase64(priv->buf, priv->len, xsink);
   if (!b)
      return NULL;

   qore_string_private *p = new qore_string_private;
   p->len = b->size() - 1;
   p->buf = (char *)b->giveBuffer();
   p->charset = QCS_DEFAULT;

   delete b;
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

bool QoreStringNode::is_equal_soft(const QoreNode *v, ExceptionSink *xsink) const
{
   QoreStringValueHelper str(v, getEncoding(), xsink);
   if (*xsink)
      return false;
   return !compare(*str);
}

bool QoreStringNode::is_equal_hard(const QoreNode *v, ExceptionSink *xsink) const
{
   const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(v);
   if (!str)
      return false;

   if (getEncoding() != str->getEncoding())
      return false;

   return !compare(str);
}

// returns the data type
const QoreType *QoreStringNode::getType() const
{
   return NT_STRING;
}

const char *QoreStringNode::getTypeName() const
{
   return "string";
}
