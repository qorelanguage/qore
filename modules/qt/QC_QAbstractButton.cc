/*
 QC_QAbstractButton.cc
 
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

#include "QC_QAbstractButton.h"
#include "QC_QIcon.h"
#include "QC_QKeySequence.h"

#include "qore-qt.h"

qore_classid_t CID_QABSTRACTBUTTON;
class QoreClass *QC_QAbstractButton = 0;

//QAbstractButton ( QWidget * parent = 0 )
static void QABSTRACTBUTTON_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTBUTTON-CONSTRUCTOR-ERROR", "QAbstractButton is an abstract class");
/*
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QABSTRACTBUTTON, new QoreQAbstractButton(self, parent ? parent->getQWidget() : 0));
   return;
*/
}

static void QABSTRACTBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQAbstractButton *qab, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//bool autoExclusive () const
static AbstractQoreNode *QABSTRACTBUTTON_autoExclusive(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qab->getQAbstractButton()->autoExclusive());
}

//bool autoRepeat () const
static AbstractQoreNode *QABSTRACTBUTTON_autoRepeat(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qab->getQAbstractButton()->autoRepeat());
}

//int autoRepeatDelay () const
static AbstractQoreNode *QABSTRACTBUTTON_autoRepeatDelay(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qab->getQAbstractButton()->autoRepeatDelay());
}

//int autoRepeatInterval () const
static AbstractQoreNode *QABSTRACTBUTTON_autoRepeatInterval(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qab->getQAbstractButton()->autoRepeatInterval());
}

////QButtonGroup * group () const
//static AbstractQoreNode *QABSTRACTBUTTON_group(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return qab->getQAbstractButton()->group();
//}

//QIcon icon () const
static AbstractQoreNode *QABSTRACTBUTTON_icon(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qab->getQAbstractButton()->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//QSize iconSize () const
static AbstractQoreNode *QABSTRACTBUTTON_iconSize(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qab->getQAbstractButton()->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//bool isCheckable () const
static AbstractQoreNode *QABSTRACTBUTTON_isCheckable(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qab->getQAbstractButton()->isCheckable());
}

//bool isChecked () const
static AbstractQoreNode *QABSTRACTBUTTON_isChecked(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qab->getQAbstractButton()->isChecked());
}

//bool isDown () const
static AbstractQoreNode *QABSTRACTBUTTON_isDown(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qab->getQAbstractButton()->isDown());
}

//void setAutoExclusive ( bool )
static AbstractQoreNode *QABSTRACTBUTTON_setAutoExclusive(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setAutoExclusive(b);
   return 0;
}

//void setAutoRepeat ( bool )
static AbstractQoreNode *QABSTRACTBUTTON_setAutoRepeat(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setAutoRepeat(b);
   return 0;
}

//void setAutoRepeatDelay ( int )
static AbstractQoreNode *QABSTRACTBUTTON_setAutoRepeatDelay(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qab->getQAbstractButton()->setAutoRepeatDelay(x);
   return 0;
}

//void setAutoRepeatInterval ( int )
static AbstractQoreNode *QABSTRACTBUTTON_setAutoRepeatInterval(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qab->getQAbstractButton()->setAutoRepeatInterval(x);
   return 0;
}

//void setCheckable ( bool )
static AbstractQoreNode *QABSTRACTBUTTON_setCheckable(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setCheckable(b);
   return 0;
}

//void setDown ( bool )
static AbstractQoreNode *QABSTRACTBUTTON_setDown(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setDown(b);
   return 0;
}

//void setIcon ( const QIcon & icon )
static AbstractQoreNode *QABSTRACTBUTTON_setIcon(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQIcon *icon = p ? (QoreQIcon *)p->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTBUTTON-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QAbstractButton::setIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
   qab->getQAbstractButton()->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setShortcut ( const QKeySequence & key )
static AbstractQoreNode *QABSTRACTBUTTON_setShortcut(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   
   QKeySequence key;
   if (get_qkeysequence(p, key, xsink))
      return 0;

   qab->getQAbstractButton()->setShortcut(key);
   return 0;
}

//void setText ( const QString & text )
static AbstractQoreNode *QABSTRACTBUTTON_setText(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      return 0;

   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   qab->getQAbstractButton()->setText(text);
   return 0;
}

//QKeySequence shortcut () const
static AbstractQoreNode *QABSTRACTBUTTON_shortcut(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qks = new QoreObject(QC_QKeySequence, getProgram());
   QoreQKeySequence *q_qks = new QoreQKeySequence(qab->getQAbstractButton()->shortcut());
   o_qks->setPrivate(CID_QKEYSEQUENCE, q_qks);
   return o_qks;
}

//QString text () const
static AbstractQoreNode *QABSTRACTBUTTON_text(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qab->getQAbstractButton()->text().toUtf8().data(), QCS_UTF8);
}

// slots
//void animateClick ( int msec = 100 )
static AbstractQoreNode *QABSTRACTBUTTON_animateClick(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int msec = !is_nothing(p) ? p->getAsInt() : 100;
   qab->getQAbstractButton()->animateClick(msec);
   return 0;
}

