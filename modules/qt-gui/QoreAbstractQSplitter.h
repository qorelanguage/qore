/*
 QoreAbstractQSplitter.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQSPLITTER_H

#define _QORE_QT_QOREABSTRACTQSPLITTER_H

#include "QoreAbstractQWidget.h"

class QoreAbstractQSplitter : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QSplitter *getQSplitter() const = 0;

      virtual QSplitterHandle *createHandle() = 0;
      virtual int closestLegalPosition (int pos, int index) = 0;
      virtual void moveSplitter ( int pos, int index ) = 0;
      virtual void setRubberBand ( int pos ) = 0;
};

template<typename T, typename V>
class QoreQSplitterBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQSplitterBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      
      DLLLOCAL virtual QSplitter *getQSplitter() const
      {
         return static_cast<QSplitter *>(&(*this->qobj));
      }

      virtual QSplitterHandle *createHandle()
      {
	 return this->qobj->parent_createHandle();
      }
      
      virtual int closestLegalPosition (int pos, int index)
      {
	 return this->qobj->parent_closestLegalPosition(pos, index);
      }
      
      virtual void moveSplitter ( int pos, int index )
      {
	 this->qobj->parent_moveSplitter(pos, index);
      }
      
      virtual void setRubberBand ( int pos )
      {
	 this->qobj->parent_setRubberBand(pos);
      }
};

template<typename T, typename V>
class QoreQtQSplitterBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQSplitterBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }
      
      DLLLOCAL virtual QSplitter *getQSplitter() const
      {
         return this->qobj;
      }
       
      // these functions can never be called
      virtual QSplitterHandle *createHandle() { return 0; }
      virtual int closestLegalPosition (int pos, int index) { return 0; }
      virtual void moveSplitter ( int pos, int index ) { }
      virtual void setRubberBand ( int pos ) { }
};

#endif  // _QORE_QT_QOREABSTRACT%s_H
