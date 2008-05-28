/*
 QC_QSplitter.h
 
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

#ifndef _QORE_QT_QC_QSPLITTER_H

#define _QORE_QT_QC_QSPLITTER_H

#include <QSplitter>
#include "QoreAbstractQSplitter.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QSPLITTER;
DLLEXPORT extern QoreClass *QC_QSplitter;
DLLEXPORT QoreClass *initQSplitterClass(QoreClass *);

class myQSplitter : public QSplitter, public QoreQWidgetExtension
{
#define QOREQTYPE QSplitter
#define MYQOREQTYPE myQSplitter
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   protected:
/*
      const QoreMethod *m_createHandle;

      void init(const QoreClass *qc)
      {
         m_createHandle = oc->findMethod("createHandle");
      }

      virtual QSplitterHandle * createHandle ()
      {
	 if (!m_createHandle) {
	    return QSplitter::createHandle();
	 }
      }
*/

   public:
      DLLLOCAL myQSplitter(QoreObject *obj, QWidget* parent = 0) : QSplitter(parent), QoreQWidgetExtension(obj, this)
      {
      }
      DLLLOCAL myQSplitter(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QSplitter(orientation, parent), QoreQWidgetExtension(obj, this)
      {
      }

      QSplitterHandle *parent_createHandle()
      {
	 return QSplitter::createHandle();
      }

      int parent_closestLegalPosition ( int pos, int index )
      {
	 return QSplitter::closestLegalPosition(pos, index);
      }

      void parent_moveSplitter ( int pos, int index )
      {
	 QSplitter::moveSplitter(pos, index);
      }

      void parent_setRubberBand ( int pos )
      {
	 QSplitter::setRubberBand(pos);
      }
};

typedef QoreQSplitterBase<myQSplitter, QoreAbstractQSplitter> QoreQSplitterImpl;

class QoreQSplitter : public QoreQSplitterImpl
{
   public:
      DLLLOCAL QoreQSplitter(QoreObject *obj, QWidget* parent = 0) : QoreQSplitterImpl(new myQSplitter(obj, parent))
      {
      }
      DLLLOCAL QoreQSplitter(QoreObject *obj, Qt::Orientation orientation, QWidget* parent = 0) : QoreQSplitterImpl(new myQSplitter(obj, orientation, parent))
      {
      }
};

typedef QoreQtQSplitterBase<QSplitter, QoreAbstractQSplitter> QoreQtQSplitterImpl;

class QoreQtQSplitter : public QoreQtQSplitterImpl
{
   public:
      DLLLOCAL QoreQtQSplitter(QoreObject *obj, QSplitter *qsplitter) : QoreQtQSplitterImpl(obj, qsplitter)
      {
      }
};

#endif // _QORE_QT_QC_QSPLITTER_H
