/*
 QC_QWidget.cc
 
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

#include "QC_QWidget.h"
#include "QC_QFont.h"
#include "QC_QLayout.h"
#include "QC_QPalette.h"
#include "QC_QPaintEvent.h"
#include "QC_QRect.h"
#include "QC_QRegion.h"

int CID_QWIDGET;

static void QWIDGET_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQWidget *qw;
   static QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQWidget(self);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      p = get_param(params, 1);
      int window_flags = p ? p->getAsInt() : 0;
      qw = new QoreQWidget(self, parent->getQWidget(), (Qt::WindowFlags)window_flags);
   }

   self->setPrivate(CID_QWIDGET, qw);
}

static void QWIDGET_copy(class Object *self, class Object *old, class QoreQWidget *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//bool acceptDrops () const
static QoreNode *QWIDGET_acceptDrops(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->acceptDrops());
}

//QString accessibleDescription () const
static QoreNode *QWIDGET_accessibleDescription(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->accessibleDescription().toUtf8().data(), QCS_UTF8));
}

//QString accessibleName () const
static QoreNode *QWIDGET_accessibleName(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->accessibleName().toUtf8().data(), QCS_UTF8));
}

//QList<QAction *> actions () const
//static QoreNode *QWIDGET_actions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void activateWindow ()
static QoreNode *QWIDGET_activateWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->adjustSize();
   return 0;
}

//void addAction ( QAction * action )
static QoreNode *QWIDGET_addAction(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ADDACTION-PARAM-ERROR", "expecting a QAction object as first argument to QWidget::addAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   qw->getQWidget()->addAction(static_cast<QAction *>(action->qobj));
   return 0;
}

//void addActions ( QList<QAction *> actions )
//static QoreNode *QWIDGET_addActions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void adjustSize ()
static QoreNode *QWIDGET_adjustSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->adjustSize();
   return 0;
}

//bool autoFillBackground () const
static QoreNode *QWIDGET_autoFillBackground(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->autoFillBackground());
}

//QPalette::ColorRole backgroundRole () const
static QoreNode *QWIDGET_backgroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->backgroundRole());
}

//QSize baseSize () const
//static QoreNode *QWIDGET_baseSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * childAt ( int x, int y ) const
//QWidget * childAt ( const QPoint & p ) const
/*
static QoreNode *QWIDGET_childAt(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->childAt(x, y));
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//QRect childrenRect () const
static QoreNode *QWIDGET_childrenRect(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->childrenRect());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QRegion childrenRegion () const
static QoreNode *QWIDGET_childrenRegion(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRegion *q_qr = new QoreQRegion(qw->getQWidget()->childrenRegion());
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//void clearFocus ()
static QoreNode *QWIDGET_clearFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->clearFocus();
   return 0;
}

//void clearMask ()
static QoreNode *QWIDGET_clearMask(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->clearMask();
   return 0;
}

//QRect contentsRect () const
static QoreNode *QWIDGET_contentsRect(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->contentsRect());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//Qt::ContextMenuPolicy contextMenuPolicy () const
static QoreNode *QWIDGET_contextMenuPolicy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->contextMenuPolicy());
}

//QCursor cursor () const
//static QoreNode *QWIDGET_cursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void ensurePolished () const
static QoreNode *QWIDGET_ensurePolished(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->ensurePolished();
   return 0;
}

//Qt::FocusPolicy focusPolicy () const
static QoreNode *QWIDGET_focusPolicy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->focusPolicy());
}

//QWidget * focusProxy () const
/*
static QoreNode *QWIDGET_focusProxy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->focusProxy());
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//QWidget * focusWidget () const
/*
static QoreNode *QWIDGET_focusWidget(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->focusWidget());
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//const QFont & font () const
static QoreNode *QWIDGET_font(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qf = new Object(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qw->getQWidget()->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//QFontInfo fontInfo () const
//static QoreNode *QWIDGET_fontInfo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QFontMetrics fontMetrics () const
//static QoreNode *QWIDGET_fontMetrics(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPalette::ColorRole foregroundRole () const
static QoreNode *QWIDGET_foregroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->foregroundRole());
}

//QRect frameGeometry () const
static QoreNode *QWIDGET_frameGeometry(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->frameGeometry());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QSize frameSize () const
static QoreNode *QWIDGET_frameSize(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->frameSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//const QRect & geometry () const
static QoreNode *QWIDGET_geometry(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->geometry());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void getContentsMargins ( int * left, int * top, int * right, int * bottom ) const
//static QoreNode *QWIDGET_getContentsMargins(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual HDC getDC () const
//static QoreNode *QWIDGET_getDC(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void grabKeyboard ()
static QoreNode *QWIDGET_grabKeyboard(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->grabKeyboard();
   return 0;
}

//void grabMouse ()
//void grabMouse ( const QCursor & cursor )
static QoreNode *QWIDGET_grabMouse(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->grabMouse();
   return 0;
}

//int grabShortcut ( const QKeySequence & key, Qt::ShortcutContext context = Qt::WindowShortcut )
static QoreNode *QWIDGET_grabShortcut(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQKeySequence *key = (p && p->type == NT_OBJECT) ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   if (!key) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-GRABSHORTCUT-PARAM-ERROR", "expecting a QKeySequence object as first argument to QWidget::grabShortcut()");
      return 0;
   }
   ReferenceHolder<QoreQKeySequence> keyHolder(key, xsink);
   p = get_param(params, 1);
   Qt::ShortcutContext context = (Qt::ShortcutContext)(p ? p->getAsInt() : 0);
   return new QoreNode((int64)qw->getQWidget()->grabShortcut(*(static_cast<QKeySequence *>(key)), context));
}

#ifdef QT_KEYPAD_NAVIGATION
//bool hasEditFocus () const
static QoreNode *QWIDGET_hasEditFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasEditFocus());
}
#endif

//bool hasFocus () const
static QoreNode *QWIDGET_hasFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasFocus());
}

//bool hasMouseTracking () const
static QoreNode *QWIDGET_hasMouseTracking(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasMouseTracking());
}

//int height () const
static QoreNode *QWIDGET_height(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->height());
}

//virtual int heightForWidth ( int w ) const
static QoreNode *QWIDGET_heightForWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-HEIGHTFORWIDTH-ERROR", "missing width argument in QWidget::heightForWidth()");
      return 0;
   }
   return new QoreNode((int64)qw->getQWidget()->heightForWidth(p->getAsInt()));
}

//QInputContext * inputContext ()
//static QoreNode *QWIDGET_inputContext(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
//static QoreNode *QWIDGET_inputMethodQuery(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void insertAction ( QAction * before, QAction * action )
static QoreNode *QWIDGET_insertAction(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-INSERTACTION-PARAM-ERROR", "expecting a QAction object as first argument to QWidget::insertAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> beforeHolder(before, xsink);
   p = get_param(params, 1);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-INSERTACTION-PARAM-ERROR", "expecting a QAction object as second argument to QWidget::insertAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   qw->getQWidget()->insertAction(static_cast<QAction *>(before->qobj), static_cast<QAction *>(action->qobj));
   return 0;
}

//void insertActions ( QAction * before, QList<QAction *> actions )
//static QoreNode *QWIDGET_insertActions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//bool isActiveWindow () const
static QoreNode *QWIDGET_isActiveWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isActiveWindow());
}

//bool isAncestorOf ( const QWidget * child ) const
static QoreNode *QWIDGET_isAncestorOf(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISANCESTOROF-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isAncestorOf()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(qwa, xsink);

   return new QoreNode(qw->getQWidget()->isAncestorOf(qwa->getQWidget()));
}

//bool isEnabled () const
static QoreNode *QWIDGET_isEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isEnabled());
}

//bool isEnabledTo ( QWidget * ancestor ) const
static QoreNode *QWIDGET_isEnabledTo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISENABLEDTO-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isEnabledTo()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(qwa, xsink);

   return new QoreNode(qw->getQWidget()->isEnabledTo(qwa->getQWidget()));
}

//bool isFullScreen () const
static QoreNode *QWIDGET_isFullScreen(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isFullScreen());
}

//bool isHidden () const
static QoreNode *QWIDGET_isHidden(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isHidden());
}

//bool isMaximized () const
static QoreNode *QWIDGET_isMaximized(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isMaximized());
}

//bool isMinimized () const
static QoreNode *QWIDGET_isMinimized(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isMinimized());
}

//bool isModal () const
static QoreNode *QWIDGET_isModal(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isModal());
}

//bool isVisible () const
static QoreNode *QWIDGET_isVisible(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isVisible());
}

//bool isVisibleTo ( QWidget * ancestor ) const
static QoreNode *QWIDGET_isVisibleTo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *qwa = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !qwa)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-ISVISIBLETO-PARAM-ERROR", "expecting a QWidget object as argument to QWidget::isVisibleTo()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(qwa, xsink);

   return new QoreNode(qw->getQWidget()->isVisibleTo(qwa->getQWidget()));
}

//bool isWindow () const
static QoreNode *QWIDGET_isWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isWindow());
}

//bool isWindowModified () const
static QoreNode *QWIDGET_isWindowModified(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isWindowModified());
}

//QLayout * layout () const
//static QoreNode *QWIDGET_layout(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::LayoutDirection layoutDirection () const
static QoreNode *QWIDGET_layoutDirection(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->layoutDirection());
}

//QLocale locale () const
//static QoreNode *QWIDGET_locale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE macCGHandle () const
//static QoreNode *QWIDGET_macCGHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE macQDHandle () const
//static QoreNode *QWIDGET_macQDHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapFrom ( QWidget * parent, const QPoint & pos ) const
static QoreNode *QWIDGET_mapFrom(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPFROM-PARAM-ERROR", "expecting a QWidget object as first argument to QWidget::mapFrom()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   p = get_param(params, 1);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPFROM-PARAM-ERROR", "expecting a QPoint object as second argument to QWidget::mapFrom()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapFrom(parent->getQWidget(), *(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint mapFromGlobal ( const QPoint & pos ) const
static QoreNode *QWIDGET_mapFromGlobal(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPFROMGLOBAL-PARAM-ERROR", "expecting a QPoint object as first argument to QWidget::mapFromGlobal()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapFromGlobal(*(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint mapFromParent ( const QPoint & pos ) const
static QoreNode *QWIDGET_mapFromParent(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPFROMPARENT-PARAM-ERROR", "expecting a QPoint object as first argument to QWidget::mapFromParent()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapFromParent(*(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint mapTo ( QWidget * parent, const QPoint & pos ) const
static QoreNode *QWIDGET_mapTo(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPTO-PARAM-ERROR", "expecting a QWidget object as first argument to QWidget::mapTo()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   p = get_param(params, 1);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPTO-PARAM-ERROR", "expecting a QPoint object as second argument to QWidget::mapTo()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapTo(parent->getQWidget(), *(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint mapToGlobal ( const QPoint & pos ) const
static QoreNode *QWIDGET_mapToGlobal(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPTOGLOBAL-PARAM-ERROR", "expecting a QPoint object as first argument to QWidget::mapToGlobal()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapToGlobal(*(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QPoint mapToParent ( const QPoint & pos ) const
static QoreNode *QWIDGET_mapToParent(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-MAPTOPARENT-PARAM-ERROR", "expecting a QPoint object as first argument to QWidget::mapToParent()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->mapToParent(*(static_cast<QPoint *>(pos))));
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QRegion mask () const
static QoreNode *QWIDGET_mask(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRegion *q_qr = new QoreQRegion(qw->getQWidget()->mask());
   Object *o_qr = new Object(self->getClass(CID_QREGION), getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//int maximumHeight () const
static QoreNode *QWIDGET_maximumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->maximumHeight());
}

//QSize maximumSize () const
static QoreNode *QWIDGET_maximumSize(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->maximumSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//int maximumWidth () const
static QoreNode *QWIDGET_maximumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->maximumWidth());
}

//int minimumHeight () const
static QoreNode *QWIDGET_minimumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->minimumHeight());
}

//QSize minimumSize () const
static QoreNode *QWIDGET_minimumSize(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->minimumSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual QSize minimumSizeHint () const
static QoreNode *QWIDGET_minimumSizeHint(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->minimumSizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//int minimumWidth () const
static QoreNode *QWIDGET_minimumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->minimumWidth());
}

//void move ( const QPoint & )
//static QoreNode *QWIDGET_move(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void move ( int x, int y )
static QoreNode *QWIDGET_move(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   qw->getQWidget()->move(x, y);
   return 0;
}

//QWidget * nextInFocusChain () const
/*
static QoreNode *QWIDGET_nextInFocusChain(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->nextInFocusChain());
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//QRect normalGeometry () const
static QoreNode *QWIDGET_normalGeometry(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->normalGeometry());
   Object *o_qr = new Object(self->getClass(CID_QRECT), getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void overrideWindowFlags ( Qt::WindowFlags flags )
static QoreNode *QWIDGET_overrideWindowFlags(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WindowFlags flags = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   qw->getQWidget()->overrideWindowFlags(flags);
   return 0;
}

//virtual QPaintEngine * paintEngine () const
//static QoreNode *QWIDGET_paintEngine(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//const QPalette & palette () const
//static QoreNode *QWIDGET_palette(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * parentWidget () const
/*
static QoreNode *QWIDGET_parentWidget(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->parentWidget());
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//QPoint pos () const
static QoreNode *QWIDGET_pos(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qw->getQWidget()->pos());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return new QoreNode(o_qp);
}

//QRect rect () const
static QoreNode *QWIDGET_rect(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRect *q_qr = new QoreQRect(qw->getQWidget()->rect());
   Object *o_qr = new Object(QC_QRect, getProgram());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual void releaseDC ( HDC hdc ) const
//static QoreNode *QWIDGET_releaseDC(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void releaseKeyboard ()
static QoreNode *QWIDGET_releaseKeyboard(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->releaseKeyboard();
   return 0;
}

//void releaseMouse ()
static QoreNode *QWIDGET_releaseMouse(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->releaseMouse();
   return 0;
}

//void releaseShortcut ( int id )
static QoreNode *QWIDGET_releaseShortcut(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   qw->getQWidget()->releaseShortcut(id);
   return 0;
}

//void removeAction ( QAction * action )
static QoreNode *QWIDGET_removeAction(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-REMOVEACTION-PARAM-ERROR", "expecting a QAction object as first argument to QWidget::removeAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   qw->getQWidget()->removeAction(static_cast<QAction *>(action->qobj));
   return 0;
}

//void render ( QPaintDevice * target, const QPoint & targetOffset = QPoint(), const QRegion & sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ) )
//static QoreNode *QWIDGET_render(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void repaint ( const QRect & r )
//void repaint ( const QRegion & rgn )
//void repaint ( int x, int y, int w, int h )
static QoreNode *QWIDGET_repaint(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qw->getQWidget()->repaint();
      return 0;
   }
   if (p->type == NT_OBJECT) {
      QoreQRect *r = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!r) {
	 QoreQRegion *rgn = (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink);
	 if (!rgn)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QWIDGET-REPAINT-PARAM-ERROR", "QWidget::repaint() cannot handle arguments of class '%s'", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQRegion> holder(rgn, xsink);
	 qw->getQWidget()->repaint(*((QRegion *)rgn));
	 return 0;
      }
      ReferenceHolder<QoreQRect> holder(r, xsink);
      qw->getQWidget()->repaint(*((QRect *)r));
      return 0;
   }

   int x = p->getAsInt();
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int h = p ? p->getAsInt() : 0;
   qw->getQWidget()->repaint(x, y, w, h);
   return 0;
}

//void resize ( const QSize & )
//void resize ( int w, int h )
static QoreNode *QWIDGET_resize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->resize(x, y);
   return 0;
}

//bool restoreGeometry ( const QByteArray & geometry )
//static QoreNode *QWIDGET_restoreGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QByteArray saveGeometry () const
//static QoreNode *QWIDGET_saveGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void scroll ( int dx, int dy )
//void scroll ( int dx, int dy, const QRect & r )
static QoreNode *QWIDGET_scroll(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int dy = p ? p->getAsInt() : 0;
   p = test_param(params, NT_OBJECT, 2);
   QoreQRect *r = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!r)
      qw->getQWidget()->scroll(dx, dy);
   else
   {
      ReferenceHolder<QoreQRect> holder(r, xsink);
      qw->getQWidget()->scroll(dx, dy, *((QRect *)r));
   }
   return 0;
}

//void setAcceptDrops ( bool on )
//static QoreNode *QWIDGET_setAcceptDrops(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setAccessibleDescription ( const QString & description )
static QoreNode *QWIDGET_setAccessibleDescription(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setAccessibleDescription(p->val.String->getBuffer());
   return 0;
}

//void setAccessibleName ( const QString & name )
static QoreNode *QWIDGET_setAccessibleName(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setAccessibleName(p->val.String->getBuffer());
   return 0;
}

//void setAttribute ( Qt::WidgetAttribute attribute, bool on = true )
static QoreNode *QWIDGET_setAttribute(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WidgetAttribute attribute = (Qt::WidgetAttribute)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool on = !is_nothing(p) ? p->getAsBool() : true;
   qw->getQWidget()->setAttribute(attribute, on);
   return 0;
}

//void setAutoFillBackground ( bool enabled )
static QoreNode *QWIDGET_setAutoFillBackground(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qw->getQWidget()->setAutoFillBackground(!num_params(params) ? true : (!p ? false : p->getAsBool()));
   return 0;
}

//void setBackgroundRole ( QPalette::ColorRole role )
static QoreNode *QWIDGET_setBackgroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
   {
      xsink->raiseException("QWIDGET-SETBACKGROUNDROLE-PARAM-ERROR", "missing role value argument for QWidget::setBackgroundRole()");
      return 0;
   }
   qw->getQWidget()->setBackgroundRole((QPalette::ColorRole)p->getAsInt());
   return 0;
}

//void setBaseSize ( const QSize & )
//void setBaseSize ( int basew, int baseh )
static QoreNode *QWIDGET_setBaseSize(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQSize *qsize = (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!qsize) {
         if (!xsink->isException())
            xsink->raiseException("QWIDGET-SETBASESIZE-PARAM-ERROR", "QWidget::setBaseSize() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQSize> qsizeHolder(qsize, xsink);
      qw->getQWidget()->setBaseSize(*(static_cast<QSize *>(qsize)));
      return 0;
   }
   int basew = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int baseh = p ? p->getAsInt() : 0;
   qw->getQWidget()->setBaseSize(basew, baseh);
   return 0;
}

//void setContentsMargins ( int left, int top, int right, int bottom )
static QoreNode *QWIDGET_setContentsMargins(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int left = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int top = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int right = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int bottom = p ? p->getAsInt() : 0;
   qw->getQWidget()->setContentsMargins(left, top, right, bottom);
   return 0;
}

//void setContextMenuPolicy ( Qt::ContextMenuPolicy policy )
static QoreNode *QWIDGET_setContextMenuPolicy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ContextMenuPolicy policy = (Qt::ContextMenuPolicy)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setContextMenuPolicy(policy);
   return 0;
}

//void setCursor ( const QCursor & )
//static QoreNode *QWIDGET_setCursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}


#ifdef QT_KEYPAD_NAVIGATION
//void setEditFocus ( bool enable )
static QoreNode *QWIDGET_setEditFocus(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : 0;
   qw->getQWidget()->setEditFocus(enable);
   return 0;
}
#endif

//void setFixedHeight ( int h )
static QoreNode *QWIDGET_setFixedHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->getQWidget()->setFixedHeight(h);
   return 0;
}

//void setFixedSize ( const QSize & s )
//void setFixedSize ( int w, int h )
static QoreNode *QWIDGET_setFixedSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->setFixedSize(w, h);
   return 0;
}

//void setFixedWidth ( int w )
static QoreNode *QWIDGET_setFixedWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETFIXEDWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->getQWidget()->setFixedWidth(w);
   return 0;
}

//void setFocusPolicy ( Qt::FocusPolicy policy )
static QoreNode *QWIDGET_setFocusPolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::FocusPolicy policy = (Qt::FocusPolicy)(p ? p->getAsInt() : 0);

   qw->getQWidget()->setFocusPolicy(policy);
   return 0;
}

//void setFocusProxy ( QWidget * w )
static QoreNode *QWIDGET_setFocusProxy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   static QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *proxy = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!proxy)
   {
      if (!xsink->isException())
	 xsink->raiseException("QWIDGET-SETFOCUSPROXY-ERROR", "expecting a QWidget object as sole argument to QWidget::setFocusProxy()");
      return 0;
   }

   ReferenceHolder<QoreAbstractQWidget> holder(proxy, xsink);
   qw->getQWidget()->setFocusProxy(proxy->getQWidget());
   return 0;
}

//void setFont ( const QFont & )
static QoreNode *QWIDGET_setFont(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->setFont(*((QFont *)qf));
   return 0;
}

//void setForegroundRole ( QPalette::ColorRole role )
static QoreNode *QWIDGET_setForegroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
   {
      xsink->raiseException("QWIDGET-SETFOREGROUNDROLE-PARAM-ERROR", "missing role value argument for QWidget::setForegroundRole()");
      return 0;
   }
   qw->getQWidget()->setForegroundRole((QPalette::ColorRole)p->getAsInt());
   return 0;
}

//void setGeometry ( const QRect & )
//void setGeometry ( int x, int y, int w, int h )
static QoreNode *QWIDGET_setGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->setGeometry(x, y, w, h);
   return 0;
}

//void setInputContext ( QInputContext * context )
//static QoreNode *QWIDGET_setInputContext(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setLayout ( QLayout * layout )
static QoreNode *QWIDGET_setLayout(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQLayout *qal = p ? (QoreAbstractQLayout *)p->val.object->getReferencedPrivateData(CID_QLAYOUT, xsink) : NULL;
   if (!p || !qal)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETLAYOUT-PARAM-ERROR", "expecting a QLayout object as argument to QWidget::setLayout()");
      return NULL;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(qal, xsink);

   qw->getQWidget()->setLayout(qal->getQLayout());
   return 0;
}

//void setLayoutDirection ( Qt::LayoutDirection direction )
static QoreNode *QWIDGET_setLayoutDirection(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setLayoutDirection(direction);
   return 0;
}

//void setLocale ( const QLocale & locale )
//static QoreNode *QWIDGET_setLocale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setMask ( const QBitmap & bitmap )
//void setMask ( const QRegion & region )
static QoreNode *QWIDGET_setMask(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQRegion *region = p ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!region) {
      QoreQBitmap *bitmap = p ? (QoreQBitmap *)p->val.object->getReferencedPrivateData(CID_QBITMAP, xsink) : 0;
      if (!bitmap) {
	 if (!xsink->isException())
	    xsink->raiseException("QWIDGET-SETMASK-PARAM-ERROR", "QWidget::setMask() was expecting QBitmap or QRegion as first argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> bitmapHolder(static_cast<AbstractPrivateData *>(bitmap), xsink);
      qw->getQWidget()->setMask(*(static_cast<QBitmap *>(bitmap)));
      return 0;
   }
   ReferenceHolder<QoreQRegion> regionHolder(region, xsink);
   qw->getQWidget()->setMask(*(static_cast<QRegion *>(region)));
   return 0;
}

//void setMaximumHeight ( int maxh )
static QoreNode *QWIDGET_setMaximumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->getQWidget()->setMaximumHeight(h);
   return 0;
}

//void setMaximumSize ( const QSize & )
//void setMaximumSize ( int maxw, int maxh )
static QoreNode *QWIDGET_setMaximumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->setMaximumSize(w, h);
   return 0;
}

//void setMaximumWidth ( int maxw )
static QoreNode *QWIDGET_setMaximumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMAXIMUMWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->getQWidget()->setMaximumWidth(w);
   return 0;
}

//void setMinimumHeight ( int minh )
static QoreNode *QWIDGET_setMinimumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMHEIGHT-ERROR", "missing height argument");
      return 0;
   }
   int h = p->getAsInt();

   qw->getQWidget()->setMinimumHeight(h);
   return 0;
}

//void setMinimumSize ( const QSize & )
//void setMinimumSize ( int minw, int minh )
static QoreNode *QWIDGET_setMinimumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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

   qw->getQWidget()->setMinimumSize(w, h);
   return 0;
}

//void setMinimumWidth ( int minw )
static QoreNode *QWIDGET_setMinimumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-SETMINIMUMWIDTH-ERROR", "missing width argument");
      return 0;
   }
   int w = p->getAsInt();
   
   qw->getQWidget()->setMinimumWidth(w);
   return 0;
}

//void setMouseTracking ( bool enable )
//static QoreNode *QWIDGET_setMouseTracking(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setPalette ( const QPalette & )
static QoreNode *QWIDGET_setPalette(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPalette *qp = p ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!qp)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as argument to QWidget::setPalette()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> holder(qp, xsink);

   qw->getQWidget()->setPalette(*qp);
   return 0;
}

//void setParent ( QWidget * parent, Qt::WindowFlags f )
//void setParent ( QWidget * parent )
static QoreNode *QWIDGET_setParent(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !parent)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETPARENT-PARAM-ERROR", "expecting a QWidget object as first argument to QWidget::setParent()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
   p = get_param(params, 1);
   if (is_nothing(p))
      qw->getQWidget()->setParent(parent->getQWidget());
   else {      
      Qt::WindowFlags f = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
      qw->getQWidget()->setParent(parent->getQWidget(), f);
   }
   return 0;
}

//void setShortcutAutoRepeat ( int id, bool enable = true )
static QoreNode *QWIDGET_setShortcutAutoRepeat(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool enable = !is_nothing(p) ? p->getAsBool() : true;
   qw->getQWidget()->setShortcutAutoRepeat(id, enable);
   return 0;
}

//void setShortcutEnabled ( int id, bool enable = true )
static QoreNode *QWIDGET_setShortcutEnabled(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int id = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool enable = !is_nothing(p) ? p->getAsBool() : true;
   qw->getQWidget()->setShortcutEnabled(id, enable);
   return 0;
}

//void setSizeIncrement ( const QSize & )
//static QoreNode *QWIDGET_setSizeIncrement(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setSizeIncrement ( int w, int h )
static QoreNode *QWIDGET_setSizeIncrement(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int h = p ? p->getAsInt() : 0;
   qw->getQWidget()->setSizeIncrement(w, h);
   return 0;
}

//void setSizePolicy ( QSizePolicy )
//void setSizePolicy ( QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical )
static QoreNode *QWIDGET_setSizePolicy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::Policy horizontal = (QSizePolicy::Policy)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::Policy vertical = (QSizePolicy::Policy)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setSizePolicy(horizontal, vertical);
   return 0;
}

//void setStatusTip ( const QString & )
static QoreNode *QWIDGET_setStatusTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setStatusTip(p->val.String->getBuffer());
   return 0;
}

//void setStyle ( QStyle * style )
//static QoreNode *QWIDGET_setStyle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setToolTip ( const QString & )
static QoreNode *QWIDGET_setToolTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setToolTip(p->val.String->getBuffer());
   return 0;
}

//void setUpdatesEnabled ( bool enable )
static QoreNode *QWIDGET_setUpdatesEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qw->getQWidget()->setUpdatesEnabled(!num_params(params) ? true : (!p ? false : p->getAsBool()));
   return 0;
}

//void setWhatsThis ( const QString & )
static QoreNode *QWIDGET_setWhatsThis(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWhatsThis(p->val.String->getBuffer());
   return 0;
}

//void setWindowFlags ( Qt::WindowFlags type )
static QoreNode *QWIDGET_setWindowFlags(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WindowFlags type = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setWindowFlags(type);
   return 0;
}

//void setWindowIcon ( const QIcon & icon )
static QoreNode *QWIDGET_setWindowIcon(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-SETWINDOWICON-PARAM-ERROR", "expecting a QIcon object as first argument to QWidget::setWindowIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
   qw->getQWidget()->setWindowIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setWindowIconText ( const QString & )
static QoreNode *QWIDGET_setWindowIconText(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWindowIconText(p->val.String->getBuffer());
   return 0;
}

//void setWindowModality ( Qt::WindowModality windowModality )
static QoreNode *QWIDGET_setWindowModality(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WindowModality windowModality = (Qt::WindowModality)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setWindowModality(windowModality);
   return 0;
}

//void setWindowOpacity ( qreal level )
static QoreNode *QWIDGET_setWindowOpacity(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float level = p ? p->getAsFloat() : 0;
   qw->getQWidget()->setWindowOpacity(level);
   return 0;
}

//void setWindowRole ( const QString & role )
static QoreNode *QWIDGET_setWindowRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWindowRole(p->val.String->getBuffer());
   return 0;
}

//void setWindowState ( Qt::WindowStates windowState )
static QoreNode *QWIDGET_setWindowState(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WindowStates windowState = (Qt::WindowStates)(p ? p->getAsInt() : 0);
   qw->getQWidget()->setWindowState(windowState);
   return 0;
}

//void setWindowSurface ( QWindowSurface * surface )   (preliminary)
//static QoreNode *QWIDGET_setWindowSurface(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QSize size () const
static QoreNode *QWIDGET_size(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->size());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual QSize sizeHint () const
static QoreNode *QWIDGET_sizeHint(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QSize sizeIncrement () const
static QoreNode *QWIDGET_sizeIncrement(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qw->getQWidget()->sizeIncrement());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QSizePolicy sizePolicy () const
/*
static QoreNode *QWIDGET_sizePolicy(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->sizePolicy());
}
*/

