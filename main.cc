/*
  main.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols
*/

#include <qore/Qore.h>

#include "command-line.h"

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <strings.h>

extern int parse_options;
extern int warnings, qore_lib_options;
extern const char *def_charset;
extern const char *cl_pgm, *exec_class_name, *eval_arg;
extern bool show_mod_errs, lock_options, exec_class, warnings_are_errors;
extern qore_license_t license;

int main(int argc, char *argv[]) {   
   int rc = 0;

   // parse the command line
   char *program_file_name = parse_command_line(argc, argv);

   // initialize Qore subsystem
   qore_init(license, def_charset, show_mod_errs, qore_lib_options);

   ExceptionSink wsink, xsink;
   { 
      QoreProgramHelper qpgm(xsink);

      // load any modules requested on the command-line
      bool mod_errs = false;
      for (cl_mod_list_t::iterator i = cl_mod_list.begin(), e = cl_mod_list.end(); i != e; ++i) {
	 // display any error messages
	 SimpleRefHolder<QoreStringNode> err(MM.parseLoadModule((*i).c_str()));
	 if (err) {
	    printf("cannot load '%s': %s\n", (*i).c_str(), err->getBuffer());
	    mod_errs = true;
	 }
      }
      
      cl_mod_list.clear();
      if (mod_errs) {
	 printf("please fix the errors listed above and try again.\n");
	 rc = 2;
	 goto exit;
      }
      
      // set the parse options
      qpgm->parseSetParseOptions(parse_options);
      
      // lock the parse options if necessary
      if (lock_options)
	 qpgm->lockOptions();
      
      // parse immediate argument if any
      if (eval_arg) {
	 QoreString str("printf(\"%N\n\", ");
	 str.concat(eval_arg);
	 str.concat(");");
	 qpgm->parse(str.getBuffer(), "<command-line>", &xsink, &wsink, warnings);
      }
      else  {
	 // set for program class execution if "exec_class" is set
	 if (exec_class) {
	    if (exec_class_name)
	       qpgm->setExecClass(exec_class_name);
	    else if (program_file_name) {
	       char *cn = make_class_name(program_file_name);
	       qpgm->setExecClass(cn);
	       free(cn);
	    }
	    else {
	       fprintf(stderr, "error, missing class name to instantiate as application\n");
	       rc = 1;
	       goto exit;
	    }
	 }
	 
	 // parse the program
	 if (cl_pgm)
	    qpgm->parse(cl_pgm, "<command-line>", &xsink, &wsink, warnings);
	 else if (program_file_name) {
	    qpgm->parseFile(program_file_name, &xsink, &wsink, warnings);
	    free(program_file_name);
	 }
	 else
	    qpgm->parse(stdin, "<stdin>", &xsink, &wsink, warnings);
      }

      // display any warnings now
      if (wsink.isException()) {	 
	 wsink.handleWarnings();
	 if (warnings_are_errors && !xsink.isException()) {
	    printf("exiting due to the above warnings...\n");
	    rc = 2; // set return code to 2 if there were parse warnings to be treated as errors
	    goto exit;
	 }
      }
      
      // if there were no parse exceptions, execute the program
      if (!xsink.isException()) {	 
	 {
	    // execute the program and get the return value
	    AbstractQoreNode *rv = qpgm->run(&xsink);
	    // set the return code for this program from the core returned by the Qore program
	    rc = rv ? rv->getAsInt() : 0;
	    discard(rv, &xsink);
	 }
	 
	 // if there is any unhandled exception, set the return code to 3
	 if (xsink.isException())
	    rc = 3;
      }
      else // set return code to 2 if there were parse errors
	 rc = 2;

      // run the default exception handler on any unhandled exceptions in the primary thread or during parsing
      xsink.handleExceptions();

exit:
      ;
   }
   // run the default exception handler on any unhandled exceptions if necessary (again)
   // -- exceptions could have been thrown in the QoreProgram object's destructor
   xsink.handleExceptions();
   
   // cleanup Qore subsystem (deallocate memory, etc)
   qore_cleanup();

   return rc;
}
