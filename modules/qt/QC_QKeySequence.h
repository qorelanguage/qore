/*
 QC_QKeySequence.h
 
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

#ifndef _QORE_QC_QKEYSEQUENCE_H

#define _QORE_QC_QKEYSEQUENCE_H

#include <QKeySequence>

DLLLOCAL extern qore_classid_t CID_QKEYSEQUENCE;
DLLLOCAL extern QoreClass *QC_QKeySequence;

DLLLOCAL class QoreClass *initQKeySequenceClass();

class QoreQKeySequence : public AbstractPrivateData, public QKeySequence
{
   public:
      DLLLOCAL QoreQKeySequence() : QKeySequence()
      {
      }
      DLLLOCAL QoreQKeySequence(StandardKey key) : QKeySequence(key)
      {
      }
      DLLLOCAL QoreQKeySequence(int k1, int k2 = 0, int k3 = 0, int k4 = 0) : QKeySequence(k1, k2, k3, k4)
      {
      }
      DLLLOCAL QoreQKeySequence(QKeySequence &KeySequence) : QKeySequence(KeySequence)
      {
      }
      DLLLOCAL QoreQKeySequence(const char *str) : QKeySequence(str)
      {
      }
};

#endif
