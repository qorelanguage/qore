/*
 ForStatement.h
 
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

#ifndef _QORE_FORSTATEMENT_H

#define _QORE_FORSTATEMENT_H

class ForStatement {
   class QoreNode *assignment;
   class QoreNode *cond;
   class QoreNode *iterator;
   class StatementBlock *code;
   class LVList *lvars;
   
public:
   DLLLOCAL ForStatement(class QoreNode *a, class QoreNode *c, class QoreNode *i, class StatementBlock *cd);
   DLLLOCAL ~ForStatement();
   DLLLOCAL int exec(class QoreNode **return_value, class ExceptionSink *xsink);
   DLLLOCAL void parseInit(lvh_t oflag, int pflag = 0);
};

#endif
