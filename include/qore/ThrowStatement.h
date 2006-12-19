/*
 ThrowStatement.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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

#ifndef _QORE_THROWSTATEMENT_H

#define _QORE_THROWSTATEMENT_H

class ThrowStatement {
private:
   class QoreNode *args;
   
public:
   DLLLOCAL ThrowStatement(class QoreNode *v);
   DLLLOCAL ~ThrowStatement();
   DLLLOCAL void exec(ExceptionSink *xsink);
   DLLLOCAL int parseInit(lvh_t oflag, int pflag = 0);
};

#endif
