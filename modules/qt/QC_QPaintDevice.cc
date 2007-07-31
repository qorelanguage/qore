/*
 QC_QPaintDevice.cc
 
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
#include "QC_QPaintDevice.h"

DLLLOCAL int CID_QPAINTDEVICE;

static void QPAINTDEVICE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("ABSTRACT-CLASS-ERROR", "QPaintDevice is an abstract builtin class and cannot be directly instantiated or referenced by user code");
}

static void QPAINTDEVICE_copy(class Object *self, class Object *old, class QoreQPaintDevice *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTDEVICE-COPY-ERROR", "objects of this class cannot be copied");
}

//int depth () const
static QoreNode *QPAINTDEVICE_depth(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->depth());
}

//int height () const
static QoreNode *QPAINTDEVICE_height(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->height());
}

//int heightMM () const
static QoreNode *QPAINTDEVICE_heightMM(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->heightMM());
}

//int logicalDpiX () const
static QoreNode *QPAINTDEVICE_logicalDpiX(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->logicalDpiX());
}

//int logicalDpiY () const
static QoreNode *QPAINTDEVICE_logicalDpiY(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->logicalDpiY());
}

//int numColors () const
static QoreNode *QPAINTDEVICE_numColors(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->numColors());
}

//virtual QPaintEngine * paintEngine () const = 0
//static QoreNode *QPAINTDEVICE_paintEngine(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qpd->getQPaintDevice()->paintEngine();
//}

//bool paintingActive () const
static QoreNode *QPAINTDEVICE_paintingActive(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpd->getQPaintDevice()->paintingActive());
}

//int physicalDpiX () const
static QoreNode *QPAINTDEVICE_physicalDpiX(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->physicalDpiX());
}

//int physicalDpiY () const
static QoreNode *QPAINTDEVICE_physicalDpiY(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->physicalDpiY());
}

//int width () const
static QoreNode *QPAINTDEVICE_width(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->width());
}

//int widthMM () const
static QoreNode *QPAINTDEVICE_widthMM(Object *self, QoreAbstractQObject *qpd, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpd->getQPaintDevice()->widthMM());
}

class QoreClass *initQPaintDeviceClass()
{
   tracein("initQPaintDeviceClass()");
   
   class QoreClass *QC_QPaintDevice = new QoreClass("QPaintDevice", QDOM_GUI);
   CID_QPAINTDEVICE = QC_QPaintDevice->getID();
   QC_QPaintDevice->setConstructor(QPAINTDEVICE_constructor);
   QC_QPaintDevice->setCopy((q_copy_t)QPAINTDEVICE_copy);

   QC_QPaintDevice->addMethod("depth",                       (q_method_t)QPAINTDEVICE_depth);
   QC_QPaintDevice->addMethod("height",                      (q_method_t)QPAINTDEVICE_height);
   QC_QPaintDevice->addMethod("heightMM",                    (q_method_t)QPAINTDEVICE_heightMM);
   QC_QPaintDevice->addMethod("logicalDpiX",                 (q_method_t)QPAINTDEVICE_logicalDpiX);
   QC_QPaintDevice->addMethod("logicalDpiY",                 (q_method_t)QPAINTDEVICE_logicalDpiY);
   QC_QPaintDevice->addMethod("numColors",                   (q_method_t)QPAINTDEVICE_numColors);
   //QC_QPaintDevice->addMethod("paintEngine",                 (q_method_t)QPAINTDEVICE_paintEngine);
   QC_QPaintDevice->addMethod("paintingActive",              (q_method_t)QPAINTDEVICE_paintingActive);
   QC_QPaintDevice->addMethod("physicalDpiX",                (q_method_t)QPAINTDEVICE_physicalDpiX);
   QC_QPaintDevice->addMethod("physicalDpiY",                (q_method_t)QPAINTDEVICE_physicalDpiY);
   QC_QPaintDevice->addMethod("width",                       (q_method_t)QPAINTDEVICE_width);
   QC_QPaintDevice->addMethod("widthMM",                     (q_method_t)QPAINTDEVICE_widthMM);

   traceout("initQPaintDeviceClass()");
   return QC_QPaintDevice;
}

