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

int CID_QWIDGET;

static void QW_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

static void QW_copy(class Object *self, class Object *old, class QoreQWidget *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

// slots
static QoreNode *QW_show(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->show();
   return 0;
}


//bool acceptDrops () const
static QoreNode *QW_acceptDrops(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->acceptDrops());
}

//QString accessibleDescription () const
static QoreNode *QW_accessibleDescription(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->accessibleDescription().toUtf8().data(), QCS_UTF8));
}

//QString accessibleName () const
static QoreNode *QW_accessibleName(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->accessibleName().toUtf8().data(), QCS_UTF8));
}

//QList<QAction *> actions () const
//static QoreNode *QW_actions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void activateWindow ()
static QoreNode *QW_activateWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->adjustSize();
   return 0;
}

//void addAction ( QAction * action )
//static QoreNode *QW_addAction(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void addActions ( QList<QAction *> actions )
//static QoreNode *QW_addActions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void adjustSize ()
static QoreNode *QW_adjustSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->adjustSize();
   return 0;
}

//bool autoFillBackground () const
static QoreNode *QW_autoFillBackground(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->autoFillBackground());
}

//QPalette::ColorRole backgroundRole () const
static QoreNode *QW_backgroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->backgroundRole());
}

//QSize baseSize () const
//static QoreNode *QW_baseSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * childAt ( int x, int y ) const
//static QoreNode *QW_childAt(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * childAt ( const QPoint & p ) const
//static QoreNode *QW_childAt(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QRect childrenRect () const
//static QoreNode *QW_childrenRect(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QRegion childrenRegion () const
//static QoreNode *QW_childrenRegion(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void clearFocus ()
static QoreNode *QW_clearFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->clearFocus();
   return 0;
}

//void clearMask ()
static QoreNode *QW_clearMask(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->clearMask();
   return 0;
}

//QRect contentsRect () const
//static QoreNode *QW_contentsRect(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::ContextMenuPolicy contextMenuPolicy () const
//static QoreNode *QW_contextMenuPolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QCursor cursor () const
//static QoreNode *QW_cursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void ensurePolished () const
//static QoreNode *QW_ensurePolished(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::FocusPolicy focusPolicy () const
//static QoreNode *QW_focusPolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * focusProxy () const
//static QoreNode *QW_focusProxy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * focusWidget () const
//static QoreNode *QW_focusWidget(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//const QFont & font () const
//static QoreNode *QW_font(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QFontInfo fontInfo () const
//static QoreNode *QW_fontInfo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QFontMetrics fontMetrics () const
//static QoreNode *QW_fontMetrics(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPalette::ColorRole foregroundRole () const
static QoreNode *QW_foregroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->foregroundRole());
}

//QRect frameGeometry () const
//static QoreNode *QW_frameGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QSize frameSize () const
//static QoreNode *QW_frameSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//const QRect & geometry () const
//static QoreNode *QW_geometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void getContentsMargins ( int * left, int * top, int * right, int * bottom ) const
//static QoreNode *QW_getContentsMargins(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual HDC getDC () const
//static QoreNode *QW_getDC(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void grabKeyboard ()
static QoreNode *QW_grabKeyboard(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->grabKeyboard();
   return 0;
}

//void grabMouse ()
//void grabMouse ( const QCursor & cursor )
static QoreNode *QW_grabMouse(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->grabMouse();
   return 0;
}

//int grabShortcut ( const QKeySequence & key, Qt::ShortcutContext context = Qt::WindowShortcut )
//static QoreNode *QW_grabShortcut(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

#ifdef QT_KEYPAD_NAVIGATION
//bool hasEditFocus () const
static QoreNode *QW_hasEditFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasEditFocus());
}
#endif

//bool hasFocus () const
static QoreNode *QW_hasFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasFocus());
}

//bool hasMouseTracking () const
static QoreNode *QW_hasMouseTracking(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->hasMouseTracking());
}

//int height () const
static QoreNode *QW_height(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->height());
}

//virtual int heightForWidth ( int w ) const
static QoreNode *QW_heightForWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QWIDGET-HEIGHTFORWIDTH-ERROR", "missing width argument in QWidget::heightForWidth()");
      return 0;
   }
   return new QoreNode((int64)qw->getQWidget()->heightForWidth(p->getAsInt()));
}

//QInputContext * inputContext ()
//static QoreNode *QW_inputContext(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const
//static QoreNode *QW_inputMethodQuery(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void insertAction ( QAction * before, QAction * action )
//static QoreNode *QW_insertAction(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void insertActions ( QAction * before, QList<QAction *> actions )
//static QoreNode *QW_insertActions(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//bool isActiveWindow () const
static QoreNode *QW_isActiveWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isActiveWindow());
}

//bool isAncestorOf ( const QWidget * child ) const
static QoreNode *QW_isAncestorOf(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_isEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isEnabled());
}

