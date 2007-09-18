/*
 QC_QChar.h
 
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

#ifndef _QORE_QT_QC_QCHAR_H

#define _QORE_QT_QC_QCHAR_H

#include <QChar>

DLLLOCAL extern int CID_QCHAR;
DLLLOCAL extern class QoreClass *QC_QChar;

DLLLOCAL class QoreClass *initQCharClass();

class QoreQChar : public AbstractPrivateData, public QChar
{
   public:
      DLLLOCAL QoreQChar() : QChar()
      {
      }
      DLLLOCAL QoreQChar(char ch) : QChar(ch)
      {
      }
      DLLLOCAL QoreQChar(int code) : QChar(code)
      {
      }
      DLLLOCAL QoreQChar(SpecialCharacter ch) : QChar(ch)
      {
      }
};

#endif // _QORE_QT_QC_QCHAR_H
