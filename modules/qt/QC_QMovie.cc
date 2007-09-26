/*
 QC_QMovie.cc
 
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

#include "QC_QMovie.h"

int CID_QMOVIE;
class QoreClass *QC_QMovie = 0;

//QMovie ( QObject * parent = 0 )
//QMovie ( QIODevice * device, const QByteArray & format = QByteArray(), QObject * parent = 0 )
//QMovie ( const QString & fileName, const QByteArray & format = QByteArray(), QObject * parent = 0 )
static void QMOVIE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QMOVIE, new QoreQMovie(self));
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreAbstractQObject *parent = (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QMOVIE, new QoreQMovie(self, parent ? parent->getQObject() : 0));
      return;
   }
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return;
   p = get_param(params, 1);
   QByteArray format;
   if (get_qbytearray(p, format, xsink, true))
      format = QByteArray();
   p = get_param(params, 2);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QMOVIE, new QoreQMovie(self, fileName, format, parent ? parent->getQObject() : 0));
   return;
}

static void QMOVIE_copy(class Object *self, class Object *old, class QoreQMovie *qm, ExceptionSink *xsink)
{
   xsink->raiseException("QMOVIE-COPY-ERROR", "objects of this class cannot be copied");
}

//QColor backgroundColor () const
static QoreNode *QMOVIE_backgroundColor(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qc = new Object(QC_QColor, getProgram());
   QoreQColor *q_qc = new QoreQColor(qm->qobj->backgroundColor());
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//CacheMode cacheMode () const
static QoreNode *QMOVIE_cacheMode(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->cacheMode());
}

//int currentFrameNumber () const
static QoreNode *QMOVIE_currentFrameNumber(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->currentFrameNumber());
}

//QImage currentImage () const
static QoreNode *QMOVIE_currentImage(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(QC_QImage, getProgram());
   QoreQImage *q_qi = new QoreQImage(qm->qobj->currentImage());
   o_qi->setPrivate(CID_QIMAGE, q_qi);
   return new QoreNode(o_qi);
}

//QPixmap currentPixmap () const
static QoreNode *QMOVIE_currentPixmap(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qm->qobj->currentPixmap());
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

/*
//QIODevice * device () const
static QoreNode *QMOVIE_device(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   ??? return new QoreNode((int64)qm->qobj->device());
}
*/

//QString fileName () const
static QoreNode *QMOVIE_fileName(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qm->qobj->fileName().toUtf8().data(), QCS_UTF8));
}

//QByteArray format () const
static QoreNode *QMOVIE_format(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qba = new Object(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qm->qobj->format());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return new QoreNode(o_qba);
}

//int frameCount () const
static QoreNode *QMOVIE_frameCount(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->frameCount());
}

//QRect frameRect () const
static QoreNode *QMOVIE_frameRect(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qm->qobj->frameRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//bool isValid () const
static QoreNode *QMOVIE_isValid(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->isValid());
}

//bool jumpToFrame ( int frameNumber )
static QoreNode *QMOVIE_jumpToFrame(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int frameNumber = p ? p->getAsInt() : 0;
   return new QoreNode(qm->qobj->jumpToFrame(frameNumber));
}

//int loopCount () const
static QoreNode *QMOVIE_loopCount(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->loopCount());
}

//int nextFrameDelay () const
static QoreNode *QMOVIE_nextFrameDelay(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->nextFrameDelay());
}

//QSize scaledSize ()
static QoreNode *QMOVIE_scaledSize(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qm->qobj->scaledSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//void setBackgroundColor ( const QColor & color )
static QoreNode *QMOVIE_setBackgroundColor(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQColor *color = (p && p->type == NT_OBJECT) ? (QoreQColor *)p->val.object->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!color) {
      if (!xsink->isException())
         xsink->raiseException("QMOVIE-SETBACKGROUNDCOLOR-PARAM-ERROR", "expecting a QColor object as first argument to QMovie::setBackgroundColor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> colorHolder(static_cast<AbstractPrivateData *>(color), xsink);
   qm->qobj->setBackgroundColor(*(static_cast<QColor *>(color)));
   return 0;
}

//void setCacheMode ( CacheMode mode )
static QoreNode *QMOVIE_setCacheMode(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMovie::CacheMode mode = (QMovie::CacheMode)(p ? p->getAsInt() : 0);
   qm->qobj->setCacheMode(mode);
   return 0;
}

/*
//void setDevice ( QIODevice * device )
static QoreNode *QMOVIE_setDevice(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? QIODevice* device = p;
   qm->qobj->setDevice(device);
   return 0;
}
*/

//void setFileName ( const QString & fileName )
static QoreNode *QMOVIE_setFileName(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   qm->qobj->setFileName(fileName);

   return 0;
}

//void setFormat ( const QByteArray & format )
static QoreNode *QMOVIE_setFormat(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QByteArray format;
   if (get_qbytearray(p, format, xsink))
      return 0;
   qm->qobj->setFormat(format);
   return 0;
}

//void setScaledSize ( const QSize & size )
static QoreNode *QMOVIE_setScaledSize(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QMOVIE-SETSCALEDSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QMovie::setScaledSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qm->qobj->setScaledSize(*(static_cast<QSize *>(size)));
   return 0;
}

//int speed () const
static QoreNode *QMOVIE_speed(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->speed());
}

//MovieState state () const
static QoreNode *QMOVIE_state(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qm->qobj->state());
}