//bool isEnabledTo ( QWidget * ancestor ) const
static QoreNode *QW_isEnabledTo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_isFullScreen(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isFullScreen());
}

//bool isHidden () const
static QoreNode *QW_isHidden(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isHidden());
}

//bool isMaximized () const
static QoreNode *QW_isMaximized(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isMaximized());
}

//bool isMinimized () const
static QoreNode *QW_isMinimized(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isMinimized());
}

//bool isModal () const
static QoreNode *QW_isModal(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isModal());
}

//bool isVisible () const
static QoreNode *QW_isVisible(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isVisible());
}

//bool isVisibleTo ( QWidget * ancestor ) const
static QoreNode *QW_isVisibleTo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_isWindow(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isWindow());
}

//bool isWindowModified () const
static QoreNode *QW_isWindowModified(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->isWindowModified());
}

//QLayout * layout () const
//static QoreNode *QW_layout(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::LayoutDirection layoutDirection () const
//static QoreNode *QW_layoutDirection(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QLocale locale () const
//static QoreNode *QW_locale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE macCGHandle () const
//static QoreNode *QW_macCGHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE macQDHandle () const
//static QoreNode *QW_macQDHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapFrom ( QWidget * parent, const QPoint & pos ) const
//static QoreNode *QW_mapFrom(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapFromGlobal ( const QPoint & pos ) const
//static QoreNode *QW_mapFromGlobal(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapFromParent ( const QPoint & pos ) const
//static QoreNode *QW_mapFromParent(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapTo ( QWidget * parent, const QPoint & pos ) const
//static QoreNode *QW_mapTo(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapToGlobal ( const QPoint & pos ) const
//static QoreNode *QW_mapToGlobal(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint mapToParent ( const QPoint & pos ) const
//static QoreNode *QW_mapToParent(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QRegion mask () const
//static QoreNode *QW_mask(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int maximumHeight () const
static QoreNode *QW_maximumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->maximumHeight());
}

//QSize maximumSize () const
//static QoreNode *QW_maximumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int maximumWidth () const
static QoreNode *QW_maximumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->maximumWidth());
}

//int minimumHeight () const
static QoreNode *QW_minimumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->minimumHeight());
}

//QSize minimumSize () const
//static QoreNode *QW_minimumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual QSize minimumSizeHint () const
//static QoreNode *QW_minimumSizeHint(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int minimumWidth () const
static QoreNode *QW_minimumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->minimumWidth());
}

//void move ( const QPoint & )
//static QoreNode *QW_move(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void move ( int x, int y )
//static QoreNode *QW_move(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * nextInFocusChain () const
//static QoreNode *QW_nextInFocusChain(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QRect normalGeometry () const
//static QoreNode *QW_normalGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void overrideWindowFlags ( Qt::WindowFlags flags )
//static QoreNode *QW_overrideWindowFlags(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual QPaintEngine * paintEngine () const
//static QoreNode *QW_paintEngine(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//const QPalette & palette () const
//static QoreNode *QW_palette(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * parentWidget () const
//static QoreNode *QW_parentWidget(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QPoint pos () const
//static QoreNode *QW_pos(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QRect rect () const
//static QoreNode *QW_rect(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual void releaseDC ( HDC hdc ) const
//static QoreNode *QW_releaseDC(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void releaseKeyboard ()
//static QoreNode *QW_releaseKeyboard(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void releaseMouse ()
//static QoreNode *QW_releaseMouse(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void releaseShortcut ( int id )
//static QoreNode *QW_releaseShortcut(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void removeAction ( QAction * action )
//static QoreNode *QW_removeAction(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void render ( QPaintDevice * target, const QPoint & targetOffset = QPoint(), const QRegion & sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ) )
//static QoreNode *QW_render(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void repaint ( int x, int y, int w, int h )
//static QoreNode *QW_repaint(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void repaint ( const QRect & r )
//static QoreNode *QW_repaint(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void repaint ( const QRegion & rgn )
//static QoreNode *QW_repaint(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void resize ( const QSize & )
//void resize ( int w, int h )
static QoreNode *QW_resize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QW_restoreGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QByteArray saveGeometry () const
//static QoreNode *QW_saveGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void scroll ( int dx, int dy )
//static QoreNode *QW_scroll(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void scroll ( int dx, int dy, const QRect & r )
//static QoreNode *QW_scroll(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setAcceptDrops ( bool on )
//static QoreNode *QW_setAcceptDrops(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setAccessibleDescription ( const QString & description )
static QoreNode *QW_setAccessibleDescription(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setAccessibleDescription(p->val.String->getBuffer());
   return 0;
}

//void setAccessibleName ( const QString & name )
static QoreNode *QW_setAccessibleName(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setAccessibleName(p->val.String->getBuffer());
   return 0;
}

