/*
 QC_QStyleOptionButton.cc
 
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

#include "QC_QStyleOptionButton.h"

int CID_QSTYLEOPTIONBUTTON;
class QoreClass *QC_QStyleOptionButton = 0;

//QStyleOptionButton ()
//QStyleOptionButton ( const QStyleOptionButton & other )
static void QSTYLEOPTIONBUTTON_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONBUTTON, new QoreQStyleOptionButton());
}

static void QSTYLEOPTIONBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionButton *qsob, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONBUTTON, new QoreQStyleOptionButton(*qsob));
}

//ButtonFeatures features ()
static QoreNode *QSTYLEOPTIONBUTTON_features(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qsob->features);
}

//QIcon icon ()
static QoreNode *QSTYLEOPTIONBUTTON_icon(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qsob->icon);
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QSize iconSize ()
static QoreNode *QSTYLEOPTIONBUTTON_iconSize(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qsob->iconSize);
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QString text ()
static QoreNode *QSTYLEOPTIONBUTTON_text(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qsob->text.toUtf8().data(), QCS_UTF8));
}

//void setFeatures ( ButtonFeatures features )
static QoreNode *QSTYLEOPTIONBUTTON_setFeatures(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyleOptionButton::ButtonFeatures features = (QStyleOptionButton::ButtonFeatures)(p ? p->getAsInt() : 0);
   qsob->features = features;
   return 0;
}

//void setIcon ( const QIcon & icon )
static QoreNode *QSTYLEOPTIONBUTTON_setIcon(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONBUTTON-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QStyleOptionButton::setIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qsob->icon = *(static_cast<QIcon *>(icon));
   return 0;
}

//void setIconSize ( const QSize & size )
static QoreNode *QSTYLEOPTIONBUTTON_setIconSize(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTIONBUTTON-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QStyleOptionButton::setIconSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qsob->iconSize = *(static_cast<QSize *>(size));
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QSTYLEOPTIONBUTTON_setText(QoreObject *self, QoreQStyleOptionButton *qsob, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qsob->text = text;
   return 0;
}

static QoreClass *initQStyleOptionButtonClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionButton = new QoreClass("QStyleOptionButton", QDOM_GUI);
   CID_QSTYLEOPTIONBUTTON = QC_QStyleOptionButton->getID();

   QC_QStyleOptionButton->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionButton->setConstructor(QSTYLEOPTIONBUTTON_constructor);
   QC_QStyleOptionButton->setCopy((q_copy_t)QSTYLEOPTIONBUTTON_copy);

   QC_QStyleOptionButton->addMethod("features",                    (q_method_t)QSTYLEOPTIONBUTTON_features);
   QC_QStyleOptionButton->addMethod("icon",                        (q_method_t)QSTYLEOPTIONBUTTON_icon);
   QC_QStyleOptionButton->addMethod("iconSize",                    (q_method_t)QSTYLEOPTIONBUTTON_iconSize);
   QC_QStyleOptionButton->addMethod("text",                        (q_method_t)QSTYLEOPTIONBUTTON_text);
   QC_QStyleOptionButton->addMethod("setFeatures",                 (q_method_t)QSTYLEOPTIONBUTTON_setFeatures);
   QC_QStyleOptionButton->addMethod("setIcon",                     (q_method_t)QSTYLEOPTIONBUTTON_setIcon);
   QC_QStyleOptionButton->addMethod("setIconSize",                 (q_method_t)QSTYLEOPTIONBUTTON_setIconSize);
   QC_QStyleOptionButton->addMethod("setText",                     (q_method_t)QSTYLEOPTIONBUTTON_setText);

   return QC_QStyleOptionButton;
}

QoreNamespace *initQStyleOptionButtonNS(QoreClass *qstyleoption)
{
   QoreNamespace *ns = new QoreNamespace("QStyleOptionButton");

   ns->addSystemClass(initQStyleOptionButtonClass(qstyleoption));

   // StyleOptionType enum
   ns->addConstant("Type",                     new QoreNode((int64)QStyleOptionButton::Type));

   // StyleOptionVersion enum
   ns->addConstant("Version",                  new QoreNode((int64)QStyleOptionButton::Version));

   // ButtonFeature enum
   ns->addConstant("None",                     new QoreNode((int64)QStyleOptionButton::None));
   ns->addConstant("Flat",                     new QoreNode((int64)QStyleOptionButton::Flat));
   ns->addConstant("HasMenu",                  new QoreNode((int64)QStyleOptionButton::HasMenu));
   ns->addConstant("DefaultButton",            new QoreNode((int64)QStyleOptionButton::DefaultButton));
   ns->addConstant("AutoDefaultButton",        new QoreNode((int64)QStyleOptionButton::AutoDefaultButton));
   ns->addConstant("CommandLinkButton",        new QoreNode((int64)QStyleOptionButton::CommandLinkButton));

   return ns;
}
