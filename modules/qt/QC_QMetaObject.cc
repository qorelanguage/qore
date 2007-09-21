/*
 QC_QMetaObject.cc
 
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

#include "QC_QMetaObject.h"

int CID_QMETAOBJECT;
class QoreClass *QC_QMetaObject = 0;

static void QMETAOBJECT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QMETAOBJECT-CONSTRUCTOR-ERROR", "objects of this class cannot be manually constructed");
}

static void QMETAOBJECT_copy(class Object *self, class Object *old, class QoreQObject *qo, ExceptionSink *xsink)
{
   xsink->raiseException("QMETAOBJECT-COPY-ERROR", "objects of this class cannot be copied");
}

////QMetaClassInfo classInfo ( int index ) const
//static QoreNode *QMETAOBJECT_classInfo(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int index = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qmo->qobj->classInfo(index));
//}

//int classInfoCount () const
static QoreNode *QMETAOBJECT_classInfoCount(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->classInfoCount());
}

//int classInfoOffset () const
static QoreNode *QMETAOBJECT_classInfoOffset(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->classInfoOffset());
}

//const char * className () const
static QoreNode *QMETAOBJECT_className(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmo->qobj->className()));
}

////QMetaEnum enumerator ( int index ) const
//static QoreNode *QMETAOBJECT_enumerator(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int index = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qmo->qobj->enumerator(index));
//}

//int enumeratorCount () const
static QoreNode *QMETAOBJECT_enumeratorCount(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->enumeratorCount());
}

//int enumeratorOffset () const
static QoreNode *QMETAOBJECT_enumeratorOffset(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->enumeratorOffset());
}

//int indexOfClassInfo ( const char * name ) const
static QoreNode *QMETAOBJECT_indexOfClassInfo(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFCLASSINFO-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfClassInfo()");
      return 0;
   }
   const char *name = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfClassInfo(name));
}

//int indexOfEnumerator ( const char * name ) const
static QoreNode *QMETAOBJECT_indexOfEnumerator(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFENUMERATOR-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfEnumerator()");
      return 0;
   }
   const char *name = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfEnumerator(name));
}

//int indexOfMethod ( const char * method ) const
static QoreNode *QMETAOBJECT_indexOfMethod(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFMETHOD-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfMethod()");
      return 0;
   }
   const char *method = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfMethod(method));
}

//int indexOfProperty ( const char * name ) const
static QoreNode *QMETAOBJECT_indexOfProperty(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFPROPERTY-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfProperty()");
      return 0;
   }
   const char *name = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfProperty(name));
}

//int indexOfSignal ( const char * signal ) const
static QoreNode *QMETAOBJECT_indexOfSignal(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFSIGNAL-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfSignal()");
      return 0;
   }
   const char *signal = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfSignal(signal));
}

//int indexOfSlot ( const char * slot ) const
static QoreNode *QMETAOBJECT_indexOfSlot(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMETAOBJECT-INDEXOFSLOT-PARAM-ERROR", "expecting a string as first argument to QMetaObject::indexOfSlot()");
      return 0;
   }
   const char *slot = p->val.String->getBuffer();
   return new QoreNode((int64)qmo->qobj->indexOfSlot(slot));
}

////QMetaMethod method ( int index ) const
//static QoreNode *QMETAOBJECT_method(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int index = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qmo->qobj->method(index));
//}

//int methodCount () const
static QoreNode *QMETAOBJECT_methodCount(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->methodCount());
}

//int methodOffset () const
static QoreNode *QMETAOBJECT_methodOffset(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->methodOffset());
}

////QMetaProperty property ( int index ) const
//static QoreNode *QMETAOBJECT_property(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int index = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qmo->qobj->property(index));
//}

//int propertyCount () const
static QoreNode *QMETAOBJECT_propertyCount(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->propertyCount());
}

//int propertyOffset () const
static QoreNode *QMETAOBJECT_propertyOffset(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmo->qobj->propertyOffset());
}

//const QMetaObject * superClass () const
static QoreNode *QMETAOBJECT_superClass(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qmo = new Object(self->getClass(CID_QMETAOBJECT), getProgram());
   QoreQMetaObject *q_qmo = new QoreQMetaObject(const_cast<QMetaObject *>(qmo->qobj->superClass()));
   o_qmo->setPrivate(CID_QMETAOBJECT, q_qmo);
   return new QoreNode(o_qmo);
}

////QMetaProperty userProperty () const
//static QoreNode *QMETAOBJECT_userProperty(Object *self, QoreQMetaObject *qmo, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qmo->qobj->userProperty());
//}

QoreClass *initQMetaObjectClass()
{
   QC_QMetaObject = new QoreClass("QMetaObject", QDOM_GUI);
   CID_QMETAOBJECT = QC_QMetaObject->getID();

   QC_QMetaObject->setConstructor(QMETAOBJECT_constructor);
   QC_QMetaObject->setCopy((q_copy_t)QMETAOBJECT_copy);

   //QC_QMetaObject->addMethod("classInfo",                   (q_method_t)QMETAOBJECT_classInfo);
   QC_QMetaObject->addMethod("classInfoCount",              (q_method_t)QMETAOBJECT_classInfoCount);
   QC_QMetaObject->addMethod("classInfoOffset",             (q_method_t)QMETAOBJECT_classInfoOffset);
   QC_QMetaObject->addMethod("className",                   (q_method_t)QMETAOBJECT_className);
   //QC_QMetaObject->addMethod("enumerator",                  (q_method_t)QMETAOBJECT_enumerator);
   QC_QMetaObject->addMethod("enumeratorCount",             (q_method_t)QMETAOBJECT_enumeratorCount);
   QC_QMetaObject->addMethod("enumeratorOffset",            (q_method_t)QMETAOBJECT_enumeratorOffset);
   QC_QMetaObject->addMethod("indexOfClassInfo",            (q_method_t)QMETAOBJECT_indexOfClassInfo);
   QC_QMetaObject->addMethod("indexOfEnumerator",           (q_method_t)QMETAOBJECT_indexOfEnumerator);
   QC_QMetaObject->addMethod("indexOfMethod",               (q_method_t)QMETAOBJECT_indexOfMethod);
   QC_QMetaObject->addMethod("indexOfProperty",             (q_method_t)QMETAOBJECT_indexOfProperty);
   QC_QMetaObject->addMethod("indexOfSignal",               (q_method_t)QMETAOBJECT_indexOfSignal);
   QC_QMetaObject->addMethod("indexOfSlot",                 (q_method_t)QMETAOBJECT_indexOfSlot);
   //QC_QMetaObject->addMethod("method",                      (q_method_t)QMETAOBJECT_method);
   QC_QMetaObject->addMethod("methodCount",                 (q_method_t)QMETAOBJECT_methodCount);
   QC_QMetaObject->addMethod("methodOffset",                (q_method_t)QMETAOBJECT_methodOffset);
   //QC_QMetaObject->addMethod("property",                    (q_method_t)QMETAOBJECT_property);
   QC_QMetaObject->addMethod("propertyCount",               (q_method_t)QMETAOBJECT_propertyCount);
   QC_QMetaObject->addMethod("propertyOffset",              (q_method_t)QMETAOBJECT_propertyOffset);
   QC_QMetaObject->addMethod("superClass",                  (q_method_t)QMETAOBJECT_superClass);
   //QC_QMetaObject->addMethod("userProperty",                (q_method_t)QMETAOBJECT_userProperty);

   return QC_QMetaObject;
}