//void setAttribute ( Qt::WidgetAttribute attribute, bool on = true )
//static QoreNode *QW_setAttribute(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setAutoFillBackground ( bool enabled )
static QoreNode *QW_setAutoFillBackground(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qw->getQWidget()->setAutoFillBackground(!num_params(params) ? true : (!p ? false : p->getAsBool()));
   return 0;
}

//void setBackgroundRole ( QPalette::ColorRole role )
static QoreNode *QW_setBackgroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QW_setBaseSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setBaseSize ( int basew, int baseh )
//static QoreNode *QW_setBaseSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setContentsMargins ( int left, int top, int right, int bottom )
//static QoreNode *QW_setContentsMargins(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setContextMenuPolicy ( Qt::ContextMenuPolicy policy )
//static QoreNode *QW_setContextMenuPolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setCursor ( const QCursor & )
//static QoreNode *QW_setCursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setEditFocus ( bool enable )
//static QoreNode *QW_setEditFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setFixedHeight ( int h )
static QoreNode *QW_setFixedHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setFixedSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setFixedWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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


//void setFocus ( Qt::FocusReason reason )
//static QoreNode *QW_setFocus(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setFocusPolicy ( Qt::FocusPolicy policy )
//static QoreNode *QW_setFocusPolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setFocusProxy ( QWidget * w )
//static QoreNode *QW_setFocusProxy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setFont ( const QFont & )
static QoreNode *QW_setFont(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setForegroundRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QW_setInputContext(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setLayout ( QLayout * layout )
static QoreNode *QW_setLayout(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QW_setLayoutDirection(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setLocale ( const QLocale & locale )
//static QoreNode *QW_setLocale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setMask ( const QBitmap & bitmap )
//static QoreNode *QW_setMask(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setMask ( const QRegion & region )
//static QoreNode *QW_setMask(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setMaximumHeight ( int maxh )
static QoreNode *QW_setMaximumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setMaximumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setMaximumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setMinimumHeight(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setMinimumSize(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QW_setMinimumWidth(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
//static QoreNode *QW_setMouseTracking(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setPalette ( const QPalette & )
//static QoreNode *QW_setPalette(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setParent ( QWidget * parent )
//static QoreNode *QW_setParent(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setParent ( QWidget * parent, Qt::WindowFlags f )
//static QoreNode *QW_setParent(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setShortcutAutoRepeat ( int id, bool enable = true )
//static QoreNode *QW_setShortcutAutoRepeat(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setShortcutEnabled ( int id, bool enable = true )
//static QoreNode *QW_setShortcutEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setSizeIncrement ( const QSize & )
//static QoreNode *QW_setSizeIncrement(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setSizeIncrement ( int w, int h )
//static QoreNode *QW_setSizeIncrement(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setSizePolicy ( QSizePolicy )
//static QoreNode *QW_setSizePolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setSizePolicy ( QSizePolicy::Policy horizontal, QSizePolicy::Policy vertical )
//static QoreNode *QW_setSizePolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setStatusTip ( const QString & )
static QoreNode *QW_setStatusTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setStatusTip(p->val.String->getBuffer());
   return 0;
}

//void setStyle ( QStyle * style )
//static QoreNode *QW_setStyle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setToolTip ( const QString & )
static QoreNode *QW_setToolTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setToolTip(p->val.String->getBuffer());
   return 0;
}

//void setUpdatesEnabled ( bool enable )
static QoreNode *QW_setUpdatesEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qw->getQWidget()->setUpdatesEnabled(!num_params(params) ? true : (!p ? false : p->getAsBool()));
   return 0;
}

//void setWhatsThis ( const QString & )
static QoreNode *QW_setWhatsThis(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWhatsThis(p->val.String->getBuffer());
   return 0;
}

//void setWindowFlags ( Qt::WindowFlags type )
//static QoreNode *QW_setWindowFlags(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setWindowIcon ( const QIcon & icon )
//static QoreNode *QW_setWindowIcon(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setWindowIconText ( const QString & )
static QoreNode *QW_setWindowIconText(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWindowIconText(p->val.String->getBuffer());
   return 0;
}

//void setWindowModality ( Qt::WindowModality windowModality )
//static QoreNode *QW_setWindowModality(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setWindowOpacity ( qreal level )
//static QoreNode *QW_setWindowOpacity(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setWindowRole ( const QString & role )
static QoreNode *QW_setWindowRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (p)
      qw->getQWidget()->setWindowRole(p->val.String->getBuffer());
   return 0;
}

//void setWindowState ( Qt::WindowStates windowState )
//static QoreNode *QW_setWindowState(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void setWindowSurface ( QWindowSurface * surface )   (preliminary)
//static QoreNode *QW_setWindowSurface(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QSize size () const
//static QoreNode *QW_size(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//virtual QSize sizeHint () const
//static QoreNode *QW_sizeHint(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QSize sizeIncrement () const
//static QoreNode *QW_sizeIncrement(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QSizePolicy sizePolicy () const
//static QoreNode *QW_sizePolicy(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void stackUnder ( QWidget * w )
//static QoreNode *QW_stackUnder(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString statusTip () const
static QoreNode *QW_statusTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->statusTip().toUtf8().data(), QCS_UTF8));
}

