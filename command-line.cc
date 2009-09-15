/*
  command-line.cc

  Qore Programming Language

  this whole file comes out of a very old getopt-stype implementation by me

  it should offer POSIX style command-line handling on any platform...

  Copyright 2003 - 2009 David Nichols
*/

#include <qore/Qore.h>
#include <qore/ParseOptionMap.h>

#include "command-line.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <ctype.h>

// license type for initializing the library
qore_license_t license = QL_GPL;

// list of modules to load after library initialization
cl_mod_list_t cl_mod_list;

// global parse_option
int parse_options = PO_DEFAULT;
int warnings = 0;
int qore_lib_options = QLO_NONE;

// lock options
bool lock_options = false;

// command-line specified default character set
const char *def_charset = 0;

// classname to instantiate as program
const char *exec_class_name = 0;

// show module errors
bool show_mod_errs = false;

// execute class
bool exec_class = false;

// treat warnings as errors
bool warnings_are_errors = false;

// program text given on the command-line
const char *cl_pgm = 0;

// argument to evaluate given on the command-line
const char *eval_arg = 0;

// program name
static char *pn;

static int opt_errors = 0;

static const char usage[] = "usage: %s [option(s)]... [program file]\n";
static const char suggest[] = "try '%s -h' for more information.\n";

static const char helpstr[] = 
   "  -a, --show-aliases           displays the list of character sets aliases\n"
   "  -b, --disable-signals        disables signal handling\n"
   "  -c, --charset=arg            sets default character set encoding\n"
   "  -e, --exec=arg               execute program given on command-line\n"
   "  -h, --help                   shows this help text and exit\n"
   "  -i, --list-warnings          list all warnings and quit\n"
   "  -l, --load=arg               load module 'arg' immediately\n"
   "      --lgpl                   sets the library's license flag to LGPL,\n"
   "                               meaning that GPL modules cannot be loaded\n"
   "  -m, --show-module-errors     shows error messages related to loading and\n"
   "                               initializing qore modules\n"
   "      --module-dir             show qore module directory and exit\n"
   "      --module-api             show compatible qore module API version and\n"
   "                               exit\n"
   "      --module-apis            show all qore module API versions\n"
   "      --latest-module-api      show most recent module API version and exit\n"
   "  -o, --list-parse-options     list all parse options\n"
   "  -p, --set-parse-option=arg   set parse option (ex: -pno-database)\n"
   "  -r, --warnings-are-errors    treat warnings as errors\n"
   "  -s, --show-charsets          displays known character encodings\n"
   "  -V, --version                show program version information and quit\n"
   "      --short-version          show short version information and quit\n"
   "  -W, --enable-all-warnings    turn on all warnings (recommended)\n"
   "  -w, --enable-warning=arg     turn on warning given by argument\n"
   "  -x, --exec-class[=arg]       instantiate class with same name as file name\n"
   "                               (override with arg, also sets --no-top-level)\n"
   "  -X, --eval=arg               evaluates argument and displays result\n"
   "\n"
   " PARSE OPTIONS:\n"
   "  -H, --parse-option-help      display options controlling parse options\n";

static const char parseopts[] =    "qore options controlling parse options:\n"
   "  -o, --list-parse-options     list all parse options\n"
   "  -p, --set-parse-option=arg   set parse option (ex: -pno-database)\n"
   "\n PARSE OPTIONS:\n"
   "  -A, --lock-warnings          do not allow changes in warning levels\n"
   "      --no-class-defs          make class definitions illegal\n"
   "  -D, --no-database            disallow access to database functionality\n"
   "  -E, --no-external-process    make access to external processes illegal\n"
   "  -F, --no-filesystem          disallow access to the local filesystem\n"
   "  -G, --no-global-vars         make global variable definitions illegal\n"
   "      --no-gui                 do not allow access to GUI functionality\n"
   "  -I, --no-child-restrictions  do not restrict subprograms' parse options\n"
   "      --no-constant-defs       make constant definitions illegal\n"
   "  -K, --lock-options           disable changes to parse options in program\n"
   "  -L, --no-top-level           make top-level statements illegal\n"
   "  -M, --no-namespace-defs      make namespace declarations illegal\n"
   "  -N, --no-new                 make using the 'new' operator illegal\n"
   "  -O, --require-our            require 'our' with global variables (recommended)\n"
   "  -P, --no-process-control     make process control illegal (fork(), exit(), etc)\n"
   "      --no-terminal-io         do not allow access to the text terminal\n"
   "  -R, --no-thread-control      make thread control operations illegal\n"
   "  -S, --no-subroutine-defs     make subroutine definitions illegal\n"
   "  -T, --no-threads             disallow thread access and control\n"
   "      --no-thread-classes      disallow access to thread classes\n" 
   "  -Y, --no-network             disallow access to the network\n";

