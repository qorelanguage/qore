/*
 QC_QPixmap.h
 
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

#ifndef _QORE_QC_QPIXMAP_H

#define _QORE_QC_QPIXMAP_H

#include "QoreAbstractQPixmap.h"

#include <QPixmap>

DLLLOCAL extern qore_classid_t CID_QPIXMAP;
DLLLOCAL extern QoreClass *QC_QPixmap;

DLLLOCAL class QoreClass *initQPixmapClass(class QoreClass *qpaintdevice);
DLLLOCAL void initQPixmapStaticFunctions();

class QoreQPixmap : public QoreAbstractQPixmap, public QPixmap
{
   public:
      DLLLOCAL QoreQPixmap(int w, int h) : QPixmap(w, h)
      {
      }
      DLLLOCAL QoreQPixmap(const QString &filename, const char *format = 0, Qt::ImageConversionFlags flags = Qt::AutoColor) : QPixmap(filename, format, flags)
      {
      }
      DLLLOCAL QoreQPixmap(const QPixmap &pix) : QPixmap(pix)
      {
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(const_cast<QoreQPixmap *>(this));
      }
      DLLLOCAL virtual QPixmap *getQPixmap() const
      {
         return static_cast<QPixmap *>(const_cast<QoreQPixmap *>(this));
      }
      DLLLOCAL virtual QPaintEngine *parent_paintEngine() const
      {
         return QPixmap::paintEngine();
      }
};

#endif
