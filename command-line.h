/*
   command-line.h

   qore
*/

#ifndef QORE_COMMAND_LINE_H

#define QORE_COMMAND_LINE_H

#define is_assign_char(a) ((((a) == '=') || ((a) == ':')))

// for list of modules to load after initialization
#include <qore/safe_dslist>
#include <string>

typedef safe_dslist<std::string> cl_mod_list_t;

extern cl_mod_list_t cl_mod_list;

char *parse_command_line(unsigned argc, char **argv);

#endif