//QStyle * style () const
//static QoreNode *QW_style(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString styleSheet () const
static QoreNode *QW_styleSheet(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->styleSheet().toUtf8().data(), QCS_UTF8));
}

//bool testAttribute ( Qt::WidgetAttribute attribute ) const
//static QoreNode *QW_testAttribute(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString toolTip () const
static QoreNode *QW_toolTip(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->toolTip().toUtf8().data(), QCS_UTF8));
}

//bool underMouse () const
static QoreNode *QW_underMouse(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->underMouse());
}

//void unsetCursor ()
static QoreNode *QW_unsetCursor(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetCursor();
   return 0;
}

//void unsetLayoutDirection ()
static QoreNode *QW_unsetLayoutDirection(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetLayoutDirection();
   return 0;
}

//void unsetLocale ()
static QoreNode *QW_unsetLocale(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->unsetLocale();
   return 0;
}

//void update ( int x, int y, int w, int h )
//static QoreNode *QW_update(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void update ( const QRect & r )
//static QoreNode *QW_update(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void update ( const QRegion & rgn )
//static QoreNode *QW_update(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//void updateGeometry ()
static QoreNode *QW_updateGeometry(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->updateGeometry();
   return 0;
}

//bool updatesEnabled () const
static QoreNode *QW_updatesEnabled(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qw->getQWidget()->updatesEnabled());
}

//QRegion visibleRegion () const
//static QoreNode *QW_visibleRegion(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString whatsThis () const
static QoreNode *QW_whatsThis(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->whatsThis().toUtf8().data(), QCS_UTF8));
}

//int width () const
static QoreNode *QW_width(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->width());
}

//WId winId () const
//static QoreNode *QW_winId(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWidget * window () const
//static QoreNode *QW_window(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::WindowFlags windowFlags () const
//static QoreNode *QW_windowFlags(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QIcon windowIcon () const
//static QoreNode *QW_windowIcon(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString windowIconText () const
static QoreNode *QW_windowIconText(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowIconText().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowModality windowModality () const
//static QoreNode *QW_windowModality(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//qreal windowOpacity () const
//static QoreNode *QW_windowOpacity(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString windowRole () const
static QoreNode *QW_windowRole(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowRole().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowStates windowState () const
//static QoreNode *QW_windowState(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QWindowSurface * windowSurface () const   (preliminary)
//static QoreNode *QW_windowSurface(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//QString windowTitle () const
static QoreNode *QW_windowTitle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qw->getQWidget()->windowTitle().toUtf8().data(), QCS_UTF8));
}

//Qt::WindowType windowType () const
//static QoreNode *QW_windowType(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int x () const
static QoreNode *QW_x(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qw->getQWidget()->x());
}

//const QX11Info & x11Info () const
//static QoreNode *QW_x11Info(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//Qt::HANDLE x11PictureHandle () const
//static QoreNode *QW_x11PictureHandle(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
//{
//}