//void stackUnder ( QWidget * w )
static QoreNode *QWIDGET_stackUnder(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *w = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !w)
   {
      if (!xsink->isException())
         xsink->raiseException("QWIDGET-STACKUNDER-PARAM-ERROR", "expecting a QWidget object as first argument to QWidget::stackUnder()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(w, xsink);
   qw->getQWidget()->stackUnder(w->getQWidget());
   return 0;
}

//QString statusTip () const
static QoreNode *QWIDGET_statusTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->statusTip().toUtf8().data(), QCS_UTF8));
}

//QStyle * style () const
//static QoreNode *QWIDGET_style(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString styleSheet () const
static QoreNode *QWIDGET_styleSheet(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->styleSheet().toUtf8().data(), QCS_UTF8));
}

//bool testAttribute ( Qt::WidgetAttribute attribute ) const
static QoreNode *QWIDGET_testAttribute(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::WidgetAttribute attribute = (Qt::WidgetAttribute)(p ? p->getAsInt() : 0);
   return new QoreNode(qw->getQWidget()->testAttribute(attribute));
}

//QString toolTip () const
static QoreNode *QWIDGET_toolTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->toolTip().toUtf8().data(), QCS_UTF8));
}

//bool underMouse () const
static QoreNode *QWIDGET_underMouse(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->underMouse());
}

