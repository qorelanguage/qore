/*
 QC_QSvgGenerator.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#include "qore-qt-gui.h"

#include "QC_QSvgGenerator.h"
#include "QC_QIODevice.h"
#include "QC_QSize.h"

qore_classid_t CID_QSVGGENERATOR;
QoreClass *QC_QSvgGenerator = 0;

//QSvgGenerator ()
static void QSVGGENERATOR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSVGGENERATOR, new QoreQSvgGenerator());
   return;
}

static void QSVGGENERATOR_copy(QoreObject *self, QoreObject *old, QoreQSvgGenerator *qsg, ExceptionSink *xsink)
{
   xsink->raiseException("QSVGGENERATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//QString fileName () const
static AbstractQoreNode *QSVGGENERATOR_fileName(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qsg->fileName().toUtf8().data(), QCS_UTF8);
}

//QIODevice * outputDevice () const
static AbstractQoreNode *QSVGGENERATOR_outputDevice(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   QIODevice *qt_qobj = qsg->outputDevice();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QIODevice, getProgram());
   QoreQtQIODevice *t_qobj = new QoreQtQIODevice(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QIODEVICE, t_qobj);
   return rv_obj;
}

//int resolution () const
static AbstractQoreNode *QSVGGENERATOR_resolution(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsg->resolution());
}

//void setFileName ( const QString & fileName )
static AbstractQoreNode *QSVGGENERATOR_setFileName(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   qsg->setFileName(fileName);
   return 0;
}

//void setOutputDevice ( QIODevice * outputDevice )
static AbstractQoreNode *QSVGGENERATOR_setOutputDevice(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQtQIODevice *outputDevice = (p && p->getType() == NT_OBJECT) ? (QoreQtQIODevice *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QIODEVICE, xsink) : 0;
   if (!outputDevice) {
      if (!xsink->isException())
         xsink->raiseException("QSVGGENERATOR-SETOUTPUTDEVICE-PARAM-ERROR", "expecting a QIODevice object as first argument to QSvgGenerator::setOutputDevice()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> outputDeviceHolder(static_cast<AbstractPrivateData *>(outputDevice), xsink);
   qsg->setOutputDevice(static_cast<QIODevice *>(outputDevice->getQIODevice()));
   return 0;
}

//void setResolution ( int resolution )
static AbstractQoreNode *QSVGGENERATOR_setResolution(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int resolution = p ? p->getAsInt() : 0;
   qsg->setResolution(resolution);
   return 0;
}

//void setSize ( const QSize & size )
static AbstractQoreNode *QSVGGENERATOR_setSize(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->getType() == NT_OBJECT) ? (QoreQSize *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QSVGGENERATOR-SETSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QSvgGenerator::setSize()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qsg->setSize(*(static_cast<QSize *>(size)));
   return 0;
}

//QSize size () const
static AbstractQoreNode *QSVGGENERATOR_size(QoreObject *self, QoreQSvgGenerator *qsg, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qsg->size()));
}

QoreClass *initQSvgGeneratorClass()
{
   QC_QSvgGenerator = new QoreClass("QSvgGenerator", QDOM_GUI);
   CID_QSVGGENERATOR = QC_QSvgGenerator->getID();

   QC_QSvgGenerator->setConstructor(QSVGGENERATOR_constructor);
   QC_QSvgGenerator->setCopy((q_copy_t)QSVGGENERATOR_copy);

   QC_QSvgGenerator->addMethod("fileName",                    (q_method_t)QSVGGENERATOR_fileName);
   QC_QSvgGenerator->addMethod("outputDevice",                (q_method_t)QSVGGENERATOR_outputDevice);
   QC_QSvgGenerator->addMethod("resolution",                  (q_method_t)QSVGGENERATOR_resolution);
   QC_QSvgGenerator->addMethod("setFileName",                 (q_method_t)QSVGGENERATOR_setFileName);
   QC_QSvgGenerator->addMethod("setOutputDevice",             (q_method_t)QSVGGENERATOR_setOutputDevice);
   QC_QSvgGenerator->addMethod("setResolution",               (q_method_t)QSVGGENERATOR_setResolution);
   QC_QSvgGenerator->addMethod("setSize",                     (q_method_t)QSVGGENERATOR_setSize);
   QC_QSvgGenerator->addMethod("size",                        (q_method_t)QSVGGENERATOR_size);

   return QC_QSvgGenerator;
}