//int y () const
static QoreNode *QW_y(class Object *self, QoreAbstractQWidget *qw, class QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *QWIDGET_repaint(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->repaint();
   return 0;
}

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

//void setFocus ()
static QoreNode *QWIDGET_setFocus(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->setFocus();
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

//void update ()
static QoreNode *QWIDGET_update(Object *self, QoreAbstractQWidget *qw, QoreNode *params, ExceptionSink *xsink)
{
   qw->getQWidget()->update();
   return 0;
}

class QoreClass *initQWidgetClass(class QoreClass *qobject)
{
   tracein("initQWidgetClass()");
   
   class QoreClass *QC_QWidget = new QoreClass("QWidget", QDOM_GUI);
   CID_QWIDGET = QC_QWidget->getID();

   QC_QWidget->addBuiltinVirtualBaseClass(qobject);

   QC_QWidget->setConstructor(QW_constructor);
   QC_QWidget->setCopy((q_copy_t)QW_copy);

   // add methods for slots
   QC_QWidget->addMethod("show",              (q_method_t)QW_show);

   QC_QWidget->addMethod("acceptDrops",                  (q_method_t)QW_acceptDrops);
   QC_QWidget->addMethod("accessibleDescription",        (q_method_t)QW_accessibleDescription);
   QC_QWidget->addMethod("accessibleName",               (q_method_t)QW_accessibleName);
   //QC_QWidget->addMethod("actions",                      (q_method_t)QW_actions);
   QC_QWidget->addMethod("activateWindow",               (q_method_t)QW_activateWindow);
   //QC_QWidget->addMethod("addAction",                    (q_method_t)QW_addAction);
   //QC_QWidget->addMethod("addActions",                   (q_method_t)QW_addActions);
   QC_QWidget->addMethod("adjustSize",                   (q_method_t)QW_adjustSize);
   QC_QWidget->addMethod("autoFillBackground",           (q_method_t)QW_autoFillBackground);
   QC_QWidget->addMethod("backgroundRole",               (q_method_t)QW_backgroundRole);
   //QC_QWidget->addMethod("baseSize",                     (q_method_t)QW_baseSize);
   //QC_QWidget->addMethod("childAt",                      (q_method_t)QW_childAt);
   //QC_QWidget->addMethod("childAt",                      (q_method_t)QW_childAt);
   //QC_QWidget->addMethod("childrenRect",                 (q_method_t)QW_childrenRect);
   //QC_QWidget->addMethod("childrenRegion",               (q_method_t)QW_childrenRegion);
   QC_QWidget->addMethod("clearFocus",                   (q_method_t)QW_clearFocus);
   QC_QWidget->addMethod("clearMask",                    (q_method_t)QW_clearMask);
   //QC_QWidget->addMethod("contentsRect",                 (q_method_t)QW_contentsRect);
   //QC_QWidget->addMethod("contextMenuPolicy",            (q_method_t)QW_contextMenuPolicy);
   //QC_QWidget->addMethod("cursor",                       (q_method_t)QW_cursor);
   //QC_QWidget->addMethod("ensurePolished",               (q_method_t)QW_ensurePolished);
   //QC_QWidget->addMethod("focusPolicy",                  (q_method_t)QW_focusPolicy);
   //QC_QWidget->addMethod("focusProxy",                   (q_method_t)QW_focusProxy);
   //QC_QWidget->addMethod("focusWidget",                  (q_method_t)QW_focusWidget);
   //QC_QWidget->addMethod("font",                         (q_method_t)QW_font);
   //QC_QWidget->addMethod("fontInfo",                     (q_method_t)QW_fontInfo);
   //QC_QWidget->addMethod("fontMetrics",                  (q_method_t)QW_fontMetrics);
   QC_QWidget->addMethod("foregroundRole",               (q_method_t)QW_foregroundRole);
   //QC_QWidget->addMethod("frameGeometry",                (q_method_t)QW_frameGeometry);
   //QC_QWidget->addMethod("frameSize",                    (q_method_t)QW_frameSize);
   //QC_QWidget->addMethod("geometry",                     (q_method_t)QW_geometry);
   //QC_QWidget->addMethod("getContentsMargins",           (q_method_t)QW_getContentsMargins);
   //QC_QWidget->addMethod("getDC",                        (q_method_t)QW_getDC);
   QC_QWidget->addMethod("grabKeyboard",                 (q_method_t)QW_grabKeyboard);
   QC_QWidget->addMethod("grabMouse",                    (q_method_t)QW_grabMouse);
   //QC_QWidget->addMethod("grabShortcut",                 (q_method_t)QW_grabShortcut);
#ifdef QT_KEYPAD_NAVIGATION
   QC_QWidget->addMethod("hasEditFocus",                 (q_method_t)QW_hasEditFocus);
#endif
   QC_QWidget->addMethod("hasFocus",                     (q_method_t)QW_hasFocus);
   QC_QWidget->addMethod("hasMouseTracking",             (q_method_t)QW_hasMouseTracking);
   QC_QWidget->addMethod("height",                       (q_method_t)QW_height);
   QC_QWidget->addMethod("heightForWidth",               (q_method_t)QW_heightForWidth);
   //QC_QWidget->addMethod("inputContext",                 (q_method_t)QW_inputContext);
   //QC_QWidget->addMethod("inputMethodQuery",             (q_method_t)QW_inputMethodQuery);
   //QC_QWidget->addMethod("insertAction",                 (q_method_t)QW_insertAction);
   //QC_QWidget->addMethod("insertActions",                (q_method_t)QW_insertActions);
   QC_QWidget->addMethod("isActiveWindow",               (q_method_t)QW_isActiveWindow);
   QC_QWidget->addMethod("isAncestorOf",                 (q_method_t)QW_isAncestorOf);
   QC_QWidget->addMethod("isEnabled",                    (q_method_t)QW_isEnabled);
   QC_QWidget->addMethod("isEnabledTo",                  (q_method_t)QW_isEnabledTo);
   QC_QWidget->addMethod("isFullScreen",                 (q_method_t)QW_isFullScreen);
   QC_QWidget->addMethod("isHidden",                     (q_method_t)QW_isHidden);
   QC_QWidget->addMethod("isMaximized",                  (q_method_t)QW_isMaximized);
   QC_QWidget->addMethod("isMinimized",                  (q_method_t)QW_isMinimized);
   QC_QWidget->addMethod("isModal",                      (q_method_t)QW_isModal);
   QC_QWidget->addMethod("isVisible",                    (q_method_t)QW_isVisible);
   QC_QWidget->addMethod("isVisibleTo",                  (q_method_t)QW_isVisibleTo);
   QC_QWidget->addMethod("isWindow",                     (q_method_t)QW_isWindow);
   QC_QWidget->addMethod("isWindowModified",             (q_method_t)QW_isWindowModified);
   //QC_QWidget->addMethod("layout",                       (q_method_t)QW_layout);
   //QC_QWidget->addMethod("layoutDirection",              (q_method_t)QW_layoutDirection);
   //QC_QWidget->addMethod("locale",                       (q_method_t)QW_locale);
   //QC_QWidget->addMethod("macCGHandle",                  (q_method_t)QW_macCGHandle);
   //QC_QWidget->addMethod("macQDHandle",                  (q_method_t)QW_macQDHandle);
   //QC_QWidget->addMethod("mapFrom",                      (q_method_t)QW_mapFrom);
   //QC_QWidget->addMethod("mapFromGlobal",                (q_method_t)QW_mapFromGlobal);
   //QC_QWidget->addMethod("mapFromParent",                (q_method_t)QW_mapFromParent);
   //QC_QWidget->addMethod("mapTo",                        (q_method_t)QW_mapTo);
   //QC_QWidget->addMethod("mapToGlobal",                  (q_method_t)QW_mapToGlobal);
   //QC_QWidget->addMethod("mapToParent",                  (q_method_t)QW_mapToParent);
   //QC_QWidget->addMethod("mask",                         (q_method_t)QW_mask);
   QC_QWidget->addMethod("maximumHeight",                (q_method_t)QW_maximumHeight);
   //QC_QWidget->addMethod("maximumSize",                  (q_method_t)QW_maximumSize);
   QC_QWidget->addMethod("maximumWidth",                 (q_method_t)QW_maximumWidth);
   QC_QWidget->addMethod("minimumHeight",                (q_method_t)QW_minimumHeight);
   //QC_QWidget->addMethod("minimumSize",                  (q_method_t)QW_minimumSize);
   //QC_QWidget->addMethod("minimumSizeHint",              (q_method_t)QW_minimumSizeHint);
   QC_QWidget->addMethod("minimumWidth",                 (q_method_t)QW_minimumWidth);
   //QC_QWidget->addMethod("move",                         (q_method_t)QW_move);
   //QC_QWidget->addMethod("nextInFocusChain",             (q_method_t)QW_nextInFocusChain);
   //QC_QWidget->addMethod("normalGeometry",               (q_method_t)QW_normalGeometry);
   //QC_QWidget->addMethod("overrideWindowFlags",          (q_method_t)QW_overrideWindowFlags);
   //QC_QWidget->addMethod("paintEngine",                  (q_method_t)QW_paintEngine);
   //QC_QWidget->addMethod("palette",                      (q_method_t)QW_palette);
   //QC_QWidget->addMethod("parentWidget",                 (q_method_t)QW_parentWidget);
   //QC_QWidget->addMethod("pos",                          (q_method_t)QW_pos);
   //QC_QWidget->addMethod("rect",                         (q_method_t)QW_rect);
   //QC_QWidget->addMethod("releaseDC",                    (q_method_t)QW_releaseDC);
   //QC_QWidget->addMethod("releaseKeyboard",              (q_method_t)QW_releaseKeyboard);
   //QC_QWidget->addMethod("releaseMouse",                 (q_method_t)QW_releaseMouse);
   //QC_QWidget->addMethod("releaseShortcut",              (q_method_t)QW_releaseShortcut);
   //QC_QWidget->addMethod("removeAction",                 (q_method_t)QW_removeAction);
   //QC_QWidget->addMethod("render",                       (q_method_t)QW_render);
   //QC_QWidget->addMethod("repaint",                      (q_method_t)QW_repaint);
   QC_QWidget->addMethod("resize",                       (q_method_t)QW_resize);
   //QC_QWidget->addMethod("restoreGeometry",              (q_method_t)QW_restoreGeometry);
   //QC_QWidget->addMethod("saveGeometry",                 (q_method_t)QW_saveGeometry);
   //QC_QWidget->addMethod("scroll",                       (q_method_t)QW_scroll);
   //QC_QWidget->addMethod("scroll",                       (q_method_t)QW_scroll);
   //QC_QWidget->addMethod("setAcceptDrops",               (q_method_t)QW_setAcceptDrops);
   QC_QWidget->addMethod("setAccessibleDescription",     (q_method_t)QW_setAccessibleDescription);
   QC_QWidget->addMethod("setAccessibleName",            (q_method_t)QW_setAccessibleName);
   //QC_QWidget->addMethod("setAttribute",                 (q_method_t)QW_setAttribute);
   QC_QWidget->addMethod("setAutoFillBackground",        (q_method_t)QW_setAutoFillBackground);
   QC_QWidget->addMethod("setBackgroundRole",            (q_method_t)QW_setBackgroundRole);
   //QC_QWidget->addMethod("setBaseSize",                  (q_method_t)QW_setBaseSize);
   //QC_QWidget->addMethod("setBaseSize",                  (q_method_t)QW_setBaseSize);
   //QC_QWidget->addMethod("setContentsMargins",           (q_method_t)QW_setContentsMargins);
   //QC_QWidget->addMethod("setContextMenuPolicy",         (q_method_t)QW_setContextMenuPolicy);
   //QC_QWidget->addMethod("setCursor",                    (q_method_t)QW_setCursor);
   //QC_QWidget->addMethod("setEditFocus",                 (q_method_t)QW_setEditFocus);
   QC_QWidget->addMethod("setFixedHeight",               (q_method_t)QW_setFixedHeight);
   QC_QWidget->addMethod("setFixedSize",                 (q_method_t)QW_setFixedSize);
   QC_QWidget->addMethod("setFixedWidth",                (q_method_t)QW_setFixedWidth);
   //QC_QWidget->addMethod("setFocus",                     (q_method_t)QW_setFocus);
   //QC_QWidget->addMethod("setFocusPolicy",               (q_method_t)QW_setFocusPolicy);
   //QC_QWidget->addMethod("setFocusProxy",                (q_method_t)QW_setFocusProxy);
   QC_QWidget->addMethod("setFont",                      (q_method_t)QW_setFont);
   QC_QWidget->addMethod("setForegroundRole",            (q_method_t)QW_setForegroundRole);
   QC_QWidget->addMethod("setGeometry",                  (q_method_t)QW_setGeometry);
   //QC_QWidget->addMethod("setInputContext",              (q_method_t)QW_setInputContext);
   QC_QWidget->addMethod("setLayout",                    (q_method_t)QW_setLayout);
   //QC_QWidget->addMethod("setLayoutDirection",           (q_method_t)QW_setLayoutDirection);
   //QC_QWidget->addMethod("setLocale",                    (q_method_t)QW_setLocale);
   //QC_QWidget->addMethod("setMask",                      (q_method_t)QW_setMask);
   //QC_QWidget->addMethod("setMask",                      (q_method_t)QW_setMask);
   QC_QWidget->addMethod("setMaximumHeight",             (q_method_t)QW_setMaximumHeight);
   QC_QWidget->addMethod("setMaximumSize",               (q_method_t)QW_setMaximumSize);
   QC_QWidget->addMethod("setMaximumWidth",              (q_method_t)QW_setMaximumWidth);
   QC_QWidget->addMethod("setMinimumHeight",             (q_method_t)QW_setMinimumHeight);
   QC_QWidget->addMethod("setMinimumSize",               (q_method_t)QW_setMinimumSize);
   QC_QWidget->addMethod("setMinimumWidth",              (q_method_t)QW_setMinimumWidth);
   //QC_QWidget->addMethod("setMouseTracking",             (q_method_t)QW_setMouseTracking);
   //QC_QWidget->addMethod("setPalette",                   (q_method_t)QW_setPalette);
   //QC_QWidget->addMethod("setParent",                    (q_method_t)QW_setParent);
   //QC_QWidget->addMethod("setParent",                    (q_method_t)QW_setParent);
   //QC_QWidget->addMethod("setShortcutAutoRepeat",        (q_method_t)QW_setShortcutAutoRepeat);
   //QC_QWidget->addMethod("setShortcutEnabled",           (q_method_t)QW_setShortcutEnabled);
   //QC_QWidget->addMethod("setSizeIncrement",             (q_method_t)QW_setSizeIncrement);
   //QC_QWidget->addMethod("setSizeIncrement",             (q_method_t)QW_setSizeIncrement);
   //QC_QWidget->addMethod("setSizePolicy",                (q_method_t)QW_setSizePolicy);
   //QC_QWidget->addMethod("setSizePolicy",                (q_method_t)QW_setSizePolicy);
   QC_QWidget->addMethod("setStatusTip",                 (q_method_t)QW_setStatusTip);
   //QC_QWidget->addMethod("setStyle",                     (q_method_t)QW_setStyle);
   QC_QWidget->addMethod("setToolTip",                   (q_method_t)QW_setToolTip);
   QC_QWidget->addMethod("setUpdatesEnabled",            (q_method_t)QW_setUpdatesEnabled);
   QC_QWidget->addMethod("setWhatsThis",                 (q_method_t)QW_setWhatsThis);
   //QC_QWidget->addMethod("setWindowFlags",               (q_method_t)QW_setWindowFlags);
   //QC_QWidget->addMethod("setWindowIcon",                (q_method_t)QW_setWindowIcon);
   QC_QWidget->addMethod("setWindowIconText",            (q_method_t)QW_setWindowIconText);
   //QC_QWidget->addMethod("setWindowModality",            (q_method_t)QW_setWindowModality);
   //QC_QWidget->addMethod("setWindowOpacity",             (q_method_t)QW_setWindowOpacity);
   QC_QWidget->addMethod("setWindowRole",                (q_method_t)QW_setWindowRole);
   //QC_QWidget->addMethod("setWindowState",               (q_method_t)QW_setWindowState);
   //QC_QWidget->addMethod("setWindowSurface",             (q_method_t)QW_setWindowSurface);
   //QC_QWidget->addMethod("size",                         (q_method_t)QW_size);
   //QC_QWidget->addMethod("sizeHint",                     (q_method_t)QW_sizeHint);
   //QC_QWidget->addMethod("sizeIncrement",                (q_method_t)QW_sizeIncrement);
   //QC_QWidget->addMethod("sizePolicy",                   (q_method_t)QW_sizePolicy);
   //QC_QWidget->addMethod("stackUnder",                   (q_method_t)QW_stackUnder);
   QC_QWidget->addMethod("statusTip",                    (q_method_t)QW_statusTip);
   //QC_QWidget->addMethod("style",                        (q_method_t)QW_style);
   QC_QWidget->addMethod("styleSheet",                   (q_method_t)QW_styleSheet);
   //QC_QWidget->addMethod("testAttribute",                (q_method_t)QW_testAttribute);
   QC_QWidget->addMethod("toolTip",                      (q_method_t)QW_toolTip);
   QC_QWidget->addMethod("underMouse",                   (q_method_t)QW_underMouse);
   QC_QWidget->addMethod("unsetCursor",                  (q_method_t)QW_unsetCursor);
   QC_QWidget->addMethod("unsetLayoutDirection",         (q_method_t)QW_unsetLayoutDirection);
   QC_QWidget->addMethod("unsetLocale",                  (q_method_t)QW_unsetLocale);
   //QC_QWidget->addMethod("update",                       (q_method_t)QW_update);
   QC_QWidget->addMethod("updateGeometry",               (q_method_t)QW_updateGeometry);
   QC_QWidget->addMethod("updatesEnabled",               (q_method_t)QW_updatesEnabled);
   //QC_QWidget->addMethod("visibleRegion",                (q_method_t)QW_visibleRegion);
   QC_QWidget->addMethod("whatsThis",                    (q_method_t)QW_whatsThis);
   QC_QWidget->addMethod("width",                        (q_method_t)QW_width);
   //QC_QWidget->addMethod("winId",                        (q_method_t)QW_winId);
   //QC_QWidget->addMethod("window",                       (q_method_t)QW_window);
   //QC_QWidget->addMethod("windowFlags",                  (q_method_t)QW_windowFlags);
   //QC_QWidget->addMethod("windowIcon",                   (q_method_t)QW_windowIcon);
   QC_QWidget->addMethod("windowIconText",               (q_method_t)QW_windowIconText);
   //QC_QWidget->addMethod("windowModality",               (q_method_t)QW_windowModality);
   //QC_QWidget->addMethod("windowOpacity",                (q_method_t)QW_windowOpacity);
   QC_QWidget->addMethod("windowRole",                   (q_method_t)QW_windowRole);
   //QC_QWidget->addMethod("windowtSate",                  (q_method_t)QW_windowState);
   //QC_QWidget->addMethod("windowSurface",                (q_method_t)QW_windowSurface);
   QC_QWidget->addMethod("windowTitle",                  (q_method_t)QW_windowTitle);
   //QC_QWidget->addMethod("windowType",                   (q_method_t)QW_windowType);
   QC_QWidget->addMethod("x",                            (q_method_t)QW_x);
   //QC_QWidget->addMethod("x11Info",                      (q_method_t)QW_x11Info);
   //QC_QWidget->addMethod("x11PictureHandle",             (q_method_t)QW_x11PictureHandle);
   QC_QWidget->addMethod("y",                            (q_method_t)QW_y);
   
   // slots
   QC_QWidget->addMethod("close",                       (q_method_t)QWIDGET_close);
   QC_QWidget->addMethod("hide",                        (q_method_t)QWIDGET_hide);
   QC_QWidget->addMethod("lower",                       (q_method_t)QWIDGET_lower);
   QC_QWidget->addMethod("raise",                       (q_method_t)QWIDGET_raise);
   QC_QWidget->addMethod("repaint",                     (q_method_t)QWIDGET_repaint);
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
   QC_QWidget->addMethod("update",                      (q_method_t)QWIDGET_update);


   traceout("initQWidgetClass()");
   return QC_QWidget;
}
