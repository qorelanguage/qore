/*
  modules/Tuxedo/hashed_parameters_helper.h

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

#ifndef QORE_TUXEDO_HASHED_PARAMETERS_HELPER_H_
#define QORE_TUXEDO_HASHED_PARAMETERS_HELPER_H_

#include <qore/common.h>
#include <qore/support.h>
#include <qore/LockedObject.h>
#include <atmi.h>
#include <vector>
#include <string>
#include <utility>

class ExceptionSink;
class Hash;

// Class used to:
// * avoid need to set environment variables (instead they are passed in a hash)
// * pass tpinit() parameters in hash form
//
// The class is multithreaded safe.

//------------------------------------------------------------------------------
class Tuxedo_hashed_parameters
{
private:
  static  LockedObject m_mutex; // must be static
  TPINIT* m_tpinit_data;
  std::vector<std::pair<std::string, std::string> > m_old_environment;

  void set_environment_variable(char* name, Hash* params, ExceptionSink* xsink);
  void set_flag(char* name, unsigned flag, Hash* params, ExceptionSink* xsink, long& flags);
  void set_string(char* name, Hash* params, ExceptionSink* xsink, std::string& str);

public:
  Tuxedo_hashed_parameters();
  ~Tuxedo_hashed_parameters();
  // Takes hash, sets environment variables from it, stores tpinit() data for later
  void process_hash(QoreNode* params, ExceptionSink* xsink); 
  // Get tpinit() data from processed hash. Returns 0 if there were none.
  TPINIT* get_tpinit_data() { return m_tpinit_data; }
};

#endif

// EOF

