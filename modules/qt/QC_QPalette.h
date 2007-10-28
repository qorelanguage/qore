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

DLLLOCAL Namespace *initQPaletteNS();

class QoreQPalette : public AbstractPrivateData, public QPalette
{
   public:
      DLLLOCAL QoreQPalette() : QPalette()
      {
      }
      DLLLOCAL QoreQPalette(Qt::GlobalColor color) : QPalette(color)
      {
      }
      DLLLOCAL QoreQPalette(const QPalette &palette) : QPalette(palette)
      {
      }
      DLLLOCAL QoreQPalette(const QColor &button) : QPalette(button)
      {
      }
      DLLLOCAL QoreQPalette(const QColor &button, const QColor &window) : QPalette(button, window)
      {
      }
      DLLLOCAL QoreQPalette(const QBrush &windowText, const QBrush &button, const QBrush &light, 
			    const QBrush &dark, const QBrush &mid, const QBrush &text, 
			    const QBrush &bright_text, const QBrush &base, const QBrush &window) : 
	 QPalette(windowText, button, light, dark, mid, text, bright_text, base, window)
      {
      }
   
   
};


#endif
