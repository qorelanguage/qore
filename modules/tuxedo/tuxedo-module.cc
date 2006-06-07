/*
  Modules/tuxedo/tuxedo-module.cc

  Tuxedo integration to QORE

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/support.h>
#include <qore/Namespace.h>
#include <qore/module.h>

#include "tuxedo-module.h"
#include "tuxedo.h"

#include <string.h>

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "tuxedo";
char qore_module_version[] = "0.1";
char qore_module_description[] = "Tuxedo integration module";
char qore_module_author[] = "David Nichols";
char qore_module_url[] = "http://qore.sourceforge.net";
#endif

void tuxedo_module_init(class QoreProgram *pgm)
{
   tracein("tuxedo_module_init()");
   class Namespace *tuxns = new Namespace("Tuxedo");
   tuxns->addSystemClass(initTuxedoClientClass());
   pgm->getQoreNS()->addInitialNamespace(tuxns);
   traceout("tuxedo_module_init()");
}

void tuxedo_module_delete()
{
   tracein("tuxedo_module_delete()");
   traceout("tuxedo_module_delete()");
}
