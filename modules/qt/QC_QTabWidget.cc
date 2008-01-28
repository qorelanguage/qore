/*
 QC_QTabWidget.cc
 
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

#include "QC_QTabWidget.h"
#include "QC_QWidget.h"
#include "QC_QIcon.h"
#include "QC_QStyleOptionTabWidgetFrame.h"
#include "QC_QTabBar.h"

#include "qore-qt.h"

int CID_QTABWIDGET;
class QoreClass *QC_QTabWidget = 0;

//QTabWidget ( QWidget * parent = 0 )
static void QTABWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTABWIDGET, new QoreQTabWidget(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTABWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQTabWidget *qtw, ExceptionSink *xsink)
{
   xsink->raiseException("QTABWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//int addTab ( QWidget * child, const QString & label )
//int addTab ( QWidget * child, const QIcon & icon, const QString & label )
static QoreNode *QTABWIDGET_addTab(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *child = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!child) {
      if (!xsink->isException())
	 xsink->raiseException("QTABWIDGET-ADDTAB-PARAM-ERROR", "QTabWidget::addTab() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> childHolder(static_cast<AbstractPrivateData *>(child), xsink);
   
   p = get_param(params, 1);
   QString label;
   if (get_qstring(p, label, xsink, true)) {
      QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink) : 0;
      if (!icon) {
         if (!xsink->isException())
            xsink->raiseException("QTABWIDGET-ADDTAB-PARAM-ERROR", "this version of QTabWidget::addTab() expects an object derived from QIcon as the second argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      p = get_param(params, 2);
      if (get_qstring(p, label, xsink))
	 return 0;
      return new QoreBigIntNode(qtw->qobj->addTab(static_cast<QWidget *>(child->getQWidget()), *(static_cast<QIcon *>(icon)), label));
   }
   return new QoreBigIntNode(qtw->qobj->addTab(static_cast<QWidget *>(child->getQWidget()), label));
}

//void clear ()
static QoreNode *QTABWIDGET_clear(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   qtw->qobj->clear();
   return 0;
}

//QWidget * cornerWidget ( Qt::Corner corner = Qt::TopRightCorner ) const
static QoreNode *QTABWIDGET_cornerWidget(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Corner corner = !is_nothing(p) ? (Qt::Corner)p->getAsInt() : Qt::TopRightCorner;
   QWidget *qt_qobj = qtw->qobj->cornerWidget(corner);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//int count () const
static QoreNode *QTABWIDGET_count(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->count());
}

//int currentIndex () const
static QoreNode *QTABWIDGET_currentIndex(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->currentIndex());
}

//QWidget * currentWidget () const
static QoreNode *QTABWIDGET_currentWidget(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qtw->qobj->currentWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//Qt::TextElideMode elideMode () const
static QoreNode *QTABWIDGET_elideMode(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->elideMode());
}

//QSize iconSize () const
static QoreNode *QTABWIDGET_iconSize(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtw->qobj->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//int indexOf ( QWidget * w ) const
static QoreNode *QTABWIDGET_indexOf(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *w = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!w) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-INDEXOF-PARAM-ERROR", "expecting a QWidget object as first argument to QTabWidget::indexOf()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> wHolder(static_cast<AbstractPrivateData *>(w), xsink);
   return new QoreBigIntNode(qtw->qobj->indexOf(static_cast<QWidget *>(w->getQWidget())));
}

//int insertTab ( int index, QWidget * widget, const QString & label )
//int insertTab ( int index, QWidget * widget, const QIcon & icon, const QString & label )
static QoreNode *QTABWIDGET_insertTab(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
	 xsink->raiseException("QTABWIDGET-INSERTTAB-PARAM-ERROR", "QTabWidget::insertTab() does not know how to handle arguments of class '%s' as passed as the second argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   p = get_param(params, 2);
   QString label;
   if (!get_qstring(p, label, xsink, true))
      return new QoreBigIntNode(qtw->qobj->insertTab(index, static_cast<QWidget *>(widget->getQWidget()), label));

   if (*xsink)
      return 0;

   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
	 xsink->raiseException("QTABWIDGET-INSERTTAB-PARAM-ERROR", "this version of QTabWidget::insertTab() expects an object derived from QIcon as the third argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   p = get_param(params, 3);
   if (get_qstring(p, label, xsink))
      return 0;
   return new QoreBigIntNode(qtw->qobj->insertTab(index, static_cast<QWidget *>(widget->getQWidget()), *(static_cast<QIcon *>(icon)), label));
}

//bool isTabEnabled ( int index ) const
static QoreNode *QTABWIDGET_isTabEnabled(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qtw->qobj->isTabEnabled(index));
}

//void removeTab ( int index )
static QoreNode *QTABWIDGET_removeTab(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtw->qobj->removeTab(index);
   return 0;
}

//void setCornerWidget ( QWidget * widget, Qt::Corner corner = Qt::TopRightCorner )
static QoreNode *QTABWIDGET_setCornerWidget(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-SETCORNERWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QTabWidget::setCornerWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   p = get_param(params, 1);
   Qt::Corner corner = !is_nothing(p) ? (Qt::Corner)p->getAsInt() : Qt::TopRightCorner;
   qtw->qobj->setCornerWidget(static_cast<QWidget *>(widget->getQWidget()), corner);
   return 0;
}

//void setElideMode ( Qt::TextElideMode )
static QoreNode *QTABWIDGET_setElideMode(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextElideMode textelidemode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
   qtw->qobj->setElideMode(textelidemode);
   return 0;
}

//void setIconSize ( const QSize & size )
static QoreNode *QTABWIDGET_setIconSize(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QTabWidget::setIconSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qtw->qobj->setIconSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void setTabEnabled ( int index, bool enable )
static QoreNode *QTABWIDGET_setTabEnabled(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool enable = p ? p->getAsBool() : false;
   qtw->qobj->setTabEnabled(index, enable);
   return 0;
}

//void setTabIcon ( int index, const QIcon & icon )
static QoreNode *QTABWIDGET_setTabIcon(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-SETTABICON-PARAM-ERROR", "expecting a QIcon object as second argument to QTabWidget::setTabIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qtw->qobj->setTabIcon(index, *(static_cast<QIcon *>(icon)));
   return 0;
}

//void setTabPosition ( TabPosition )
static QoreNode *QTABWIDGET_setTabPosition(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QTabWidget::TabPosition tabposition = (QTabWidget::TabPosition)(p ? p->getAsInt() : 0);
   qtw->qobj->setTabPosition(tabposition);
   return 0;
}

//void setTabShape ( TabShape s )
static QoreNode *QTABWIDGET_setTabShape(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QTabWidget::TabShape s = (QTabWidget::TabShape)(p ? p->getAsInt() : 0);
   qtw->qobj->setTabShape(s);
   return 0;
}

//void setTabText ( int index, const QString & label )
static QoreNode *QTABWIDGET_setTabText(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString label;
   if (get_qstring(p, label, xsink))
      return 0;
   qtw->qobj->setTabText(index, label);
   return 0;
}

//void setTabToolTip ( int index, const QString & tip )
static QoreNode *QTABWIDGET_setTabToolTip(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString tip;
   if (get_qstring(p, tip, xsink))
      return 0;
   qtw->qobj->setTabToolTip(index, tip);
   return 0;
}

//void setTabWhatsThis ( int index, const QString & text )
static QoreNode *QTABWIDGET_setTabWhatsThis(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qtw->qobj->setTabWhatsThis(index, text);
   return 0;
}

//void setUsesScrollButtons ( bool useButtons )
static QoreNode *QTABWIDGET_setUsesScrollButtons(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool useButtons = p ? p->getAsBool() : false;
   qtw->qobj->setUsesScrollButtons(useButtons);
   return 0;
}

//QIcon tabIcon ( int index ) const
static QoreNode *QTABWIDGET_tabIcon(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qtw->qobj->tabIcon(index));
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//TabPosition tabPosition () const
static QoreNode *QTABWIDGET_tabPosition(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->tabPosition());
}

//TabShape tabShape () const
static QoreNode *QTABWIDGET_tabShape(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtw->qobj->tabShape());
}

//QString tabText ( int index ) const
static QoreNode *QTABWIDGET_tabText(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreStringNode(qtw->qobj->tabText(index).toUtf8().data(), QCS_UTF8);
}

//QString tabToolTip ( int index ) const
static QoreNode *QTABWIDGET_tabToolTip(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreStringNode(qtw->qobj->tabToolTip(index).toUtf8().data(), QCS_UTF8);
}

//QString tabWhatsThis ( int index ) const
static QoreNode *QTABWIDGET_tabWhatsThis(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreStringNode(qtw->qobj->tabWhatsThis(index).toUtf8().data(), QCS_UTF8);
}

//bool usesScrollButtons () const
static QoreNode *QTABWIDGET_usesScrollButtons(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtw->qobj->usesScrollButtons());
}

//QWidget * widget ( int index ) const
static QoreNode *QTABWIDGET_widget(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = qtw->qobj->widget(index);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//void setCurrentIndex ( int index )
static QoreNode *QTABWIDGET_setCurrentIndex(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtw->qobj->setCurrentIndex(index);
   return 0;
}

//void setCurrentWidget ( QWidget * widget )
static QoreNode *QTABWIDGET_setCurrentWidget(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-SETCURRENTWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QTabWidget::setCurrentWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qtw->qobj->setCurrentWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void initStyleOption ( QStyleOptionTabWidgetFrame * option ) const
static QoreNode *QTABWIDGET_initStyleOption(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQStyleOptionTabWidgetFrame *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionTabWidgetFrame *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONTABWIDGETFRAME, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionTabWidgetFrame object as first argument to QTabWidget::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qtw->initStyleOption(static_cast<QStyleOptionTabWidgetFrame *>(option));
   return 0;
}

//virtual void paintEvent ( QPaintEvent * event )
static QoreNode *QTABWIDGET_paintEvent(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPaintEvent *event = (p && p->type == NT_OBJECT) ? (QoreQPaintEvent *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTEVENT, xsink) : 0;
   if (!event) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-PAINTEVENT-PARAM-ERROR", "expecting a QPaintEvent object as first argument to QTabWidget::paintEvent()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> eventHolder(static_cast<AbstractPrivateData *>(event), xsink);
   qtw->paintEvent(static_cast<QPaintEvent *>(event));
   return 0;
}

//void setTabBar ( QTabBar * tb )
static QoreNode *QTABWIDGET_setTabBar(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTabBar *tb = (p && p->type == NT_OBJECT) ? (QoreQTabBar *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QTABBAR, xsink) : 0;
   if (!tb) {
      if (!xsink->isException())
         xsink->raiseException("QTABWIDGET-SETTABBAR-PARAM-ERROR", "expecting a QTabBar object as first argument to QTabWidget::setTabBar()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> tbHolder(static_cast<AbstractPrivateData *>(tb), xsink);
   qtw->setTabBar(static_cast<QTabBar *>(tb->qobj));
   return 0;
}

//QTabBar * tabBar () const
static QoreNode *QTABWIDGET_tabBar(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QTabBar *qt_qobj = qtw->tabBar();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QTabBar, getProgram());
      QoreQtQTabBar *t_qobj = new QoreQtQTabBar(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QTABBAR, t_qobj);
   }
   return rv_obj;
}

//virtual void tabInserted ( int index )
static QoreNode *QTABWIDGET_tabInserted(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtw->tabInserted(index);
   return 0;
}

//virtual void tabRemoved ( int index )
static QoreNode *QTABWIDGET_tabRemoved(QoreObject *self, QoreQTabWidget *qtw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtw->tabRemoved(index);
   return 0;
}

QoreClass *initQTabWidgetClass(QoreClass *qwidget)
{
   QC_QTabWidget = new QoreClass("QTabWidget", QDOM_GUI);
   CID_QTABWIDGET = QC_QTabWidget->getID();

   QC_QTabWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QTabWidget->setConstructor(QTABWIDGET_constructor);
   QC_QTabWidget->setCopy((q_copy_t)QTABWIDGET_copy);

   QC_QTabWidget->addMethod("addTab",                      (q_method_t)QTABWIDGET_addTab);
   QC_QTabWidget->addMethod("clear",                       (q_method_t)QTABWIDGET_clear);
   QC_QTabWidget->addMethod("cornerWidget",                (q_method_t)QTABWIDGET_cornerWidget);
   QC_QTabWidget->addMethod("count",                       (q_method_t)QTABWIDGET_count);
   QC_QTabWidget->addMethod("currentIndex",                (q_method_t)QTABWIDGET_currentIndex);
   QC_QTabWidget->addMethod("currentWidget",               (q_method_t)QTABWIDGET_currentWidget);
   QC_QTabWidget->addMethod("elideMode",                   (q_method_t)QTABWIDGET_elideMode);
   QC_QTabWidget->addMethod("iconSize",                    (q_method_t)QTABWIDGET_iconSize);
   QC_QTabWidget->addMethod("indexOf",                     (q_method_t)QTABWIDGET_indexOf);
   QC_QTabWidget->addMethod("insertTab",                   (q_method_t)QTABWIDGET_insertTab);
   QC_QTabWidget->addMethod("isTabEnabled",                (q_method_t)QTABWIDGET_isTabEnabled);
   QC_QTabWidget->addMethod("removeTab",                   (q_method_t)QTABWIDGET_removeTab);
   QC_QTabWidget->addMethod("setCornerWidget",             (q_method_t)QTABWIDGET_setCornerWidget);
   QC_QTabWidget->addMethod("setElideMode",                (q_method_t)QTABWIDGET_setElideMode);
   QC_QTabWidget->addMethod("setIconSize",                 (q_method_t)QTABWIDGET_setIconSize);
   QC_QTabWidget->addMethod("setTabEnabled",               (q_method_t)QTABWIDGET_setTabEnabled);
   QC_QTabWidget->addMethod("setTabIcon",                  (q_method_t)QTABWIDGET_setTabIcon);
   QC_QTabWidget->addMethod("setTabPosition",              (q_method_t)QTABWIDGET_setTabPosition);
   QC_QTabWidget->addMethod("setTabShape",                 (q_method_t)QTABWIDGET_setTabShape);
   QC_QTabWidget->addMethod("setTabText",                  (q_method_t)QTABWIDGET_setTabText);
   QC_QTabWidget->addMethod("setTabToolTip",               (q_method_t)QTABWIDGET_setTabToolTip);
   QC_QTabWidget->addMethod("setTabWhatsThis",             (q_method_t)QTABWIDGET_setTabWhatsThis);
   QC_QTabWidget->addMethod("setUsesScrollButtons",        (q_method_t)QTABWIDGET_setUsesScrollButtons);
   QC_QTabWidget->addMethod("tabIcon",                     (q_method_t)QTABWIDGET_tabIcon);
   QC_QTabWidget->addMethod("tabPosition",                 (q_method_t)QTABWIDGET_tabPosition);
   QC_QTabWidget->addMethod("tabShape",                    (q_method_t)QTABWIDGET_tabShape);
   QC_QTabWidget->addMethod("tabText",                     (q_method_t)QTABWIDGET_tabText);
   QC_QTabWidget->addMethod("tabToolTip",                  (q_method_t)QTABWIDGET_tabToolTip);
   QC_QTabWidget->addMethod("tabWhatsThis",                (q_method_t)QTABWIDGET_tabWhatsThis);
   QC_QTabWidget->addMethod("usesScrollButtons",           (q_method_t)QTABWIDGET_usesScrollButtons);
   QC_QTabWidget->addMethod("widget",                      (q_method_t)QTABWIDGET_widget);
   QC_QTabWidget->addMethod("setCurrentIndex",             (q_method_t)QTABWIDGET_setCurrentIndex);
   QC_QTabWidget->addMethod("setCurrentWidget",            (q_method_t)QTABWIDGET_setCurrentWidget);

   QC_QTabWidget->addMethod("initStyleOption",             (q_method_t)QTABWIDGET_initStyleOption, true);
   QC_QTabWidget->addMethod("paintEvent",                  (q_method_t)QTABWIDGET_paintEvent, true);
   QC_QTabWidget->addMethod("setTabBar",                   (q_method_t)QTABWIDGET_setTabBar, true);
   QC_QTabWidget->addMethod("tabBar",                      (q_method_t)QTABWIDGET_tabBar, true);
   QC_QTabWidget->addMethod("tabInserted",                 (q_method_t)QTABWIDGET_tabInserted, true);
   QC_QTabWidget->addMethod("tabRemoved",                  (q_method_t)QTABWIDGET_tabRemoved, true);

   return QC_QTabWidget;
}
