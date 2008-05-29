/*
 QC_QBitmap.h
 
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

#ifndef _QORE_QC_QBITMAP_H

#define _QORE_QC_QBITMAP_H

#include "QoreAbstractQPaintDevice.h"

#include <QBitmap>

DLLEXPORT extern qore_classid_t CID_QBITMAP;
DLLEXPORT extern QoreClass *QC_QBitmap;

DLLEXPORT class QoreClass *initQBitmapClass(class QoreClass *qpaintdevice);

class QoreQBitmap : public AbstractPrivateData, public QoreAbstractQPaintDevice, public QBitmap
{
   public:
      DLLLOCAL QoreQBitmap(int w, int h) : QBitmap(w, h)
      {
      }
      DLLLOCAL QoreQBitmap(const char *filename, const char *format = 0) : QBitmap(filename, format)
      {
      }
      DLLLOCAL QoreQBitmap(const QPixmap &pix) : QBitmap(pix)
      {
      }
      DLLLOCAL QoreQBitmap(const QBitmap &bitmap) : QBitmap(bitmap)
      {
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(const_cast<QoreQBitmap *>(this));
      }
      DLLLOCAL virtual QPixmap *getQPixmap() const
      {
         return static_cast<QPixmap *>(const_cast<QoreQBitmap *>(this));
      }

      DLLLOCAL virtual QPaintEngine *parent_paintEngine() const
      {
         return QBitmap::paintEngine();
      }
};

#endif
