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

int CID_QABSTRACTBUTTON;
class QoreClass *QC_QAbstractButton = 0;

//QAbstractButton ( QWidget * parent = 0 )
static void QABSTRACTBUTTON_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTBUTTON-CONSTRUCTOR-ERROR", "QAbstractButton is an abstract class");
/*
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QABSTRACTBUTTON, new QoreQAbstractButton(self, parent ? parent->getQWidget() : 0));
   return;
*/
}

static void QABSTRACTBUTTON_copy(class Object *self, class Object *old, class QoreQAbstractButton *qab, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//bool autoExclusive () const
static QoreNode *QABSTRACTBUTTON_autoExclusive(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qab->getQAbstractButton()->autoExclusive());
}

//bool autoRepeat () const
static QoreNode *QABSTRACTBUTTON_autoRepeat(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qab->getQAbstractButton()->autoRepeat());
}

//int autoRepeatDelay () const
static QoreNode *QABSTRACTBUTTON_autoRepeatDelay(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qab->getQAbstractButton()->autoRepeatDelay());
}

//int autoRepeatInterval () const
static QoreNode *QABSTRACTBUTTON_autoRepeatInterval(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qab->getQAbstractButton()->autoRepeatInterval());
}

////QButtonGroup * group () const
//static QoreNode *QABSTRACTBUTTON_group(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qab->getQAbstractButton()->group();
//}

//QIcon icon () const
static QoreNode *QABSTRACTBUTTON_icon(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qab->getQAbstractButton()->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QSize iconSize () const
static QoreNode *QABSTRACTBUTTON_iconSize(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qab->getQAbstractButton()->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//bool isCheckable () const
static QoreNode *QABSTRACTBUTTON_isCheckable(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qab->getQAbstractButton()->isCheckable());
}

//bool isChecked () const
static QoreNode *QABSTRACTBUTTON_isChecked(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qab->getQAbstractButton()->isChecked());
}

//bool isDown () const
static QoreNode *QABSTRACTBUTTON_isDown(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qab->getQAbstractButton()->isDown());
}

//void setAutoExclusive ( bool )
static QoreNode *QABSTRACTBUTTON_setAutoExclusive(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setAutoExclusive(b);
   return 0;
}

//void setAutoRepeat ( bool )
static QoreNode *QABSTRACTBUTTON_setAutoRepeat(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setAutoRepeat(b);
   return 0;
}

//void setAutoRepeatDelay ( int )
static QoreNode *QABSTRACTBUTTON_setAutoRepeatDelay(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qab->getQAbstractButton()->setAutoRepeatDelay(x);
   return 0;
}

//void setAutoRepeatInterval ( int )
static QoreNode *QABSTRACTBUTTON_setAutoRepeatInterval(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qab->getQAbstractButton()->setAutoRepeatInterval(x);
   return 0;
}

//void setCheckable ( bool )
static QoreNode *QABSTRACTBUTTON_setCheckable(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setCheckable(b);
   return 0;
}

//void setDown ( bool )
static QoreNode *QABSTRACTBUTTON_setDown(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qab->getQAbstractButton()->setDown(b);
   return 0;
}

//void setIcon ( const QIcon & icon )
static QoreNode *QABSTRACTBUTTON_setIcon(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
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
static QoreNode *QABSTRACTBUTTON_setShortcut(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQKeySequence *key = (p && p->type == NT_OBJECT) ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   if (!key) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTBUTTON-SETSHORTCUT-PARAM-ERROR", "expecting a QKeySequence object as first argument to QAbstractButton::setShortcut()");
      return 0;
   }
   ReferenceHolder<QoreQKeySequence> keyHolder(key, xsink);
   qab->getQAbstractButton()->setShortcut(*(static_cast<QKeySequence *>(key)));
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QABSTRACTBUTTON_setText(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      return 0;

   QoreNodeTypeHelper str(p, NT_STRING, xsink);
   if (*xsink)
      return 0;
   
   const char *text = str->val.String->getBuffer();
   qab->getQAbstractButton()->setText(text);
   return 0;
}

//QKeySequence shortcut () const
static QoreNode *QABSTRACTBUTTON_shortcut(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qks = new Object(QC_QKeySequence, getProgram());
   QoreQKeySequence *q_qks = new QoreQKeySequence(qab->getQAbstractButton()->shortcut());
   o_qks->setPrivate(CID_QKEYSEQUENCE, q_qks);
   return new QoreNode(o_qks);
}

//QString text () const
static QoreNode *QABSTRACTBUTTON_text(Object *self, QoreAbstractQAbstractButton *qab, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qab->getQAbstractButton()->text().toUtf8().data(), QCS_UTF8));
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

   return QC_QAbstractButton;
}
