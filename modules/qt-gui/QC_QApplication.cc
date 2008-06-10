/*
 QC_QApplication.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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
#include "QC_QApplication.h"
#include "QC_QObject.h"
#include "QC_QClipboard.h"
#include "QC_QStyle.h"
#include "QC_QWidget.h"
#include "QC_QDesktopWidget.h"
#include "QC_QFont.h"
#include "QC_QLocale.h"

qore_classid_t CID_QAPPLICATION;

static QoreThreadLock qapp_lock;
static QoreObject *qore_qapp = 0;

void qapp_dec()
{
   AutoLocker al(&qapp_lock);
   qore_qapp = 0;
}

class AbstractQoreNode *get_qore_qapp()
{
   AutoLocker al(&qapp_lock);
   if (!qore_qapp)
      return 0;

   qore_qapp->ref();
   return qore_qapp;
}

static void QA_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AutoLocker al(&qapp_lock);
   if (qore_qapp) {
      xsink->raiseException("QAPPLICATION-ERROR", "only one QApplication can exist at one time");
      return;
   }

   // get argument list
   const QoreListNode *args = test_list_param(params, 0);
   QoreQtArgs *qt_args = new QoreQtArgs(args);

   self->setPrivate(CID_QAPPLICATION, new QoreQApplication(self, qt_args));
   qore_qapp = self;
}

static void QA_copy(class QoreObject *self, class QoreObject *old, class QoreQApplication *qa, ExceptionSink *xsink)
{
   xsink->raiseException("QAPPLICATION-COPY-ERROR", "objects of this class cannot be copied");
}

//QWidget * activeModalWidget ()
static AbstractQoreNode *f_QApplication_activeModalWidget(const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = QApplication::activeModalWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QWidget * activePopupWidget ()
static AbstractQoreNode *f_QApplication_activePopupWidget(const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = QApplication::activePopupWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QWidget * activeWindow ()
static AbstractQoreNode *f_QApplication_activeWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = QApplication::activeWindow();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//void alert ( QWidget * widget, int msec = 0 )
static AbstractQoreNode *f_QApplication_alert(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QAPPLICATION-ALERT-PARAM-ERROR", "expecting a QWidget object as first argument to QApplication::alert()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   int msec = !is_nothing(p) ? p->getAsInt() : 0;
   QApplication::alert(static_cast<QWidget *>(widget->getQWidget()), msec);
   return 0;
}

////QWidgetQoreListNode allWidgets ()
//static AbstractQoreNode *f_QApplication_allWidgets(const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(QApplication::allWidgets());
//}

//void beep ()
static AbstractQoreNode *f_QApplication_beep(const QoreListNode *params, ExceptionSink *xsink)
{
   QApplication::beep();
   return 0;
}

////void changeOverrideCursor ( const QCursor & cursor )
//static AbstractQoreNode *f_QApplication_changeOverrideCursor(const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QCursor cursor = p;
//   QApplication::changeOverrideCursor(cursor);
//   return 0;
//}

static AbstractQoreNode *f_QApplication_clipboard(const QoreListNode *params, class ExceptionSink *xsink)
{
   static QoreThreadLock lClipboard;

   AutoLocker al(&lClipboard);

   if (!C_Clipboard) {
      QoreObject *o = new QoreObject(QC_QClipboard, getProgram());
      QoreQClipboard *qcb = new QoreQClipboard(o, QApplication::clipboard());
      o->setPrivate(CID_QCLIPBOARD, qcb);
      C_Clipboard = o;
   }
   return C_Clipboard->refSelf();
}

//int colorSpec ()
static AbstractQoreNode *f_QApplication_colorSpec(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::colorSpec());
}

//int cursorFlashTime ()
static AbstractQoreNode *f_QApplication_cursorFlashTime(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::cursorFlashTime());
}

//QDesktopWidget * desktop ()
static AbstractQoreNode *f_QApplication_desktop(const QoreListNode *params, ExceptionSink *xsink)
{
   QDesktopWidget *qt_qobj = QApplication::desktop();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QDesktopWidget, getProgram());
      QoreQtQDesktopWidget *t_qobj = new QoreQtQDesktopWidget(rv_obj, qt_qobj, false);
      rv_obj->setPrivate(CID_QDESKTOPWIDGET, t_qobj);
   }
   return rv_obj;
}

//bool desktopSettingsAware ()
static AbstractQoreNode *f_QApplication_desktopSettingsAware(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QApplication::desktopSettingsAware());
}

//int doubleClickInterval ()
static AbstractQoreNode *f_QApplication_doubleClickInterval(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::doubleClickInterval());
}

//int exec ()
static AbstractQoreNode *f_QApplication_exec(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::exec());
}

//QWidget * focusWidget ()
static AbstractQoreNode *f_QApplication_focusWidget(const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = QApplication::focusWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QFont font ()
//QFont font ( const QWidget * widget )
//QFont font ( const char * className )
static AbstractQoreNode *f_QApplication_font(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
      QoreQFont *q_qf = new QoreQFont(QApplication::font());
      o_qf->setPrivate(CID_QFONT, q_qf);
      return o_qf;
   }
   qore_type_t ptype = p->getType();
   if (ptype == NT_OBJECT) {
      QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
         if (!xsink->isException())
            xsink->raiseException("QAPPLICATION-FONT-PARAM-ERROR", "QApplication::font() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
      QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
      QoreQFont *q_qf = new QoreQFont(QApplication::font(static_cast<QWidget *>(widget->getQWidget())));
      o_qf->setPrivate(CID_QFONT, q_qf);
      return o_qf;
   }
   if (ptype != NT_STRING) {
      xsink->raiseException("QAPPLICATION-FONT-PARAM-ERROR", "QApplication::font() expects either a string, a QWidget as the sole argument or no arguments at all");
      return 0;
   }
   const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(p);
   const char *className = pstr->getBuffer();
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(QApplication::font(className));
   o_qf->setPrivate(CID_QFONT, q_qf);
   return o_qf;
}

//QFontMetrics fontMetrics ()
static AbstractQoreNode *f_QApplication_fontMetrics(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qfm = new QoreObject(QC_QFontMetrics, getProgram());
   QoreQFontMetrics *q_qfm = new QoreQFontMetrics(QApplication::fontMetrics());
   o_qfm->setPrivate(CID_QFONTMETRICS, q_qfm);
   return o_qfm;
}

//QSize globalStrut ()
static AbstractQoreNode *f_QApplication_globalStrut(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(QApplication::globalStrut());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//bool isEffectEnabled ( Qt::UIEffect effect )
static AbstractQoreNode *f_QApplication_isEffectEnabled(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::UIEffect effect = (Qt::UIEffect)(p ? p->getAsInt() : 0);
   return get_bool_node(QApplication::isEffectEnabled(effect));
}

//bool isLeftToRight ()
static AbstractQoreNode *f_QApplication_isLeftToRight(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QApplication::isLeftToRight());
}

//bool isRightToLeft ()
static AbstractQoreNode *f_QApplication_isRightToLeft(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QApplication::isRightToLeft());
}

//Qt::LayoutDirection keyboardInputDirection ()
static AbstractQoreNode *f_QApplication_keyboardInputDirection(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::keyboardInputDirection());
}

//int keyboardInputInterval ()
static AbstractQoreNode *f_QApplication_keyboardInputInterval(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::keyboardInputInterval());
}

//QLocale keyboardInputLocale ()
static AbstractQoreNode *f_QApplication_keyboardInputLocale(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLocale, getProgram());
   QoreQLocale *q_ql = new QoreQLocale(QApplication::keyboardInputLocale());
   o_ql->setPrivate(CID_QLOCALE, q_ql);
   return o_ql;
}

//Qt::KeyboardModifiers keyboardModifiers ()
static AbstractQoreNode *f_QApplication_keyboardModifiers(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::keyboardModifiers());
}

#ifdef QT_KEYPAD_NAVIGATION
//bool keypadNavigationEnabled ()
static AbstractQoreNode *f_QApplication_keypadNavigationEnabled(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QApplication::keypadNavigationEnabled());
}
#endif

//Qt::LayoutDirection layoutDirection ()
static AbstractQoreNode *f_QApplication_layoutDirection(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::layoutDirection());
}

//Qt::MouseButtons mouseButtons ()
static AbstractQoreNode *f_QApplication_mouseButtons(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::mouseButtons());
}

////QCursor * overrideCursor ()
//static AbstractQoreNode *f_QApplication_overrideCursor(const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(QApplication::overrideCursor());
//}

//QPalette palette ()
//QPalette palette ( const QWidget * widget )
//QPalette palette ( const char * className )
static AbstractQoreNode *f_QApplication_palette(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
      QoreQPalette *q_qp = new QoreQPalette(QApplication::palette());
      o_qp->setPrivate(CID_QPALETTE, q_qp);
      return o_qp;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
         if (!xsink->isException())
            xsink->raiseException("QAPPLICATION-PALETTE-PARAM-ERROR", "QApplication::palette() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
      QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
      QoreQPalette *q_qp = new QoreQPalette(QApplication::palette(static_cast<QWidget *>(widget->getQWidget())));
      o_qp->setPrivate(CID_QPALETTE, q_qp);
      return o_qp;
   }
   const QoreStringNode *pstr = dynamic_cast<const QoreStringNode *>(p);
   if (!pstr) {
      xsink->raiseException("QAPPLICATION-PALETTE-PARAM-ERROR", "expecting a string as first argument to QApplication::palette()");
      return 0;
   }
   const char *className = pstr->getBuffer();
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(QApplication::palette(className));
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}

//bool quitOnLastWindowClosed ()
static AbstractQoreNode *f_QApplication_quitOnLastWindowClosed(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QApplication::quitOnLastWindowClosed());
}

////QDecoration & qwsDecoration ()
//static AbstractQoreNode *f_QApplication_qwsDecoration(const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(QApplication::qwsDecoration());
//}

////void qwsSetDecoration ( QDecoration * decoration )
////QDecoration * qwsSetDecoration ( const QString & decoration )
//static AbstractQoreNode *f_QApplication_qwsSetDecoration(const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QDecoration* decoration = p;
//   QApplication::qwsSetDecoration(decoration);
//   return 0;
//}

//void restoreOverrideCursor ()
static AbstractQoreNode *f_QApplication_restoreOverrideCursor(const QoreListNode *params, ExceptionSink *xsink)
{
   QApplication::restoreOverrideCursor();
   return 0;
}

//void setActiveWindow ( QWidget * active )
static AbstractQoreNode *f_QApplication_setActiveWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *active = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!active) {
      if (!xsink->isException())
         xsink->raiseException("QAPPLICATION-SETACTIVEWINDOW-PARAM-ERROR", "expecting a QWidget object as first argument to QApplication::setActiveWindow()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> activeHolder(static_cast<AbstractPrivateData *>(active), xsink);
   QApplication::setActiveWindow(static_cast<QWidget *>(active->getQWidget()));
   return 0;
}

//void setColorSpec ( int spec )
static AbstractQoreNode *f_QApplication_setColorSpec(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int spec = p ? p->getAsInt() : 0;
   QApplication::setColorSpec(spec);
   return 0;
}

//void setCursorFlashTime ( int )
static AbstractQoreNode *f_QApplication_setCursorFlashTime(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   QApplication::setCursorFlashTime(x);
   return 0;
}

//void setDesktopSettingsAware ( bool on )
static AbstractQoreNode *f_QApplication_setDesktopSettingsAware(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   QApplication::setDesktopSettingsAware(on);
   return 0;
}

//void setDoubleClickInterval ( int )
static AbstractQoreNode *f_QApplication_setDoubleClickInterval(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   QApplication::setDoubleClickInterval(x);
   return 0;
}

//void setEffectEnabled ( Qt::UIEffect effect, bool enable = true )
static AbstractQoreNode *f_QApplication_setEffectEnabled(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::UIEffect effect = (Qt::UIEffect)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   bool enable = !is_nothing(p) ? p->getAsBool() : true;
   QApplication::setEffectEnabled(effect, enable);
   return 0;
}

//void setFont ( const QFont & font, const char * className = 0 )
static AbstractQoreNode *f_QApplication_setFont(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQFont *font = p ? (QoreQFont *)p->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QAPPLICATION-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QApplication::setFont()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   const QoreStringNode *pstr = test_string_param(params, 1);
   const char *className = pstr ? pstr->getBuffer() : 0;
   QApplication::setFont(*(static_cast<QFont *>(font)), className);
   return 0;
}

////void setGlobalStrut ( const QSize & )
//static AbstractQoreNode *f_QApplication_setGlobalStrut(const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? const^QSize const^qsize = p;
//   QApplication::setGlobalStrut(const^qsize);
//   return 0;
//}

//void setKeyboardInputInterval ( int )
static AbstractQoreNode *f_QApplication_setKeyboardInputInterval(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   QApplication::setKeyboardInputInterval(x);
   return 0;
}

#ifdef QT_KEYPAD_NAVIGATION
//void setKeypadNavigationEnabled ( bool enable )
static AbstractQoreNode *f_QApplication_setKeypadNavigationEnabled(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   QApplication::setKeypadNavigationEnabled(enable);
   return 0;
}
#endif

//void setLayoutDirection ( Qt::LayoutDirection direction )
static AbstractQoreNode *f_QApplication_setLayoutDirection(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   QApplication::setLayoutDirection(direction);
   return 0;
}

////void setOverrideCursor ( const QCursor & cursor )
//static AbstractQoreNode *f_QApplication_setOverrideCursor(const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QCursor cursor = p;
//   QApplication::setOverrideCursor(cursor);
//   return 0;
//}

//void setPalette ( const QPalette & palette, const char * className = 0 )
static AbstractQoreNode *f_QApplication_setPalette(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQPalette *palette = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QAPPLICATION-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as first argument to QApplication::setPalette()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> paletteHolder(static_cast<AbstractPrivateData *>(palette), xsink);
   const QoreStringNode *pstr = test_string_param(params, 1);
   const char *className = pstr ? pstr->getBuffer() : 0;
   QApplication::setPalette(*(palette->getQPalette()), className);
   return 0;
}

//void setQuitOnLastWindowClosed ( bool quit )
static AbstractQoreNode *f_QApplication_setQuitOnLastWindowClosed(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool quit = p ? p->getAsBool() : false;
   QApplication::setQuitOnLastWindowClosed(quit);
   return 0;
}

//void setStartDragDistance ( int l )
static AbstractQoreNode *f_QApplication_setStartDragDistance(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int l = p ? p->getAsInt() : 0;
   QApplication::setStartDragDistance(l);
   return 0;
}

//void setStartDragTime ( int ms )
static AbstractQoreNode *f_QApplication_setStartDragTime(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int ms = p ? p->getAsInt() : 0;
   QApplication::setStartDragTime(ms);
   return 0;
}

//void setStyle ( QStyle * style )
//QStyle * setStyle ( const QString & style )
static AbstractQoreNode *f_QApplication_setStyle(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
      QoreAbstractQStyle *style = (QoreAbstractQStyle *)o->getReferencedPrivateData(CID_QSTYLE, xsink);
      if (!style) {
         if (!xsink->isException())
            xsink->raiseException("QAPPLICATION-SETSTYLE-PARAM-ERROR", "QApplication::setStyle() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> styleHolder(static_cast<AbstractPrivateData *>(style), xsink);
      QApplication::setStyle(style->getQStyle());
      return 0;
   }
   QString style;
   if (get_qstring(p, style, xsink))
      return 0;
   return return_qstyle(style, QApplication::setStyle(style), xsink);
}

//void setWheelScrollLines ( int )
static AbstractQoreNode *f_QApplication_setWheelScrollLines(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   QApplication::setWheelScrollLines(x);
   return 0;
}

//void setWindowIcon ( const QIcon & icon )
static AbstractQoreNode *f_QApplication_setWindowIcon(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQIcon *icon = p ? (QoreQIcon *)p->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QAPPLICATION-SETWINDOWICON-PARAM-ERROR", "expecting a QIcon object as first argument to QApplication::setWindowIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   QApplication::setWindowIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//int startDragDistance ()
static AbstractQoreNode *f_QApplication_startDragDistance(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::startDragDistance());
}

//int startDragTime ()
static AbstractQoreNode *f_QApplication_startDragTime(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::startDragTime());
}

//QStyle * style ()
static AbstractQoreNode *f_QApplication_style(const QoreListNode *params, ExceptionSink *xsink)
{
   QStyle *qt_qobj = QApplication::style();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else
   {
      rv_obj = new QoreObject(QC_QStyle, getProgram());
      QoreQtQStyle *qs = new QoreQtQStyle(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QSTYLE, qs);
   }
   return rv_obj;
}

//void syncX ()
static AbstractQoreNode *f_QApplication_syncX(const QoreListNode *params, ExceptionSink *xsink)
{
   QApplication::syncX();
   return 0;
}

//QWidget * topLevelAt ( const QPoint & point )
//QWidget * topLevelAt ( int x, int y )
static AbstractQoreNode *f_QApplication_topLevelAt(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QAPPLICATION-TOPLEVELAT-PARAM-ERROR", "QApplication::topLevelAt() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QWidget *qt_qobj = QApplication::topLevelAt(*(static_cast<QPoint *>(point)));
      if (!qt_qobj)
         return 0;
      QVariant qv_ptr = qt_qobj->property("qobject");
      QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
      assert(rv_obj);
      rv_obj->ref();
      return rv_obj;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = QApplication::topLevelAt(x, y);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

////QWidgetQoreListNode topLevelWidgets ()
//static AbstractQoreNode *f_QApplication_topLevelWidgets(const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(QApplication::topLevelWidgets());
//}

//Type type ()
static AbstractQoreNode *f_QApplication_type(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::type());
}

//int wheelScrollLines ()
static AbstractQoreNode *f_QApplication_wheelScrollLines(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QApplication::wheelScrollLines());
}

//QWidget * widgetAt ( const QPoint & point )
//QWidget * widgetAt ( int x, int y )
static AbstractQoreNode *f_QApplication_widgetAt(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QAPPLICATION-WIDGETAT-PARAM-ERROR", "QApplication::widgetAt() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QWidget *qt_qobj = QApplication::widgetAt(*(static_cast<QPoint *>(point)));
      if (!qt_qobj)
         return 0;
      QVariant qv_ptr = qt_qobj->property("qobject");
      QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
      assert(rv_obj);
      rv_obj->ref();
      return rv_obj;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = QApplication::widgetAt(x, y);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QIcon windowIcon ()
static AbstractQoreNode *f_QApplication_windowIcon(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(QApplication::windowIcon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

QoreClass *initQApplicationClass(class QoreClass *qcoreapplication)
{
   tracein("initQApplicationClass()");
   
   QoreClass *QC_QApplication = new QoreClass("QApplication", QDOM_GUI);
   CID_QAPPLICATION = QC_QApplication->getID();

   QC_QApplication->addBuiltinVirtualBaseClass(qcoreapplication);

   QC_QApplication->setConstructor(QA_constructor);
   QC_QApplication->setCopy((q_copy_t)QA_copy);

   // static functions
   QC_QApplication->addStaticMethod("activeModalWidget",            f_QApplication_activeModalWidget);
   QC_QApplication->addStaticMethod("activePopupWidget",            f_QApplication_activePopupWidget);
   QC_QApplication->addStaticMethod("activeWindow",                 f_QApplication_activeWindow);
   QC_QApplication->addStaticMethod("alert",                        f_QApplication_alert);
   //QC_QApplication->addStaticMethod("allWidgets",                   f_QApplication_allWidgets);
   QC_QApplication->addStaticMethod("beep",                         f_QApplication_beep);
   //QC_QApplication->addStaticMethod("changeOverrideCursor",         f_QApplication_changeOverrideCursor);
   QC_QApplication->addStaticMethod("clipboard",                    f_QApplication_clipboard);
   QC_QApplication->addStaticMethod("colorSpec",                    f_QApplication_colorSpec);
   QC_QApplication->addStaticMethod("cursorFlashTime",              f_QApplication_cursorFlashTime);
   QC_QApplication->addStaticMethod("desktop",                      f_QApplication_desktop);
   QC_QApplication->addStaticMethod("desktopSettingsAware",         f_QApplication_desktopSettingsAware);
   QC_QApplication->addStaticMethod("doubleClickInterval",          f_QApplication_doubleClickInterval);
   QC_QApplication->addStaticMethod("exec",                         f_QApplication_exec);
   QC_QApplication->addStaticMethod("focusWidget",                  f_QApplication_focusWidget);
   QC_QApplication->addStaticMethod("font",                         f_QApplication_font);
   QC_QApplication->addStaticMethod("fontMetrics",                  f_QApplication_fontMetrics);
   QC_QApplication->addStaticMethod("globalStrut",                  f_QApplication_globalStrut);
   QC_QApplication->addStaticMethod("isEffectEnabled",              f_QApplication_isEffectEnabled);
   QC_QApplication->addStaticMethod("isLeftToRight",                f_QApplication_isLeftToRight);
   QC_QApplication->addStaticMethod("isRightToLeft",                f_QApplication_isRightToLeft);
   QC_QApplication->addStaticMethod("keyboardInputDirection",       f_QApplication_keyboardInputDirection);
   QC_QApplication->addStaticMethod("keyboardInputInterval",        f_QApplication_keyboardInputInterval);
   QC_QApplication->addStaticMethod("keyboardInputLocale",          f_QApplication_keyboardInputLocale);
   QC_QApplication->addStaticMethod("keyboardModifiers",            f_QApplication_keyboardModifiers);
#ifdef QT_KEYPAD_NAVIGATION
   QC_QApplication->addStaticMethod("keypadNavigationEnabled",      f_QApplication_keypadNavigationEnabled);
#endif
   QC_QApplication->addStaticMethod("layoutDirection",              f_QApplication_layoutDirection);
   QC_QApplication->addStaticMethod("mouseButtons",                 f_QApplication_mouseButtons);
   //QC_QApplication->addStaticMethod("overrideCursor",               f_QApplication_overrideCursor);
   QC_QApplication->addStaticMethod("palette",                      f_QApplication_palette);
   QC_QApplication->addStaticMethod("quitOnLastWindowClosed",       f_QApplication_quitOnLastWindowClosed);
   //QC_QApplication->addStaticMethod("qwsDecoration",                f_QApplication_qwsDecoration);
   //QC_QApplication->addStaticMethod("qwsSetDecoration",             f_QApplication_qwsSetDecoration);
   QC_QApplication->addStaticMethod("restoreOverrideCursor",        f_QApplication_restoreOverrideCursor);
   QC_QApplication->addStaticMethod("setActiveWindow",              f_QApplication_setActiveWindow);
   QC_QApplication->addStaticMethod("setColorSpec",                 f_QApplication_setColorSpec);
   QC_QApplication->addStaticMethod("setCursorFlashTime",           f_QApplication_setCursorFlashTime);
   QC_QApplication->addStaticMethod("setDesktopSettingsAware",      f_QApplication_setDesktopSettingsAware);
   QC_QApplication->addStaticMethod("setDoubleClickInterval",       f_QApplication_setDoubleClickInterval);
   QC_QApplication->addStaticMethod("setEffectEnabled",             f_QApplication_setEffectEnabled);
   QC_QApplication->addStaticMethod("setFont",                      f_QApplication_setFont);
   //QC_QApplication->addStaticMethod("setGlobalStrut",               f_QApplication_setGlobalStrut);
   QC_QApplication->addStaticMethod("setKeyboardInputInterval",     f_QApplication_setKeyboardInputInterval);
#ifdef QT_KEYPAD_NAVIGATION
   QC_QApplication->addStaticMethod("setKeypadNavigationEnabled",   f_QApplication_setKeypadNavigationEnabled);
#endif
   QC_QApplication->addStaticMethod("setLayoutDirection",           f_QApplication_setLayoutDirection);
   //QC_QApplication->addStaticMethod("setOverrideCursor",            f_QApplication_setOverrideCursor);
   QC_QApplication->addStaticMethod("setPalette",                   f_QApplication_setPalette);
   QC_QApplication->addStaticMethod("setQuitOnLastWindowClosed",    f_QApplication_setQuitOnLastWindowClosed);
   QC_QApplication->addStaticMethod("setStartDragDistance",         f_QApplication_setStartDragDistance);
   QC_QApplication->addStaticMethod("setStartDragTime",             f_QApplication_setStartDragTime);
   QC_QApplication->addStaticMethod("setStyle",                     f_QApplication_setStyle);
   QC_QApplication->addStaticMethod("setWheelScrollLines",          f_QApplication_setWheelScrollLines);
   QC_QApplication->addStaticMethod("setWindowIcon",                f_QApplication_setWindowIcon);
   QC_QApplication->addStaticMethod("startDragDistance",            f_QApplication_startDragDistance);
   QC_QApplication->addStaticMethod("startDragTime",                f_QApplication_startDragTime);
   QC_QApplication->addStaticMethod("style",                        f_QApplication_style);
   QC_QApplication->addStaticMethod("syncX",                        f_QApplication_syncX);
   QC_QApplication->addStaticMethod("topLevelAt",                   f_QApplication_topLevelAt);
   //QC_QApplication->addStaticMethod("topLevelWidgets",              f_QApplication_topLevelWidgets);
   QC_QApplication->addStaticMethod("type",                         f_QApplication_type);
   QC_QApplication->addStaticMethod("wheelScrollLines",             f_QApplication_wheelScrollLines);
   QC_QApplication->addStaticMethod("widgetAt",                     f_QApplication_widgetAt);
   QC_QApplication->addStaticMethod("windowIcon",                   f_QApplication_windowIcon);

   traceout("initQApplicationClass()");
   return QC_QApplication;
}
