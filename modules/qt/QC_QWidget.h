/*
 QC_QWidget.h
 
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

#ifndef _QORE_QC_QWIDGET_H

#define _QORE_QC_QWIDGET_H

#include "QoreAbstractQWidget.h"
#include "QoreAbstractQLayout.h"

#include <QWidget>

DLLEXPORT extern int CID_QWIDGET;

DLLLOCAL class QoreClass *initQWidgetClass();

class QoreQWidget : public QoreAbstractQWidget
{
   public:
      QPointer<QWidget>qobj;

      DLLLOCAL QoreQWidget(QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : qobj(new QWidget(parent, window_flags))
      {
      }
      DLLLOCAL virtual void destructor(class ExceptionSink *xsink)
      {
	 //QObject::disconnect(qobj, SLOT(isDeleted()));
	 if (qobj && !qobj->parent())
	    delete qobj;
      }
      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual QWidget *getQWidget() const
      {
	 return &(*qobj);
      }
};

// template functions for inherited methods
template<typename T>
QoreNode *QW_resize(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-RESIZE-ERROR", "missing first argument: x size");
      return 0;
   }
   int x = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-RESIZE-ERROR", "missing second argument: y size");
      return 0;
   }
   int y = p->getAsInt();

   qw->qobj->resize(x, y);
   return 0;
}

template<typename T>
static QoreNode *QW_setGeometry(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETGEOMETRY-ERROR", "missing first argument: x size");
      return 0;
   }
   int x = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETGEOMETRY-ERROR", "missing second argument: y size");
      return 0;
   }
   int y = p->getAsInt();

   p = get_param(params, 2);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETGEOMETRY-ERROR", "missing third argument: width");
      return 0;
   }
   int w = p->getAsInt();

   p = get_param(params, 3);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETGEOMETRY-ERROR", "missing fourth argument: height");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setGeometry(x, y, w, h);
   return 0;
}

template<typename T>
QoreNode *QW_setFixedWidth(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->qobj->setFixedWidth(w);
   return 0;
}

template<typename T>
QoreNode *QW_setFixedHeight(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setFixedHeight(h);
   return 0;
}

template<typename T>
QoreNode *QW_setFixedSize(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDSIZE-ERROR", "missing first argument: width");
      return 0;
   }
   int w = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDSIZE-ERROR", "missing second argument: height");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setFixedSize(w, h);
   return 0;
}

template<typename T>
QoreNode *QW_setMinimumWidth(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->qobj->setMinimumWidth(w);
   return 0;
}

template<typename T>
QoreNode *QW_setMinimumHeight(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setMinimumHeight(h);
   return 0;
}

template<typename T>
QoreNode *QW_setMinimumSize(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMSIZE-ERROR", "missing first argument: width");
      return 0;
   }
   int w = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMSIZE-ERROR", "missing second argument: height");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setMinimumSize(w, h);
   return 0;
}

template<typename T>
QoreNode *QW_setMaximumWidth(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->qobj->setMaximumWidth(w);
   return 0;
}

template<typename T>
QoreNode *QW_setMaximumHeight(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setMaximumHeight(h);
   return 0;
}

template<typename T>
QoreNode *QW_setMaximumSize(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMSIZE-ERROR", "missing first argument: width");
      return 0;
   }
   int w = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMSIZE-ERROR", "missing second argument: height");
      return 0;
   }
   int h = p->getAsInt();

   qw->qobj->setMaximumSize(w, h);
   return 0;
}

template<typename T>
static QoreNode *QW_show(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->qobj->show();
   return 0;
}

template<typename T>
static QoreNode *QW_setFont(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQFont *qf = p ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : NULL;
   if (!p || !qf)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETFONT-PARAM-EXCEPTION", "expecting a QFont object as parameter to QWidget::setFont()");
      return NULL;
   }
   ReferenceHolder<QoreQFont> holder(qf, xsink);

   qw->qobj->setFont(*((QFont *)qf));
   return 0;
}

template<typename T>
static QoreNode *QW_setLayout(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQLayout *qal = p ? (QoreAbstractQLayout *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QLAYOUT, xsink) : NULL;
   if (!p || !qal)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETLAYOUT-PARAM-ERROR", "expecting a QLayout object as argument to QWidget::setLayout()");
      return NULL;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qal, xsink);

   qw->qobj->setLayout(qal->getQLayout());
   return 0;
}

template<typename T>
static QoreNode *QW_setBackgroundRole(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
   {
      xsink->raiseException("QWIDGET-SETBACKGROUNDROLE-PARAM-ERROR", "missing role value argument for QWidget::setBackgroundRole()");
      return 0;
   }
   qw->qobj->setBackgroundRole(p->getAsInt());
   return 0;
}

template<typename T>
static QoreNode *QW_setForegroundRole(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
   {
      xsink->raiseException("QWIDGET-SETFOREGROUNDROLE-PARAM-ERROR", "missing role value argument for QWidget::setForegroundRole()");
      return 0;
   }
   qw->qobj->setForegroundRole(p->getAsInt());
   return 0;
}

template<typename T>
static QoreNode *QW_backgroundRole(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->backgroundRole());
}

template<typename T>
static QoreNode *QW_foregroundRole(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->foregroundRole());
}

template<typename T>
static QoreNode *QW_updateGeometry(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->qobj->updateGeometry();
   return 0;
}

//void grabKeyboard ()
template<typename T>
static QoreNode *QW_grabKeyboard(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->qobj->grabKeyboard();
   return 0;
}

//void grabMouse ()
template<typename T>
static QoreNode *QW_grabMouse(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->qobj->grabMouse();
   return 0;
}

//bool hasEditFocus () const
template<typename T>
static QoreNode *QW_hasEditFocus(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->hasEditFocus());
}

//bool hasFocus () const
template<typename T>
static QoreNode *QW_hasFocus(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->hasFocus());
}

//bool hasMouseTracking () const
template<typename T>
static QoreNode *QW_hasMouseTracking(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->hasMouseTracking());
}

//int height () const
template<typename T>
static QoreNode *QW_height(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->qobj->height());
}

//virtual int heightForWidth ( int w ) const
template<typename T>
static QoreNode *QW_heightForWidth(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-HEIGHTFORWIDTH-ERROR", "missing width argument in QWidget::heightForWidth()");
      return 0;
   }
   return new QoreNode((int64)qw->qobj->heightForWidth(p->getAsInt()));
}

//bool isActiveWindow () const
template<typename T>
static QoreNode *QW_isActiveWindow(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isActiveWindow());
}

//bool isAncestorOf ( const QWidget * child ) const
template<typename T>
static QoreNode *QW_isAncestorOf(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISANCESTOROF-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isAncestorOf()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qwa, xsink);

   return new QoreNode(qw->qobj->isAncestorOf(qwa->getQWidget()));
}

//bool isEnabled () const
template<typename T>
static QoreNode *QW_isEnabled(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isEnabled());
}

//bool isEnabledTo ( QWidget * ancestor ) const
template<typename T>
static QoreNode *QW_isEnabledTo(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISENABLEDTO-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isEnabledTo()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qwa, xsink);

   return new QoreNode(qw->qobj->isEnabledTo(qwa->getQWidget()));
}

//bool isFullScreen () const
template<typename T>
static QoreNode *QW_isFullScreen(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isFullScreen());
}

//bool isHidden () const
template<typename T>
static QoreNode *QW_isHidden(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isHidden());
}

//bool isMaximized () const
template<typename T>
static QoreNode *QW_isMaximized(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isMaximized());
}

//bool isMinimized () const
template<typename T>
static QoreNode *QW_isMinimized(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isMinimized());
}

//bool isModal () const
template<typename T>
static QoreNode *QW_isModal(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isModal());
}

//bool isVisible () const
template<typename T>
static QoreNode *QW_isVisible(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isVisible());
}

//bool isVisibleTo ( QWidget * ancestor ) const
template<typename T>
static QoreNode *QW_isVisibleTo(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISVISIBLETO-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isVisibleTo()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qwa, xsink);

   return new QoreNode(qw->qobj->isVisibleTo(qwa->getQWidget()));
}

//bool isWindow () const
template<typename T>
static QoreNode *QW_isWindow(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isWindow());
}

//bool isWindowModified () const
template<typename T>
static QoreNode *QW_isWindowModified(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->qobj->isWindowModified());
}

//void grabMouse ( const QCursor & cursor )
/*
template<typename T>
static QoreNode *QW_grabMouse(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
}
*/

//int grabShortcut ( const QKeySequence & key, Qt::ShortcutContext context = Qt::WindowShortcut )
/*
template<typename T>
static QoreNode *QW_grabShortcut(class Object *self, T *qw, class QoreNode *params, ExceptionSink *xsink)
{
}
*/

#endif