//void unsetCursor ()
static QoreNode *QWIDGET_unsetCursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetCursor();
   return 0;
}

//void unsetLayoutDirection ()
static QoreNode *QWIDGET_unsetLayoutDirection(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetLayoutDirection();
   return 0;
}

//void unsetLocale ()
static QoreNode *QWIDGET_unsetLocale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetLocale();
   return 0;
}

//void update ( int x, int y, int w, int h )
//void update ( const QRect & r )
//void update ( const QRegion & rgn )
//void update ()
static QoreNode *QWIDGET_update(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qw->getQWidget()->update();
      return 0;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQRegion *rgn = (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink);
      if (!rgn) {
         QoreQRect *r = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
         if (!r) {
            if (!xsink->isException())
               xsink->raiseException("QWIDGET-UPDATE-PARAM-ERROR", "QWidget::update() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<QoreQRect> rHolder(r, xsink);
         qw->getQWidget()->update(*(static_cast<QRect *>(r)));
         return 0;
      }
      ReferenceHolder<QoreQRegion> rgnHolder(rgn, xsink);
      qw->getQWidget()->update(*(static_cast<QRegion *>(rgn)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int w = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int h = p ? p->getAsInt() : 0;
   qw->getQWidget()->update(x, y, w, h);
   return 0;
}

//void updateGeometry ()
static QoreNode *QWIDGET_updateGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->updateGeometry();
   return 0;
}

//bool updatesEnabled () const
static QoreNode *QWIDGET_updatesEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->updatesEnabled());
}

//QRegion visibleRegion () const
static QoreNode *QWIDGET_visibleRegion(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQRegion *q_qr = new QoreQRegion(qw->getQWidget()->visibleRegion());
   Object *o_qr = new Object(QC_QRegion, getProgram());
   o_qr->setPrivate(CID_QREGION, q_qr);
   return new QoreNode(o_qr);
}

//QString whatsThis () const
static QoreNode *QWIDGET_whatsThis(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->whatsThis().toUtf8().data(), QCS_UTF8));
}

