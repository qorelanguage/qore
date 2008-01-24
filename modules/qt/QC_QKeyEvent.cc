/*
 QC_QKeyEvent.cc
 
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

#include "QC_QKeyEvent.h"

int CID_QKEYEVENT;

class QoreClass *QC_QKeyEvent = 0;

static void QKEYEVENT_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreQKeyEvent *qke;

   QoreNode *p = get_param(params, 0);
   QEvent::Type type = (QEvent::Type)(p ? p->getAsInt() : 0);

   p = get_param(params, 1);
   int key = p ? p->getAsInt() : 0;

   p = get_param(params, 2);
   Qt::KeyboardModifiers km = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);

   QoreStringNode *str = test_string_param(params, 3);
   const char *text = str ? str->getBuffer() : 0;

   p = get_param(params, 4);
   bool autorep = p ? p->getAsBool() : false;

   p = get_param(params, 5);
   ushort count = !is_nothing(p) ? p->getAsInt() : 1;

   if (text)
      qke = new QoreQKeyEvent(type, key, km, text, autorep, count);
   else
      qke = new QoreQKeyEvent(type, key, km, QString(), autorep, count);

   self->setPrivate(CID_QKEYEVENT, qke);
}

static void QKEYEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQKeyEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QKEYEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//int count () const
static QoreNode *QKEYEVENT_count(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->count());
}

//bool isAutoRepeat () const
static QoreNode *QKEYEVENT_isAutoRepeat(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qke->isAutoRepeat());
}

//int key () const
static QoreNode *QKEYEVENT_key(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->key());
}

//bool matches ( QKeySequence::StandardKey key ) const
static QoreNode *QKEYEVENT_matches(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QKeySequence::StandardKey key = (QKeySequence::StandardKey)(p ? p->getAsInt() : 0);
   return new QoreNode(qke->matches(key));
}

//Qt::KeyboardModifiers modifiers () const
static QoreNode *QKEYEVENT_modifiers(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->modifiers());
}

//quint32 nativeModifiers () const
static QoreNode *QKEYEVENT_nativeModifiers(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->nativeModifiers());
}

//quint32 nativeScanCode () const
static QoreNode *QKEYEVENT_nativeScanCode(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->nativeScanCode());
}

//quint32 nativeVirtualKey () const
static QoreNode *QKEYEVENT_nativeVirtualKey(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qke->nativeVirtualKey());
}

//QString text () const
static QoreNode *QKEYEVENT_text(QoreObject *self, QoreQKeyEvent *qke, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qke->text().toUtf8().data(), QCS_UTF8);
}

class QoreClass *initQKeyEventClass(class QoreClass *qinputevent)
{
   tracein("initQKeyEventClass()");
   
   QC_QKeyEvent = new QoreClass("QKeyEvent", QDOM_GUI);
   CID_QKEYEVENT = QC_QKeyEvent->getID();

   QC_QKeyEvent->addBuiltinVirtualBaseClass(qinputevent);

   QC_QKeyEvent->setConstructor(QKEYEVENT_constructor);
   QC_QKeyEvent->setCopy((q_copy_t)QKEYEVENT_copy);

   QC_QKeyEvent->addMethod("count",                       (q_method_t)QKEYEVENT_count);
   QC_QKeyEvent->addMethod("isAutoRepeat",                (q_method_t)QKEYEVENT_isAutoRepeat);
   QC_QKeyEvent->addMethod("key",                         (q_method_t)QKEYEVENT_key);
   QC_QKeyEvent->addMethod("matches",                     (q_method_t)QKEYEVENT_matches);
   QC_QKeyEvent->addMethod("modifiers",                   (q_method_t)QKEYEVENT_modifiers);
   QC_QKeyEvent->addMethod("nativeModifiers",             (q_method_t)QKEYEVENT_nativeModifiers);
   QC_QKeyEvent->addMethod("nativeScanCode",              (q_method_t)QKEYEVENT_nativeScanCode);
   QC_QKeyEvent->addMethod("nativeVirtualKey",            (q_method_t)QKEYEVENT_nativeVirtualKey);
   QC_QKeyEvent->addMethod("text",                        (q_method_t)QKEYEVENT_text);

   traceout("initQKeyEventClass()");
   return QC_QKeyEvent;
}
