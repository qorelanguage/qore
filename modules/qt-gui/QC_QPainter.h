/*
 QC_QPainter.h
 
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

#ifndef _QORE_QC_QPAINTER_H

#define _QORE_QC_QPAINTER_H

#include <QPainter>

DLLEXPORT extern qore_classid_t CID_QPAINTER;
DLLEXPORT extern QoreClass *QC_QPainter;

DLLEXPORT class QoreClass *initQPainterClass();

class QoreQPainter : public AbstractPrivateData
{
   private:
      QPainter *qpainter;
      bool managed;

   public:
      DLLLOCAL QoreQPainter() : qpainter(new QPainter()), managed(true)
      {
      }
      DLLLOCAL ~QoreQPainter()
      {
	 if (managed)
	    delete qpainter;
      }
      DLLLOCAL QoreQPainter(QPaintDevice *device) : qpainter(new QPainter(device)), managed(true)
      {
      }
      DLLLOCAL QoreQPainter(QPainter *qp) : qpainter(qp), managed(false)
      {
      }
      DLLLOCAL QPainter *getQPainter() const
      {
	 return qpainter;
      }
/*
      DLLLOCAL QoreObject *getQoreObject() const
      {
	 return qore_obj;
      }
*/
};


#endif