//int width () const
static QoreNode *QWIDGET_width(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->width());
}

//WId winId () const
//static QoreNode *QWIDGET_winId(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * window () const
/*
static QoreNode *QWIDGET_window(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{

   QoreQWidget *q_qw = new QoreQWidget(qw->getQWidget()->window());
   Object *o_qw = new Object(self->getClass(CID_QWIDGET*), getProgram());
   o_qw*->setPrivate(CID_QWIDGET*, q_qw*);
   return new QoreNode(o_qw*);
}
*/

//Qt::WindowFlags windowFlags () const
static QoreNode *QWIDGET_windowFlags(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->windowFlags());
}

//QIcon windowIcon () const
static QoreNode *QWIDGET_windowIcon(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qw->getQWidget()->windowIcon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QString windowIconText () const
static QoreNode *QWIDGET_windowIconText(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowIconText().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowModality windowModality () const
static QoreNode *QWIDGET_windowModality(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->windowModality());
}

//qreal windowOpacity () const
static QoreNode *QWIDGET_windowOpacity(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->windowOpacity());
}

//QString windowRole () const
static QoreNode *QWIDGET_windowRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowRole().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowStates windowState () const
static QoreNode *QWIDGET_windowState(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->windowState());
}

//QWindowSurface * windowSurface () const   (preliminary)
//static QoreNode *QWIDGET_windowSurface(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString windowTitle () const
static QoreNode *QWIDGET_windowTitle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowTitle().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowType windowType () const
static QoreNode *QWIDGET_windowType(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->windowType());
}

