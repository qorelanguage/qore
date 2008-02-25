/*
 QC_QByteArray.cc
 
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

#include "QC_QByteArray.h"

#include "qore-qt.h"

int CID_QBYTEARRAY;
class QoreClass *QC_QByteArray = 0;

//QByteArray ()
//QByteArray ( const char * str )
//QByteArray ( const QByteArray & other )
static void QBYTEARRAY_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QBYTEARRAY, new QoreQByteArray());
      return;
   }
   qore_type_t ntype = p->getType();

   if (ntype == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p);
      self->setPrivate(CID_QBYTEARRAY, new QoreQByteArray(pstr->getBuffer(), pstr->strlen()));
      return;
   }
   if (p->getType() != NT_BINARY) {
      xsink->raiseException("QBYTEARRAY-CONSTRUCTOR-ERROR", "expecting a string or binary object as sole argument to QByteArray::constructor()");
      return;
   }
   const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
   const char *data = (const char *)b->getPtr();
   self->setPrivate(CID_QBYTEARRAY, new QoreQByteArray(data, b->size()));
   return;
}

static void QBYTEARRAY_copy(class QoreObject *self, class QoreObject *old, class QoreQByteArray *qba, ExceptionSink *xsink)
{
   self->setPrivate(CID_QBYTEARRAY, new QoreQByteArray(*qba));
}

//QByteArray & append ( const QByteArray & ba )
//QByteArray & append ( const QString & str )
////QByteArray & append ( const char * str )
////QByteArray & append ( char ch )
static AbstractQoreNode *QBYTEARRAY_append(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   QString str;
   if (!get_qstring(p, str, xsink, true))
      qba->append(str);
   else {
      QByteArray qba_a;

      if (get_qbytearray(p, qba_a, xsink, true)) {
	 xsink->raiseException("QBYTEARRAY-APPEND-PARAM-ERROR", "QByteArray::append() was expecting either a string or a binary object to append");
	 return 0;
      }
      qba->append(qba_a);
   }

   // returns itself
   self->ref();
   return self;
}

//const char at ( int i ) const
static AbstractQoreNode *QBYTEARRAY_at(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   char c_rv = qba->at(i);
   QoreStringNode *rv_str = new QoreStringNode();
   rv_str->concat(c_rv);
   return rv_str;
}

//int capacity () const
static AbstractQoreNode *QBYTEARRAY_capacity(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qba->capacity());
}

//void chop ( int n )
static AbstractQoreNode *QBYTEARRAY_chop(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int n = p ? p->getAsInt() : 0;
   qba->chop(n);
   return 0;
}

//void clear ()
static AbstractQoreNode *QBYTEARRAY_clear(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   qba->clear();
   return 0;
}

//const char * constData () const
static AbstractQoreNode *QBYTEARRAY_constData(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   BinaryNode *b = new BinaryNode();
   b->append(qba->constData(), qba->size());
   return b;
}

////bool contains ( const QByteArray & ba ) const
////bool contains ( const char * str ) const
////bool contains ( char ch ) const
//static AbstractQoreNode *QBYTEARRAY_contains(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      return get_bool_node(qba->contains(ba));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-CONTAINS-PARAM-ERROR", "expecting a string as first argument to QByteArray::contains()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      return get_bool_node(qba->contains(str));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   return get_bool_node(qba->contains(ch));
//}

////int count ( const QByteArray & ba ) const
////int count ( const char * str ) const
////int count ( char ch ) const
////int count () const
//static AbstractQoreNode *QBYTEARRAY_count(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (is_nothing(p)) {
//      return new QoreBigIntNode(qba->count());
//   }
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      return new QoreBigIntNode(qba->count(ba));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-COUNT-PARAM-ERROR", "expecting a string as first argument to QByteArray::count()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      return new QoreBigIntNode(qba->count(str));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   return new QoreBigIntNode(qba->count(ch));
//}

//char * data ()
//const char * data () const
static AbstractQoreNode *QBYTEARRAY_data(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   BinaryNode *b = new BinaryNode();
   b->append(qba->constData(), qba->size());
   return b;
}

////DataPtr & data_ptr ()
//static AbstractQoreNode *QBYTEARRAY_data_ptr(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qba->data_ptr());
//}

////bool endsWith ( const QByteArray & ba ) const
////bool endsWith ( const char * str ) const
////bool endsWith ( char ch ) const
//static AbstractQoreNode *QBYTEARRAY_endsWith(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      return get_bool_node(qba->endsWith(ba));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-ENDSWITH-PARAM-ERROR", "expecting a string as first argument to QByteArray::endsWith()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      return get_bool_node(qba->endsWith(str));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   return get_bool_node(qba->endsWith(ch));
//}

//QByteArray & fill ( char ch, int size = -1 )
static AbstractQoreNode *QBYTEARRAY_fill(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen()) {
      xsink->raiseException("QBYTEARRAY-FILL-ERROR", "Expecting a string giving the character to use to fill the array as first argument");
      return 0;
   }
   char ch = p->getBuffer()[0];
   const AbstractQoreNode *pn = get_param(params, 1);
   int size = !is_nothing(pn) ? p->getAsInt() : -1;

   qba->fill(ch, size);

   // return self
   self->ref();
   return self;
}

////int indexOf ( const QByteArray & ba, int from = 0 ) const
////int indexOf ( const QString & str, int from = 0 ) const
////int indexOf ( const char * str, int from = 0 ) const
////int indexOf ( char ch, int from = 0 ) const
//static AbstractQoreNode *QBYTEARRAY_indexOf(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//   p = get_param(params, 1);
//   int from = p ? p->getAsInt() : 0;
//   return new QoreBigIntNode(qba->indexOf(ba, from));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-INDEXOF-PARAM-ERROR", "expecting a string as first argument to QByteArray::indexOf()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//   p = get_param(params, 1);
//   int from = p ? p->getAsInt() : 0;
//   return new QoreBigIntNode(qba->indexOf(str, from));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-INDEXOF-PARAM-ERROR", "expecting a string as first argument to QByteArray::indexOf()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//   p = get_param(params, 1);
//   int from = p ? p->getAsInt() : 0;
//   return new QoreBigIntNode(qba->indexOf(str, from));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   int from = p ? p->getAsInt() : 0;
//   return new QoreBigIntNode(qba->indexOf(ch, from));
//}

////QByteArray & insert ( int i, const QByteArray & ba )
////QByteArray & insert ( int i, const QString & str )
////QByteArray & insert ( int i, const char * str )
////QByteArray & insert ( int i, char ch )
//static AbstractQoreNode *QBYTEARRAY_insert(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   int i = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->insert(i, ba));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-INSERT-PARAM-ERROR", "expecting a string as second argument to QByteArray::insert()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->insert(i, str));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-INSERT-PARAM-ERROR", "expecting a string as second argument to QByteArray::insert()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->insert(i, str));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->insert(i, ch));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//}

//bool isEmpty () const
static AbstractQoreNode *QBYTEARRAY_isEmpty(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qba->isEmpty());
}

//bool isNull () const
static AbstractQoreNode *QBYTEARRAY_isNull(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qba->isNull());
}

////int lastIndexOf ( const QByteArray & ba, int from = -1 ) const
////int lastIndexOf ( const QString & str, int from = -1 ) const
////int lastIndexOf ( const char * str, int from = -1 ) const
////int lastIndexOf ( char ch, int from = -1 ) const
//static AbstractQoreNode *QBYTEARRAY_lastIndexOf(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//   p = get_param(params, 1);
//   int from = !is_nothing(p) ? p->getAsInt() : -1;
//   return new QoreBigIntNode(qba->lastIndexOf(ba, from));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-LASTINDEXOF-PARAM-ERROR", "expecting a string as first argument to QByteArray::lastIndexOf()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//   p = get_param(params, 1);
//   int from = !is_nothing(p) ? p->getAsInt() : -1;
//   return new QoreBigIntNode(qba->lastIndexOf(str, from));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-LASTINDEXOF-PARAM-ERROR", "expecting a string as first argument to QByteArray::lastIndexOf()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//   p = get_param(params, 1);
//   int from = !is_nothing(p) ? p->getAsInt() : -1;
//   return new QoreBigIntNode(qba->lastIndexOf(str, from));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   int from = !is_nothing(p) ? p->getAsInt() : -1;
//   return new QoreBigIntNode(qba->lastIndexOf(ch, from));
//}

//QByteArray left ( int len ) const
static AbstractQoreNode *QBYTEARRAY_left(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int len = p ? p->getAsInt() : 0;
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->left(len));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray leftJustified ( int width, char fill = ' ', bool truncate = false ) const
static AbstractQoreNode *QBYTEARRAY_leftJustified(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;

   const QoreStringNode *str = test_string_param(params, 1);
   char fill = str ? str->getBuffer()[0] : ' ';

   p = get_param(params, 2);
   bool truncate = p ? p->getAsBool() : false;

   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->leftJustified(width, fill, truncate));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//int length () const
static AbstractQoreNode *QBYTEARRAY_length(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qba->length());
}

//QByteArray mid ( int pos, int len = -1 ) const
static AbstractQoreNode *QBYTEARRAY_mid(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int len = !is_nothing(p) ? p->getAsInt() : -1;
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->mid(pos, len));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

////QByteArray & prepend ( const QByteArray & ba )
////QByteArray & prepend ( const char * str )
////QByteArray & prepend ( char ch )
//static AbstractQoreNode *QBYTEARRAY_prepend(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->prepend(ba));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-PREPEND-PARAM-ERROR", "expecting a string as first argument to QByteArray::prepend()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->prepend(str));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->prepend(ch));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//}

////void push_back ( const QByteArray & other )
////void push_back ( const char * str )
////void push_back ( char ch )
//static AbstractQoreNode *QBYTEARRAY_push_back(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray other;
//      if (get_qbytearray(p, other, xsink))
//         return 0;
//      qba->push_back(other);
//      return 0;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-PUSH_BACK-PARAM-ERROR", "expecting a string as first argument to QByteArray::push_back()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      qba->push_back(str);
//      return 0;
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   qba->push_back(ch);
//   return 0;
//}

////void push_front ( const QByteArray & other )
////void push_front ( const char * str )
////void push_front ( char ch )
//static AbstractQoreNode *QBYTEARRAY_push_front(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray other;
//      if (get_qbytearray(p, other, xsink))
//         return 0;
//      qba->push_front(other);
//      return 0;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-PUSH_FRONT-PARAM-ERROR", "expecting a string as first argument to QByteArray::push_front()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      qba->push_front(str);
//      return 0;
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   qba->push_front(ch);
//   return 0;
//}

//QByteArray & remove ( int pos, int len )
static AbstractQoreNode *QBYTEARRAY_remove(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int len = p ? p->getAsInt() : 0;

   qba->remove(pos, len);

   // returns itself
   self->ref();
   return self;
}

////QByteArray & replace ( int pos, int len, const QByteArray & after )
////QByteArray & replace ( int pos, int len, const char * after )
////QByteArray & replace ( const QByteArray & before, const QByteArray & after )
////QByteArray & replace ( const char * before, const QByteArray & after )
////QByteArray & replace ( const QByteArray & before, const char * after )
////QByteArray & replace ( const QString & before, const QByteArray & after )
////QByteArray & replace ( const QString & before, const char * after )
////QByteArray & replace ( const char * before, const char * after )
////QByteArray & replace ( char before, const QByteArray & after )
////QByteArray & replace ( char before, const QString & after )
////QByteArray & replace ( char before, const char * after )
////QByteArray & replace ( char before, char after )
//static AbstractQoreNode *QBYTEARRAY_replace(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray before;
//      if (get_qbytearray(p, before, xsink))
//         return 0;
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as first argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *before = p->getBuffer();
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   if (p && p->getType() == NT_???) {
//      QByteArray before;
//      if (get_qbytearray(p, before, xsink))
//         return 0;
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as first argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *before = p->getBuffer();
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as first argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *before = p->getBuffer();
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as first argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *before = p->getBuffer();
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(before, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//   }
//   int pos = p ? p->getAsInt() : 0;
//   p = get_param(params, 1);
//   if (p && p->getType() == NT_???) {
//      QByteArray after;
//      if (get_qbytearray(p, after, xsink))
//         return 0;
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(pos, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(pos, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-REPLACE-PARAM-ERROR", "expecting a string as second argument to QByteArray::replace()");
//         return 0;
//      }
//      const char *after = p->getBuffer();
//      QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//      QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(pos, after));
//      o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//      return o_qba;
//   }
//   int len = p ? p->getAsInt() : 0;
//   p = get_param(params, 2);
//   QByteArray after;
//   if (get_qbytearray(p, after, xsink))
//      return 0;
//   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
//   QoreQByteArray *q_qba = new QoreQByteArray(qba->replace(pos, len, after));
//   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
//   return o_qba;
//}

//void reserve ( int size )
static AbstractQoreNode *QBYTEARRAY_reserve(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qba->reserve(size);
   return 0;
}

//void resize ( int size )
static AbstractQoreNode *QBYTEARRAY_resize(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qba->resize(size);
   return 0;
}

//QByteArray right ( int len ) const
static AbstractQoreNode *QBYTEARRAY_right(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int len = p ? p->getAsInt() : 0;
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->right(len));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray rightJustified ( int width, char fill = ' ', bool truncate = false ) const
static AbstractQoreNode *QBYTEARRAY_rightJustified(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;

   const QoreStringNode *str = test_string_param(params, 1);
   char fill = str ? str->getBuffer()[0] : ' ';

   p = get_param(params, 2);
   bool truncate = p ? p->getAsBool() : false;

   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->rightJustified(width, fill, truncate));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray & setNum ( int n, int base = 10 )
//QByteArray & setNum ( uint n, int base = 10 )
//QByteArray & setNum ( short n, int base = 10 )
//QByteArray & setNum ( ushort n, int base = 10 )
//QByteArray & setNum ( qlonglong n, int base = 10 )
//QByteArray & setNum ( qulonglong n, int base = 10 )
//QByteArray & setNum ( double n, char f = 'g', int prec = 6 )
//QByteArray & setNum ( float n, char f = 'g', int prec = 6 )
static AbstractQoreNode *QBYTEARRAY_setNum(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_INT) {
      int n = reinterpret_cast<const QoreBigIntNode *>(p)->val;
      p = get_param(params, 1);
      int base = (!is_nothing(p) ? p->getAsInt() : 10);

      qba->setNum(n, base);
   }
   else {
      double n = p ? p->getAsFloat() : 0.0;
      p = get_param(params, 1);
      char f;
      if (p && p->getType() == NT_STRING)
	 f = (reinterpret_cast<const QoreStringNode *>(p))->getBuffer()[0];
      else
	 f = 'g';
      p = get_param(params, 2);
      int prec = !is_nothing(p) ? p->getAsInt() : 6;

      qba->setNum(n, f, prec);
   }

   // returns itself
   self->ref();
   return self;
}

//QByteArray simplified () const
static AbstractQoreNode *QBYTEARRAY_simplified(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->simplified());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//int size () const
static AbstractQoreNode *QBYTEARRAY_size(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qba->size());
}

////QList<QByteArray> split ( char sep ) const
//static AbstractQoreNode *QBYTEARRAY_split(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   QByteArray::char sep = (QByteArray::char)(p ? p->getAsInt() : 0);
//   ??? return new QoreBigIntNode(qba->split(sep));
//}

//void squeeze ()
static AbstractQoreNode *QBYTEARRAY_squeeze(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   qba->squeeze();
   return 0;
}

////bool startsWith ( const QByteArray & ba ) const
////bool startsWith ( const char * str ) const
////bool startsWith ( char ch ) const
//static AbstractQoreNode *QBYTEARRAY_startsWith(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   if (p && p->getType() == NT_???) {
//      QByteArray ba;
//      if (get_qbytearray(p, ba, xsink))
//         return 0;
//      return get_bool_node(qba->startsWith(ba));
//   }
//   if (p && p->getType() == NT_STRING) {
//      if (!p || p->getType() != NT_STRING) {
//         xsink->raiseException("QBYTEARRAY-STARTSWITH-PARAM-ERROR", "expecting a string as first argument to QByteArray::startsWith()");
//         return 0;
//      }
//      const char *str = p->getBuffer();
//      return get_bool_node(qba->startsWith(str));
//   }
//   QByteArray::char ch = (QByteArray::char)(p ? p->getAsInt() : 0);
//   return get_bool_node(qba->startsWith(ch));
//}

//QByteArray toBase64 () const
static AbstractQoreNode *QBYTEARRAY_toBase64(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->toBase64());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

////double toDouble ( bool * ok = 0 ) const
//static AbstractQoreNode *QBYTEARRAY_toDouble(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   return new QoreFloatNode((double)qba->toDouble(ok));
//}

////float toFloat ( bool * ok = 0 ) const
//static AbstractQoreNode *QBYTEARRAY_toFloat(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   return new QoreFloatNode((double)qba->toFloat(ok));
//}

//QByteArray toHex () const
static AbstractQoreNode *QBYTEARRAY_toHex(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->toHex());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

////int toInt ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toInt(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   return new QoreBigIntNode(qba->toInt(ok, base));
//}

////long toLong ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toLong(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   ??? return new QoreBigIntNode(qba->toLong(ok, base));
//}

////qlonglong toLongLong ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toLongLong(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   ??? return new QoreBigIntNode(qba->toLongLong(ok, base));
//}

//QByteArray toLower () const
static AbstractQoreNode *QBYTEARRAY_toLower(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->toLower());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

////short toShort ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toShort(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   return new QoreBigIntNode(qba->toShort(ok, base));
//}

////uint toUInt ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toUInt(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   return new QoreBigIntNode(qba->toUInt(ok, base));
//}

////ulong toULong ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toULong(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   return new QoreBigIntNode(qba->toULong(ok, base));
//}

////qulonglong toULongLong ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toULongLong(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   ??? return new QoreBigIntNode(qba->toULongLong(ok, base));
//}

////ushort toUShort ( bool * ok = 0, int base = 10 ) const
//static AbstractQoreNode *QBYTEARRAY_toUShort(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   p = get_param(params, 1);
//   int base = !is_nothing(p) ? p->getAsInt() : 10;
//   return new QoreBigIntNode(qba->toUShort(ok, base));
//}

//QByteArray toUpper () const
static AbstractQoreNode *QBYTEARRAY_toUpper(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->toUpper());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QByteArray trimmed () const
static AbstractQoreNode *QBYTEARRAY_trimmed(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(self->getClass(CID_QBYTEARRAY), getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qba->trimmed());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//void truncate ( int pos )
static AbstractQoreNode *QBYTEARRAY_truncate(QoreObject *self, QoreQByteArray *qba, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   qba->truncate(pos);
   return 0;
}

QoreClass *initQByteArrayClass()
{
   QC_QByteArray = new QoreClass("QByteArray", QDOM_GUI);
   CID_QBYTEARRAY = QC_QByteArray->getID();

   QC_QByteArray->setConstructor(QBYTEARRAY_constructor);
   QC_QByteArray->setCopy((q_copy_t)QBYTEARRAY_copy);

   QC_QByteArray->addMethod("append",                      (q_method_t)QBYTEARRAY_append);
   QC_QByteArray->addMethod("at",                          (q_method_t)QBYTEARRAY_at);
   QC_QByteArray->addMethod("capacity",                    (q_method_t)QBYTEARRAY_capacity);
   QC_QByteArray->addMethod("chop",                        (q_method_t)QBYTEARRAY_chop);
   QC_QByteArray->addMethod("clear",                       (q_method_t)QBYTEARRAY_clear);
   QC_QByteArray->addMethod("constData",                   (q_method_t)QBYTEARRAY_constData);
   //QC_QByteArray->addMethod("contains",                    (q_method_t)QBYTEARRAY_contains);
   //QC_QByteArray->addMethod("count",                       (q_method_t)QBYTEARRAY_count);
   QC_QByteArray->addMethod("data",                        (q_method_t)QBYTEARRAY_data);
   //QC_QByteArray->addMethod("data_ptr",                    (q_method_t)QBYTEARRAY_data_ptr);
   //QC_QByteArray->addMethod("endsWith",                    (q_method_t)QBYTEARRAY_endsWith);
   QC_QByteArray->addMethod("fill",                        (q_method_t)QBYTEARRAY_fill);
   //QC_QByteArray->addMethod("indexOf",                     (q_method_t)QBYTEARRAY_indexOf);
   //QC_QByteArray->addMethod("insert",                      (q_method_t)QBYTEARRAY_insert);
   QC_QByteArray->addMethod("isEmpty",                     (q_method_t)QBYTEARRAY_isEmpty);
   QC_QByteArray->addMethod("isNull",                      (q_method_t)QBYTEARRAY_isNull);
   //QC_QByteArray->addMethod("lastIndexOf",                 (q_method_t)QBYTEARRAY_lastIndexOf);
   QC_QByteArray->addMethod("left",                        (q_method_t)QBYTEARRAY_left);
   QC_QByteArray->addMethod("leftJustified",               (q_method_t)QBYTEARRAY_leftJustified);
   QC_QByteArray->addMethod("length",                      (q_method_t)QBYTEARRAY_length);
   QC_QByteArray->addMethod("mid",                         (q_method_t)QBYTEARRAY_mid);
   //QC_QByteArray->addMethod("prepend",                     (q_method_t)QBYTEARRAY_prepend);
   //QC_QByteArray->addMethod("push_back",                   (q_method_t)QBYTEARRAY_push_back);
   //QC_QByteArray->addMethod("push_front",                  (q_method_t)QBYTEARRAY_push_front);
   QC_QByteArray->addMethod("remove",                      (q_method_t)QBYTEARRAY_remove);
   //QC_QByteArray->addMethod("replace",                     (q_method_t)QBYTEARRAY_replace);
   QC_QByteArray->addMethod("reserve",                     (q_method_t)QBYTEARRAY_reserve);
   QC_QByteArray->addMethod("resize",                      (q_method_t)QBYTEARRAY_resize);
   QC_QByteArray->addMethod("right",                       (q_method_t)QBYTEARRAY_right);
   QC_QByteArray->addMethod("rightJustified",              (q_method_t)QBYTEARRAY_rightJustified);
   QC_QByteArray->addMethod("setNum",                      (q_method_t)QBYTEARRAY_setNum);
   QC_QByteArray->addMethod("simplified",                  (q_method_t)QBYTEARRAY_simplified);
   QC_QByteArray->addMethod("size",                        (q_method_t)QBYTEARRAY_size);
   //QC_QByteArray->addMethod("split",                       (q_method_t)QBYTEARRAY_split);
   QC_QByteArray->addMethod("squeeze",                     (q_method_t)QBYTEARRAY_squeeze);
   //QC_QByteArray->addMethod("startsWith",                  (q_method_t)QBYTEARRAY_startsWith);
   QC_QByteArray->addMethod("toBase64",                    (q_method_t)QBYTEARRAY_toBase64);
   //QC_QByteArray->addMethod("toDouble",                    (q_method_t)QBYTEARRAY_toDouble);
   //QC_QByteArray->addMethod("toFloat",                     (q_method_t)QBYTEARRAY_toFloat);
   QC_QByteArray->addMethod("toHex",                       (q_method_t)QBYTEARRAY_toHex);
   //QC_QByteArray->addMethod("toInt",                       (q_method_t)QBYTEARRAY_toInt);
   //QC_QByteArray->addMethod("toLong",                      (q_method_t)QBYTEARRAY_toLong);
   //QC_QByteArray->addMethod("toLongLong",                  (q_method_t)QBYTEARRAY_toLongLong);
   QC_QByteArray->addMethod("toLower",                     (q_method_t)QBYTEARRAY_toLower);
   //QC_QByteArray->addMethod("toShort",                     (q_method_t)QBYTEARRAY_toShort);
   //QC_QByteArray->addMethod("toUInt",                      (q_method_t)QBYTEARRAY_toUInt);
   //QC_QByteArray->addMethod("toULong",                     (q_method_t)QBYTEARRAY_toULong);
   //QC_QByteArray->addMethod("toULongLong",                 (q_method_t)QBYTEARRAY_toULongLong);
   //QC_QByteArray->addMethod("toUShort",                    (q_method_t)QBYTEARRAY_toUShort);
   QC_QByteArray->addMethod("toUpper",                     (q_method_t)QBYTEARRAY_toUpper);
   QC_QByteArray->addMethod("trimmed",                     (q_method_t)QBYTEARRAY_trimmed);
   QC_QByteArray->addMethod("truncate",                    (q_method_t)QBYTEARRAY_truncate);

   return QC_QByteArray;
}
