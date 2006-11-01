#ifndef QORE_TUXEDO_ADAPTER_IMPL_H_
#define QORE_TUXEDO_ADAPTER_IMPL_H_

/*
  modules/Tuxedo/QoreTuxedoAdapter.h

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 QoreTechnologies

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
#include <qore/Object.h>

#include <string>
#include <vector>
#include <utility>

#include <atmi.h>

//------------------------------------------------------------------------------
#ifdef DEBUG
// Values to be used as environment variables, names and passwords for tests.
// Undefine them to skip the tests that use them.
//

// The simplest test with TOUPPER service on strings
#define TUXDIR_SIMPLE  "/opt/bea/tuxedo9.1"
#define TUXCONFIG_SIMPLE "/home/pavel/tuxedo_tests/tuxedo_simple_app/tuxconfig"

#endif

//------------------------------------------------------------------------------
class QoreTuxedoAdapter : public ReferenceObject
{
public:
  // data used to connect Tuxedo server
  std::string m_username;
  std::string m_clientname;
  std::string m_groupname;
  std::string m_password;
  typedef std::vector<std::pair<std::string, std::string> > env_var_t;
  env_var_t m_env_variables;
  std::vector<char> m_binary_data;
  long m_connection_flags;

public:
  QoreTuxedoAdapter();
  ~QoreTuxedoAdapter();

  int getNeededAuthentication(int& out_auth) const;

  void deref() { 
    if (ROdereference()) {
      delete this;
    }
  }
};

#endif

// EOF