//int x () const
static QoreNode *QWIDGET_x(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->x());
}

//const QX11Info & x11Info () const
//static QoreNode *QWIDGET_x11Info(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE x11PictureHandle () const
//static QoreNode *QWIDGET_x11PictureHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int y () const
static QoreNode *QWIDGET_y(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->y());
}

// slots
//bool close ()
static QoreNode *QWIDGET_close(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->close());
}

//void hide ()
static QoreNode *QWIDGET_hide(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->hide();
   return 0;
}

//void lower ()
static QoreNode *QWIDGET_lower(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->lower();
   return 0;
}

//void raise ()
static QoreNode *QWIDGET_raise(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->raise();
   return 0;
}

//void repaint ()
// is also a normal method

//void setDisabled ( bool disable )
static QoreNode *QWIDGET_setDisabled(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool disable = p ? p->getAsBool() : 0;
   qw->getQWidget()->setDisabled(disable);
   return 0;
}

//void setEnabled ( bool )
static QoreNode *QWIDGET_setEnabled(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qw->getQWidget()->setEnabled(b);
   return 0;
}

// slot and method
//void setFocus ()
static QoreNode *QWIDGET_setFocus(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qw->getQWidget()->setFocus();
   else {
      Qt::FocusReason reason = (Qt::FocusReason)(p ? p->getAsInt() : 0);
      qw->getQWidget()->setFocus(reason);
   }
   return 0;
}

//void setHidden ( bool hidden )
static QoreNode *QWIDGET_setHidden(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool hidden = p ? p->getAsBool() : 0;
   qw->getQWidget()->setHidden(hidden);
   return 0;
}

//void setStyleSheet ( const QString & styleSheet )
static QoreNode *QWIDGET_setStyleSheet(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QWIDGET-SETSTYLESHEET-PARAM-ERROR", "expecting a string as first argument to QWidget::setStyleSheet()");
      return 0;
   }
   const char *styleSheet = p->val.String->getBuffer();
   qw->getQWidget()->setStyleSheet(styleSheet);
   return 0;
}

//virtual void setVisible ( bool visible )
static QoreNode *QWIDGET_setVisible(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : 0;
   qw->getQWidget()->setVisible(visible);
   return 0;
}

//void setWindowModified ( bool )
static QoreNode *QWIDGET_setWindowModified(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qw->getQWidget()->setWindowModified(b);
   return 0;
}

//void setWindowTitle ( const QString & )
static QoreNode *QWIDGET_setWindowTitle(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QWIDGET-SETWINDOWTITLE-PARAM-ERROR", "expecting a string as first argument to QWidget::setWindowTitle()");
      return 0;
   }
   const char *qstring = p->val.String->getBuffer();
   qw->getQWidget()->setWindowTitle(qstring);
   return 0;
}

//void show ()
static QoreNode *QWIDGET_show(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->show();
   return 0;
}

//void showFullScreen ()
static QoreNode *QWIDGET_showFullScreen(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->showFullScreen();
   return 0;
}

