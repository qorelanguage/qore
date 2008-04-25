/*
 QC_QTime.h
 
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

#ifndef _QORE_QC_QTIME_H

#define _QORE_QC_QTIME_H

#include <QTime>

DLLLOCAL extern qore_classid_t CID_QTIME;
DLLLOCAL extern QoreClass *QC_QTime;

DLLLOCAL class QoreClass *initQTimeClass();

class QoreQTime : public AbstractPrivateData, public QTime
{
   public:
      DLLLOCAL QoreQTime()
      {
      }
      DLLLOCAL QoreQTime(const QTime &time) : QTime(time)
      {
      }
      DLLLOCAL QoreQTime(int h, int m, int s = 0, int ms = 0) : QTime(h, m, s, ms)
      {
      }
};


#endif
