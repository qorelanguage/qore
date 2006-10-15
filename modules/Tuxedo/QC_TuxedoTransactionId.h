#ifndef QC_TUXEDO_TRANSACTION_ID_H_
#define QC_TUXEDO_TRANSACTION_ID_H_

/*
  modules/Tuxedo/QC_TuxedoQueueTransactionId.h

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

// Instances of class Tuxedo::TuxedoTransactionTd are used
// for calls tpsuspend() and tpresume().
// Thesy serve as opaque structures and provide no member methods.
//
// For more details see documentaion:
// * http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c65.htm
// * http://edocs.bea.com/tuxedo/tux91/rf3c/rf3c81.htm


extern int CID_TUXEDOTRANSACTIOONID;
extern class QoreClass* initTuxedoTransactionIdClass();

#endif

// EOF

