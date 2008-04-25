/*
 QoreAbstractQSplitterHandle.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQSPLITTERHANDLE_H

#define _QORE_QT_QOREABSTRACTQSPLITTERHANDLE_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQSplitterHandle : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QSplitterHandle *getQSplitterHandle() const = 0;

      DLLLOCAL virtual int closestLegalPosition ( int pos ) = 0;
      DLLLOCAL virtual void moveSplitter ( int pos ) = 0;
};

template<typename T, typename V>
class QoreQSplitterHandleBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQSplitterHandleBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }

      DLLLOCAL virtual QSplitterHandle *getQSplitterHandle() const
      {
         return static_cast<QSplitterHandle *>(&(*this->qobj));
      }

      DLLLOCAL virtual int closestLegalPosition ( int pos )
      {
	 return this->qobj->parent_closestLegalPosition(pos);
      }

      DLLLOCAL virtual void moveSplitter ( int pos )
      {
	 this->qobj->parent_moveSplitter(pos);
      }
};

template<typename T, typename V>
class QoreQtQSplitterHandleBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQSplitterHandleBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QSplitterHandle *getQSplitterHandle() const
      {
         return this->qobj;
      }

      // these functions can never be called
      DLLLOCAL virtual int closestLegalPosition ( int pos )
      {
	 return 0;
      }

      DLLLOCAL virtual void moveSplitter ( int pos )
      {
      }

};

#endif  // _QORE_QT_QOREABSTRACT%s_H
