/*
 QC_QPaintDevice.cc
 
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

#include "QC_QPaintDevice.h"
#include "QC_QPaintEngine.h"

qore_classid_t CID_QPAINTDEVICE;
QoreClass *QC_QPaintDevice = 0;

static void QPAINTDEVICE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("ABSTRACT-CLASS-ERROR", "QPaintDevice is an abstract builtin class and cannot be directly instantiated or referenced by user code");
}

static void QPAINTDEVICE_copy(class QoreObject *self, class QoreObject *old, class QoreQPaintDevice *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTDEVICE-COPY-ERROR", "objects of this class cannot be copied");
}

//int depth () const
static AbstractQoreNode *QPAINTDEVICE_depth(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->depth());
}

//int height () const
static AbstractQoreNode *QPAINTDEVICE_height(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->height());
}

//int heightMM () const
static AbstractQoreNode *QPAINTDEVICE_heightMM(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->heightMM());
}

//int logicalDpiX () const
static AbstractQoreNode *QPAINTDEVICE_logicalDpiX(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->logicalDpiX());
}

//int logicalDpiY () const
static AbstractQoreNode *QPAINTDEVICE_logicalDpiY(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->logicalDpiY());
}

//int numColors () const
static AbstractQoreNode *QPAINTDEVICE_numColors(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->numColors());
}

static AbstractQoreNode *QPAINTDEVICE_paintEngine(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   QPaintEngine *pe = qpd->parent_paintEngine();
   if (!pe) {
      xsink->raiseException("QPAINTDEVICE-PAINTENGINE-ERROR", "This function must be overridden in a subclass; QPaintDevice::paintEngine() is a pure virtual function in the Qt Library");
      return 0;
   }
   return return_object(QC_QPaintEngine, new QoreQtQPaintEngine(pe));
}

//bool paintingActive () const
static AbstractQoreNode *QPAINTDEVICE_paintingActive(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qpd->getQPaintDevice()->paintingActive());
}

//int physicalDpiX () const
static AbstractQoreNode *QPAINTDEVICE_physicalDpiX(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->physicalDpiX());
}

//int physicalDpiY () const
static AbstractQoreNode *QPAINTDEVICE_physicalDpiY(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->physicalDpiY());
}

//int width () const
static AbstractQoreNode *QPAINTDEVICE_width(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->width());
}

//int widthMM () const
static AbstractQoreNode *QPAINTDEVICE_widthMM(QoreObject *self, QoreAbstractQPaintDeviceData *qpd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpd->getQPaintDevice()->widthMM());
}

class QoreClass *initQPaintDeviceClass()
{
   tracein("initQPaintDeviceClass()");
   
   QC_QPaintDevice = new QoreClass("QPaintDevice", QDOM_GUI);
   CID_QPAINTDEVICE = QC_QPaintDevice->getID();
   QC_QPaintDevice->setConstructor(QPAINTDEVICE_constructor);
   QC_QPaintDevice->setCopy((q_copy_t)QPAINTDEVICE_copy);

   QC_QPaintDevice->addMethod("depth",                       (q_method_t)QPAINTDEVICE_depth);
   QC_QPaintDevice->addMethod("height",                      (q_method_t)QPAINTDEVICE_height);
   QC_QPaintDevice->addMethod("heightMM",                    (q_method_t)QPAINTDEVICE_heightMM);
   QC_QPaintDevice->addMethod("logicalDpiX",                 (q_method_t)QPAINTDEVICE_logicalDpiX);
   QC_QPaintDevice->addMethod("logicalDpiY",                 (q_method_t)QPAINTDEVICE_logicalDpiY);
   QC_QPaintDevice->addMethod("numColors",                   (q_method_t)QPAINTDEVICE_numColors);
   QC_QPaintDevice->addMethod("paintEngine",                 (q_method_t)QPAINTDEVICE_paintEngine);
   QC_QPaintDevice->addMethod("paintingActive",              (q_method_t)QPAINTDEVICE_paintingActive);
   QC_QPaintDevice->addMethod("physicalDpiX",                (q_method_t)QPAINTDEVICE_physicalDpiX);
   QC_QPaintDevice->addMethod("physicalDpiY",                (q_method_t)QPAINTDEVICE_physicalDpiY);
   QC_QPaintDevice->addMethod("width",                       (q_method_t)QPAINTDEVICE_width);
   QC_QPaintDevice->addMethod("widthMM",                     (q_method_t)QPAINTDEVICE_widthMM);

   traceout("initQPaintDeviceClass()");
   return QC_QPaintDevice;
}

