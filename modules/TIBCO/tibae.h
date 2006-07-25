/*
  tibco.h

  TIBCO integration to QORE

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

#ifndef QORE_TIBCO_H

#define QORE_TIBCO_H
#include <Maverick.h>

#define TIBCO_EXPLICIT_CREATE_SESSION 1
#define DEFAULT_SUBJECT "QORE.MESSAGE"

extern class QoreClass *QC_TIBCO;

class QoreNode *map_mdata_to_node(MData *md, class ExceptionSink *xsink);
class QoreClass *initTibcoAdapterClass();

#endif
