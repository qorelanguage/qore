/*
  modules/Tuxedo/QoreTuxedoAdapter.cc

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
#include <qore/minitest.hpp>

#include <atmi.h>

#include "QoreTuxedoAdapter.h"

//------------------------------------------------------------------------------
namespace {

typedef std::vector<std::pair<std::string, std::string> > env_t;

struct EnvironmentSetter
{
  env_t m_old_env;
  EnvironmentSetter(const env_t& new_env);
  ~EnvironmentSetter();
};

EnvironmentSetter::EnvironmentSetter(const env_t& new_env)
{
  for (env_t::const_iterator it = new_env.begin(), end = new_env.end(); it != end; ++it) {
    char* old = getenv(it->first.c_str());
    if (!old) old = "";
    m_old_env.push_back(std::make_pair(it->first, old));

    if (it->second.empty()) {
      unsetenv(it->first.c_str());
    } else {
      setenv(it->first.c_str(), it->second.c_str(), 1);
    }
  }
}

EnvironmentSetter::~EnvironmentSetter()
{
  for (env_t::const_iterator it = m_old_env.begin(), end = m_old_env.end(); it != end; ++it) {
    if (it->second.empty()) {
      unsetenv(it->first.c_str());
    } else {
      setenv(it->first.c_str(), it->second.c_str(), 1);
    }
  }
}

} // namespace

#ifdef DEBUG
TEST()
{
  using namespace std;
  vector<pair<string, string> > envs;
  envs.push_back(make_pair("test_aaa", "xyz"));
  envs.push_back(make_pair("test_bbb", ""));

  setenv("test_aaa", "old aaa", 1);
  setenv("test_bbb", "old bbb", 1);

  { 
    EnvironmentSetter setter(envs);
    char* s = getenv("test_aaa");
    assert(s);
    assert(!strcmp(s, "xyz"));
    s = getenv("test_bbb");
    if (s) assert(!s[0]);
  }
  char* s = getenv("test_aaa");
  assert(s);
  assert(!strcmp(s, "old aaa"));
  s = getenv("test_bbb");
  assert(s);
  assert(!strcmp(s, "old bbb"));
}
#endif

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter()
: m_connection_flags(0)
{
}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::getNeededAuthentication(int& out_auth) const
{
  EnvironmentSetter setter(m_env_variables);
  int res = tpchkauth();
  if (res == -1) return tperrno;
  out_auth = res;
  return 0;
}

// EOF