//void showMaximized ()
static QoreNode *QWIDGET_showMaximized(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->showMaximized();
   return 0;
}

//void showMinimized ()
static QoreNode *QWIDGET_showMinimized(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->showMinimized();
   return 0;
}

//void showNormal ()
static QoreNode *QWIDGET_showNormal(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->showNormal();
   return 0;
}

class QoreClass *initQWidgetClass(class QoreClass *qobject, class QoreClass *qpaintdevice)
{
   tracein("initQWidgetClass()");
   
   class QoreClass *QC_QWidget = new QoreClass("QWidget", QDOM_GUI);
   CID_QWIDGET = QC_QWidget->getID();

   QC_QWidget->addBuiltinVirtualBaseClass(qobject);
   QC_QWidget->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QWidget->setConstructor(QWIDGET_constructor);
   QC_QWidget->setCopy((q_copy_t)QWIDGET_copy);

   // add methods for slots
   QC_QWidget->addMethod("show",              (q_method_t)QWIDGET_show);

   QC_QWidget->addMethod("acceptDrops",                  (q_method_t)QWIDGET_acceptDrops);
   QC_QWidget->addMethod("accessibleDescription",        (q_method_t)QWIDGET_accessibleDescription);
   QC_QWidget->addMethod("accessibleName",               (q_method_t)QWIDGET_accessibleName);
   //QC_QWidget->addMethod("actions",                      (q_method_t)QWIDGET_actions);
   QC_QWidget->addMethod("activateWindow",               (q_method_t)QWIDGET_activateWindow);
   QC_QWidget->addMethod("addAction",                    (q_method_t)QWIDGET_addAction);
   //QC_QWidget->addMethod("addActions",                   (q_method_t)QWIDGET_addActions);
   QC_QWidget->addMethod("adjustSize",                   (q_method_t)QWIDGET_adjustSize);
   QC_QWidget->addMethod("autoFillBackground",           (q_method_t)QWIDGET_autoFillBackground);
   QC_QWidget->addMethod("backgroundRole",               (q_method_t)QWIDGET_backgroundRole);
   //QC_QWidget->addMethod("baseSize",                     (q_method_t)QWIDGET_baseSize);
   //QC_QWidget->addMethod("childAt",                      (q_method_t)QWIDGET_childAt);
   //QC_QWidget->addMethod("childAt",                      (q_method_t)QWIDGET_childAt);
   QC_QWidget->addMethod("childrenRect",                 (q_method_t)QWIDGET_childrenRect);
   QC_QWidget->addMethod("childrenRegion",               (q_method_t)QWIDGET_childrenRegion);
   QC_QWidget->addMethod("clearFocus",                   (q_method_t)QWIDGET_clearFocus);
   QC_QWidget->addMethod("clearMask",                    (q_method_t)QWIDGET_clearMask);
   QC_QWidget->addMethod("contentsRect",                 (q_method_t)QWIDGET_contentsRect);
   QC_QWidget->addMethod("contextMenuPolicy",            (q_method_t)QWIDGET_contextMenuPolicy);
   //QC_QWidget->addMethod("cursor",                       (q_method_t)QWIDGET_cursor);
   QC_QWidget->addMethod("ensurePolished",               (q_method_t)QWIDGET_ensurePolished);
   QC_QWidget->addMethod("focusPolicy",                  (q_method_t)QWIDGET_focusPolicy);
   //QC_QWidget->addMethod("focusProxy",                   (q_method_t)QWIDGET_focusProxy);
   //QC_QWidget->addMethod("focusWidget",                  (q_method_t)QWIDGET_focusWidget);
   QC_QWidget->addMethod("font",                         (q_method_t)QWIDGET_font);
   //QC_QWidget->addMethod("fontInfo",                     (q_method_t)QWIDGET_fontInfo);
   //QC_QWidget->addMethod("fontMetrics",                  (q_method_t)QWIDGET_fontMetrics);
   QC_QWidget->addMethod("foregroundRole",               (q_method_t)QWIDGET_foregroundRole);
   QC_QWidget->addMethod("frameGeometry",                (q_method_t)QWIDGET_frameGeometry);
   QC_QWidget->addMethod("frameSize",                    (q_method_t)QWIDGET_frameSize);
   QC_QWidget->addMethod("geometry",                     (q_method_t)QWIDGET_geometry);
   //QC_QWidget->addMethod("getContentsMargins",           (q_method_t)QWIDGET_getContentsMargins);
   //QC_QWidget->addMethod("getDC",                        (q_method_t)QWIDGET_getDC);
   QC_QWidget->addMethod("grabKeyboard",                 (q_method_t)QWIDGET_grabKeyboard);
   QC_QWidget->addMethod("grabMouse",                    (q_method_t)QWIDGET_grabMouse);
   QC_QWidget->addMethod("grabShortcut",                 (q_method_t)QWIDGET_grabShortcut);
#ifdef QT_KEYPAD_NAVIGATION
   QC_QWidget->addMethod("hasEditFocus",                 (q_method_t)QWIDGET_hasEditFocus);
#endif
   QC_QWidget->addMethod("hasFocus",                     (q_method_t)QWIDGET_hasFocus);
   QC_QWidget->addMethod("hasMouseTracking",             (q_method_t)QWIDGET_hasMouseTracking);
   QC_QWidget->addMethod("height",                       (q_method_t)QWIDGET_height);
   QC_QWidget->addMethod("heightForWidth",               (q_method_t)QWIDGET_heightForWidth);
   //QC_QWidget->addMethod("inputContext",                 (q_method_t)QWIDGET_inputContext);
   //QC_QWidget->addMethod("inputMethodQuery",             (q_method_t)QWIDGET_inputMethodQuery);
   QC_QWidget->addMethod("insertAction",                 (q_method_t)QWIDGET_insertAction);
   //QC_QWidget->addMethod("insertActions",                (q_method_t)QWIDGET_insertActions);
   QC_QWidget->addMethod("isActiveWindow",               (q_method_t)QWIDGET_isActiveWindow);
   QC_QWidget->addMethod("isAncestorOf",                 (q_method_t)QWIDGET_isAncestorOf);
   QC_QWidget->addMethod("isEnabled",                    (q_method_t)QWIDGET_isEnabled);
   QC_QWidget->addMethod("isEnabledTo",                  (q_method_t)QWIDGET_isEnabledTo);
   QC_QWidget->addMethod("isFullScreen",                 (q_method_t)QWIDGET_isFullScreen);
   QC_QWidget->addMethod("isHidden",                     (q_method_t)QWIDGET_isHidden);
   QC_QWidget->addMethod("isMaximized",                  (q_method_t)QWIDGET_isMaximized);
   QC_QWidget->addMethod("isMinimized",                  (q_method_t)QWIDGET_isMinimized);
   QC_QWidget->addMethod("isModal",                      (q_method_t)QWIDGET_isModal);
   QC_QWidget->addMethod("isVisible",                    (q_method_t)QWIDGET_isVisible);
   QC_QWidget->addMethod("isVisibleTo",                  (q_method_t)QWIDGET_isVisibleTo);
   QC_QWidget->addMethod("isWindow",                     (q_method_t)QWIDGET_isWindow);
   QC_QWidget->addMethod("isWindowModified",             (q_method_t)QWIDGET_isWindowModified);
   //QC_QWidget->addMethod("layout",                       (q_method_t)QWIDGET_layout);
   QC_QWidget->addMethod("layoutDirection",              (q_method_t)QWIDGET_layoutDirection);
   //QC_QWidget->addMethod("locale",                       (q_method_t)QWIDGET_locale);
   //QC_QWidget->addMethod("macCGHandle",                  (q_method_t)QWIDGET_macCGHandle);
   //QC_QWidget->addMethod("macQDHandle",                  (q_method_t)QWIDGET_macQDHandle);
   QC_QWidget->addMethod("mapFrom",                      (q_method_t)QWIDGET_mapFrom);
   QC_QWidget->addMethod("mapFromGlobal",                (q_method_t)QWIDGET_mapFromGlobal);
   QC_QWidget->addMethod("mapFromParent",                (q_method_t)QWIDGET_mapFromParent);
   QC_QWidget->addMethod("mapTo",                        (q_method_t)QWIDGET_mapTo);
   QC_QWidget->addMethod("mapToGlobal",                  (q_method_t)QWIDGET_mapToGlobal);
   QC_QWidget->addMethod("mapToParent",                  (q_method_t)QWIDGET_mapToParent);
   QC_QWidget->addMethod("mask",                         (q_method_t)QWIDGET_mask);
   QC_QWidget->addMethod("maximumHeight",                (q_method_t)QWIDGET_maximumHeight);
   QC_QWidget->addMethod("maximumSize",                  (q_method_t)QWIDGET_maximumSize);
   QC_QWidget->addMethod("maximumWidth",                 (q_method_t)QWIDGET_maximumWidth);
   QC_QWidget->addMethod("minimumHeight",                (q_method_t)QWIDGET_minimumHeight);
   QC_QWidget->addMethod("minimumSize",                  (q_method_t)QWIDGET_minimumSize);
   QC_QWidget->addMethod("minimumSizeHint",              (q_method_t)QWIDGET_minimumSizeHint);
   QC_QWidget->addMethod("minimumWidth",                 (q_method_t)QWIDGET_minimumWidth);
   QC_QWidget->addMethod("move",                         (q_method_t)QWIDGET_move);
   //QC_QWidget->addMethod("nextInFocusChain",             (q_method_t)QWIDGET_nextInFocusChain);
   QC_QWidget->addMethod("normalGeometry",               (q_method_t)QWIDGET_normalGeometry);
   QC_QWidget->addMethod("overrideWindowFlags",          (q_method_t)QWIDGET_overrideWindowFlags);
   //QC_QWidget->addMethod("paintEngine",                  (q_method_t)QWIDGET_paintEngine);
   //QC_QWidget->addMethod("palette",                      (q_method_t)QWIDGET_palette);
   //QC_QWidget->addMethod("parentWidget",                 (q_method_t)QWIDGET_parentWidget);
   QC_QWidget->addMethod("pos",                          (q_method_t)QWIDGET_pos);
   QC_QWidget->addMethod("rect",                         (q_method_t)QWIDGET_rect);
   //QC_QWidget->addMethod("releaseDC",                    (q_method_t)QWIDGET_releaseDC);
   QC_QWidget->addMethod("releaseKeyboard",              (q_method_t)QWIDGET_releaseKeyboard);
   QC_QWidget->addMethod("releaseMouse",                 (q_method_t)QWIDGET_releaseMouse);
   QC_QWidget->addMethod("releaseShortcut",              (q_method_t)QWIDGET_releaseShortcut);
   QC_QWidget->addMethod("removeAction",                 (q_method_t)QWIDGET_removeAction);
   //QC_QWidget->addMethod("render",                       (q_method_t)QWIDGET_render);
   QC_QWidget->addMethod("repaint",                      (q_method_t)QWIDGET_repaint);
   QC_QWidget->addMethod("resize",                       (q_method_t)QWIDGET_resize);
   //QC_QWidget->addMethod("restoreGeometry",              (q_method_t)QWIDGET_restoreGeometry);
   //QC_QWidget->addMethod("saveGeometry",                 (q_method_t)QWIDGET_saveGeometry);
   QC_QWidget->addMethod("scroll",                       (q_method_t)QWIDGET_scroll);
   //QC_QWidget->addMethod("setAcceptDrops",               (q_method_t)QWIDGET_setAcceptDrops);
   QC_QWidget->addMethod("setAccessibleDescription",     (q_method_t)QWIDGET_setAccessibleDescription);
   QC_QWidget->addMethod("setAccessibleName",            (q_method_t)QWIDGET_setAccessibleName);
   QC_QWidget->addMethod("setAttribute",                 (q_method_t)QWIDGET_setAttribute);
   QC_QWidget->addMethod("setAutoFillBackground",        (q_method_t)QWIDGET_setAutoFillBackground);
   QC_QWidget->addMethod("setBackgroundRole",            (q_method_t)QWIDGET_setBackgroundRole);
   QC_QWidget->addMethod("setBaseSize",                  (q_method_t)QWIDGET_setBaseSize);
   QC_QWidget->addMethod("setContentsMargins",           (q_method_t)QWIDGET_setContentsMargins);
   QC_QWidget->addMethod("setContextMenuPolicy",         (q_method_t)QWIDGET_setContextMenuPolicy);
   //QC_QWidget->addMethod("setCursor",                    (q_method_t)QWIDGET_setCursor);
#ifdef QT_KEYPAD_NAVIGATION
   QC_QWidget->addMethod("setEditFocus",                 (q_method_t)QWIDGET_setEditFocus);
#endif
   QC_QWidget->addMethod("setFixedHeight",               (q_method_t)QWIDGET_setFixedHeight);
   QC_QWidget->addMethod("setFixedSize",                 (q_method_t)QWIDGET_setFixedSize);
   QC_QWidget->addMethod("setFixedWidth",                (q_method_t)QWIDGET_setFixedWidth);
   QC_QWidget->addMethod("setFocusPolicy",               (q_method_t)QWIDGET_setFocusPolicy);
   QC_QWidget->addMethod("setFocusProxy",                (q_method_t)QWIDGET_setFocusProxy);
   QC_QWidget->addMethod("setFont",                      (q_method_t)QWIDGET_setFont);
   QC_QWidget->addMethod("setForegroundRole",            (q_method_t)QWIDGET_setForegroundRole);
   QC_QWidget->addMethod("setGeometry",                  (q_method_t)QWIDGET_setGeometry);
   //QC_QWidget->addMethod("setInputContext",              (q_method_t)QWIDGET_setInputContext);
   QC_QWidget->addMethod("setLayout",                    (q_method_t)QWIDGET_setLayout);
   QC_QWidget->addMethod("setLayoutDirection",           (q_method_t)QWIDGET_setLayoutDirection);
   //QC_QWidget->addMethod("setLocale",                    (q_method_t)QWIDGET_setLocale);
   QC_QWidget->addMethod("setMask",                      (q_method_t)QWIDGET_setMask);
   QC_QWidget->addMethod("setMaximumHeight",             (q_method_t)QWIDGET_setMaximumHeight);
   QC_QWidget->addMethod("setMaximumSize",               (q_method_t)QWIDGET_setMaximumSize);
   QC_QWidget->addMethod("setMaximumWidth",              (q_method_t)QWIDGET_setMaximumWidth);
   QC_QWidget->addMethod("setMinimumHeight",             (q_method_t)QWIDGET_setMinimumHeight);
   QC_QWidget->addMethod("setMinimumSize",               (q_method_t)QWIDGET_setMinimumSize);
   QC_QWidget->addMethod("setMinimumWidth",              (q_method_t)QWIDGET_setMinimumWidth);
   //QC_QWidget->addMethod("setMouseTracking",             (q_method_t)QWIDGET_setMouseTracking);
   QC_QWidget->addMethod("setPalette",                   (q_method_t)QWIDGET_setPalette);
   QC_QWidget->addMethod("setParent",                    (q_method_t)QWIDGET_setParent);
   QC_QWidget->addMethod("setShortcutAutoRepeat",        (q_method_t)QWIDGET_setShortcutAutoRepeat);
   QC_QWidget->addMethod("setShortcutEnabled",           (q_method_t)QWIDGET_setShortcutEnabled);
   QC_QWidget->addMethod("setSizeIncrement",             (q_method_t)QWIDGET_setSizeIncrement);
   QC_QWidget->addMethod("setSizePolicy",                (q_method_t)QWIDGET_setSizePolicy);
   QC_QWidget->addMethod("setStatusTip",                 (q_method_t)QWIDGET_setStatusTip);
   //QC_QWidget->addMethod("setStyle",                     (q_method_t)QWIDGET_setStyle);
   QC_QWidget->addMethod("setToolTip",                   (q_method_t)QWIDGET_setToolTip);
   QC_QWidget->addMethod("setUpdatesEnabled",            (q_method_t)QWIDGET_setUpdatesEnabled);
   QC_QWidget->addMethod("setWhatsThis",                 (q_method_t)QWIDGET_setWhatsThis);
   QC_QWidget->addMethod("setWindowFlags",               (q_method_t)QWIDGET_setWindowFlags);
   QC_QWidget->addMethod("setWindowIcon",                (q_method_t)QWIDGET_setWindowIcon);
   QC_QWidget->addMethod("setWindowIconText",            (q_method_t)QWIDGET_setWindowIconText);
   QC_QWidget->addMethod("setWindowModality",            (q_method_t)QWIDGET_setWindowModality);
   QC_QWidget->addMethod("setWindowOpacity",             (q_method_t)QWIDGET_setWindowOpacity);
   QC_QWidget->addMethod("setWindowRole",                (q_method_t)QWIDGET_setWindowRole);
   QC_QWidget->addMethod("setWindowState",               (q_method_t)QWIDGET_setWindowState);
   //QC_QWidget->addMethod("setWindowSurface",             (q_method_t)QWIDGET_setWindowSurface);
   QC_QWidget->addMethod("size",                         (q_method_t)QWIDGET_size);
   QC_QWidget->addMethod("sizeHint",                     (q_method_t)QWIDGET_sizeHint);
   QC_QWidget->addMethod("sizeIncrement",                (q_method_t)QWIDGET_sizeIncrement);
   //QC_QWidget->addMethod("sizePolicy",                   (q_method_t)QWIDGET_sizePolicy);
   QC_QWidget->addMethod("stackUnder",                   (q_method_t)QWIDGET_stackUnder);
   QC_QWidget->addMethod("statusTip",                    (q_method_t)QWIDGET_statusTip);
   //QC_QWidget->addMethod("style",                        (q_method_t)QWIDGET_style);
   QC_QWidget->addMethod("styleSheet",                   (q_method_t)QWIDGET_styleSheet);
   QC_QWidget->addMethod("testAttribute",                (q_method_t)QWIDGET_testAttribute);
   QC_QWidget->addMethod("toolTip",                      (q_method_t)QWIDGET_toolTip);
   QC_QWidget->addMethod("underMouse",                   (q_method_t)QWIDGET_underMouse);
   QC_QWidget->addMethod("unsetCursor",                  (q_method_t)QWIDGET_unsetCursor);
   QC_QWidget->addMethod("unsetLayoutDirection",         (q_method_t)QWIDGET_unsetLayoutDirection);
   QC_QWidget->addMethod("unsetLocale",                  (q_method_t)QWIDGET_unsetLocale);
   QC_QWidget->addMethod("update",                       (q_method_t)QWIDGET_update);
   QC_QWidget->addMethod("updateGeometry",               (q_method_t)QWIDGET_updateGeometry);
   QC_QWidget->addMethod("updatesEnabled",               (q_method_t)QWIDGET_updatesEnabled);
   QC_QWidget->addMethod("visibleRegion",                (q_method_t)QWIDGET_visibleRegion);
   QC_QWidget->addMethod("whatsThis",                    (q_method_t)QWIDGET_whatsThis);
   QC_QWidget->addMethod("width",                        (q_method_t)QWIDGET_width);
   //QC_QWidget->addMethod("winId",                        (q_method_t)QWIDGET_winId);
   //QC_QWidget->addMethod("window",                       (q_method_t)QWIDGET_window);
   QC_QWidget->addMethod("windowFlags",                  (q_method_t)QWIDGET_windowFlags);
   QC_QWidget->addMethod("windowIcon",                   (q_method_t)QWIDGET_windowIcon);
   QC_QWidget->addMethod("windowIconText",               (q_method_t)QWIDGET_windowIconText);
   QC_QWidget->addMethod("windowModality",               (q_method_t)QWIDGET_windowModality);
   QC_QWidget->addMethod("windowOpacity",                (q_method_t)QWIDGET_windowOpacity);
   QC_QWidget->addMethod("windowRole",                   (q_method_t)QWIDGET_windowRole);
   QC_QWidget->addMethod("windowtSate",                  (q_method_t)QWIDGET_windowState);
   //QC_QWidget->addMethod("windowSurface",                (q_method_t)QWIDGET_windowSurface);
   QC_QWidget->addMethod("windowTitle",                  (q_method_t)QWIDGET_windowTitle);
   QC_QWidget->addMethod("windowType",                   (q_method_t)QWIDGET_windowType);
   QC_QWidget->addMethod("x",                            (q_method_t)QWIDGET_x);
   //QC_QWidget->addMethod("x11Info",                      (q_method_t)QWIDGET_x11Info);
   //QC_QWidget->addMethod("x11PictureHandle",             (q_method_t)QWIDGET_x11PictureHandle);
   QC_QWidget->addMethod("y",                            (q_method_t)QWIDGET_y);
   
