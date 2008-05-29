/*
 QC_QShortcut.h
 
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

#ifndef _QORE_QC_QSHORTCUT_H

#define _QORE_QC_QSHORTCUT_H

#include "QoreAbstractQWidget.h"

#include <QShortcut>

DLLEXPORT extern qore_classid_t CID_QSHORTCUT;
DLLEXPORT extern QoreClass *QC_QShortcut;

DLLEXPORT class QoreClass *initQShortcutClass(class QoreClass *parent);

DLLEXPORT extern void *static_void_args[];
DLLEXPORT extern QByteArray static_void_sig;

class myQShortcut : public QShortcut, public QoreQObjectExtension
{
#define QORE_QT_METACALL if (id == 1) { activated(); return -1; } else if (id == 2) { activatedAmbiguously(); return -1; }

#define QOREQTYPE QShortcut
#define MYQOREQTYPE myQShortcut
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   private:
      QoreAbstractQWidget *parent_widget;
      int target_m, target_am;

      DLLLOCAL void init_shortcut(QoreAbstractQWidget *n_parent)
      {
	 parent_widget = n_parent;
	 target_m = target_am = -1;

	 // create dummy slot entry for "activated" signal
	 methodMap.addMethod(new QoreQtDynamicSlot(qore_obj, 0, 0));
	 // catch activated signal
	 QByteArray theSignal = QMetaObject::normalizedSignature("activated()");
	 int sigid = metaObject()->indexOfSignal(theSignal);
	 //printd(5, "sigid=%d %s\n", sigid, theSignal.data());
	 assert(sigid >= 0);

	 QMetaObject::connect(this, sigid, this, metaObject()->methodCount() + 1);
	 //printd(5, "init_shortcut() connect b=%s sigid_a=%d\n", b ? "true" : "false", sigid); 

	 // create dummy slot entry for "activatedAmbiguously" signal
	 methodMap.addMethod(new QoreQtDynamicSlot(qore_obj, 0, 0));
	 theSignal = QMetaObject::normalizedSignature("activatedAmbiguously()");
	 sigid = metaObject()->indexOfSignal(theSignal);
	 //printd(5, "sigid=%d %s\n", sigid, theSignal.data());
	 assert(sigid >= 0);
	 QMetaObject::connect(this, sigid, this, metaObject()->methodCount() + 2);
      }

      DLLLOCAL void activated()
      {
	 //printd(5, "QoreQShortcut::activated() this=%08p target_m=%d\n", this, target_m);
	 if (target_m != -1)
	    parent_widget->getQObject()->qt_metacall(QMetaObject::InvokeMetaMethod, target_m, static_void_args);
      }
      
      DLLLOCAL void activatedAmbigously()
      {
	 if (target_am != -1)
	    parent_widget->getQObject()->qt_metacall(QMetaObject::InvokeMetaMethod, target_am, static_void_args);
      }
      
   public:
      DLLLOCAL myQShortcut(QoreObject *obj, QoreAbstractQWidget *parent = 0) : QShortcut(parent->getQWidget()), QoreQObjectExtension(obj, this)
      {
	 
	 init_shortcut(parent);
      }
      DLLLOCAL myQShortcut(QoreObject *obj, const QKeySequence & key, QoreAbstractQWidget * parent, Qt::ShortcutContext context = Qt::WindowShortcut) : QShortcut(key, parent->getQWidget(), 0, 0, context), QoreQObjectExtension(obj, this)
      {
	 
	 init_shortcut(parent);
      }

      DLLLOCAL void setMember(const char *target, class ExceptionSink *xsink)
      {
	 QByteArray theSlot = QMetaObject::normalizedSignature(target + 1);

	 if (!QMetaObject::checkConnectArgs(static_void_sig, theSlot)) {
	    xsink->raiseException("SHORTCUT-ERROR", "incompatible signature '%s' with 'void activated()' signal", target + 1);
	    return;
	 }

	 target_m = (target[0] == '1') ? parent_widget->getSlotIndex(theSlot, xsink) : parent_widget->getSignalIndex(theSlot);
	 if (target_m < 0 && target[0] != '1') {
	    xsink->raiseException("SHOTCUT-ERROR", "target signal '%s' does not exist", target + 1);
	    return;
	 }
	 //printd(5, "QoreQShortcut::setMember() this=%08p target=%s target_m=%d\n", this, target, target_m);
      }

      DLLLOCAL void setAmbiguousMember(const char *target, class ExceptionSink *xsink)
      {
	 QByteArray theSlot = QMetaObject::normalizedSignature(target + 1);

	 if (!QMetaObject::checkConnectArgs(static_void_sig, theSlot)) {
	    xsink->raiseException("SHORTCUT-ERROR", "incompatible signature '%s' with 'void activatedAmbiguously()' signal", target + 1);
	    return;
	 }

	 target_am = (target[0] == '1') ? parent_widget->getSlotIndex(theSlot, xsink) : parent_widget->getSignalIndex(theSlot);
	 if (target_am < 0 && target[0] != '1') {
	    xsink->raiseException("SHOTCUT-ERROR", "target signal '%s' does not exist", target + 1);
	    return;
	 }
      }
};

typedef QoreQObjectBase<myQShortcut, QoreAbstractQObject> QoreQShortcutImpl;

class QoreQShortcut : public QoreQShortcutImpl
{
   public:
      DLLLOCAL QoreQShortcut(QoreObject *obj, QoreAbstractQWidget *parent = 0) : QoreQShortcutImpl(new myQShortcut(obj, parent))
      {
      }

      DLLLOCAL QoreQShortcut(QoreObject *obj, const QKeySequence & key, QoreAbstractQWidget * parent, Qt::ShortcutContext context = Qt::WindowShortcut) : QoreQShortcutImpl(new myQShortcut(obj, key, parent, context))
      {
      }

      DLLLOCAL void setMember(const char *target, class ExceptionSink *xsink)
      {
	 qobj->setMember(target, xsink);
      }

      DLLLOCAL void setAmbiguousMember(const char *target, class ExceptionSink *xsink)
      {
	 qobj->setAmbiguousMember(target, xsink);
      }
};


#endif
