/*
 QC_QEvent.cc
 
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

#include "QC_QEvent.h"
#include "QC_QRegion.h"
#include "QC_QRect.h"

int CID_QEVENT;

class QoreClass *QC_QEvent = 0;

static void QEVENT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QoreQEvent *qe = new QoreQEvent((QEvent::Type)(p ? p->getAsInt() : 0));

   self->setPrivate(CID_QEVENT, qe);
}

static void QEVENT_copy(class Object *self, class Object *old, class QoreQEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void accept ()
static QoreNode *QEVENT_accept(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   qe->accept();
   return 0;
}

//void ignore ()
static QoreNode *QEVENT_ignore(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   qe->ignore();
   return 0;
}

//bool isAccepted () const
static QoreNode *QEVENT_isAccepted(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qe->isAccepted());
}

//void setAccepted ( bool accepted )
static QoreNode *QEVENT_setAccepted(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool accepted = p ? p->getAsBool() : false;
   qe->setAccepted(accepted);
   return 0;
}

//bool spontaneous () const
static QoreNode *QEVENT_spontaneous(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qe->spontaneous());
}

//Type type () const
static QoreNode *QEVENT_type(Object *self, QoreQEvent *qe, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qe->type());
}

static class QoreClass *initQEventClass()
{
   tracein("initQEventClass()");
   
   QC_QEvent = new QoreClass("QEvent", QDOM_GUI);
   CID_QEVENT = QC_QEvent->getID();
   QC_QEvent->setConstructor(QEVENT_constructor);
   QC_QEvent->setCopy((q_copy_t)QEVENT_copy);

   QC_QEvent->addMethod("accept",                      (q_method_t)QEVENT_accept);
   QC_QEvent->addMethod("ignore",                      (q_method_t)QEVENT_ignore);
   QC_QEvent->addMethod("isAccepted",                  (q_method_t)QEVENT_isAccepted);
   QC_QEvent->addMethod("setAccepted",                 (q_method_t)QEVENT_setAccepted);
   QC_QEvent->addMethod("spontaneous",                 (q_method_t)QEVENT_spontaneous);
   QC_QEvent->addMethod("type",                        (q_method_t)QEVENT_type);

   traceout("initQEventClass()");
   return QC_QEvent;
}

Namespace *initQEventNS()
{
   Namespace *ns = new Namespace("QEvent");

   class QoreClass *qevent, *qinputevent, *qdropevent, *qdragmoveevent;
   ns->addSystemClass((qevent = initQEventClass()));
   ns->addSystemClass(initQPaintEventClass(qevent));
   ns->addSystemClass(initQMoveEventClass(qevent));
   ns->addSystemClass(initQResizeEventClass(qevent));

   ns->addSystemClass((qinputevent = initQInputEventClass(qevent)));
   ns->addSystemClass(initQKeyEventClass(qinputevent));
   ns->addSystemClass(initQMouseEventClass(qinputevent));
   ns->addSystemClass(initQContextMenuEventClass(qinputevent));
   ns->addSystemClass(initQTabletEventClass(qinputevent));
   ns->addSystemClass(initQWheelEventClass(qinputevent));

   ns->addSystemClass(initQActionEventClass(qevent));
   ns->addSystemClass(initQCloseEventClass(qevent));

   ns->addSystemClass((qdropevent = initQDropEventClass(qevent)));
   ns->addSystemClass((qdragmoveevent = initQDragMoveEventClass(qdropevent)));
   ns->addSystemClass(initQDragEnterEventClass(qdragmoveevent));

   ns->addSystemClass(initQDragLeaveEventClass(qevent));
   ns->addSystemClass(initQFocusEventClass(qevent));
   ns->addSystemClass(initQHideEventClass(qevent));
   ns->addSystemClass(initQInputMethodEventClass(qevent));
   ns->addSystemClass(initQShowEventClass(qevent));

   ns->addSystemClass(initQChildEventClass(qevent));
   ns->addSystemClass(initQTimerEventClass(qevent));
   ns->addSystemClass(initQHelpEventClass(qevent));

   // Type enum
   ns->addConstant("None",                     new QoreNode((int64)QEvent::None));
   ns->addConstant("Timer",                    new QoreNode((int64)QEvent::Timer));
   ns->addConstant("MouseButtonPress",         new QoreNode((int64)QEvent::MouseButtonPress));
   ns->addConstant("MouseButtonRelease",       new QoreNode((int64)QEvent::MouseButtonRelease));
   ns->addConstant("MouseButtonDblClick",      new QoreNode((int64)QEvent::MouseButtonDblClick));
   ns->addConstant("MouseMove",                new QoreNode((int64)QEvent::MouseMove));
   ns->addConstant("KeyPress",                 new QoreNode((int64)QEvent::KeyPress));
   ns->addConstant("KeyRelease",               new QoreNode((int64)QEvent::KeyRelease));
   ns->addConstant("FocusIn",                  new QoreNode((int64)QEvent::FocusIn));
   ns->addConstant("FocusOut",                 new QoreNode((int64)QEvent::FocusOut));
   ns->addConstant("Enter",                    new QoreNode((int64)QEvent::Enter));
   ns->addConstant("Leave",                    new QoreNode((int64)QEvent::Leave));
   ns->addConstant("Paint",                    new QoreNode((int64)QEvent::Paint));
   ns->addConstant("Move",                     new QoreNode((int64)QEvent::Move));
   ns->addConstant("Resize",                   new QoreNode((int64)QEvent::Resize));
   ns->addConstant("Create",                   new QoreNode((int64)QEvent::Create));
   ns->addConstant("Destroy",                  new QoreNode((int64)QEvent::Destroy));
   ns->addConstant("Show",                     new QoreNode((int64)QEvent::Show));
   ns->addConstant("Hide",                     new QoreNode((int64)QEvent::Hide));
   ns->addConstant("Close",                    new QoreNode((int64)QEvent::Close));
   ns->addConstant("Quit",                     new QoreNode((int64)QEvent::Quit));
   ns->addConstant("ParentChange",             new QoreNode((int64)QEvent::ParentChange));
   ns->addConstant("ParentAboutToChange",      new QoreNode((int64)QEvent::ParentAboutToChange));
   ns->addConstant("ThreadChange",             new QoreNode((int64)QEvent::ThreadChange));
   ns->addConstant("WindowActivate",           new QoreNode((int64)QEvent::WindowActivate));
   ns->addConstant("WindowDeactivate",         new QoreNode((int64)QEvent::WindowDeactivate));
   ns->addConstant("ShowToParent",             new QoreNode((int64)QEvent::ShowToParent));
   ns->addConstant("HideToParent",             new QoreNode((int64)QEvent::HideToParent));
   ns->addConstant("Wheel",                    new QoreNode((int64)QEvent::Wheel));
   ns->addConstant("WindowTitleChange",        new QoreNode((int64)QEvent::WindowTitleChange));
   ns->addConstant("WindowIconChange",         new QoreNode((int64)QEvent::WindowIconChange));
   ns->addConstant("ApplicationWindowIconChange", new QoreNode((int64)QEvent::ApplicationWindowIconChange));
   ns->addConstant("ApplicationFontChange",    new QoreNode((int64)QEvent::ApplicationFontChange));
   ns->addConstant("ApplicationLayoutDirectionChange", new QoreNode((int64)QEvent::ApplicationLayoutDirectionChange));
   ns->addConstant("ApplicationPaletteChange", new QoreNode((int64)QEvent::ApplicationPaletteChange));
   ns->addConstant("PaletteChange",            new QoreNode((int64)QEvent::PaletteChange));
   ns->addConstant("Clipboard",                new QoreNode((int64)QEvent::Clipboard));
   ns->addConstant("Speech",                   new QoreNode((int64)QEvent::Speech));
   ns->addConstant("MetaCall",                 new QoreNode((int64)QEvent::MetaCall));
   ns->addConstant("SockAct",                  new QoreNode((int64)QEvent::SockAct));
   ns->addConstant("WinEventAct",              new QoreNode((int64)QEvent::WinEventAct));
   ns->addConstant("DeferredDelete",           new QoreNode((int64)QEvent::DeferredDelete));
   ns->addConstant("DragEnter",                new QoreNode((int64)QEvent::DragEnter));
   ns->addConstant("DragMove",                 new QoreNode((int64)QEvent::DragMove));
   ns->addConstant("DragLeave",                new QoreNode((int64)QEvent::DragLeave));
   ns->addConstant("Drop",                     new QoreNode((int64)QEvent::Drop));
   ns->addConstant("DragResponse",             new QoreNode((int64)QEvent::DragResponse));
   ns->addConstant("ChildAdded",               new QoreNode((int64)QEvent::ChildAdded));
   ns->addConstant("ChildPolished",            new QoreNode((int64)QEvent::ChildPolished));
   ns->addConstant("ChildRemoved",             new QoreNode((int64)QEvent::ChildRemoved));
   ns->addConstant("ShowWindowRequest",        new QoreNode((int64)QEvent::ShowWindowRequest));
   ns->addConstant("PolishRequest",            new QoreNode((int64)QEvent::PolishRequest));
   ns->addConstant("Polish",                   new QoreNode((int64)QEvent::Polish));
   ns->addConstant("LayoutRequest",            new QoreNode((int64)QEvent::LayoutRequest));
   ns->addConstant("UpdateRequest",            new QoreNode((int64)QEvent::UpdateRequest));
   ns->addConstant("UpdateLater",              new QoreNode((int64)QEvent::UpdateLater));
   ns->addConstant("EmbeddingControl",         new QoreNode((int64)QEvent::EmbeddingControl));
   ns->addConstant("ActivateControl",          new QoreNode((int64)QEvent::ActivateControl));
   ns->addConstant("DeactivateControl",        new QoreNode((int64)QEvent::DeactivateControl));
   ns->addConstant("ContextMenu",              new QoreNode((int64)QEvent::ContextMenu));
   ns->addConstant("InputMethod",              new QoreNode((int64)QEvent::InputMethod));
   ns->addConstant("AccessibilityPrepare",     new QoreNode((int64)QEvent::AccessibilityPrepare));
   ns->addConstant("TabletMove",               new QoreNode((int64)QEvent::TabletMove));
   ns->addConstant("LocaleChange",             new QoreNode((int64)QEvent::LocaleChange));
   ns->addConstant("LanguageChange",           new QoreNode((int64)QEvent::LanguageChange));
   ns->addConstant("LayoutDirectionChange",    new QoreNode((int64)QEvent::LayoutDirectionChange));
   ns->addConstant("Style",                    new QoreNode((int64)QEvent::Style));
   ns->addConstant("TabletPress",              new QoreNode((int64)QEvent::TabletPress));
   ns->addConstant("TabletRelease",            new QoreNode((int64)QEvent::TabletRelease));
   ns->addConstant("OkRequest",                new QoreNode((int64)QEvent::OkRequest));
   ns->addConstant("HelpRequest",              new QoreNode((int64)QEvent::HelpRequest));
   ns->addConstant("IconDrag",                 new QoreNode((int64)QEvent::IconDrag));
   ns->addConstant("FontChange",               new QoreNode((int64)QEvent::FontChange));
   ns->addConstant("EnabledChange",            new QoreNode((int64)QEvent::EnabledChange));
   ns->addConstant("ActivationChange",         new QoreNode((int64)QEvent::ActivationChange));
   ns->addConstant("StyleChange",              new QoreNode((int64)QEvent::StyleChange));
   ns->addConstant("IconTextChange",           new QoreNode((int64)QEvent::IconTextChange));
   ns->addConstant("ModifiedChange",           new QoreNode((int64)QEvent::ModifiedChange));
   ns->addConstant("MouseTrackingChange",      new QoreNode((int64)QEvent::MouseTrackingChange));
   ns->addConstant("WindowBlocked",            new QoreNode((int64)QEvent::WindowBlocked));
   ns->addConstant("WindowUnblocked",          new QoreNode((int64)QEvent::WindowUnblocked));
   ns->addConstant("WindowStateChange",        new QoreNode((int64)QEvent::WindowStateChange));
   ns->addConstant("ToolTip",                  new QoreNode((int64)QEvent::ToolTip));
   ns->addConstant("WhatsThis",                new QoreNode((int64)QEvent::WhatsThis));
   ns->addConstant("StatusTip",                new QoreNode((int64)QEvent::StatusTip));
   ns->addConstant("ActionChanged",            new QoreNode((int64)QEvent::ActionChanged));
   ns->addConstant("ActionAdded",              new QoreNode((int64)QEvent::ActionAdded));
   ns->addConstant("ActionRemoved",            new QoreNode((int64)QEvent::ActionRemoved));
   ns->addConstant("FileOpen",                 new QoreNode((int64)QEvent::FileOpen));
   ns->addConstant("Shortcut",                 new QoreNode((int64)QEvent::Shortcut));
   ns->addConstant("ShortcutOverride",         new QoreNode((int64)QEvent::ShortcutOverride));
   ns->addConstant("WhatsThisClicked",         new QoreNode((int64)QEvent::WhatsThisClicked));
   ns->addConstant("ToolBarChange",            new QoreNode((int64)QEvent::ToolBarChange));
   ns->addConstant("ApplicationActivate",      new QoreNode((int64)QEvent::ApplicationActivate));
   ns->addConstant("ApplicationActivated",     new QoreNode((int64)QEvent::ApplicationActivated));
   ns->addConstant("ApplicationDeactivate",    new QoreNode((int64)QEvent::ApplicationDeactivate));
   ns->addConstant("ApplicationDeactivated",   new QoreNode((int64)QEvent::ApplicationDeactivated));
   ns->addConstant("QueryWhatsThis",           new QoreNode((int64)QEvent::QueryWhatsThis));
   ns->addConstant("EnterWhatsThisMode",       new QoreNode((int64)QEvent::EnterWhatsThisMode));
   ns->addConstant("LeaveWhatsThisMode",       new QoreNode((int64)QEvent::LeaveWhatsThisMode));
   ns->addConstant("ZOrderChange",             new QoreNode((int64)QEvent::ZOrderChange));
   ns->addConstant("HoverEnter",               new QoreNode((int64)QEvent::HoverEnter));
   ns->addConstant("HoverLeave",               new QoreNode((int64)QEvent::HoverLeave));
   ns->addConstant("HoverMove",                new QoreNode((int64)QEvent::HoverMove));
   ns->addConstant("AccessibilityHelp",        new QoreNode((int64)QEvent::AccessibilityHelp));
   ns->addConstant("AccessibilityDescription", new QoreNode((int64)QEvent::AccessibilityDescription));
#ifdef QT_KEYPAD_NAVIGATION
   ns->addConstant("EnterEditFocus",           new QoreNode((int64)QEvent::EnterEditFocus));
   ns->addConstant("LeaveEditFocus",           new QoreNode((int64)QEvent::LeaveEditFocus));
#endif
   ns->addConstant("AcceptDropsChange",        new QoreNode((int64)QEvent::AcceptDropsChange));
   ns->addConstant("MenubarUpdated",           new QoreNode((int64)QEvent::MenubarUpdated));
   ns->addConstant("ZeroTimerEvent",           new QoreNode((int64)QEvent::ZeroTimerEvent));
   ns->addConstant("GraphicsSceneMouseMove",   new QoreNode((int64)QEvent::GraphicsSceneMouseMove));
   ns->addConstant("GraphicsSceneMousePress",  new QoreNode((int64)QEvent::GraphicsSceneMousePress));
   ns->addConstant("GraphicsSceneMouseRelease", new QoreNode((int64)QEvent::GraphicsSceneMouseRelease));
   ns->addConstant("GraphicsSceneMouseDoubleClick", new QoreNode((int64)QEvent::GraphicsSceneMouseDoubleClick));
   ns->addConstant("GraphicsSceneContextMenu", new QoreNode((int64)QEvent::GraphicsSceneContextMenu));
   ns->addConstant("GraphicsSceneHoverEnter",  new QoreNode((int64)QEvent::GraphicsSceneHoverEnter));
   ns->addConstant("GraphicsSceneHoverMove",   new QoreNode((int64)QEvent::GraphicsSceneHoverMove));
   ns->addConstant("GraphicsSceneHoverLeave",  new QoreNode((int64)QEvent::GraphicsSceneHoverLeave));
   ns->addConstant("GraphicsSceneHelp",        new QoreNode((int64)QEvent::GraphicsSceneHelp));
   ns->addConstant("GraphicsSceneDragEnter",   new QoreNode((int64)QEvent::GraphicsSceneDragEnter));
   ns->addConstant("GraphicsSceneDragMove",    new QoreNode((int64)QEvent::GraphicsSceneDragMove));
   ns->addConstant("GraphicsSceneDragLeave",   new QoreNode((int64)QEvent::GraphicsSceneDragLeave));
   ns->addConstant("GraphicsSceneDrop",        new QoreNode((int64)QEvent::GraphicsSceneDrop));
   ns->addConstant("GraphicsSceneWheel",       new QoreNode((int64)QEvent::GraphicsSceneWheel));
   ns->addConstant("KeyboardLayoutChange",     new QoreNode((int64)QEvent::KeyboardLayoutChange));
   ns->addConstant("DynamicPropertyChange",    new QoreNode((int64)QEvent::DynamicPropertyChange));
   ns->addConstant("TabletEnterProximity",     new QoreNode((int64)QEvent::TabletEnterProximity));
   ns->addConstant("TabletLeaveProximity",     new QoreNode((int64)QEvent::TabletLeaveProximity));
   ns->addConstant("NonClientAreaMouseMove",   new QoreNode((int64)QEvent::NonClientAreaMouseMove));
   ns->addConstant("NonClientAreaMouseButtonPress", new QoreNode((int64)QEvent::NonClientAreaMouseButtonPress));
   ns->addConstant("NonClientAreaMouseButtonRelease", new QoreNode((int64)QEvent::NonClientAreaMouseButtonRelease));
   ns->addConstant("NonClientAreaMouseButtonDblClick", new QoreNode((int64)QEvent::NonClientAreaMouseButtonDblClick));
   ns->addConstant("MacSizeChange",            new QoreNode((int64)QEvent::MacSizeChange));
   ns->addConstant("ContentsRectChange",       new QoreNode((int64)QEvent::ContentsRectChange));
   ns->addConstant("MacGLWindowChange",        new QoreNode((int64)QEvent::MacGLWindowChange));
   ns->addConstant("User",                     new QoreNode((int64)QEvent::User));
   ns->addConstant("MaxUser",                  new QoreNode((int64)QEvent::MaxUser));

   return ns;
}
