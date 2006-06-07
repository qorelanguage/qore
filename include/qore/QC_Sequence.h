/* 
   QC_Sequence.h

   Thread Sequence object

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

#ifndef _QORE_CLASS_SEQUENCE_H

#define _QORE_CLASS_SEQUENCE_H

#include <qore/Sequence.h>
#include <qore/ReferenceObject.h>

class QoreSequence : public ReferenceObject, public Sequence
{
   protected:
      inline ~QoreSequence() {}

   public:
      inline QoreSequence(int start = 0) : Sequence(start) {}
      inline void deref();
};

extern int CID_SEQUENCE;

class QoreClass *initSequenceClass();

inline void QoreSequence::deref()
{
   if (ROdereference())
      delete this;
}


#endif // _QORE_CLASS_SEQUENCE_H