//void click ()
static AbstractQoreNode *QABSTRACTBUTTON_click(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   qab->getQAbstractButton()->click();
   return 0;
}

//void setChecked ( bool )
static AbstractQoreNode *QABSTRACTBUTTON_setChecked(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setChecked(b);
   return 0;
}

//void setIconSize ( const QSize & size )
static AbstractQoreNode *QABSTRACTBUTTON_setIconSize(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQSize *size = p ? (QoreQSize *)p->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTBUTTON-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QAbstractButton::setIconSize()");
      return 0;
   }
   ReferenceHolder<QoreQSize> sizeHolder(size, xsink);
   qab->getQAbstractButton()->setIconSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void toggle ()
static AbstractQoreNode *QABSTRACTBUTTON_toggle(QoreObject *self, QoreAbstractQAbstractButton *qab, const QoreListNode *params, ExceptionSink *xsink)
{
   qab->getQAbstractButton()->toggle();
   return 0;
}

QoreClass *initQAbstractButtonClass(QoreClass *qwidget)
{
   QC_QAbstractButton = new QoreClass("QAbstractButton", QDOM_GUI);
   CID_QABSTRACTBUTTON = QC_QAbstractButton->getID();

   QC_QAbstractButton->addBuiltinVirtualBaseClass(qwidget);

   QC_QAbstractButton->setConstructor(QABSTRACTBUTTON_constructor);
   QC_QAbstractButton->setCopy((q_copy_t)QABSTRACTBUTTON_copy);

   QC_QAbstractButton->addMethod("autoExclusive",               (q_method_t)QABSTRACTBUTTON_autoExclusive);
   QC_QAbstractButton->addMethod("autoRepeat",                  (q_method_t)QABSTRACTBUTTON_autoRepeat);
   QC_QAbstractButton->addMethod("autoRepeatDelay",             (q_method_t)QABSTRACTBUTTON_autoRepeatDelay);
   QC_QAbstractButton->addMethod("autoRepeatInterval",          (q_method_t)QABSTRACTBUTTON_autoRepeatInterval);
   //QC_QAbstractButton->addMethod("group",                       (q_method_t)QABSTRACTBUTTON_group);
   QC_QAbstractButton->addMethod("icon",                        (q_method_t)QABSTRACTBUTTON_icon);
   QC_QAbstractButton->addMethod("iconSize",                    (q_method_t)QABSTRACTBUTTON_iconSize);
   QC_QAbstractButton->addMethod("isCheckable",                 (q_method_t)QABSTRACTBUTTON_isCheckable);
   QC_QAbstractButton->addMethod("isChecked",                   (q_method_t)QABSTRACTBUTTON_isChecked);
   QC_QAbstractButton->addMethod("isDown",                      (q_method_t)QABSTRACTBUTTON_isDown);
   QC_QAbstractButton->addMethod("setAutoExclusive",            (q_method_t)QABSTRACTBUTTON_setAutoExclusive);
   QC_QAbstractButton->addMethod("setAutoRepeat",               (q_method_t)QABSTRACTBUTTON_setAutoRepeat);
   QC_QAbstractButton->addMethod("setAutoRepeatDelay",          (q_method_t)QABSTRACTBUTTON_setAutoRepeatDelay);
   QC_QAbstractButton->addMethod("setAutoRepeatInterval",       (q_method_t)QABSTRACTBUTTON_setAutoRepeatInterval);
   QC_QAbstractButton->addMethod("setCheckable",                (q_method_t)QABSTRACTBUTTON_setCheckable);
   QC_QAbstractButton->addMethod("setDown",                     (q_method_t)QABSTRACTBUTTON_setDown);
   QC_QAbstractButton->addMethod("setIcon",                     (q_method_t)QABSTRACTBUTTON_setIcon);
   QC_QAbstractButton->addMethod("setShortcut",                 (q_method_t)QABSTRACTBUTTON_setShortcut);
   QC_QAbstractButton->addMethod("setText",                     (q_method_t)QABSTRACTBUTTON_setText);
   QC_QAbstractButton->addMethod("shortcut",                    (q_method_t)QABSTRACTBUTTON_shortcut);
   QC_QAbstractButton->addMethod("text",                        (q_method_t)QABSTRACTBUTTON_text);

   // slots
   QC_QAbstractButton->addMethod("animateClick",                (q_method_t)QABSTRACTBUTTON_animateClick);
   QC_QAbstractButton->addMethod("click",                       (q_method_t)QABSTRACTBUTTON_click);
   QC_QAbstractButton->addMethod("setChecked",                  (q_method_t)QABSTRACTBUTTON_setChecked);
   QC_QAbstractButton->addMethod("setIconSize",                 (q_method_t)QABSTRACTBUTTON_setIconSize);
   QC_QAbstractButton->addMethod("toggle",                      (q_method_t)QABSTRACTBUTTON_toggle);

   return QC_QAbstractButton;
}
