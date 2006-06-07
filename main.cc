/*
  main.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols
*/

#include <qore/Qore.h>
#include "command-line.h"

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <strings.h>

extern int parse_options;
extern char *def_charset;
extern char *cl_pgm, *exec_class_name;
extern bool show_mod_errs, lock_options, exec_class;

int main(int argc, char *argv[])
{
   // parse the command line
   char *program_file_name = parse_command_line(argc, argv);

   // initialize Qore subsystem
   qore_init(def_charset, show_mod_errs);
   if (def_charset)
      free(def_charset);

   class QoreProgram *qpgm = new QoreProgram();

   qpgm->parseSetParseOptions(parse_options);
   if (lock_options)
      qpgm->lockOptions();
   if (exec_class)
      qpgm->setExecClass(exec_class_name);
   //QoreProgram *pgm = new QoreProgram(parse_options, lock_options, exec_class, exec_class_name);

   // parse and run program
   if (cl_pgm)
      qpgm->parseAndRun(cl_pgm, "<command-line>");
   else if (program_file_name)
   {
      qpgm->parseFileAndRun(program_file_name);
      free(program_file_name);
   }
   else
      qpgm->parseAndRun(stdin, "<stdin>");

   qpgm->waitForTerminationAndDeref();

   // cleanup Qore subsystem (deallocate memory, etc)
   qore_cleanup();

   return 0;
}
