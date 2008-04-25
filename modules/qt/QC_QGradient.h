/*
 QC_QGradient.h
 
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

#ifndef _QORE_QT_QC_QGRADIENT_H

#define _QORE_QT_QC_QGRADIENT_H

#include <QGradient>

DLLLOCAL extern int CID_QGRADIENT;
DLLLOCAL extern QoreClass *QC_QGradient;
DLLLOCAL QoreNamespace *initQGradientNS();

class QoreAbstractQGradient : public AbstractPrivateData
{
   public:
      DLLLOCAL virtual QGradient *getQGradient() = 0;
};

class QoreQGradient : public QoreAbstractQGradient, public QGradient
{
   public:
      DLLLOCAL QoreQGradient() : QGradient()
      {
      }

      DLLLOCAL virtual QGradient *getQGradient()
      {
	 return this;
      }
};

// this class manages temporary QGradient pointers that will not be deleted
class QoreQtQGradient : public QoreAbstractQGradient
{
   private:
      QGradient *qg;

   public:
      DLLLOCAL QoreQtQGradient(QGradient *n_qg) : qg(n_qg)
      {
      }

      DLLLOCAL virtual QGradient *getQGradient()
      {
	 return qg;
      }
};


#endif // _QORE_QT_QC_QGRADIENT_H
