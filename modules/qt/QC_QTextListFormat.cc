/*
 QC_QTextListFormat.cc
 
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

#include "QC_QTextListFormat.h"

int CID_QTEXTLISTFORMAT;
class QoreClass *QC_QTextListFormat = 0;

//QTextListFormat ()
static void QTEXTLISTFORMAT_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTLISTFORMAT, new QoreQTextListFormat());
}

static void QTEXTLISTFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextListFormat *qtlf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTLISTFORMAT, new QoreQTextListFormat(*qtlf));
}

//int indent () const
static QoreNode *QTEXTLISTFORMAT_indent(QoreObject *self, QoreQTextListFormat *qtlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtlf->indent());
}

//bool isValid () const
static QoreNode *QTEXTLISTFORMAT_isValid(QoreObject *self, QoreQTextListFormat *qtlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qtlf->isValid());
}

//void setIndent ( int indentation )
static QoreNode *QTEXTLISTFORMAT_setIndent(QoreObject *self, QoreQTextListFormat *qtlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int indentation = p ? p->getAsInt() : 0;
   qtlf->setIndent(indentation);
   return 0;
}

//void setStyle ( Style style )
static QoreNode *QTEXTLISTFORMAT_setStyle(QoreObject *self, QoreQTextListFormat *qtlf, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QTextListFormat::Style style = (QTextListFormat::Style)(p ? p->getAsInt() : 0);
   qtlf->setStyle(style);
   return 0;
}

//Style style () const
static QoreNode *QTEXTLISTFORMAT_style(QoreObject *self, QoreQTextListFormat *qtlf, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtlf->style());
}

QoreClass *initQTextListFormatClass(QoreClass *qtextformat)
{
   QC_QTextListFormat = new QoreClass("QTextListFormat", QDOM_GUI);
   CID_QTEXTLISTFORMAT = QC_QTextListFormat->getID();

   QC_QTextListFormat->addBuiltinVirtualBaseClass(qtextformat);

   QC_QTextListFormat->setConstructor(QTEXTLISTFORMAT_constructor);
   QC_QTextListFormat->setCopy((q_copy_t)QTEXTLISTFORMAT_copy);

   QC_QTextListFormat->addMethod("indent",                      (q_method_t)QTEXTLISTFORMAT_indent);
   QC_QTextListFormat->addMethod("isValid",                     (q_method_t)QTEXTLISTFORMAT_isValid);
   QC_QTextListFormat->addMethod("setIndent",                   (q_method_t)QTEXTLISTFORMAT_setIndent);
   QC_QTextListFormat->addMethod("setStyle",                    (q_method_t)QTEXTLISTFORMAT_setStyle);
   QC_QTextListFormat->addMethod("style",                       (q_method_t)QTEXTLISTFORMAT_style);

   return QC_QTextListFormat;
}
