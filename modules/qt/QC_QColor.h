#/*
 QC_QColor.h
 
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

#ifndef _QORE_QC_QCOLOR_H

#define _QORE_QC_QCOLOR_H

#include <QColor>

DLLLOCAL extern qore_classid_t CID_QCOLOR;
DLLLOCAL extern QoreClass *QC_QColor;

DLLLOCAL class QoreClass *initQColorClass();
DLLLOCAL void initQColorStaticFunctions();

class QoreQColor : public AbstractPrivateData, public QColor
{
   public:
      DLLLOCAL QoreQColor()
      {
      }
      DLLLOCAL QoreQColor(int r, int g, int b, int a = 255) : QColor(r, g, b, a)
      {
      }
      DLLLOCAL QoreQColor(const char *name) : QColor(name)
      {
      }
      DLLLOCAL QoreQColor(Qt::GlobalColor globalcolor) : QColor(globalcolor)
      {
      }
      DLLLOCAL QoreQColor(QRgb rgb) : QColor(rgb)
      {
      }
      DLLLOCAL QoreQColor(const QColor &color) : QColor(color)
      {
      }
};


#endif
