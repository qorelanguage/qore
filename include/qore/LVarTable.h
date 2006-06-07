/*
  Variable.h

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

#ifndef _QORE_LVARTABLE_H

#define _QORE_LVARTABLE_H

#include <qore/common.h>
#include <qore/LockedObject.h>

#include <string.h>
#include <stdlib.h>

class LVTNode
{
  public:
   char *name;
   class LVTNode *next;

   inline LVTNode(char *n)
   {
      name = strdup(n);
      next = NULL;
   }
   inline ~LVTNode()
   {
      free(name);
   }
};

class LVarTable : public LockedObject
{
  private:
   class LVTNode *head, *tail;
   int num;

  public:
   inline LVarTable()
   {
      head = tail = NULL;
      num = 0;
   }
   inline LVarTable::~LVarTable()
   {
      while (head)
      {
	 class LVTNode *w = head->next;
	 delete head;
	 head = w;
      }
      num = 0;
   }
   inline lvh_t add(char *name)
   {
      class LVTNode *n = new LVTNode(name);
      
      lock();
      if (!tail)
	 head = n;
      else
	 tail->next = n;
      tail = n;
      unlock();
      
      return n->name;
   }

   inline char *getName(lvh_t lvh)
   {
      return (char *)lvh;
   }
   //inline int find(char *name);                                                                                                     
};

#endif // _QORE_LVARTABLE_H
