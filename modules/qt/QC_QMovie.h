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

#ifndef _QORE_QT_QC_QMOVIE_H

#define _QORE_QT_QC_QMOVIE_H

#include <QMovie>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMOVIE;
DLLLOCAL extern class QoreClass *QC_QMovie;

DLLLOCAL class QoreClass *initQMovieClass(QoreClass *);
DLLLOCAL void initQMovieStaticFunctions();

class myQMovie : public QMovie, public QoreQObjectExtension
{
#define QOREQTYPE QMovie
#define MYQOREQTYPE myQMovie
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMovie(QoreObject *obj, QObject* parent = 0) : QMovie(parent), QoreQObjectExtension(obj, this)
      {
         
      }
      DLLLOCAL myQMovie (QoreObject *obj, QIODevice * device, const QByteArray & format = QByteArray(), QObject * parent = 0) : QMovie(device, format, parent), QoreQObjectExtension(obj, this)
      {
	 
      }
      DLLLOCAL myQMovie(QoreObject *obj, const QString& fileName, const QByteArray& format = QByteArray(), QObject* parent = 0) : QMovie(fileName, format, parent), QoreQObjectExtension(obj, this)
      {
         
      }
};

typedef QoreQObjectBase<myQMovie, QoreAbstractQObject> QoreQMovieImpl;

class QoreQMovie : public QoreQMovieImpl
{
   public:
      DLLLOCAL QoreQMovie(QoreObject *obj, QObject* parent = 0) : QoreQMovieImpl(new myQMovie(obj, parent))
      {
      }
      DLLLOCAL QoreQMovie (QoreObject *obj, QIODevice * device, const QByteArray & format = QByteArray(), QObject * parent = 0) : QoreQMovieImpl(new myQMovie(obj, device, format, parent))
      {
      }
      DLLLOCAL QoreQMovie(QoreObject *obj, const QString& fileName, const QByteArray& format = QByteArray(), QObject* parent = 0) : QoreQMovieImpl(new myQMovie(obj, fileName, format, parent))
      {
      }
};

#endif // _QORE_QT_QC_QMOVIE_H
