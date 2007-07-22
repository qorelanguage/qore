/*
 QoreAbstractQObject.h
 
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

#ifndef _QORE_QOREABSTRACTQOBJECT_H

#define _QORE_QOREABSTRACTQOBJECT_H

#include <qore/Qore.h>

#include <QPointer>
#include <QObject>

class QoreAbstractQObject : public AbstractPrivateData
{
   private:
      //QOBJECT

      //LockedObject m_qobj;
      //bool deleted;

      DLLLOCAL void qinit()
      {
	 //QObject::connect(qobj, SIGNAL(destroyed()), qobj, SLOT(is_Deleted()));
      }

   public:

/*
   public slots:
      DLLLOCAL void isDeleted()
      {
	 AutoLocker al(&m_qobj);
	 deleted = true;
      }
*/
      DLLLOCAL QoreAbstractQObject() //: deleted(0)
      {
      }

      DLLLOCAL virtual void destructor(class ExceptionSink *xsink) = 0;

      DLLLOCAL virtual class QObject *getQObject() const = 0;
};

#endif
