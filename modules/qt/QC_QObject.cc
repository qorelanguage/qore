/*
 QC_QObject.cc
 
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

#include "QC_QObject.h"
#include "QC_QFont.h"
#include "QC_QMetaObject.h"

int CID_QOBJECT;
QoreClass *QC_QObject = 0;

static void QOBJECT_constructor(class QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreQObject *qo;
   int np = num_params(params);
   if (!np)
      qo = new QoreQObject(self);
   else 
   {
      QoreNode *p = test_param(params, NT_OBJECT, 0);
      QoreAbstractQObject *parent = p ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (!parent)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QOBJECT-CONSTRUCTOR-ERROR", "expecting an object derived from QObject as parameter to QObject::constructor() in first argument if passed, argument is either not an argument or not derived from QObject (type passed: %s)", p ? p->type->getName() : "NOTHING");
	 return;
      }
      ReferenceHolder<QoreAbstractQObject> holder(parent, xsink);
      qo = new QoreQObject(self, parent->getQObject());
   }

   self->setPrivate(CID_QOBJECT, qo);
}

static void QOBJECT_copy(class QoreObject *self, class QoreObject *old, class QoreQObject *qo, ExceptionSink *xsink)
{
   xsink->raiseException("QOBJECT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool blockSignals ( bool block )
static QoreNode *QOBJECT_blockSignals(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool block = p ? p->getAsBool() : 0;
   return new QoreNode(qo->getQObject()->blockSignals(block));
}

//const QObjectQoreList & children () const
//static QoreNode *QOBJECT_children(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qo->getQObject()->children());
//}

//bool connect ( const QObject * sender, const char * signal, const char * method, Qt::ConnectionType type = Qt::AutoCompatConnection ) const
static QoreNode *QOBJECT_connect(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QoreAbstractQObject *sender = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!p || !sender)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-CONNECT-PARAM-ERROR", "expecting a QObject object as first argument to QObject::connect()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQObject> holder(sender, xsink);

   QoreStringNode *pstr = test_string_param(params, 1);
   if (!pstr) {
      xsink->raiseException("QOBJECT-CONNECT-PARAM-ERROR", "expecting a string as second argument to QObject::connect()");
      return 0;
   }
   const char *signal = pstr->getBuffer();

   pstr = test_string_param(params, 2);
   if (!pstr) {
      xsink->raiseException("QOBJECT-CONNECT-PARAM-ERROR", "expecting a string as third argument to QObject::connect()");
      return 0;
   }
   const char *method = pstr->getBuffer();

   //p = get_param(params, 3);
   //Qt::ConnectionType type = (Qt::ConnectionType)(p ? p->getAsInt() : 0);
   //return new QoreNode(qo->getQObject()->connect(sender->getQObject(), signal, method, type));

   qo->connectDynamic(sender, signal, method, xsink);
   return 0;
}

//bool disconnect ( const char * signal = 0, const QObject * receiver = 0, const char * method = 0 )
//bool disconnect ( const QObject * receiver, const char * method = 0 )
static QoreNode *QOBJECT_disconnect(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   int offset = 0;

   QoreNode *p = get_param(params, 0);
   if (!p || (p->type != NT_STRING && p->type != NT_OBJECT)) {
      xsink->raiseException("QOBJECT-DISCONNECT-PARAM-ERROR", "expecting a string or QObject as first argument to QObject::disconnect()");
      return 0;
   }

   const char *signal = 0;
   {
      QoreStringNode *str = dynamic_cast<QoreStringNode *>(p);
      if (str) {
	 signal = str->getBuffer();
	 offset = 1;
      }
   }

   p = get_param(params, offset++);
   QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!p || !receiver)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-DISCONNECT-PARAM-ERROR", "expecting a QObject object argument to QObject::disconnect()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQObject> holder(receiver, xsink);

   QoreStringNode *pstr = test_string_param(params, offset);
   if (!pstr) {
      xsink->raiseException("QOBJECT-DISCONNECT-PARAM-ERROR", "expecting a string as third argument to QObject::disconnect()");
      return 0;
   }
   const char *method = pstr->getBuffer();

   if (signal)
      return new QoreNode(qo->getQObject()->disconnect(signal, receiver->getQObject(), method));

   return new QoreNode(qo->getQObject()->disconnect(receiver->getQObject(), method));
}

//void dumpObjectInfo ()
static QoreNode *QOBJECT_dumpObjectInfo(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   qo->getQObject()->dumpObjectInfo();
   return 0;
}

//void dumpObjectTree ()
static QoreNode *QOBJECT_dumpObjectTree(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   qo->getQObject()->dumpObjectTree();
   return 0;
}

//QList<QByteArray> dynamicPropertyNames () const
//static QoreNode *QOBJECT_dynamicPropertyNames(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qo->getQObject()->dynamicPropertyNames());
//}

//virtual bool event ( QEvent * e )
//static QoreNode *QOBJECT_event(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QEvent* e = p;
//   return new QoreNode(qo->getQObject()->event(e));
//}

//virtual bool eventFilter ( QObject * watched, QEvent * event )
//static QoreNode *QOBJECT_eventFilter(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   QoreAbstractQObject *watched = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
//   if (!p || !watched)
//   {
//      if (!xsink->isException())
//         xsink->raiseException("QOBJECT-EVENTFILTER-PARAM-ERROR", "expecting a QObject object as first argument to QObject::eventFilter()");
//      return 0;
//   }
//   ReferenceHolder<QoreAbstractQObject> holder(watched, xsink);
//   p = get_param(params, 1);
//   ??? QEvent* event = p;
//   return new QoreNode(qo->getQObject()->eventFilter(watched->getQObject(), event));
//}

//T findChild ( const QString & name = QString() ) const
//static QoreNode *QOBJECT_findChild(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QOBJECT-FINDCHILD-PARAM-ERROR", "expecting a string as first argument to QObject::findChild()");
//      return 0;
//   }
//   const char *name = p->getBuffer();
//   ??? return new QoreNode((int64)qo->getQObject()->findChild(name));
//}

//QList<T> findChildren ( const QString & name = QString() ) const
//static QoreNode *QOBJECT_findChildren(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (!p || p->type != NT_STRING) {
//      xsink->raiseException("QOBJECT-FINDCHILDREN-PARAM-ERROR", "expecting a string as first argument to QObject::findChildren()");
//      return 0;
//   }
//   const char *name = p->getBuffer();
//   ??? return new QoreNode((int64)qo->getQObject()->findChildren(name));
//}
//QList<T> findChildren ( const QRegExp & regExp ) const
//static QoreNode *QOBJECT_findChildren(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QRegExp& regExp = p;
//   ??? return new QoreNode((int64)qo->getQObject()->findChildren(regExp));
//}

//bool inherits ( const char * className ) const
static QoreNode *QOBJECT_inherits(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QOBJECT-INHERITS-PARAM-ERROR", "expecting a string as first argument to QObject::inherits()");
      return 0;
   }
   const char *className = p->getBuffer();
   return new QoreNode(qo->getQObject()->inherits(className));
}

//void installEventFilter ( QObject * filterObj )
static QoreNode *QOBJECT_installEventFilter(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *filterObj = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!p || !filterObj)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-INSTALLEVENTFILTER-PARAM-ERROR", "expecting a QObject object as first argument to QObject::installEventFilter()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQObject> holder(filterObj, xsink);
   qo->getQObject()->installEventFilter(filterObj->getQObject());
   return 0;
}

//bool isWidgetType () const
static QoreNode *QOBJECT_isWidgetType(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qo->getQObject()->isWidgetType());
}

//void killTimer ( int id )
static QoreNode *QOBJECT_killTimer(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   qo->getQObject()->killTimer(id);
   return 0;
}

//virtual const QMetaObject * metaObject () const
static QoreNode *QOBJECT_metaObject(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qmo = new QoreObject(QC_QMetaObject, getProgram());
   QoreQMetaObject *q_qmo = new QoreQMetaObject(const_cast<QMetaObject *>(qo->getQObject()->metaObject()));
   o_qmo->setPrivate(CID_QMETAOBJECT, q_qmo);
   return new QoreNode(o_qmo);
}

//void moveToThread ( QThread * targetThread )
//static QoreNode *QOBJECT_moveToThread(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QThread* targetThread = p;
//   qo->getQObject()->moveToThread(targetThread);
//   return 0;
//}

//QString objectName () const
static QoreNode *QOBJECT_objectName(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qo->getQObject()->objectName().toUtf8().data(), QCS_UTF8);
}

//QObject * parent () const
static QoreNode *QOBJECT_parent(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QObject *parent = qo->getQObject()->parent();

   if (!parent)
      return 0;

   QVariant qv_ptr = parent->property("qobject");
   QoreObject *obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   //printd(0, "sender=%08p class=%s\n", obj, obj->getClass()->getName());
   assert(obj);
   obj->ref();
   return new QoreNode(obj);
}

//QVariant property ( const char * name ) const
static QoreNode *QOBJECT_property(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QOBJECT-PROPERTY-PARAM-ERROR", "expecting a string as first argument to QObject::property()");
      return 0;
   }
   const char *name = p->getBuffer();
   return return_qvariant(qo->getQObject()->property(name));
}

//void removeEventFilter ( QObject * obj )
static QoreNode *QOBJECT_removeEventFilter(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *obj = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!p || !obj)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-REMOVEEVENTFILTER-PARAM-ERROR", "expecting a QObject object as first argument to QObject::removeEventFilter()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQObject> holder(obj, xsink);
   qo->getQObject()->removeEventFilter(obj->getQObject());
   return 0;
}

//void setObjectName ( const QString & name )
static QoreNode *QOBJECT_setObjectName(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   qo->getQObject()->setObjectName(name);
   return 0;
}

//void setParent ( QObject * parent )
static QoreNode *QOBJECT_setParent(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!p || !parent)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-SETPARENT-PARAM-ERROR", "expecting a QObject object as first argument to QObject::setParent()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQObject> holder(parent, xsink);
   qo->getQObject()->setParent(parent->getQObject());
   return 0;
}

//bool setProperty ( const char * name, const QVariant & value )
static QoreNode *QOBJECT_setProperty(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *pstr = test_string_param(params, 0);
   if (!pstr) {
      xsink->raiseException("QOBJECT-SETPROPERTY-PARAM-ERROR", "expecting a string as first argument to QObject::setProperty()");
      return 0;
   }
   const char *name = pstr->getBuffer();

   QoreNode *p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;

   return new QoreNode(qo->getQObject()->setProperty(name, value));
}

//bool signalsBlocked () const
static QoreNode *QOBJECT_signalsBlocked(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qo->getQObject()->signalsBlocked());
}

//int startTimer ( int interval )
static QoreNode *QOBJECT_startTimer(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int interval = p ? p->getAsInt() : 0;
   return new QoreNode((int64)qo->getQObject()->startTimer(interval));
}

//QThread * thread () const
//static QoreNode *QOBJECT_thread(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qo->getQObject()->thread();
//}

// custom methods
static QoreNode *QOBJECT_createSignal(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QOBJECT-CREATE-SIGNAL-ERROR", "no string argument passed to QObject::createSignal()");
      return 0;
   }

   qo->createSignal(p->getBuffer(), xsink);
   return 0;
}

static QoreNode *QOBJECT_emit(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QOBJECT-EMIT-ERROR", "no string argument passed to QObject::emit()");
      return 0;
   }

   qo->emit_signal(p->getBuffer(), params->val.list);
   return 0;
}

static QoreNode *QOBJECT_sender(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QObject *sender = qo->sender();
   if (!sender)
      return 0;

   QVariant qv_ptr = sender->property("qobject");
   QoreObject *obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   //printd(0, "sender=%08p class=%s\n", obj, obj->getClass()->getName());
   assert(obj);
   obj->ref();
   return new QoreNode(obj);
}

//virtual void childEvent ( QChildEvent * event )
static QoreNode *QOBJECT_childEvent(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQChildEvent *event = (p && p->type == NT_OBJECT) ? (QoreQChildEvent *)p->val.object->getReferencedPrivateData(CID_QCHILDEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-CHILDEVENT-PARAM-ERROR", "expecting a QChildEvent object as first argument to QObject::childEvent()");
      return 0;
   }
   ReferenceHolder<QoreQChildEvent> eventHolder(event, xsink);
   qo->childEvent(static_cast<QChildEvent *>(event));
   return 0;
}

//virtual void timerEvent ( QTimerEvent * event )
static QoreNode *QOBJECT_timerEvent(QoreObject *self, QoreAbstractQObject *qo, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTimerEvent *event = (p && p->type == NT_OBJECT) ? (QoreQTimerEvent *)p->val.object->getReferencedPrivateData(CID_QTIMEREVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-TIMEREVENT-PARAM-ERROR", "expecting a QTimerEvent object as first argument to QObject::timerEvent()");
      return 0;
   }
   ReferenceHolder<QoreQTimerEvent> eventHolder(event, xsink);
   qo->timerEvent(static_cast<QTimerEvent *>(event));
   return 0;
}

class QoreClass *initQObjectClass()
{
   tracein("initQObjectClass()");
   
   QC_QObject = new QoreClass("QObject", QDOM_GUI);
   CID_QOBJECT = QC_QObject->getID();

   QC_QObject->setConstructor(QOBJECT_constructor);
   QC_QObject->setCopy((q_copy_t)QOBJECT_copy);
  
   QC_QObject->addMethod("blockSignals",                (q_method_t)QOBJECT_blockSignals);
   //QC_QObject->addMethod("children",                    (q_method_t)QOBJECT_children);
   QC_QObject->addMethod("connect",                     (q_method_t)QOBJECT_connect);
   QC_QObject->addMethod("disconnect",                  (q_method_t)QOBJECT_disconnect);
   QC_QObject->addMethod("dumpObjectInfo",              (q_method_t)QOBJECT_dumpObjectInfo);
   QC_QObject->addMethod("dumpObjectTree",              (q_method_t)QOBJECT_dumpObjectTree);
   //QC_QObject->addMethod("dynamicPropertyNames",        (q_method_t)QOBJECT_dynamicPropertyNames);
   //QC_QObject->addMethod("event",                       (q_method_t)QOBJECT_event);
   //QC_QObject->addMethod("eventFilter",                 (q_method_t)QOBJECT_eventFilter);
   //QC_QObject->addMethod("findChild",                   (q_method_t)QOBJECT_findChild);
   //QC_QObject->addMethod("findChildren",                (q_method_t)QOBJECT_findChildren);
   QC_QObject->addMethod("inherits",                    (q_method_t)QOBJECT_inherits);
   QC_QObject->addMethod("installEventFilter",          (q_method_t)QOBJECT_installEventFilter);
   QC_QObject->addMethod("isWidgetType",                (q_method_t)QOBJECT_isWidgetType);
   QC_QObject->addMethod("killTimer",                   (q_method_t)QOBJECT_killTimer);
   QC_QObject->addMethod("metaObject",                  (q_method_t)QOBJECT_metaObject);
   //QC_QObject->addMethod("moveToThread",                (q_method_t)QOBJECT_moveToThread);
   QC_QObject->addMethod("objectName",                  (q_method_t)QOBJECT_objectName);
   QC_QObject->addMethod("parent",                      (q_method_t)QOBJECT_parent);
   QC_QObject->addMethod("property",                    (q_method_t)QOBJECT_property);
   QC_QObject->addMethod("removeEventFilter",           (q_method_t)QOBJECT_removeEventFilter);
   QC_QObject->addMethod("setObjectName",               (q_method_t)QOBJECT_setObjectName);
   QC_QObject->addMethod("setParent",                   (q_method_t)QOBJECT_setParent);
   QC_QObject->addMethod("setProperty",                 (q_method_t)QOBJECT_setProperty);
   QC_QObject->addMethod("signalsBlocked",              (q_method_t)QOBJECT_signalsBlocked);
   QC_QObject->addMethod("startTimer",                  (q_method_t)QOBJECT_startTimer);
   //QC_QObject->addMethod("thread",                      (q_method_t)QOBJECT_thread);

   // custom methods
   QC_QObject->addMethod("createSignal",                (q_method_t)QOBJECT_createSignal);
   QC_QObject->addMethod("emit",                        (q_method_t)QOBJECT_emit);

   // private methods
   QC_QObject->addMethod("sender",                      (q_method_t)QOBJECT_sender, true);
   QC_QObject->addMethod("childEvent",                  (q_method_t)QOBJECT_childEvent, true);
   QC_QObject->addMethod("timerEvent",                  (q_method_t)QOBJECT_timerEvent, true);

   traceout("initQObjectClass()");
   return QC_QObject;
}
