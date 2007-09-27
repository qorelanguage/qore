/*
 QC_QImageWriter.cc
 
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

#include "QC_QImageWriter.h"

int CID_QIMAGEWRITER;
class QoreClass *QC_QImageWriter = 0;

//QImageWriter ()
//QImageWriter ( QIODevice * device, const QByteArray & format )
//QImageWriter ( const QString & fileName, const QByteArray & format = QByteArray() )
static void QIMAGEWRITER_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QIMAGEWRITER, new QoreQImageWriter());
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreAbstractQIODevice *device = (QoreAbstractQIODevice *)p->val.object->getReferencedPrivateData(CID_QIODEVICE, xsink);
      if (!device) {
         if (!xsink->isException())
            xsink->raiseException("QIMAGEWRITER-CONSTRUCTOR-PARAM-ERROR", "QImageWriter::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> deviceHolder(static_cast<AbstractPrivateData *>(device), xsink);
      p = get_param(params, 1);
      QByteArray format;
      if (get_qbytearray(p, format, xsink))
         return;
      self->setPrivate(CID_QIMAGEWRITER, new QoreQImageWriter(static_cast<QIODevice *>(device->getQIODevice()), format));
      return;
   }
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return;
   p = get_param(params, 1);
   QByteArray format;
   if (get_qbytearray(p, format, xsink, true))
      format = QByteArray();
   self->setPrivate(CID_QIMAGEWRITER, new QoreQImageWriter(fileName, format));
   return;
}

static void QIMAGEWRITER_copy(class Object *self, class Object *old, class QoreQImageWriter *qiw, ExceptionSink *xsink)
{
   xsink->raiseException("QIMAGEWRITER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool canWrite () const
static QoreNode *QIMAGEWRITER_canWrite(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qiw->canWrite());
}

//int compression () const
static QoreNode *QIMAGEWRITER_compression(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiw->compression());
}

//QIODevice * device () const
static QoreNode *QIMAGEWRITER_device(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QIODevice *qt_qobj = qiw->device();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QIODevice, getProgram());
      QoreQtQIODevice *t_qobj = new QoreQtQIODevice(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QIODEVICE, t_qobj);
   }
   return new QoreNode(rv_obj);
}

/*
//ImageWriterError error () const
static QoreNode *QIMAGEWRITER_error(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   ??? return new QoreNode((int64)qiw->error());
}
*/

//QString errorString () const
static QoreNode *QIMAGEWRITER_errorString(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qiw->errorString().toUtf8().data(), QCS_UTF8));
}

//QString fileName () const
static QoreNode *QIMAGEWRITER_fileName(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qiw->fileName().toUtf8().data(), QCS_UTF8));
}

//QByteArray format () const
static QoreNode *QIMAGEWRITER_format(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qba = new Object(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qiw->format());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return new QoreNode(o_qba);
}

//float gamma () const
static QoreNode *QIMAGEWRITER_gamma(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qiw->gamma());
}

//int quality () const
static QoreNode *QIMAGEWRITER_quality(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qiw->quality());
}

//void setCompression ( int compression )
static QoreNode *QIMAGEWRITER_setCompression(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int compression = p ? p->getAsInt() : 0;
   qiw->setCompression(compression);
   return 0;
}

