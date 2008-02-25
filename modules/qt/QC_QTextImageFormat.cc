/*
 QC_QTextImageFormat.cc
 
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

#include "QC_QTextImageFormat.h"

#include "qore-qt.h"

int CID_QTEXTIMAGEFORMAT;
class QoreClass *QC_QTextImageFormat = 0;

//QTextImageFormat ()
static void QTEXTIMAGEFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTIMAGEFORMAT, new QoreQTextImageFormat());
   return;
}

static void QTEXTIMAGEFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextImageFormat *qtif, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTIMAGEFORMAT, new QoreQTextImageFormat(*qtif));
}

//qreal height () const
static AbstractQoreNode *QTEXTIMAGEFORMAT_height(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtif->height());
}

//bool isValid () const
static AbstractQoreNode *QTEXTIMAGEFORMAT_isValid(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtif->isValid());
}

//QString name () const
static AbstractQoreNode *QTEXTIMAGEFORMAT_name(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtif->name().toUtf8().data(), QCS_UTF8);
}

//void setHeight ( qreal height )
static AbstractQoreNode *QTEXTIMAGEFORMAT_setHeight(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal height = p ? p->getAsFloat() : 0.0;
   qtif->setHeight(height);
   return 0;
}

//void setName ( const QString & name )
static AbstractQoreNode *QTEXTIMAGEFORMAT_setName(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   qtif->setName(name);
   return 0;
}

//void setWidth ( qreal width )
static AbstractQoreNode *QTEXTIMAGEFORMAT_setWidth(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal width = p ? p->getAsFloat() : 0.0;
   qtif->setWidth(width);
   return 0;
}

//qreal width () const
static AbstractQoreNode *QTEXTIMAGEFORMAT_width(QoreObject *self, QoreQTextImageFormat *qtif, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtif->width());
}

QoreClass *initQTextImageFormatClass(QoreClass *qtextcharformat)
{
   QC_QTextImageFormat = new QoreClass("QTextImageFormat", QDOM_GUI);
   CID_QTEXTIMAGEFORMAT = QC_QTextImageFormat->getID();

   QC_QTextImageFormat->addBuiltinVirtualBaseClass(qtextcharformat);

   QC_QTextImageFormat->setConstructor(QTEXTIMAGEFORMAT_constructor);
   QC_QTextImageFormat->setCopy((q_copy_t)QTEXTIMAGEFORMAT_copy);

   QC_QTextImageFormat->addMethod("height",                      (q_method_t)QTEXTIMAGEFORMAT_height);
   QC_QTextImageFormat->addMethod("isValid",                     (q_method_t)QTEXTIMAGEFORMAT_isValid);
   QC_QTextImageFormat->addMethod("name",                        (q_method_t)QTEXTIMAGEFORMAT_name);
   QC_QTextImageFormat->addMethod("setHeight",                   (q_method_t)QTEXTIMAGEFORMAT_setHeight);
   QC_QTextImageFormat->addMethod("setName",                     (q_method_t)QTEXTIMAGEFORMAT_setName);
   QC_QTextImageFormat->addMethod("setWidth",                    (q_method_t)QTEXTIMAGEFORMAT_setWidth);
   QC_QTextImageFormat->addMethod("width",                       (q_method_t)QTEXTIMAGEFORMAT_width);

   return QC_QTextImageFormat;
}
