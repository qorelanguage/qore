/*
  ParseOptionMap.cpp

  Qore Programming language

  Copyright (C) 2003 - 2016 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/ParseOptionMap.h>
#include <qore/Restrictions.h>

opt_map_t ParseOptionMap::map;
rev_opt_map_t ParseOptionMap::rmap;

static ParseOptionMap parse_option_map;

#define DO_MAP(a, b) map[(a)] = (b); rmap[(b)] = (a);

ParseOptionMap::ParseOptionMap() {
   static_init();
}

void ParseOptionMap::static_init() {
   DO_MAP("no-global-vars",           PO_NO_GLOBAL_VARS);
   DO_MAP("no-subroutine-defs",       PO_NO_SUBROUTINE_DEFS);
   DO_MAP("no-thread-control",        PO_NO_THREAD_CONTROL);
   DO_MAP("no-thread-classes",        PO_NO_THREAD_CLASSES);
   DO_MAP("no-top-level",             PO_NO_TOP_LEVEL_STATEMENTS);
   DO_MAP("no-class-defs",            PO_NO_CLASS_DEFS);
   DO_MAP("no-namespace-defs",        PO_NO_NAMESPACE_DEFS);
   DO_MAP("no-constant-defs",         PO_NO_CONSTANT_DEFS);
   DO_MAP("no-new",                   PO_NO_NEW);
   DO_MAP("no-system-classes",        PO_NO_INHERIT_SYSTEM_CLASSES);
   DO_MAP("no-user-classes",          PO_NO_INHERIT_USER_CLASSES);
   DO_MAP("no-child-restrictions",    PO_NO_CHILD_PO_RESTRICTIONS);
   DO_MAP("no-external-access",       PO_NO_EXTERNAL_ACCESS);
   DO_MAP("no-external-info",         PO_NO_EXTERNAL_INFO);
   DO_MAP("no-external-process",      PO_NO_EXTERNAL_PROCESS);
   DO_MAP("require-our",              PO_REQUIRE_OUR);
   DO_MAP("no-process-control",       PO_NO_PROCESS_CONTROL);
   DO_MAP("no-network",               PO_NO_NETWORK);
   DO_MAP("no-filesystem",            PO_NO_FILESYSTEM);
   DO_MAP("no-database",              PO_NO_DATABASE);
   DO_MAP("no-gui",                   PO_NO_GUI);
   DO_MAP("no-terminal-io",           PO_NO_TERMINAL_IO);
   DO_MAP("require-types",            PO_REQUIRE_TYPES);
   DO_MAP("no-thread-info",           PO_NO_THREAD_INFO);
   DO_MAP("no-locale-control",        PO_NO_LOCALE_CONTROL);
   DO_MAP("no-io",                    PO_NO_IO);
   DO_MAP("no-modules",               PO_NO_MODULES);
   DO_MAP("lockdown",                 PO_LOCKDOWN);
   DO_MAP("no-embedded-logic",        PO_NO_EMBEDDED_LOGIC);
   DO_MAP("strict-bool-eval",         PO_STRICT_BOOLEAN_EVAL);
   DO_MAP("allow-injection",          PO_ALLOW_INJECTION);
   DO_MAP("no-user-api",              PO_NO_USER_API);
   DO_MAP("no-system-api",            PO_NO_SYSTEM_API);
   DO_MAP("no-api",                   PO_NO_API);
   DO_MAP("no-user-constants",        PO_NO_INHERIT_USER_CONSTANTS);
   DO_MAP("no-system-constants",      PO_NO_INHERIT_SYSTEM_CONSTANTS);
   DO_MAP("broken-list-parsing",      PO_BROKEN_LIST_PARSING);
   DO_MAP("broken-logic-precedence",  PO_BROKEN_LOGIC_PRECEDENCE);
   DO_MAP("broken-int-assignments",   PO_BROKEN_INT_ASSIGNMENTS);
   DO_MAP("broken-operators",         PO_BROKEN_OPERATORS);
}

int ParseOptionMap::find_code(const char *name) {
   opt_map_t::iterator i = map.find(name);
   //printd(5, "find_code(%s) returning %p\n", name, i == map.end() ? -1 : i->second);
   return (int)(i == map.end() ? -1 : i->second);
}

int64 ParseOptionMap::find_code64(const char *name) {
   opt_map_t::iterator i = map.find(name);
   //printd(5, "find_code(%s) returning %p\n", name, i == map.end() ? -1 : i->second);
   return (i == map.end() ? -1 : i->second);
}

const char *ParseOptionMap::find_name(int code) {
   rev_opt_map_t::iterator i = rmap.find(code);
   return (i == rmap.end() ? 0 : i->second);
}

void ParseOptionMap::list_options() {
   for (opt_map_t::iterator i = map.begin(), e = map.end(); i != e; ++i)
      printf("%s\n", i->first);
}

#undef DO_MAP
