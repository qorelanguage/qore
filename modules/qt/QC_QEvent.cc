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

qore_classid_t CID_QEVENT;

class QoreClass *QC_QEvent = 0;

static void QEVENT_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   QoreQEvent *qe = new QoreQEvent((QEvent::Type)(p ? p->getAsInt() : 0));

   self->setPrivate(CID_QEVENT, qe);
}

static void QEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQEvent *qr, ExceptionSink *xsink)
{
   xsink->raiseException("QEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void accept ()
static AbstractQoreNode *QEVENT_accept(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   qe->accept();
   return 0;
}

//void ignore ()
static AbstractQoreNode *QEVENT_ignore(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   qe->ignore();
   return 0;
}

//bool isAccepted () const
static AbstractQoreNode *QEVENT_isAccepted(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qe->isAccepted());
}

//void setAccepted ( bool accepted )
static AbstractQoreNode *QEVENT_setAccepted(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool accepted = p ? p->getAsBool() : false;
   qe->setAccepted(accepted);
   return 0;
}

//bool spontaneous () const
static AbstractQoreNode *QEVENT_spontaneous(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qe->spontaneous());
}

//Type type () const
static AbstractQoreNode *QEVENT_type(QoreObject *self, QoreQEvent *qe, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qe->type());
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