static const char debugstr[] = "\n DEBUGGING OPTIONS:\n"
   "  -d, --debug=arg              sets debugging level (higher number = more output)\n"
   "  -t, --trace                  turns on function tracing\n" 
   ;

static inline void show_usage() {
   printf(usage, pn);
}

static void show_parse_option_help(const char *arg) {
   printf("%s\n", parseopts);
   exit(0);
}

static void do_debug(const char *arg) {
   debug = atoi(arg);
}

static void set_eval_arg(const char *arg) {
   eval_arg = arg;
}

static void do_trace(const char *arg) {
   qore_trace = 1;
}

static void show_module_dir(const char *arg) {
   printf("%s\n", qore_module_dir);
   exit(0);
}

static void show_module_api(const char *arg) {
   printf("%d.%d\n", qore_mod_api_list[qore_mod_api_list_len - 1].major, qore_mod_api_list[qore_mod_api_list_len - 1].minor);
   exit(0);
}

static void show_module_apis(const char *arg) {
   // show all module apis
   printf("%d.%d", qore_mod_api_list[0].major, qore_mod_api_list[0].minor);
   for (unsigned i = 1; i < qore_mod_api_list_len; ++i)
      printf(", %d.%d", qore_mod_api_list[i].major, qore_mod_api_list[i].minor);
   printf("\n");

   exit(0);
}

static void show_latest_module_api(const char *arg) {
   printf("%d.%d\n", qore_mod_api_list[0].major, qore_mod_api_list[0].minor);
   exit(0);
}

static void set_parse_option(const char *arg) {
   int code = ParseOptionMap::find_code(arg);
   if (code == -1) {
      printf("unknown parse option '%s', use -L or --list-parse-options\n", arg);
      exit(1);
   }
   parse_options |= code;
}

static void list_parse_options(const char *arg) {
   ParseOptionMap::list_options();
   exit(0);
}

static void do_help(const char *arg) {
   show_usage();
   puts(helpstr);
   if (qore_has_debug())
      puts(debugstr);
   exit(0);
}

static void disable_signals(const char *arg) {
   qore_lib_options |= QLO_DISABLE_SIGNAL_HANDLING;
}

static void load_module(const char *arg) {
   cl_mod_list.push_back(arg);
}

static void warn_to_err(const char *arg) {
   warnings_are_errors = true;
}

static void enable_warnings(const char *arg) {
   warnings = -1;
}

static void enable_warning(const char *arg) {
   int code = get_warning_code(arg);
   if (!code) {
      printf("cannot enable unknown warning '%s'\n", arg);
      exit(1);
   }
   warnings |= code;
}

static void list_warnings(const char *arg) {
   for (unsigned i = 0; i < qore_num_warnings; i++)
      printf("%s\n", qore_warnings[i]);
   exit(0);
}

static void do_no_terminal_io(const char *arg) {
   parse_options |= PO_NO_TERMINAL_IO;
}

static void do_no_gui(const char *arg) {
   parse_options |= PO_NO_GUI;
}

static void do_no_database(const char *arg) {
   parse_options |= PO_NO_DATABASE;
}

static void do_lock_warnings(const char *arg) {
   parse_options |= PO_LOCK_WARNINGS;
}

static void do_no_global_vars(const char *arg) {
   parse_options |= PO_NO_GLOBAL_VARS;
}

static void do_no_subroutine_defs(const char *arg) {
   parse_options |= PO_NO_SUBROUTINE_DEFS;
}

static void do_no_network(const char *arg) {
   parse_options |= PO_NO_NETWORK;
}

static void do_no_threads(const char *arg) {
   parse_options |= PO_NO_THREADS;
}

static void do_no_thread_control(const char *arg) {
   parse_options |= PO_NO_THREAD_CONTROL;
}

static void do_no_thread_classes(const char *arg) {
   parse_options |= PO_NO_THREAD_CLASSES;
}

static void do_no_top_level(const char *arg) {
   parse_options |= PO_NO_TOP_LEVEL_STATEMENTS;
}

static void do_no_class_defs(const char *arg) {
   parse_options |= PO_NO_CLASS_DEFS;
}

static void do_no_namespace_defs(const char *arg) {
   parse_options |= PO_NO_NAMESPACE_DEFS;
}

