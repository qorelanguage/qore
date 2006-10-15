#ifndef QC_TUXEDO_QUEUE_CONTROL_PARAMS_H_
#define QC_TUXEDO_QUEUE_CONTROL_PARAMS_H_

/*
  modules/Tuxedo/QC_TuxedoQueueControlParams.h

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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

#include <qore/common.h>
#include <qore/support.h>

// Instances of class Tuxedo::TuxedoQueueCtl are used as in/out parameters
// for functions tpenqueue() and tpdequeue(), as a Qore replacement
// of TPQCTL structure from atmi.h.
//
// Newly constructed object is zeroed, the flags item has value TPNOFLAGS.
// Getters and setters for all items in the structure are provided.
//
// For more details see documentaion:
// * http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c38.htm
// * http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c36.htm
// * $TUXDIR/include/atmi.h


extern int CID_TUXEDOQUEUECONTROLPARAMS;
extern class QoreClass* initTuxedoQueueControlParamsClass();

#endif

// EOF