QoreNamespace *initQEventNS()
{
   QoreNamespace *ns = new QoreNamespace("QEvent");

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
   ns->addConstant("None",                     new QoreBigIntNode(QEvent::None));
   ns->addConstant("Timer",                    new QoreBigIntNode(QEvent::Timer));
   ns->addConstant("MouseButtonPress",         new QoreBigIntNode(QEvent::MouseButtonPress));
   ns->addConstant("MouseButtonRelease",       new QoreBigIntNode(QEvent::MouseButtonRelease));
   ns->addConstant("MouseButtonDblClick",      new QoreBigIntNode(QEvent::MouseButtonDblClick));
   ns->addConstant("MouseMove",                new QoreBigIntNode(QEvent::MouseMove));
   ns->addConstant("KeyPress",                 new QoreBigIntNode(QEvent::KeyPress));
   ns->addConstant("KeyRelease",               new QoreBigIntNode(QEvent::KeyRelease));
   ns->addConstant("FocusIn",                  new QoreBigIntNode(QEvent::FocusIn));
   ns->addConstant("FocusOut",                 new QoreBigIntNode(QEvent::FocusOut));
   ns->addConstant("Enter",                    new QoreBigIntNode(QEvent::Enter));
   ns->addConstant("Leave",                    new QoreBigIntNode(QEvent::Leave));
   ns->addConstant("Paint",                    new QoreBigIntNode(QEvent::Paint));
   ns->addConstant("Move",                     new QoreBigIntNode(QEvent::Move));
   ns->addConstant("Resize",                   new QoreBigIntNode(QEvent::Resize));
   ns->addConstant("Create",                   new QoreBigIntNode(QEvent::Create));
   ns->addConstant("Destroy",                  new QoreBigIntNode(QEvent::Destroy));
   ns->addConstant("Show",                     new QoreBigIntNode(QEvent::Show));
   ns->addConstant("Hide",                     new QoreBigIntNode(QEvent::Hide));
   ns->addConstant("Close",                    new QoreBigIntNode(QEvent::Close));
   ns->addConstant("Quit",                     new QoreBigIntNode(QEvent::Quit));
   ns->addConstant("ParentChange",             new QoreBigIntNode(QEvent::ParentChange));
   ns->addConstant("ParentAboutToChange",      new QoreBigIntNode(QEvent::ParentAboutToChange));
   ns->addConstant("ThreadChange",             new QoreBigIntNode(QEvent::ThreadChange));
   ns->addConstant("WindowActivate",           new QoreBigIntNode(QEvent::WindowActivate));
   ns->addConstant("WindowDeactivate",         new QoreBigIntNode(QEvent::WindowDeactivate));
   ns->addConstant("ShowToParent",             new QoreBigIntNode(QEvent::ShowToParent));
   ns->addConstant("HideToParent",             new QoreBigIntNode(QEvent::HideToParent));
   ns->addConstant("Wheel",                    new QoreBigIntNode(QEvent::Wheel));
   ns->addConstant("WindowTitleChange",        new QoreBigIntNode(QEvent::WindowTitleChange));
   ns->addConstant("WindowIconChange",         new QoreBigIntNode(QEvent::WindowIconChange));
   ns->addConstant("ApplicationWindowIconChange", new QoreBigIntNode(QEvent::ApplicationWindowIconChange));
   ns->addConstant("ApplicationFontChange",    new QoreBigIntNode(QEvent::ApplicationFontChange));
   ns->addConstant("ApplicationLayoutDirectionChange", new QoreBigIntNode(QEvent::ApplicationLayoutDirectionChange));
   ns->addConstant("ApplicationPaletteChange", new QoreBigIntNode(QEvent::ApplicationPaletteChange));
   ns->addConstant("PaletteChange",            new QoreBigIntNode(QEvent::PaletteChange));
   ns->addConstant("Clipboard",                new QoreBigIntNode(QEvent::Clipboard));
   ns->addConstant("Speech",                   new QoreBigIntNode(QEvent::Speech));
   ns->addConstant("MetaCall",                 new QoreBigIntNode(QEvent::MetaCall));
   ns->addConstant("SockAct",                  new QoreBigIntNode(QEvent::SockAct));
   ns->addConstant("WinEventAct",              new QoreBigIntNode(QEvent::WinEventAct));
   ns->addConstant("DeferredDelete",           new QoreBigIntNode(QEvent::DeferredDelete));
   ns->addConstant("DragEnter",                new QoreBigIntNode(QEvent::DragEnter));
   ns->addConstant("DragMove",                 new QoreBigIntNode(QEvent::DragMove));
   ns->addConstant("DragLeave",                new QoreBigIntNode(QEvent::DragLeave));
   ns->addConstant("Drop",                     new QoreBigIntNode(QEvent::Drop));
   ns->addConstant("DragResponse",             new QoreBigIntNode(QEvent::DragResponse));
   ns->addConstant("ChildAdded",               new QoreBigIntNode(QEvent::ChildAdded));
   ns->addConstant("ChildPolished",            new QoreBigIntNode(QEvent::ChildPolished));
   ns->addConstant("ChildRemoved",             new QoreBigIntNode(QEvent::ChildRemoved));
   ns->addConstant("ShowWindowRequest",        new QoreBigIntNode(QEvent::ShowWindowRequest));
   ns->addConstant("PolishRequest",            new QoreBigIntNode(QEvent::PolishRequest));
   ns->addConstant("Polish",                   new QoreBigIntNode(QEvent::Polish));
   ns->addConstant("LayoutRequest",            new QoreBigIntNode(QEvent::LayoutRequest));
   ns->addConstant("UpdateRequest",            new QoreBigIntNode(QEvent::UpdateRequest));
   ns->addConstant("UpdateLater",              new QoreBigIntNode(QEvent::UpdateLater));
   ns->addConstant("EmbeddingControl",         new QoreBigIntNode(QEvent::EmbeddingControl));
   ns->addConstant("ActivateControl",          new QoreBigIntNode(QEvent::ActivateControl));
   ns->addConstant("DeactivateControl",        new QoreBigIntNode(QEvent::DeactivateControl));
   ns->addConstant("ContextMenu",              new QoreBigIntNode(QEvent::ContextMenu));
   ns->addConstant("InputMethod",              new QoreBigIntNode(QEvent::InputMethod));
   ns->addConstant("AccessibilityPrepare",     new QoreBigIntNode(QEvent::AccessibilityPrepare));
   ns->addConstant("TabletMove",               new QoreBigIntNode(QEvent::TabletMove));
   ns->addConstant("LocaleChange",             new QoreBigIntNode(QEvent::LocaleChange));
   ns->addConstant("LanguageChange",           new QoreBigIntNode(QEvent::LanguageChange));
   ns->addConstant("LayoutDirectionChange",    new QoreBigIntNode(QEvent::LayoutDirectionChange));
   ns->addConstant("Style",                    new QoreBigIntNode(QEvent::Style));
   ns->addConstant("TabletPress",              new QoreBigIntNode(QEvent::TabletPress));
   ns->addConstant("TabletRelease",            new QoreBigIntNode(QEvent::TabletRelease));
   ns->addConstant("OkRequest",                new QoreBigIntNode(QEvent::OkRequest));
   ns->addConstant("HelpRequest",              new QoreBigIntNode(QEvent::HelpRequest));
   ns->addConstant("IconDrag",                 new QoreBigIntNode(QEvent::IconDrag));
   ns->addConstant("FontChange",               new QoreBigIntNode(QEvent::FontChange));
   ns->addConstant("EnabledChange",            new QoreBigIntNode(QEvent::EnabledChange));
   ns->addConstant("ActivationChange",         new QoreBigIntNode(QEvent::ActivationChange));
   ns->addConstant("StyleChange",              new QoreBigIntNode(QEvent::StyleChange));
   ns->addConstant("IconTextChange",           new QoreBigIntNode(QEvent::IconTextChange));
   ns->addConstant("ModifiedChange",           new QoreBigIntNode(QEvent::ModifiedChange));
   ns->addConstant("MouseTrackingChange",      new QoreBigIntNode(QEvent::MouseTrackingChange));
   ns->addConstant("WindowBlocked",            new QoreBigIntNode(QEvent::WindowBlocked));
   ns->addConstant("WindowUnblocked",          new QoreBigIntNode(QEvent::WindowUnblocked));
   ns->addConstant("WindowStateChange",        new QoreBigIntNode(QEvent::WindowStateChange));
   ns->addConstant("ToolTip",                  new QoreBigIntNode(QEvent::ToolTip));
   ns->addConstant("WhatsThis",                new QoreBigIntNode(QEvent::WhatsThis));
   ns->addConstant("StatusTip",                new QoreBigIntNode(QEvent::StatusTip));
   ns->addConstant("ActionChanged",            new QoreBigIntNode(QEvent::ActionChanged));
   ns->addConstant("ActionAdded",              new QoreBigIntNode(QEvent::ActionAdded));
   ns->addConstant("ActionRemoved",            new QoreBigIntNode(QEvent::ActionRemoved));
   ns->addConstant("FileOpen",                 new QoreBigIntNode(QEvent::FileOpen));
   ns->addConstant("Shortcut",                 new QoreBigIntNode(QEvent::Shortcut));
   ns->addConstant("ShortcutOverride",         new QoreBigIntNode(QEvent::ShortcutOverride));
   ns->addConstant("WhatsThisClicked",         new QoreBigIntNode(QEvent::WhatsThisClicked));
   ns->addConstant("ToolBarChange",            new QoreBigIntNode(QEvent::ToolBarChange));
   ns->addConstant("ApplicationActivate",      new QoreBigIntNode(QEvent::ApplicationActivate));
   ns->addConstant("ApplicationActivated",     new QoreBigIntNode(QEvent::ApplicationActivated));
   ns->addConstant("ApplicationDeactivate",    new QoreBigIntNode(QEvent::ApplicationDeactivate));
   ns->addConstant("ApplicationDeactivated",   new QoreBigIntNode(QEvent::ApplicationDeactivated));
   ns->addConstant("QueryWhatsThis",           new QoreBigIntNode(QEvent::QueryWhatsThis));
   ns->addConstant("EnterWhatsThisMode",       new QoreBigIntNode(QEvent::EnterWhatsThisMode));
   ns->addConstant("LeaveWhatsThisMode",       new QoreBigIntNode(QEvent::LeaveWhatsThisMode));
   ns->addConstant("ZOrderChange",             new QoreBigIntNode(QEvent::ZOrderChange));
   ns->addConstant("HoverEnter",               new QoreBigIntNode(QEvent::HoverEnter));
   ns->addConstant("HoverLeave",               new QoreBigIntNode(QEvent::HoverLeave));
   ns->addConstant("HoverMove",                new QoreBigIntNode(QEvent::HoverMove));
   ns->addConstant("AccessibilityHelp",        new QoreBigIntNode(QEvent::AccessibilityHelp));
   ns->addConstant("AccessibilityDescription", new QoreBigIntNode(QEvent::AccessibilityDescription));
#ifdef QT_KEYPAD_NAVIGATION
   ns->addConstant("EnterEditFocus",           new QoreBigIntNode(QEvent::EnterEditFocus));
   ns->addConstant("LeaveEditFocus",           new QoreBigIntNode(QEvent::LeaveEditFocus));
#endif
   ns->addConstant("AcceptDropsChange",        new QoreBigIntNode(QEvent::AcceptDropsChange));
   ns->addConstant("MenubarUpdated",           new QoreBigIntNode(QEvent::MenubarUpdated));
   ns->addConstant("ZeroTimerEvent",           new QoreBigIntNode(QEvent::ZeroTimerEvent));
   ns->addConstant("GraphicsSceneMouseMove",   new QoreBigIntNode(QEvent::GraphicsSceneMouseMove));
   ns->addConstant("GraphicsSceneMousePress",  new QoreBigIntNode(QEvent::GraphicsSceneMousePress));
   ns->addConstant("GraphicsSceneMouseRelease", new QoreBigIntNode(QEvent::GraphicsSceneMouseRelease));
   ns->addConstant("GraphicsSceneMouseDoubleClick", new QoreBigIntNode(QEvent::GraphicsSceneMouseDoubleClick));
   ns->addConstant("GraphicsSceneContextMenu", new QoreBigIntNode(QEvent::GraphicsSceneContextMenu));
   ns->addConstant("GraphicsSceneHoverEnter",  new QoreBigIntNode(QEvent::GraphicsSceneHoverEnter));
   ns->addConstant("GraphicsSceneHoverMove",   new QoreBigIntNode(QEvent::GraphicsSceneHoverMove));
   ns->addConstant("GraphicsSceneHoverLeave",  new QoreBigIntNode(QEvent::GraphicsSceneHoverLeave));
   ns->addConstant("GraphicsSceneHelp",        new QoreBigIntNode(QEvent::GraphicsSceneHelp));
   ns->addConstant("GraphicsSceneDragEnter",   new QoreBigIntNode(QEvent::GraphicsSceneDragEnter));
   ns->addConstant("GraphicsSceneDragMove",    new QoreBigIntNode(QEvent::GraphicsSceneDragMove));
   ns->addConstant("GraphicsSceneDragLeave",   new QoreBigIntNode(QEvent::GraphicsSceneDragLeave));
   ns->addConstant("GraphicsSceneDrop",        new QoreBigIntNode(QEvent::GraphicsSceneDrop));
   ns->addConstant("GraphicsSceneWheel",       new QoreBigIntNode(QEvent::GraphicsSceneWheel));
   ns->addConstant("KeyboardLayoutChange",     new QoreBigIntNode(QEvent::KeyboardLayoutChange));
   ns->addConstant("DynamicPropertyChange",    new QoreBigIntNode(QEvent::DynamicPropertyChange));
   ns->addConstant("TabletEnterProximity",     new QoreBigIntNode(QEvent::TabletEnterProximity));
   ns->addConstant("TabletLeaveProximity",     new QoreBigIntNode(QEvent::TabletLeaveProximity));
   ns->addConstant("NonClientAreaMouseMove",   new QoreBigIntNode(QEvent::NonClientAreaMouseMove));
   ns->addConstant("NonClientAreaMouseButtonPress", new QoreBigIntNode(QEvent::NonClientAreaMouseButtonPress));
   ns->addConstant("NonClientAreaMouseButtonRelease", new QoreBigIntNode(QEvent::NonClientAreaMouseButtonRelease));
   ns->addConstant("NonClientAreaMouseButtonDblClick", new QoreBigIntNode(QEvent::NonClientAreaMouseButtonDblClick));
   ns->addConstant("MacSizeChange",            new QoreBigIntNode(QEvent::MacSizeChange));
   ns->addConstant("ContentsRectChange",       new QoreBigIntNode(QEvent::ContentsRectChange));
   ns->addConstant("MacGLWindowChange",        new QoreBigIntNode(QEvent::MacGLWindowChange));
   ns->addConstant("User",                     new QoreBigIntNode(QEvent::User));
   ns->addConstant("MaxUser",                  new QoreBigIntNode(QEvent::MaxUser));

   return ns;
}
