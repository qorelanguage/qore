/*
 QC_QFontComboBox.cc
 
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

#include "QC_QFontComboBox.h"
#include "QC_QWidget.h"
#include "QC_QFont.h"

#include "qore-qt.h"

int CID_QFONTCOMBOBOX;
class QoreClass *QC_QFontComboBox = 0;

//QFontComboBox ( QWidget * parent = 0 )
static void QFONTCOMBOBOX_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!is_nothing(p) && !parent) {
      if (!xsink->isException())
         xsink->raiseException("QFONTCOMBOBOX-CONSTRUCTOR-PARAM-ERROR", "expecting a QWidget object as first argument to QFontComboBox::constructor()");
      return;
   }
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QFONTCOMBOBOX, new QoreQFontComboBox(self, parent ? parent->getQWidget() : 0));
   return;
}

static void QFONTCOMBOBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQFontComboBox *qfcb, ExceptionSink *xsink)
{
   xsink->raiseException("QFONTCOMBOBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//QFont currentFont () const
static QoreNode *QFONTCOMBOBOX_currentFont(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qfcb->qobj->currentFont());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//FontFilters fontFilters () const
static QoreNode *QFONTCOMBOBOX_fontFilters(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfcb->qobj->fontFilters());
}

//void setFontFilters ( FontFilters filters )
static QoreNode *QFONTCOMBOBOX_setFontFilters(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFontComboBox::FontFilters filters = (QFontComboBox::FontFilters)(p ? p->getAsInt() : 0);
   qfcb->qobj->setFontFilters(filters);
   return 0;
}

//void setWritingSystem ( QFontDatabase::WritingSystem script )
static QoreNode *QFONTCOMBOBOX_setWritingSystem(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QFontDatabase::WritingSystem script = (QFontDatabase::WritingSystem)(p ? p->getAsInt() : 0);
   qfcb->qobj->setWritingSystem(script);
   return 0;
}

//QFontDatabase::WritingSystem writingSystem () const
static QoreNode *QFONTCOMBOBOX_writingSystem(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qfcb->qobj->writingSystem());
}

//void setCurrentFont ( const QFont & font )
static QoreNode *QFONTCOMBOBOX_setCurrentFont(QoreObject *self, QoreQFontComboBox *qfcb, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QFONTCOMBOBOX-SETCURRENTFONT-PARAM-ERROR", "expecting a QFont object as first argument to QFontComboBox::setCurrentFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);
   qfcb->qobj->setCurrentFont(*(static_cast<QFont *>(font)));
   return 0;
}

QoreClass *initQFontComboBoxClass(QoreClass *qcombobox)
{
   QC_QFontComboBox = new QoreClass("QFontComboBox", QDOM_GUI);
   CID_QFONTCOMBOBOX = QC_QFontComboBox->getID();

   QC_QFontComboBox->addBuiltinVirtualBaseClass(qcombobox);

   QC_QFontComboBox->setConstructor(QFONTCOMBOBOX_constructor);
   QC_QFontComboBox->setCopy((q_copy_t)QFONTCOMBOBOX_copy);

   QC_QFontComboBox->addMethod("currentFont",                 (q_method_t)QFONTCOMBOBOX_currentFont);
   QC_QFontComboBox->addMethod("fontFilters",                 (q_method_t)QFONTCOMBOBOX_fontFilters);
   QC_QFontComboBox->addMethod("setFontFilters",              (q_method_t)QFONTCOMBOBOX_setFontFilters);
   QC_QFontComboBox->addMethod("setWritingSystem",            (q_method_t)QFONTCOMBOBOX_setWritingSystem);
   QC_QFontComboBox->addMethod("writingSystem",               (q_method_t)QFONTCOMBOBOX_writingSystem);
   QC_QFontComboBox->addMethod("setCurrentFont",              (q_method_t)QFONTCOMBOBOX_setCurrentFont);

   return QC_QFontComboBox;
}
