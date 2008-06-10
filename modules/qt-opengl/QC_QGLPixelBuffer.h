/*
 QC_QGLPixelBuffer.h
 
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

#ifndef _QORE_QT_QC_QGLPIXELBUFFER_H

#define _QORE_QT_QC_QGLPIXELBUFFER_H

#include <QGLPixelBuffer>
#include "QoreAbstractQPaintDevice.h"

DLLLOCAL extern qore_classid_t CID_QGLPIXELBUFFER;
DLLLOCAL extern QoreClass *QC_QGLPixelBuffer;

DLLLOCAL QoreClass *initQGLPixelBufferClass(QoreClass *qpaintdevice);

class QoreQGLPixelBuffer : public AbstractPrivateData, public QoreAbstractQPaintDevice, public QGLPixelBuffer
{
   public:
      DLLLOCAL QoreQGLPixelBuffer(const QSize& size, const QGLFormat& format = QGLFormat::defaultFormat(), QGLWidget* shareWidget = 0) : QGLPixelBuffer(size, format, shareWidget)
      {
      }
      DLLLOCAL QoreQGLPixelBuffer(int width, int height, const QGLFormat& format = QGLFormat::defaultFormat(), QGLWidget* shareWidget = 0) : QGLPixelBuffer(width, height, format, shareWidget)
      {
      }
      
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
	 return const_cast<QPaintDevice *>(static_cast<const QPaintDevice *>(this));
      }

      DLLLOCAL virtual QPaintEngine *parent_paintEngine() const
      {
         return QGLPixelBuffer::paintEngine();
      }
};

#endif // _QORE_QT_QC_QGLPIXELBUFFER_H
