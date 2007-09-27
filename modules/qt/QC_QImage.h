/*
 QC_QImage.h
 
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

#ifndef _QORE_QC_QIMAGE_H

#define _QORE_QC_QIMAGE_H

#include "QoreAbstractQPaintDevice.h"

#include <QImage>

DLLLOCAL extern int CID_QIMAGE;
DLLLOCAL extern QoreClass *QC_QImage;

DLLLOCAL class QoreClass *initQImageClass(class QoreClass *qpaintdevice);

class QoreQImage : public AbstractPrivateData, public QoreAbstractQPaintDevice, public QImage
{
   public:

      DLLLOCAL QoreQImage(const QSize & size, Format format) : QImage(size, format)
      {
      }
      DLLLOCAL QoreQImage(const uchar *data, int w, int h, QImage::Format format) : QImage(data, w, h, format)
      {
      }
      DLLLOCAL QoreQImage(const uchar *data, int w, int h, int bytesPerLine, QImage::Format format) : QImage(data, w, h, bytesPerLine, format)
      {
      }
      DLLLOCAL QoreQImage(int w, int h, QImage::Format format) : QImage(w, h, format)
      {
      }
      DLLLOCAL QoreQImage(const char *filename, const char *format) : QImage(filename, format)
      {
      }
      DLLLOCAL QoreQImage(const QImage &image) : QImage(image)
      {
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(const_cast<QoreQImage *>(this));
      }
};

#endif