   // slots
   QC_QWidget->addMethod("close",                       (q_method_t)QWIDGET_close);
   QC_QWidget->addMethod("hide",                        (q_method_t)QWIDGET_hide);
   QC_QWidget->addMethod("lower",                       (q_method_t)QWIDGET_lower);
   QC_QWidget->addMethod("raise",                       (q_method_t)QWIDGET_raise);
   QC_QWidget->addMethod("setDisabled",                 (q_method_t)QWIDGET_setDisabled);
   QC_QWidget->addMethod("setEnabled",                  (q_method_t)QWIDGET_setEnabled);
   QC_QWidget->addMethod("setFocus",                    (q_method_t)QWIDGET_setFocus);
   QC_QWidget->addMethod("setHidden",                   (q_method_t)QWIDGET_setHidden);
   QC_QWidget->addMethod("setStyleSheet",               (q_method_t)QWIDGET_setStyleSheet);
   QC_QWidget->addMethod("setVisible",                  (q_method_t)QWIDGET_setVisible);
   QC_QWidget->addMethod("setWindowModified",           (q_method_t)QWIDGET_setWindowModified);
   QC_QWidget->addMethod("setWindowTitle",              (q_method_t)QWIDGET_setWindowTitle);
   QC_QWidget->addMethod("show",                        (q_method_t)QWIDGET_show);
   QC_QWidget->addMethod("showFullScreen",              (q_method_t)QWIDGET_showFullScreen);
   QC_QWidget->addMethod("showMaximized",               (q_method_t)QWIDGET_showMaximized);
   QC_QWidget->addMethod("showMinimized",               (q_method_t)QWIDGET_showMinimized);
   QC_QWidget->addMethod("showNormal",                  (q_method_t)QWIDGET_showNormal);

   traceout("initQWidgetClass()");
   return QC_QWidget;
}
