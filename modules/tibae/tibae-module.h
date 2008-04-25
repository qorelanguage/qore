/*
  modules/TIBCO/tibco-module.h

  TIBCO integration to QORE

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

#ifndef _QORE_TIBCO_MODULE_H

#define _QORE_TIBCO_MODULE_H

#include <qore/Qore.h>

QoreStringNode *tibae_module_init();
void tibae_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns);
void tibae_module_delete();

#endif