//void setDevice ( QIODevice * device )
static QoreNode *QIMAGEWRITER_setDevice(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQIODevice *device = (p && p->type == NT_OBJECT) ? (QoreAbstractQIODevice *)p->val.object->getReferencedPrivateData(CID_QIODEVICE, xsink) : 0;
   if (!device) {
      if (!xsink->isException())
         xsink->raiseException("QIMAGEWRITER-SETDEVICE-PARAM-ERROR", "expecting a QIODevice object as first argument to QImageWriter::setDevice()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> deviceHolder(static_cast<AbstractPrivateData *>(device), xsink);
   qiw->setDevice(static_cast<QIODevice *>(device->getQIODevice()));
   return 0;
}

//void setFileName ( const QString & fileName )
static QoreNode *QIMAGEWRITER_setFileName(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   qiw->setFileName(fileName);
   return 0;
}

//void setFormat ( const QByteArray & format )
static QoreNode *QIMAGEWRITER_setFormat(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QByteArray format;
   if (get_qbytearray(p, format, xsink))
      return 0;
   qiw->setFormat(format);
   return 0;
}

//void setGamma ( float gamma )
static QoreNode *QIMAGEWRITER_setGamma(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float gamma = p ? p->getAsFloat() : 0.0;
   qiw->setGamma(gamma);
   return 0;
}

//void setQuality ( int quality )
static QoreNode *QIMAGEWRITER_setQuality(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int quality = p ? p->getAsInt() : 0;
   qiw->setQuality(quality);
   return 0;
}

//void setText ( const QString & key, const QString & text )
static QoreNode *QIMAGEWRITER_setText(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qiw->setText(key, text);
   return 0;
}

//bool supportsOption ( QImageIOHandler::ImageOption option ) const
static QoreNode *QIMAGEWRITER_supportsOption(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QImageIOHandler::ImageOption option = (QImageIOHandler::ImageOption)(p ? p->getAsInt() : 0);
   return new QoreNode(qiw->supportsOption(option));
}

//bool write ( const QImage & image )
static QoreNode *QIMAGEWRITER_write(Object *self, QoreQImageWriter *qiw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QIMAGEWRITER-WRITE-PARAM-ERROR", "expecting a QImage object as first argument to QImageWriter::write()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
   return new QoreNode(qiw->write(*(static_cast<QImage *>(image))));
}

QoreClass *initQImageWriterClass()
{
   QC_QImageWriter = new QoreClass("QImageWriter", QDOM_GUI);
   CID_QIMAGEWRITER = QC_QImageWriter->getID();

   QC_QImageWriter->setConstructor(QIMAGEWRITER_constructor);
   QC_QImageWriter->setCopy((q_copy_t)QIMAGEWRITER_copy);

   QC_QImageWriter->addMethod("canWrite",                    (q_method_t)QIMAGEWRITER_canWrite);
   QC_QImageWriter->addMethod("compression",                 (q_method_t)QIMAGEWRITER_compression);
   QC_QImageWriter->addMethod("device",                      (q_method_t)QIMAGEWRITER_device);
   //QC_QImageWriter->addMethod("error",                       (q_method_t)QIMAGEWRITER_error);
   QC_QImageWriter->addMethod("errorString",                 (q_method_t)QIMAGEWRITER_errorString);
   QC_QImageWriter->addMethod("fileName",                    (q_method_t)QIMAGEWRITER_fileName);
   QC_QImageWriter->addMethod("format",                      (q_method_t)QIMAGEWRITER_format);
   QC_QImageWriter->addMethod("gamma",                       (q_method_t)QIMAGEWRITER_gamma);
   QC_QImageWriter->addMethod("quality",                     (q_method_t)QIMAGEWRITER_quality);
   QC_QImageWriter->addMethod("setCompression",              (q_method_t)QIMAGEWRITER_setCompression);
   QC_QImageWriter->addMethod("setDevice",                   (q_method_t)QIMAGEWRITER_setDevice);
   QC_QImageWriter->addMethod("setFileName",                 (q_method_t)QIMAGEWRITER_setFileName);
   QC_QImageWriter->addMethod("setFormat",                   (q_method_t)QIMAGEWRITER_setFormat);
   QC_QImageWriter->addMethod("setGamma",                    (q_method_t)QIMAGEWRITER_setGamma);
   QC_QImageWriter->addMethod("setQuality",                  (q_method_t)QIMAGEWRITER_setQuality);
   QC_QImageWriter->addMethod("setText",                     (q_method_t)QIMAGEWRITER_setText);
   QC_QImageWriter->addMethod("supportsOption",              (q_method_t)QIMAGEWRITER_supportsOption);
   QC_QImageWriter->addMethod("write",                       (q_method_t)QIMAGEWRITER_write);

   return QC_QImageWriter;
}

//QList<QByteArray> supportedImageFormats ()
static QoreNode *f_QImageWriter_supportedImageFormats(QoreNode *params, ExceptionSink *xsink)
{
   List *ql = new List();

   QList<QByteArray> l = QImageWriter::supportedImageFormats();
   for (QList<QByteArray>::iterator i = l.begin(), e=l.end(); i != e; ++i)
      ql->push(new QoreNode(new QoreString((*i).data())));

   return new QoreNode(ql);
}

void initQImageWriterStaticFunctions()
{
   builtinFunctions.add("QImageWriter_supportedImageFormats", f_QImageWriter_supportedImageFormats);
}
