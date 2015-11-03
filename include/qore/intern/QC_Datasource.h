/*
  QC_Datasource.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

/* 
   FIXME: commit()s when autocommit=true should be made here, also after
          select()s (in case of a select for update, for example)

   FIXME: when raising an timeout exception there is a race condition
          getting the TID of the thread holding the lock, because the lock 
	  could have been released after the ::enter() call fails... but it's
	  only cosmetic (for the exception text)
 */

#ifndef _QORE_QC_DATASOURCE_H

#define _QORE_QC_DATASOURCE_H

#ifdef _QORE_LIB_INTERN
#include <qore/intern/ManagedDatasource.h>

extern qore_classid_t CID_DATASOURCE;

DLLLOCAL class QoreClass *initDatasourceClass();

#endif // _QORE_LIB_INTERN

#endif // _QORE_SQL_OBJECTS_DATASOURCE_H