static void do_no_constant_defs(const char *arg) {
   parse_options |= PO_NO_CONSTANT_DEFS;
}

static void do_no_filesystem(const char *arg) {
   parse_options |= PO_NO_FILESYSTEM;
}

static void do_no_new(const char *arg) {
   parse_options |= PO_NO_NEW;
}

static void do_no_child_po_restrictions(const char *arg) {
   parse_options |= PO_NO_CHILD_PO_RESTRICTIONS;
}

static void do_no_external_process(const char *arg) {
   parse_options |= PO_NO_EXTERNAL_PROCESS;
}

static void do_no_process_control(const char *arg) {
   parse_options |= PO_NO_PROCESS_CONTROL;
}

static void do_require_our(const char *arg) {
   parse_options |= PO_REQUIRE_OUR;
}

static void do_lock_options(const char *arg) {
   lock_options = true;
}

static void short_version(const char *arg) {
   printf("%s\n", qore_version_string);
   exit(0);
}

static const char *tlist[] = { "OPTION", "ALGORITHM", "FUNCTION", "UNKNOWN" };

static void do_version(const char *arg) {
   printf("QORE for %s %s (%d-bit build), Copyright (C) 2003 - 2009 David Nichols\n", qore_target_os, qore_target_arch, qore_target_bits);

   printf("version %s", qore_version_string);
   FeatureList::iterator i = qoreFeatureList.begin();
   if (i != qoreFeatureList.end()) {
      printf(" (builtin features: ");
      while (i != qoreFeatureList.end()) {
	 fputs((*i).c_str(), stdout);
	 i++;
	 if (i != qoreFeatureList.end())
	    printf(", ");
      }
      putchar(')');
   }

   // show module api and compatible module apis
   printf("\n  module API: %d.%d", qore_mod_api_list[0].major, qore_mod_api_list[0].minor);
   if (qore_mod_api_list_len == 1)
       printf("\n");
   else {
       printf(" (");
       for (unsigned i = 1; i < qore_mod_api_list_len; ++i) {
	   printf("%d.%d", qore_mod_api_list[i].major, qore_mod_api_list[i].minor);
	   if (i != (qore_mod_api_list_len - 1))
	       printf(", ");
       }
       printf(")\n");
   }

   printf("  build host: %s\n  C++ compiler: %s\n  CFLAGS: %s\n  LDFLAGS: %s\n", 
	  qore_build_host, qore_cplusplus_compiler, qore_cflags, qore_ldflags);

   printf("this build has options:\n");
   // find longest option name
   int len = 0;
   for (size_t j = 0; j < qore_option_list_size; ++j) {
      int i = strlen(qore_option_list[j].option);
      if (i > len)
	 len = i;
   }
   // create format string
   QoreString fmt(" %9s %-");
   fmt.sprintf("%d", len + 1);
   fmt.concat("s = %s\n");

   for (unsigned j = 0; j < qore_option_list_size; ++j) {
      int type = qore_option_list[j].type;
      if (type > QO_FUNCTION)
	 type = QO_FUNCTION + 1;
      printf(fmt.getBuffer(), tlist[type], qore_option_list[j].option, 
             qore_option_list[j].value ? "true" : "false");   
   }
   exit(0);
}

static void set_charset(const char *arg) {
   def_charset = arg;
}

static void show_charsets(const char *arg) {
   QEM.showEncodings();
   exit(0);
}

static void show_charset_aliases(const char *arg) {
   QEM.showAliases();
   exit(0);
}

static void set_exec(const char *arg) {
   cl_pgm = arg;
}

static void show_module_errors(const char *arg) {
   show_mod_errs = true;
}

static void do_exec_class(const char *arg) {
   //printf("do_exec_class(%s)\n", arg);
   exec_class = true;
   exec_class_name = arg;
   parse_options |= PO_NO_TOP_LEVEL_STATEMENTS;
}

static void set_lgpl(const char *arg) {
   license = QL_LGPL;
}

#define ARG_NONE 0
#define ARG_MAND 1
#define ARG_OPT  2