//bool jumpToNextFrame ()
static QoreNode *QMOVIE_jumpToNextFrame(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->jumpToNextFrame());
}

//void setPaused ( bool paused )
static QoreNode *QMOVIE_setPaused(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool paused = p ? p->getAsBool() : false;
   qm->qobj->setPaused(paused);
   return 0;
}

//void setSpeed ( int percentSpeed )
static QoreNode *QMOVIE_setSpeed(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int percentSpeed = p ? p->getAsInt() : 0;
   qm->qobj->setSpeed(percentSpeed);
   return 0;
}

//void start ()
static QoreNode *QMOVIE_start(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   qm->qobj->start();
   return 0;
}

//void stop ()
static QoreNode *QMOVIE_stop(Object *self, QoreQMovie *qm, QoreNode *params, ExceptionSink *xsink)
{
   qm->qobj->stop();
   return 0;
}

QoreClass *initQMovieClass(QoreClass *qobject)
{
   QC_QMovie = new QoreClass("QMovie", QDOM_GUI);
   CID_QMOVIE = QC_QMovie->getID();

   QC_QMovie->addBuiltinVirtualBaseClass(qobject);

   QC_QMovie->setConstructor(QMOVIE_constructor);
   QC_QMovie->setCopy((q_copy_t)QMOVIE_copy);

   QC_QMovie->addMethod("backgroundColor",             (q_method_t)QMOVIE_backgroundColor);
   QC_QMovie->addMethod("cacheMode",                   (q_method_t)QMOVIE_cacheMode);
   QC_QMovie->addMethod("currentFrameNumber",          (q_method_t)QMOVIE_currentFrameNumber);
   QC_QMovie->addMethod("currentImage",                (q_method_t)QMOVIE_currentImage);
   QC_QMovie->addMethod("currentPixmap",               (q_method_t)QMOVIE_currentPixmap);
   //QC_QMovie->addMethod("device",                      (q_method_t)QMOVIE_device);
   QC_QMovie->addMethod("fileName",                    (q_method_t)QMOVIE_fileName);
   QC_QMovie->addMethod("format",                      (q_method_t)QMOVIE_format);
   QC_QMovie->addMethod("frameCount",                  (q_method_t)QMOVIE_frameCount);
   QC_QMovie->addMethod("frameRect",                   (q_method_t)QMOVIE_frameRect);
   QC_QMovie->addMethod("isValid",                     (q_method_t)QMOVIE_isValid);
   QC_QMovie->addMethod("jumpToFrame",                 (q_method_t)QMOVIE_jumpToFrame);
   QC_QMovie->addMethod("loopCount",                   (q_method_t)QMOVIE_loopCount);
   QC_QMovie->addMethod("nextFrameDelay",              (q_method_t)QMOVIE_nextFrameDelay);
   QC_QMovie->addMethod("scaledSize",                  (q_method_t)QMOVIE_scaledSize);
   QC_QMovie->addMethod("setBackgroundColor",          (q_method_t)QMOVIE_setBackgroundColor);
   QC_QMovie->addMethod("setCacheMode",                (q_method_t)QMOVIE_setCacheMode);
   //QC_QMovie->addMethod("setDevice",                   (q_method_t)QMOVIE_setDevice);
   QC_QMovie->addMethod("setFileName",                 (q_method_t)QMOVIE_setFileName);
   QC_QMovie->addMethod("setFormat",                   (q_method_t)QMOVIE_setFormat);
   QC_QMovie->addMethod("setScaledSize",               (q_method_t)QMOVIE_setScaledSize);
   QC_QMovie->addMethod("speed",                       (q_method_t)QMOVIE_speed);
   QC_QMovie->addMethod("state",                       (q_method_t)QMOVIE_state);
   QC_QMovie->addMethod("jumpToNextFrame",             (q_method_t)QMOVIE_jumpToNextFrame);
   QC_QMovie->addMethod("setPaused",                   (q_method_t)QMOVIE_setPaused);
   QC_QMovie->addMethod("setSpeed",                    (q_method_t)QMOVIE_setSpeed);
   QC_QMovie->addMethod("start",                       (q_method_t)QMOVIE_start);
   QC_QMovie->addMethod("stop",                        (q_method_t)QMOVIE_stop);

   return QC_QMovie;
}

static QoreNode *f_QMovie_supportedFormats(QoreNode *params, ExceptionSink *xsink)
{
   List *ql = new List();

   QList<QByteArray> l = QMovie::supportedFormats();
   for (QList<QByteArray>::iterator i = l.begin(), e=l.end(); i != e; ++i)
      ql->push(new QoreNode(new QoreString((*i).data())));

   return new QoreNode(ql);
}

void initQMovieStaticFunctions()
{
   builtinFunctions.add("QMovie_supportedFormats", f_QMovie_supportedFormats);
}
