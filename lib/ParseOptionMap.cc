/*
 ParseOptionMap.cc
 
 Qore Programming language
 
 Copyright 2003 - 2009 David Nichols
 
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

#include <qore/Qore.h>
#include <qore/ParseOptionMap.h>
#include <qore/Restrictions.h>

opt_map_t ParseOptionMap::map;
rev_opt_map_t ParseOptionMap::rmap;

static ParseOptionMap parse_option_map;

#define DO_MAP(a, b) map[(a)] = (b); rmap[(b)] = (a);

ParseOptionMap::ParseOptionMap()
{
   static_init();
}

void ParseOptionMap::static_init()
{
   DO_MAP("no-global-vars",           PO_NO_GLOBAL_VARS);
   DO_MAP("no-subroutine-defs",       PO_NO_SUBROUTINE_DEFS);
   DO_MAP("no-thread-control",        PO_NO_THREAD_CONTROL);
   DO_MAP("no-thread-classes",        PO_NO_THREAD_CLASSES);
   DO_MAP("no-top-level",             PO_NO_TOP_LEVEL_STATEMENTS);
   DO_MAP("no-class-defs",            PO_NO_CLASS_DEFS);
   DO_MAP("no-namespace-defs",        PO_NO_NAMESPACE_DEFS);
   DO_MAP("no-constant-defs",         PO_NO_CONSTANT_DEFS);
   DO_MAP("no-new",                   PO_NO_NEW);
   DO_MAP("no-system-classes",        PO_NO_SYSTEM_CLASSES);
   DO_MAP("no-user-classes",          PO_NO_USER_CLASSES);
   DO_MAP("no-child-restrictions",    PO_NO_CHILD_PO_RESTRICTIONS);
   DO_MAP("no-external-process",      PO_NO_EXTERNAL_PROCESS);
   DO_MAP("require-our",              PO_REQUIRE_OUR);
   DO_MAP("no-process-control",       PO_NO_PROCESS_CONTROL);
   DO_MAP("no-network",               PO_NO_NETWORK);
   DO_MAP("no-filesystem",            PO_NO_FILESYSTEM);
   DO_MAP("no-database",              PO_NO_DATABASE);
   DO_MAP("no-gui",                   PO_NO_GUI);
   DO_MAP("no-terminal-io",           PO_NO_TERMINAL_IO);
}

int ParseOptionMap::find_code(const char *name)
{
   opt_map_t::iterator i = map.find(name);
   //printd(5, "find_code(%s) returning %08x\n", name, i == map.end() ? -1 : i->second);
   return (i == map.end() ? -1 : i->second);
}

const char *ParseOptionMap::find_name(int code)
{
   rev_opt_map_t::iterator i = rmap.find(code);
   return (i == rmap.end() ? 0 : i->second);
}

void ParseOptionMap::list_options()
{
   for (opt_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i)
      printf("%s\n", i->first);
}

#undef DO_MAP
