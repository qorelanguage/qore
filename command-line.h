/*
   command-line.h

   core
*/

#ifndef QORE_COMMAND_LINE_H

#define QORE_COMMAND_LINE_H

#define is_assign_char(a) ((((a) == '=') || ((a) == ':')))

char *parse_command_line(unsigned argc, char **argv);

#endif
