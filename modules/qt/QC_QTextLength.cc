/*
 QC_QTextLength.cc
 
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

#include "QC_QTextLength.h"

int CID_QTEXTLENGTH;
class QoreClass *QC_QTextLength = 0;

//QTextLength ()
//QTextLength ( Type type, qreal value )
static void QTEXTLENGTH_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTEXTLENGTH, new QoreQTextLength());
      return;
   }
   QTextLength::Type type = (QTextLength::Type)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   qreal value = p ? p->getAsFloat() : 0.0;
   self->setPrivate(CID_QTEXTLENGTH, new QoreQTextLength(type, value));
}

static void QTEXTLENGTH_copy(class QoreObject *self, class QoreObject *old, class QoreQTextLength *qtl, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTLENGTH, new QoreQTextLength(*qtl));
}

//qreal rawValue () const
static QoreNode *QTEXTLENGTH_rawValue(QoreObject *self, QoreQTextLength *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qtl->rawValue());
}

//Type type () const
static QoreNode *QTEXTLENGTH_type(QoreObject *self, QoreQTextLength *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->type());
}

//qreal value ( qreal maximumLength ) const
static QoreNode *QTEXTLENGTH_value(QoreObject *self, QoreQTextLength *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal maximumLength = p ? p->getAsFloat() : 0.0;
   return new QoreNode((double)qtl->value(maximumLength));
}

QoreClass *initQTextLengthClass()
{
   QC_QTextLength = new QoreClass("QTextLength", QDOM_GUI);
   CID_QTEXTLENGTH = QC_QTextLength->getID();

   QC_QTextLength->setConstructor(QTEXTLENGTH_constructor);
   QC_QTextLength->setCopy((q_copy_t)QTEXTLENGTH_copy);

   QC_QTextLength->addMethod("rawValue",                    (q_method_t)QTEXTLENGTH_rawValue);
   QC_QTextLength->addMethod("type",                        (q_method_t)QTEXTLENGTH_type);
   QC_QTextLength->addMethod("value",                       (q_method_t)QTEXTLENGTH_value);

   return QC_QTextLength;
}
