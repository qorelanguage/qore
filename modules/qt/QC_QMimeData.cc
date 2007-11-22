/*
 QC_QMimeData.cc
 
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

#include "QC_QMimeData.h"

int CID_QMIMEDATA;
class QoreClass *QC_QMimeData = 0;

//QMimeData ()
static void QMIMEDATA_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QMIMEDATA, new QoreQMimeData(self));
   return;
}

static void QMIMEDATA_copy(class Object *self, class Object *old, class QoreQMimeData *qmd, ExceptionSink *xsink)
{
   xsink->raiseException("QMIMEDATA-COPY-ERROR", "objects of this class cannot be copied");
}

//void clear ()
static QoreNode *QMIMEDATA_clear(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   qmd->qobj->clear();
   return 0;
}

//QVariant colorData () const
static QoreNode *QMIMEDATA_colorData(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return return_qvariant(qmd->qobj->colorData());
}

//QByteArray data ( const QString & mimeType ) const
static QoreNode *QMIMEDATA_data(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString mimeType;
   if (get_qstring(p, mimeType, xsink))
      return 0;

   Object *o_qba = new Object(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qmd->qobj->data(mimeType));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return new QoreNode(o_qba);
}

//virtual QStringList formats () const
static QoreNode *QMIMEDATA_formats(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = qmd->qobj->formats();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}

//bool hasColor () const
static QoreNode *QMIMEDATA_hasColor(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmd->qobj->hasColor());
}

//virtual bool hasFormat ( const QString & mimeType ) const
static QoreNode *QMIMEDATA_hasFormat(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString mimeType;
   if (get_qstring(p, mimeType, xsink))
      return 0;

   return new QoreNode(qmd->qobj->hasFormat(mimeType));
}

//bool hasHtml () const
static QoreNode *QMIMEDATA_hasHtml(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmd->qobj->hasHtml());
}

//bool hasImage () const
static QoreNode *QMIMEDATA_hasImage(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmd->qobj->hasImage());
}

//bool hasText () const
static QoreNode *QMIMEDATA_hasText(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmd->qobj->hasText());
}

//bool hasUrls () const
static QoreNode *QMIMEDATA_hasUrls(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmd->qobj->hasUrls());
}

//QString html () const
static QoreNode *QMIMEDATA_html(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmd->qobj->html().toUtf8().data(), QCS_UTF8));
}

//QVariant imageData () const
static QoreNode *QMIMEDATA_imageData(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return return_qvariant(qmd->qobj->imageData());
}

//void setColorData ( const QVariant & color )
static QoreNode *QMIMEDATA_setColorData(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QVariant color;
   if (get_qvariant(p, color, xsink))
      return 0;
   qmd->qobj->setColorData(color);
   return 0;
}

//void setData ( const QString & mimeType, const QByteArray & data )
static QoreNode *QMIMEDATA_setData(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString mimeType;
   if (get_qstring(p, mimeType, xsink))
      return 0;

   p = get_param(params, 1);
   QByteArray data;
   if (get_qbytearray(p, data, xsink))
      return 0;
   qmd->qobj->setData(mimeType, data);
   return 0;
}

//void setHtml ( const QString & html )
static QoreNode *QMIMEDATA_setHtml(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString html;
   if (get_qstring(p, html, xsink))
      return 0;

   qmd->qobj->setHtml(html);
   return 0;
}

//void setImageData ( const QVariant & image )
static QoreNode *QMIMEDATA_setImageData(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QVariant image;
   if (get_qvariant(p, image, xsink))
      return 0;
   qmd->qobj->setImageData(image);
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QMIMEDATA_setText(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   qmd->qobj->setText(text);
   return 0;
}

////void setUrls ( const QList<QUrl> & urls )
//static QoreNode *QMIMEDATA_setUrls(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QList<QUrl> urls = p;
//   qmd->qobj->setUrls(urls);
//   return 0;
//}

//QString text () const
static QoreNode *QMIMEDATA_text(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qmd->qobj->text().toUtf8().data(), QCS_UTF8));
}

////QList<QUrl> urls () const
//static QoreNode *QMIMEDATA_urls(Object *self, QoreQMimeData *qmd, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qmd->qobj->urls());
//}

QoreClass *initQMimeDataClass(QoreClass *qobject)
{
   QC_QMimeData = new QoreClass("QMimeData", QDOM_GUI);
   CID_QMIMEDATA = QC_QMimeData->getID();

   QC_QMimeData->addBuiltinVirtualBaseClass(qobject);

   QC_QMimeData->setConstructor(QMIMEDATA_constructor);
   QC_QMimeData->setCopy((q_copy_t)QMIMEDATA_copy);

   QC_QMimeData->addMethod("clear",                       (q_method_t)QMIMEDATA_clear);
   QC_QMimeData->addMethod("colorData",                   (q_method_t)QMIMEDATA_colorData);
   QC_QMimeData->addMethod("data",                        (q_method_t)QMIMEDATA_data);
   QC_QMimeData->addMethod("formats",                     (q_method_t)QMIMEDATA_formats);
   QC_QMimeData->addMethod("hasColor",                    (q_method_t)QMIMEDATA_hasColor);
   QC_QMimeData->addMethod("hasFormat",                   (q_method_t)QMIMEDATA_hasFormat);
   QC_QMimeData->addMethod("hasHtml",                     (q_method_t)QMIMEDATA_hasHtml);
   QC_QMimeData->addMethod("hasImage",                    (q_method_t)QMIMEDATA_hasImage);
   QC_QMimeData->addMethod("hasText",                     (q_method_t)QMIMEDATA_hasText);
   QC_QMimeData->addMethod("hasUrls",                     (q_method_t)QMIMEDATA_hasUrls);
   QC_QMimeData->addMethod("html",                        (q_method_t)QMIMEDATA_html);
   QC_QMimeData->addMethod("imageData",                   (q_method_t)QMIMEDATA_imageData);
   QC_QMimeData->addMethod("setColorData",                (q_method_t)QMIMEDATA_setColorData);
   QC_QMimeData->addMethod("setData",                     (q_method_t)QMIMEDATA_setData);
   QC_QMimeData->addMethod("setHtml",                     (q_method_t)QMIMEDATA_setHtml);
   QC_QMimeData->addMethod("setImageData",                (q_method_t)QMIMEDATA_setImageData);
   QC_QMimeData->addMethod("setText",                     (q_method_t)QMIMEDATA_setText);
   //QC_QMimeData->addMethod("setUrls",                     (q_method_t)QMIMEDATA_setUrls);
   QC_QMimeData->addMethod("text",                        (q_method_t)QMIMEDATA_text);
   //QC_QMimeData->addMethod("urls",                        (q_method_t)QMIMEDATA_urls);

   return QC_QMimeData;
}
