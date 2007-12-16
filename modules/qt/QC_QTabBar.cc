/*
 QC_QTabBar.cc
 
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

#include "QC_QTabBar.h"

int CID_QTABBAR;
class QoreClass *QC_QTabBar = 0;

//QTabBar ( QWidget * parent = 0 )
static void QTABBAR_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTABBAR, new QoreQTabBar(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTABBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQTabBar *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTABBAR-COPY-ERROR", "objects of this class cannot be copied");
}

//int addTab ( const QString & text )
//int addTab ( const QIcon & icon, const QString & text )
static QoreNode *QTABBAR_addTab(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         if (!xsink->isException())
            xsink->raiseException("QTABBAR-ADDTAB-PARAM-ERROR", "QTabBar::addTab() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      p = get_param(params, 1);
      QString text;
      if (get_qstring(p, text, xsink))
         return 0;
      return new QoreNode((int64)qtb->getQTabBar()->addTab(*(static_cast<QIcon *>(icon)), text));
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   return new QoreNode((int64)qtb->getQTabBar()->addTab(text));
}

//int count () const
static QoreNode *QTABBAR_count(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQTabBar()->count());
}

//int currentIndex () const
static QoreNode *QTABBAR_currentIndex(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQTabBar()->currentIndex());
}

//bool drawBase () const
static QoreNode *QTABBAR_drawBase(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->getQTabBar()->drawBase());
}

//Qt::TextElideMode elideMode () const
static QoreNode *QTABBAR_elideMode(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQTabBar()->elideMode());
}

//QSize iconSize () const
static QoreNode *QTABBAR_iconSize(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->getQTabBar()->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//int insertTab ( int index, const QString & text )
//int insertTab ( int index, const QIcon & icon, const QString & text )
static QoreNode *QTABBAR_insertTab(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);

   QString text;
   if (!get_qstring(p, text, xsink, true))
      return new QoreNode((int64)qtb->getQTabBar()->insertTab(index, text));

   if (*xsink)
      return 0;

   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-INSERTTAB-PARAM-ERROR", "this version of QTabBar::insertTab() expects an object derived from QIcon as the second argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   p = get_param(params, 2);
   if (get_qstring(p, text, xsink))
      return 0;
   return new QoreNode((int64)qtb->getQTabBar()->insertTab(index, *(static_cast<QIcon *>(icon)), text));
}

//bool isTabEnabled ( int index ) const
static QoreNode *QTABBAR_isTabEnabled(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreNode(qtb->getQTabBar()->isTabEnabled(index));
}

//void removeTab ( int index )
static QoreNode *QTABBAR_removeTab(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtb->getQTabBar()->removeTab(index);
   return 0;
}

//void setDrawBase ( bool drawTheBase )
static QoreNode *QTABBAR_setDrawBase(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool drawTheBase = p ? p->getAsBool() : false;
   qtb->getQTabBar()->setDrawBase(drawTheBase);
   return 0;
}

//void setElideMode ( Qt::TextElideMode )
static QoreNode *QTABBAR_setElideMode(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::TextElideMode textelidemode = (Qt::TextElideMode)(p ? p->getAsInt() : 0);
   qtb->getQTabBar()->setElideMode(textelidemode);
   return 0;
}

//void setIconSize ( const QSize & size )
static QoreNode *QTABBAR_setIconSize(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QTabBar::setIconSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qtb->getQTabBar()->setIconSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void setShape ( Shape shape )
static QoreNode *QTABBAR_setShape(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QTabBar::Shape shape = (QTabBar::Shape)(p ? p->getAsInt() : 0);
   qtb->getQTabBar()->setShape(shape);
   return 0;
}

//void setTabData ( int index, const QVariant & data )
static QoreNode *QTABBAR_setTabData(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QVariant data;
   if (get_qvariant(p, data, xsink))
      return 0;
   qtb->getQTabBar()->setTabData(index, data);
   return 0;
}

//void setTabEnabled ( int index, bool enabled )
static QoreNode *QTABBAR_setTabEnabled(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool enabled = p ? p->getAsBool() : false;
   qtb->getQTabBar()->setTabEnabled(index, enabled);
   return 0;
}

//void setTabIcon ( int index, const QIcon & icon )
static QoreNode *QTABBAR_setTabIcon(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-SETTABICON-PARAM-ERROR", "expecting a QIcon object as second argument to QTabBar::setTabIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qtb->getQTabBar()->setTabIcon(index, *(static_cast<QIcon *>(icon)));
   return 0;
}

//void setTabText ( int index, const QString & text )
static QoreNode *QTABBAR_setTabText(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qtb->getQTabBar()->setTabText(index, text);
   return 0;
}

//void setTabTextColor ( int index, const QColor & color )
static QoreNode *QTABBAR_setTabTextColor(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQColor *color = (p && p->type == NT_OBJECT) ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-SETTABTEXTCOLOR-PARAM-ERROR", "expecting a QColor object as second argument to QTabBar::setTabTextColor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> colorHolder(static_cast<AbstractPrivateData *>(color), xsink);
   qtb->getQTabBar()->setTabTextColor(index, *(static_cast<QColor *>(color)));
   return 0;
}

//void setTabToolTip ( int index, const QString & tip )
static QoreNode *QTABBAR_setTabToolTip(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString tip;
   if (get_qstring(p, tip, xsink))
      return 0;
   qtb->getQTabBar()->setTabToolTip(index, tip);
   return 0;
}

//void setTabWhatsThis ( int index, const QString & text )
static QoreNode *QTABBAR_setTabWhatsThis(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qtb->getQTabBar()->setTabWhatsThis(index, text);
   return 0;
}

//void setUsesScrollButtons ( bool useButtons )
static QoreNode *QTABBAR_setUsesScrollButtons(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool useButtons = p ? p->getAsBool() : false;
   qtb->getQTabBar()->setUsesScrollButtons(useButtons);
   return 0;
}

//Shape shape () const
static QoreNode *QTABBAR_shape(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQTabBar()->shape());
}

//int tabAt ( const QPoint & position ) const
static QoreNode *QTABBAR_tabAt(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-TABAT-PARAM-ERROR", "expecting a QPoint object as first argument to QTabBar::tabAt()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   return new QoreNode((int64)qtb->getQTabBar()->tabAt(*(static_cast<QPoint *>(position))));
}

//QVariant tabData ( int index ) const
static QoreNode *QTABBAR_tabData(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return return_qvariant(qtb->getQTabBar()->tabData(index));
}

//QIcon tabIcon ( int index ) const
static QoreNode *QTABBAR_tabIcon(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qtb->getQTabBar()->tabIcon(index));
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QRect tabRect ( int index ) const
static QoreNode *QTABBAR_tabRect(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qtb->getQTabBar()->tabRect(index));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QString tabText ( int index ) const
static QoreNode *QTABBAR_tabText(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(qtb->getQTabBar()->tabText(index).toUtf8().data(), QCS_UTF8));
}

//QColor tabTextColor ( int index ) const
static QoreNode *QTABBAR_tabTextColor(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qc = new QoreObject(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qtb->getQTabBar()->tabTextColor(index));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//QString tabToolTip ( int index ) const
static QoreNode *QTABBAR_tabToolTip(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(qtb->getQTabBar()->tabToolTip(index).toUtf8().data(), QCS_UTF8));
}

//QString tabWhatsThis ( int index ) const
static QoreNode *QTABBAR_tabWhatsThis(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(qtb->getQTabBar()->tabWhatsThis(index).toUtf8().data(), QCS_UTF8));
}

//bool usesScrollButtons () const
static QoreNode *QTABBAR_usesScrollButtons(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->getQTabBar()->usesScrollButtons());
}

//void setCurrentIndex ( int index )
static QoreNode *QTABBAR_setCurrentIndex(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtb->getQTabBar()->setCurrentIndex(index);
   return 0;
}

//void initStyleOption ( QStyleOptionTab * option, int tabIndex ) const
static QoreNode *QTABBAR_initStyleOption(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQStyleOptionTab *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionTab *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONTAB, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QTABBAR-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionTab object as first argument to QTabBar::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 1);
   int tabIndex = p ? p->getAsInt() : 0;
   qtb->initStyleOption(static_cast<QStyleOptionTab *>(option), tabIndex);
   return 0;
}

//virtual void tabInserted ( int index )
static QoreNode *QTABBAR_tabInserted(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtb->tabInserted(index);
   return 0;
}

//virtual void tabLayoutChange ()
static QoreNode *QTABBAR_tabLayoutChange(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   qtb->tabLayoutChange();
   return 0;
}

//virtual void tabRemoved ( int index )
static QoreNode *QTABBAR_tabRemoved(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qtb->tabRemoved(index);
   return 0;
}

//virtual QSize tabSizeHint ( int index ) const
static QoreNode *QTABBAR_tabSizeHint(QoreObject *self, QoreAbstractQTabBar *qtb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->tabSizeHint(index));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

QoreClass *initQTabBarClass(QoreClass *qwidget)
{
   QC_QTabBar = new QoreClass("QTabBar", QDOM_GUI);
   CID_QTABBAR = QC_QTabBar->getID();

   QC_QTabBar->addBuiltinVirtualBaseClass(qwidget);

   QC_QTabBar->setConstructor(QTABBAR_constructor);
   QC_QTabBar->setCopy((q_copy_t)QTABBAR_copy);

   QC_QTabBar->addMethod("addTab",                      (q_method_t)QTABBAR_addTab);
   QC_QTabBar->addMethod("count",                       (q_method_t)QTABBAR_count);
   QC_QTabBar->addMethod("currentIndex",                (q_method_t)QTABBAR_currentIndex);
   QC_QTabBar->addMethod("drawBase",                    (q_method_t)QTABBAR_drawBase);
   QC_QTabBar->addMethod("elideMode",                   (q_method_t)QTABBAR_elideMode);
   QC_QTabBar->addMethod("iconSize",                    (q_method_t)QTABBAR_iconSize);
   QC_QTabBar->addMethod("insertTab",                   (q_method_t)QTABBAR_insertTab);
   QC_QTabBar->addMethod("isTabEnabled",                (q_method_t)QTABBAR_isTabEnabled);
   QC_QTabBar->addMethod("removeTab",                   (q_method_t)QTABBAR_removeTab);
   QC_QTabBar->addMethod("setDrawBase",                 (q_method_t)QTABBAR_setDrawBase);
   QC_QTabBar->addMethod("setElideMode",                (q_method_t)QTABBAR_setElideMode);
   QC_QTabBar->addMethod("setIconSize",                 (q_method_t)QTABBAR_setIconSize);
   QC_QTabBar->addMethod("setShape",                    (q_method_t)QTABBAR_setShape);
   QC_QTabBar->addMethod("setTabData",                  (q_method_t)QTABBAR_setTabData);
   QC_QTabBar->addMethod("setTabEnabled",               (q_method_t)QTABBAR_setTabEnabled);
   QC_QTabBar->addMethod("setTabIcon",                  (q_method_t)QTABBAR_setTabIcon);
   QC_QTabBar->addMethod("setTabText",                  (q_method_t)QTABBAR_setTabText);
   QC_QTabBar->addMethod("setTabTextColor",             (q_method_t)QTABBAR_setTabTextColor);
   QC_QTabBar->addMethod("setTabToolTip",               (q_method_t)QTABBAR_setTabToolTip);
   QC_QTabBar->addMethod("setTabWhatsThis",             (q_method_t)QTABBAR_setTabWhatsThis);
   QC_QTabBar->addMethod("setUsesScrollButtons",        (q_method_t)QTABBAR_setUsesScrollButtons);
   QC_QTabBar->addMethod("shape",                       (q_method_t)QTABBAR_shape);
   QC_QTabBar->addMethod("tabAt",                       (q_method_t)QTABBAR_tabAt);
   QC_QTabBar->addMethod("tabData",                     (q_method_t)QTABBAR_tabData);
   QC_QTabBar->addMethod("tabIcon",                     (q_method_t)QTABBAR_tabIcon);
   QC_QTabBar->addMethod("tabRect",                     (q_method_t)QTABBAR_tabRect);
   QC_QTabBar->addMethod("tabText",                     (q_method_t)QTABBAR_tabText);
   QC_QTabBar->addMethod("tabTextColor",                (q_method_t)QTABBAR_tabTextColor);
   QC_QTabBar->addMethod("tabToolTip",                  (q_method_t)QTABBAR_tabToolTip);
   QC_QTabBar->addMethod("tabWhatsThis",                (q_method_t)QTABBAR_tabWhatsThis);
   QC_QTabBar->addMethod("usesScrollButtons",           (q_method_t)QTABBAR_usesScrollButtons);
   QC_QTabBar->addMethod("setCurrentIndex",             (q_method_t)QTABBAR_setCurrentIndex);

   QC_QTabBar->addMethod("initStyleOption",             (q_method_t)QTABBAR_initStyleOption, true);
   QC_QTabBar->addMethod("tabInserted",                 (q_method_t)QTABBAR_tabInserted, true);
   QC_QTabBar->addMethod("tabLayoutChange",             (q_method_t)QTABBAR_tabLayoutChange, true);
   QC_QTabBar->addMethod("tabRemoved",                  (q_method_t)QTABBAR_tabRemoved, true);
   QC_QTabBar->addMethod("tabSizeHint",                 (q_method_t)QTABBAR_tabSizeHint, true);

   return QC_QTabBar;
}
