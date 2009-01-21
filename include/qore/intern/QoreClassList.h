/*
  QoreClassList.h

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

#ifndef _QORE_QORECLASSLIST_H

#define _QORE_QORECLASSLIST_H

#include <qore/common.h>

#include <qore/QoreClass.h>
#include <qore/hash_map.h>

#include <string.h>
#include <stdlib.h>

class QoreClassList
{
   private:
      hm_qc_t hm;        // hash_map for name lookups
      
      DLLLOCAL void deleteAll();
      DLLLOCAL void assimilate(QoreClassList *n);
      DLLLOCAL void remove(hm_qc_t::iterator i)
      {
	 class QoreClass *qc = i->second;
	 //printd(5, "QCL::remove() this=%08p '%s' (%08p)\n", this, qc->getName(), qc);
         hm.erase(i);
	 qc->nderef();
      }
            
   public:
      DLLLOCAL QoreClassList() {}
      DLLLOCAL ~QoreClassList();
      DLLLOCAL int add(class QoreClass *ot);
      DLLLOCAL class QoreClass *find(const char *name);
      DLLLOCAL class QoreClass *findChange(const char *name);
      DLLLOCAL class QoreClassList *copy(int po);
      DLLLOCAL void parseInit();
      DLLLOCAL void parseRollback();
      DLLLOCAL void parseCommit(QoreClassList *n);
      DLLLOCAL void reset();
      DLLLOCAL void assimilate(QoreClassList *n, QoreClassList *otherlist, class QoreNamespaceList *nsl, class QoreNamespaceList *pendNSL, const char *nsname);
      DLLLOCAL class QoreHashNode *getInfo();
};

#endif // _QORE_QORECLASSLIST_H
