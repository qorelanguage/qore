/*
 QC_QMovie.h
 
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

#ifndef _QORE_QC_QMOVIE_H

#define _QORE_QC_QMOVIE_H

#include "QoreAbstractQObject.h"

#include <QMovie>

DLLLOCAL extern int CID_QMOVIE;
DLLLOCAL extern QoreClass *QC_QMovie;

DLLLOCAL class QoreClass *initQMovieClass(class QoreClass *parent);

class myQMovie : public QMovie
{
#define QOREQTYPE QMovie
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQMovie(Object *obj, QObject *parent = 0) : QMovie(parent)
      {
	 init(obj);
      }
      DLLLOCAL myQMovie(Object *obj, const char *filename, const QByteArray &format, QObject *parent = 0) 
	 : QMovie(filename, format, parent)
      {
	 init(obj);
      }
      DLLLOCAL myQMovie(Object *obj, QIODevice *device, const QByteArray &format) : QMovie(device, format)
      {
	 init(obj);
      }
      DLLLOCAL myQMovie(Object *obj, const QString &filename, const QByteArray &format) : QMovie(filename, format)
      {
	 init(obj);
      }
};

class QoreQMovie : public QoreAbstractQObject
{
   public:
      myQMovie *qobj;

      DLLLOCAL QoreQMovie(Object *obj, QObject *parent = 0) : qobj(new myQMovie(obj, parent))
      {
      }

      DLLLOCAL QoreQMovie(Object *obj, const char *filename, const QByteArray &format, QObject *parent = 0) : qobj(new myQMovie(obj, filename, format, parent))
      {
      }

      DLLLOCAL QoreQMovie(Object *obj, QIODevice *device, const QByteArray &format) : qobj(new myQMovie(obj, device, format))
      {
      }

      DLLLOCAL QoreQMovie(Object *obj, const QString &filename, const QByteArray &format) : qobj(new myQMovie(obj, filename, format))
      {
      }

      DLLLOCAL virtual ~QoreQMovie()
      {
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(qobj);
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};


#endif