static struct opt_struct_s {
      char short_opt;
      const char *long_opt;
      int arg;
      void (*opt_func)(const char *arg);
} options[] = {
   { 'a', "show-aliases",          ARG_NONE, show_charset_aliases },
   { 'c', "charset",               ARG_MAND, set_charset },
   { 'e', "exec",                  ARG_MAND, set_exec },
   { 'h', "help",                  ARG_NONE, do_help },
   { 'i', "list-warnings",         ARG_NONE, list_warnings },
   { 'l', "load",                  ARG_MAND, load_module },
   { 'm', "show-module-errors",    ARG_NONE, show_module_errors },
   { 'o', "list-parse-options",    ARG_NONE, list_parse_options },
   { 'p', "set-parse-option",      ARG_MAND, set_parse_option },
   { 's', "show-charsets",         ARG_NONE, show_charsets },
   { 'r', "warnings-are-errors",   ARG_NONE, warn_to_err },
   { 'w', "enable-warning",        ARG_MAND, enable_warning },
   { 'x', "exec-class",            ARG_OPT,  do_exec_class },
   { 'A', "lock-warnings",         ARG_NONE, do_lock_warnings },
   { '\0', "no-class-defs",         ARG_NONE, do_no_class_defs },
   { 'D', "no-database",           ARG_NONE, do_no_database },
   { 'E', "no-external-process",   ARG_NONE, do_no_external_process },
   { 'F', "no-filesystem",         ARG_NONE, do_no_filesystem },
   { 'G', "no-global-vars",        ARG_NONE, do_no_global_vars },
   { 'H', "parse-option-help",     ARG_NONE, show_parse_option_help },
   { 'I', "no-child-restrictions", ARG_NONE, do_no_child_po_restrictions },
   { '\0', "no-constant-defs",      ARG_NONE, do_no_constant_defs },
   { 'K', "lock-options",          ARG_NONE, do_lock_options },
   { 'L', "no-top-level",          ARG_NONE, do_no_top_level },
   { 'M', "no-namespace-defs",     ARG_NONE, do_no_namespace_defs },
   { 'N', "no-new",                ARG_NONE, do_no_new },
   { 'O', "require-our",           ARG_NONE, do_require_our  },
   { 'P', "no-process-control",    ARG_NONE, do_no_process_control },
   { 'R', "no-thread-control",     ARG_NONE, do_no_thread_control },
   { 'S', "no-subroutine-defs",    ARG_NONE, do_no_subroutine_defs },
   { 'T', "no-threads",            ARG_NONE, do_no_threads },
   { 'V', "version",               ARG_NONE, do_version },
   { 'W', "enable-all-warnings",   ARG_NONE, enable_warnings },
   { '\0', "no-thread-classes",     ARG_NONE, do_no_thread_classes },
   { 'Y', "no-network",            ARG_NONE, do_no_network },
   { '\0', "no-terminal-io",       ARG_NONE, do_no_terminal_io },
   { '\0', "no-gui",               ARG_NONE, do_no_gui },
   { '\0', "lgpl",                 ARG_NONE, set_lgpl },
   { '\0', "module-dir",           ARG_NONE, show_module_dir },
   { '\0', "short-version",        ARG_NONE, short_version },
   { '\0', "module-api",           ARG_NONE, show_module_api },
   { '\0', "module-apis",          ARG_NONE, show_module_apis },
   { '\0', "latest-module-api",    ARG_NONE, show_latest_module_api },
   { 'X', "eval",                  ARG_MAND, set_eval_arg },
// debugging options
   { 'b', "disable-signals",       ARG_NONE, disable_signals },
   { 'd', "debug",                 ARG_MAND, do_debug },
   { 't', "trace",                 ARG_NONE, do_trace },
};

#define NUM_OPTS (sizeof(options) / sizeof(struct opt_struct_s))

static inline void missing_char_option(int i)
{
   printe("option '%c' requires an argument.\n", options[i].short_opt);
   opt_errors++;
}

static inline void missing_str_option(int i)
{
   printe("option '%s' requires an argument.\n", options[i].long_opt);
   opt_errors++;
}

static inline void excess_option(int i)
{
   printe("option '%s' does not take an argument.\n", options[i].long_opt);
   opt_errors++;
}

static inline void invalid_option(char *opt)
{
   printe("error: '--%s' is not a valid long option.\n", opt);
   opt_errors++;
}

static inline void invalid_option(char opt)
{
   printe("error: '-%c' is not a valid short option.\n", opt);
   opt_errors++;
}

// *i is the argument position, *j is the index in the string
static const char *get_arg(char *argv[], unsigned *i, unsigned *j, unsigned argc) {
   if (*i >= argc)
      return 0;
   // if next character is an assignment character, then advance character pointer
   if (is_assign_char(argv[*i][*j]))
      (*j)++;
   // if at end of argument string, try next one
   if (!argv[*i][*j])
   {
      // increment argument pointer
      (*i)++;
      // set character pointer to first character
      (*j) = 0;
      // if there are no more strings then return 0!
      if ((*i) == argc)
	 return 0;
   }
   return &argv[*i][*j];
}

