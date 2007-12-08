/*
 QC_QPalette.h
 
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

#ifndef _QORE_QC_QPALETTE_H

#define _QORE_QC_QPALETTE_H

#include <QPalette>

extern int CID_QPALETTE;
extern QoreClass *QC_QPalette;

DLLLOCAL QoreNamespace *initQPaletteNS();

class QoreQPalette : public AbstractPrivateData
{
      QPalette *qp;
      bool managed;
      
   public:
      DLLLOCAL QoreQPalette() : qp(new QPalette()), managed(true)
      {
      }
      DLLLOCAL QoreQPalette(QPalette *qpalette) : qp(qpalette), managed(false)
      {
      }
      DLLLOCAL ~QoreQPalette()
      {
	 if (managed)
	    delete qp;
      }
      
      DLLLOCAL QoreQPalette(Qt::GlobalColor color) : qp(new QPalette(color)), managed(true)
      {
      }
      DLLLOCAL QoreQPalette(const QPalette &palette) : qp(new QPalette(palette)), managed(true)
      {
      }
      DLLLOCAL QoreQPalette(const QColor &button) : qp(new QPalette(button)), managed(true)
      {
      }
      DLLLOCAL QoreQPalette(const QColor &button, const QColor &window) : qp(new QPalette(button, window)), managed(true)
      {
      }
      DLLLOCAL QoreQPalette(const QBrush &windowText, const QBrush &button, const QBrush &light, 
			    const QBrush &dark, const QBrush &mid, const QBrush &text, 
			    const QBrush &bright_text, const QBrush &base, const QBrush &window) : 
	 qp(new QPalette(windowText, button, light, dark, mid, text, bright_text, base, window)), managed(true)
      {
      }

      DLLLOCAL class QPalette *getQPalette() const
      {
	 return qp;
      }
};


#endif
