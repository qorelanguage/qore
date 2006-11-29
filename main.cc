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

extern int parse_options, warnings;
extern char *def_charset;
extern char *cl_pgm, *exec_class_name;
extern bool show_mod_errs, lock_options, exec_class, warnings_are_errors;

int main(int argc, char *argv[])
{   
   int rc = 0;

   // parse the command line
   char *program_file_name = parse_command_line(argc, argv);

   // initialize Qore subsystem
   qore_init(def_charset, show_mod_errs);
   if (def_charset)
      free(def_charset);

   class ExceptionSink wsink, xsink;
   class QoreProgram *qpgm = new QoreProgram();

   qpgm->parseSetParseOptions(parse_options);
   if (lock_options)
      qpgm->lockOptions();
   if (exec_class)
   {
      if (exec_class_name)
	 qpgm->setExecClass(exec_class_name);
      else if (program_file_name)
      {
	 char *cn = make_class_name(program_file_name);
	 qpgm->setExecClass(cn);
	 free(cn);
      }
      else
      {
	 fprintf(stderr, "error, missing class name to instantiate as application\n");
	 rc = 1;
	 goto exit;
      }
   }
   //QoreProgram *pgm = new QoreProgram(parse_options, lock_options, exec_class, exec_class_name);

   // parse and run program
   if (cl_pgm)
      qpgm->parse(cl_pgm, "<command-line>", &xsink, &wsink, warnings);
   else if (program_file_name)
   {
      qpgm->parseFile(program_file_name, &xsink, &wsink, warnings);
      free(program_file_name);
   }
   else
      qpgm->parse(stdin, "<stdin>", &xsink, &wsink, warnings);

   if (xsink.isException())
      xsink.handleExceptions();
   else
   {
      if (wsink.isException())
      {
	 wsink.handleWarnings();
	 if (warnings_are_errors)
	 {
	    printf("exiting due to the above warnings...\n");
	    rc = 2;
	    goto exit;
	 }
      }

      class QoreNode *rv = qpgm->run(&xsink);
      rc = rv ? rv->getAsInt() : 0;
      if (rv)
	 rv->deref(&xsink);
      if (xsink.isException()) {
         rc = 3; // uncaught exception in Qore
      }
      xsink.handleExceptions();
      // wait for any other threads to terminate
      qpgm->waitForTermination();
   }
  exit:
   // destroy the program object (cannot call destructor explicitly)
   qpgm->deref(&xsink);
   xsink.handleExceptions();
   
   // cleanup Qore subsystem (deallocate memory, etc)
   qore_cleanup();

   return rc;
}
