/*
 QC_QBrush.h
 
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

#ifndef _QORE_QC_QBRUSH_H

#define _QORE_QC_QBRUSH_H

#include <QBrush>

DLLLOCAL extern int CID_QBRUSH;
DLLLOCAL extern QoreClass *QC_QBrush;

DLLLOCAL class QoreClass *initQBrushClass();

class QoreQBrush : public AbstractPrivateData
{
   private:
      QBrush *brush;
      bool managed;

   public:
      DLLLOCAL QoreQBrush() : brush(new QBrush()), managed(true)
      {
      }
      DLLLOCAL QoreQBrush(QBrush *br) : brush(br), managed(false)
      {
      }
      DLLLOCAL QoreQBrush(const QBrush *br) : brush(const_cast<QBrush *>(br)), managed(false)
      {
      }
      DLLLOCAL ~QoreQBrush()
      {
	 if (managed)
	    delete brush;
      }
      DLLLOCAL QoreQBrush(const QColor &color, Qt::BrushStyle style = Qt::SolidPattern) : brush(new QBrush(color, style)), managed(true)
      {
      }
      DLLLOCAL QoreQBrush(Qt::BrushStyle style) : brush(new QBrush(style)), managed(true)
      {
      }
      DLLLOCAL QoreQBrush(Qt::GlobalColor color, Qt::BrushStyle style = Qt::SolidPattern) : brush(new QBrush(color, style)), managed(true)
      {
      }
      DLLLOCAL QoreQBrush(const QColor &color, const QPixmap &pixmap) : brush(new QBrush(color, pixmap)), managed(true)
      {
      }
      DLLLOCAL QoreQBrush(Qt::GlobalColor color, const QPixmap &pixmap) : brush(new QBrush(color, pixmap)), managed(true)
      {
      }   
      DLLLOCAL QoreQBrush(const QBrush &brush) : brush(new QBrush(brush)), managed(true)
      {
      }
      DLLLOCAL QBrush *getQBrush() const
      {
	 return brush;
      }
};


#endif
