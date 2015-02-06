/*
  main.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#include "command-line.h"

#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <strings.h>

#include <map>

extern int64 parse_options;
extern int warnings, qore_lib_options;
extern const char *def_charset;
extern const char *cl_pgm, *exec_class_name, *eval_arg, *cmd_zone;
extern bool show_mod_errs, lock_options, exec_class, warnings_are_errors, only_first_except;
extern qore_license_t license;
// define map type
typedef std::map<std::string, std::string> defmap_t;
// parse define map
extern defmap_t defmap;

int main(int argc, char *argv[]) {   
   int rc = 0;

   // parse the command line
   char *program_file_name = parse_command_line(argc, argv);

   // initialize Qore subsystem
   qore_init(license, def_charset, show_mod_errs, qore_lib_options);

   ExceptionSink wsink, xsink;
   {
      QoreProgramHelper qpgm(parse_options, xsink);

      // set parse defines
      for (defmap_t::iterator i = defmap.begin(), e = defmap.end(); i != e; ++i)
	 qpgm->parseDefine(i->first.c_str(), i->second.c_str());

      // load any modules requested on the command-line
      bool mod_errs = false;
      for (cl_mod_list_t::iterator i = cl_mod_list.begin(), e = cl_mod_list.end(); i != e; ++i) {
	 // display any error messages
	 SimpleRefHolder<QoreStringNode> err(MM.parseLoadModule((*i).c_str(), *qpgm));
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

      // set time zone if requested
      if (cmd_zone)
	 qpgm->parseSetTimeZone(cmd_zone);
      
      // lock the parse options if necessary
      if (lock_options)
	 qpgm->lockOptions();
      
      // parse immediate argument if any
      if (eval_arg) {
	 QoreString str("printf(\"%N\n\", (");
	 str.concat(eval_arg);
	 str.concat("));");
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
	    qpgm->parseFile(program_file_name, &xsink, &wsink, warnings, only_first_except);
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