static void process_str_opt(char *argv[], unsigned *i, unsigned j, unsigned argc);

// *i is the argument position, *j is the index in the string
static int process_char_opt(char *argv[], unsigned *i, unsigned *j, unsigned argc) {
   unsigned x;

   if (isblank(argv[*i][*j])) {
      do 
	 (*j)++;
      while (isblank(argv[*i][*j]));
      if (argv[*i][*j] == '-') {
	 if (argv[*i][*j] == '-') {
	    process_str_opt(argv, i, *j + 1, argc);
	    return 1;
	 }
	 else
	    (*j)++;
      }
   }

   char c = argv[*i][*j];

   for (x = 0; x < NUM_OPTS; x++)
      if (options[x].short_opt == c) {
	 //printf("found '%c' %s (%d)\n", c, options[x].long_opt, options[x].arg);
	 if (options[x].arg == ARG_MAND || 
	     (options[x].arg == ARG_OPT && (argv[*i][(*j) + 1] == '='))) {
	    const char *arg;

	    // increment string index
	    (*j)++;
	    if (!(arg = get_arg(argv, i, j, argc)))
	       missing_char_option(x);
	    else
	       options[x].opt_func(arg);
	    /* as the argument pointer always advances to the next argument
	     * due to the get_arg() function, return 1 to break out of the
	     * character loop for the current arg */
	    return 1;
	 }
	 options[x].opt_func(0);
	 return 0;
      }
   // if the option is not present, then raise an error
   invalid_option(c);
   return 0;
}

// *i is the argument position
static void process_str_opt(char *argv[], unsigned *i, unsigned j, unsigned argc)
{
   unsigned x, option_present = 0;
   char *opt = &argv[*i][j];

   // find option string (left side of string if there is an assignment char)
   for (x = 2; x < strlen(argv[*i]); x++) {
      if (is_assign_char(argv[*i][x])) {
	 option_present = x + 1;
	 opt = (char *)malloc(sizeof(char) * (x - 1));
	 strncpy(opt, &argv[*i][2], x - 2);
	 opt[x - 2] = '\0';
	 break;
      }
   }
   // if the option is not in the same argument, then increment the argument pointer
   if (!option_present)
      ++(*i);

   for (x = 0; x < NUM_OPTS; x++) {
      if (!strcmp(options[x].long_opt, opt)) {
	 if (!options[x].arg) {
	    if (option_present)
	       excess_option(x);
	    else
	       --(*i);
	    options[x].opt_func(0);
	 }
	 else if (options[x].arg == ARG_OPT) {
	    const char *arg = 0;

	    if (option_present)
	       arg = get_arg(argv, i, &option_present, argc);
	    else
	       --(*i);
	    options[x].opt_func(arg);
	 }
	 else {
	    const char *arg;

	    if (!(arg = get_arg(argv, i, &option_present, argc)))
	       missing_str_option(x);
	    else
	       options[x].opt_func(arg);
	 }
	 break;
      }
   }

   // if the option is not present, then raise an error
   if (x == NUM_OPTS)
      invalid_option(opt);

   if (option_present)
      free(opt);
}

// returns either 0 or a string that must be freed with free()
// also sets up the global ARGV argument list
char *parse_command_line(unsigned argc, char *argv[])
{
   pn = basename(argv[0]);

   // file name to return, if any
   char *fn = 0;

   unsigned i = 1;

   // check all arguments
   for (; i < argc; i++) {
      printd(5, "parse_command_line() %d/%d=%s\n", i, argc, argv[i]);
      if (argv[i][0] == '-') {
	 if (!argv[i][1]) {
	    i++;
	    break;
	 }
	 else {
	    if (argv[i][1] == '-') {
	       if (!argv[i][2]) {
		  i++;
		  break;
	       }
	       process_str_opt(argv, &i, 2, argc);
	    }
	    else {
	       unsigned j;

	       for (j = 1; j < strlen(argv[i]); j++)		  
		  if (process_char_opt(argv, &i, &j, argc))
		     break;
	    }
	 }
      }
      else {
	 // only set the file name if the --exec option has not been set
	 if (!cl_pgm)
	    fn = strdup(argv[i++]);
	 break;
      }
   }

// pvanek - argc check removed, just because the i value is checked
// directly in qore_setup_argv(). ARGV and mainly QORE_ARGV should
// be set in any case.
   qore_setup_argv(i, argc, argv);

   if (opt_errors) {
      printe(suggest, pn);
      exit(1);
   }
   return fn;
}
