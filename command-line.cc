/*
  command-line.cc

  Qore Programming Language

  this whole file comes out of a very old getopt-stype implementation by me

  it should offer POSIX style command-line handling on any platform...

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
*/

#include <qore/Qore.h>
#include <qore/QoreWarnings.h>

#include "command-line.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <ctype.h>

// list of modules to load after library initialization
cl_mod_list_t cl_mod_list;

// global parse_option
int parse_options = 0;
int warnings = 0;
int qore_lib_options = QLO_NONE;

// lock options
bool lock_options = false;

// command-line specified default character set
char *def_charset = NULL;

// classname to instantiate as program
char *exec_class_name = NULL;

// show module errors
bool show_mod_errs = false;

// execute class
bool exec_class = false;

// treat warnings as errors
bool warnings_are_errors = false;

// program text given on the command-line
char *cl_pgm = NULL;

// program name
static char *pn;

static int opt_errors = 0;

static char usage[] = "usage: %s [option(s)]... [program file]\n";
static char suggest[] = "try '%s -h' for more information.\n";

static char helpstr[] = 
"  -a, --show-aliases           displays the list of character sets aliases\n"
"  -c, --charset=arg            sets default character set encoding\n"
"  -e, --exec=arg               execute program given on command-line\n"
"  -h, --help                   shows this help text\n"
"  -i, --list-warnings          list all warnings and quit\n"
"  -l, --load=arg               load module 'arg' immediately\n"
"  -m, --show-module-errors     show error messages related to loading and\n"
"                               initializing qore modules\n"
"  -r, --warnings-are-errors    treat warnings as errors\n"
"  -s, --show-charsets          displays the list of known character sets\n"
"  -V, --version                show program version information and quit\n"
"  -W, --enable-all-warnings    turn on all warnings (recommended)\n"
"  -w, --enable-warning=arg     turn on warning given by argument\n"
"  -x, --exec-class[=arg]       instantiate class with same name as file name\n"
"                               (override with arg, also sets --no-top-level)\n"
"\n"
" PARSE OPTIONS:\n"
"  -A, --lock-warnings          do not allow changes in warning levels\n"
"  -C, --no-class-defs          make class definitions illegal\n"
"  -D, --no-database            disallow access to database functionality\n"
"  -E, --no-external-process    make access to external processes illegal\n"
"  -F, --no-constant-defs       make constant definitions illegal\n"
"  -G, --no-global-vars         make global variable definitions illegal\n"
"  -I, --no-child-restrictions  do not restrict subprograms' parse options\n"
"  -K, --lock-options           disable changes to parse options in program\n"
"  -L, --no-top-level           make top-level statements illegal\n"
"  -M, --no-namespace-defs      make namespace declarations illegal\n"
"  -N, --no-new                 make using the 'new' operator illegal\n"
"  -O, --require-our            require 'our' with global variables (recommended)\n"
"  -P, --no-process-control     make process control illegal (fork(), exit(), etc)\n"
"  -R, --no-thread-control      make thread control operations illegal\n"
"  -S, --no-subroutine-defs     make subroutine definitions illegal\n"
"  -T, --no-threads             disallow thread access and control\n"
"  -X, --no-thread-classes      disallow access to thread classes\n" 
#ifdef DEBUG
"\n DEBUGGING OPTIONS:\n"
"  -d, --debug=arg              sets debugging level (higher number = more output)\n"
"  -t, --trace                  turns on function tracing\n" 
"  -b, --disable-signals        disables signal handling\n"
#endif
;

static inline void show_usage()
{
   printf(usage, pn);
}

#ifdef DEBUG
static void do_debug(char *arg)
{
   debug = atoi(arg);
}

static void do_trace(char *arg)
{
   qore_trace = 1;
}
#endif

static void do_help(char *arg)
{
   show_usage();
   printf(helpstr);
   exit(0);
}

static void disable_signals(char *arg)
{
   qore_lib_options |= QLO_DISABLE_SIGNAL_HANDLING;
}

static void load_module(char *arg)
{
   cl_mod_list.push_back(arg);
}

static void warn_to_err(char *arg)
{
   warnings_are_errors = true;
}

static void enable_warnings(char *arg)
{
   warnings = -1;
}

static void enable_warning(char *arg)
{
   int code = get_warning_code(arg);
   if (!code)
   {
      printf("cannot enable unknown warning '%s'\n", arg);
      exit(1);
   }
   warnings |= code;
}

static void list_warnings(char *arg)
{
   for (unsigned i = 0; i < qore_num_warnings; i++)
      printf("%s\n", qore_warnings[i]);
   exit(0);
}

static void do_no_database(char *arg)
{
   parse_options |= PO_NO_DATABASE;
}

static void do_lock_warnings(char *arg)
{
   parse_options |= PO_LOCK_WARNINGS;
}

static void do_no_global_vars(char *arg)
{
   parse_options |= PO_NO_GLOBAL_VARS;
}

static void do_no_subroutine_defs(char *arg)
{
   parse_options |= PO_NO_SUBROUTINE_DEFS;
}

static void do_no_threads(char *arg)
{
   parse_options |= PO_NO_THREADS;
}

static void do_no_thread_control(char *arg)
{
   parse_options |= PO_NO_THREAD_CONTROL;
}

static void do_no_thread_classes(char *arg)
{
   parse_options |= PO_NO_THREAD_CLASSES;
}

static void do_no_top_level(char *arg)
{
   parse_options |= PO_NO_TOP_LEVEL_STATEMENTS;
}

static void do_no_class_defs(char *arg)
{
   parse_options |= PO_NO_CLASS_DEFS;
}

static void do_no_namespace_defs(char *arg)
{
   parse_options |= PO_NO_NAMESPACE_DEFS;
}

static void do_no_constant_defs(char *arg)
{
   parse_options |= PO_NO_CONSTANT_DEFS;
}

static void do_no_new(char *arg)
{
   parse_options |= PO_NO_NEW;
}

static void do_no_child_po_restrictions(char *arg)
{
   parse_options |= PO_NO_CHILD_PO_RESTRICTIONS;
}

static void do_no_external_process(char *arg)
{
   parse_options |= PO_NO_EXTERNAL_PROCESS;
}

static void do_no_process_control(char *arg)
{
   parse_options |= PO_NO_PROCESS_CONTROL;
}

static void do_require_our(char *arg)
{
   parse_options |= PO_REQUIRE_OUR;
}

static void do_lock_options(char *arg)
{
   lock_options = true;
}

static void do_version(char *arg)
{
   printf("QORE for %s %s (%d-bit build), Copyright (C) 2003 - 2007 David Nichols\nversion %s", qore_target_os, qore_target_arch, qore_target_bits, qore_version_string);

   charPtrList::iterator i = qoreFeatureList.begin();
   if (i != qoreFeatureList.end())
   {
      printf(" (builtin features: ");
      while (i != qoreFeatureList.end())
      {
	 printf(*i);
	 i++;
	 if (i != qoreFeatureList.end())
	    printf(", ");
      }
      putchar(')');
   }
   putchar('\n');
   exit(0);
}

static void set_charset(char *arg)
{
   def_charset = strdup(arg);
}

static void show_charsets(char *arg)
{
   QEM.showEncodings();
   exit(0);
}

static void show_charset_aliases(char *arg)
{
   QEM.showAliases();
   exit(0);
}

static void set_exec(char *arg)
{
   cl_pgm = arg;
}

static void show_module_errors(char *arg)
{
   show_mod_errs = true;
}

static void do_exec_class(char *arg)
{
   //printf("do_exec_class(%s)\n", arg);
   exec_class = true;
   exec_class_name = arg;
   parse_options |= PO_NO_TOP_LEVEL_STATEMENTS;
}

#define ARG_NONE 0
#define ARG_MAND 1
#define ARG_OPT  2

static struct opt_struct_s {
      char short_opt;
      char *long_opt;
      int arg;
      void (*opt_func)(char *arg);
} options[] = {
   { 'a', "show-aliases",          ARG_NONE, show_charset_aliases },
   { 'c', "charset",               ARG_MAND, set_charset },
   { 'e', "exec",                  ARG_MAND, set_exec },
   { 'h', "help",                  ARG_NONE, do_help },
   { 'i', "list-warnings",         ARG_NONE, list_warnings },
   { 'l', "load",                  ARG_MAND, load_module },
   { 'm', "show-module-errors",    ARG_NONE, show_module_errors },
   { 's', "show-charsets",         ARG_NONE, show_charsets },
   { 'r', "warnings-are-errors",   ARG_NONE, warn_to_err },
   { 'w', "enable-warning",        ARG_MAND, enable_warning },
   { 'x', "exec-class",            ARG_OPT,  do_exec_class },
   { 'A', "lock-warnings",         ARG_NONE, do_lock_warnings },
   { 'C', "no-class-defs",         ARG_NONE, do_no_class_defs },
   { 'D', "no-database",           ARG_NONE, do_no_database },
   { 'E', "no-external-process",   ARG_NONE, do_no_external_process },
   { 'F', "no-constant-defs",      ARG_NONE, do_no_constant_defs },
   { 'G', "no-global-vars",        ARG_NONE, do_no_global_vars },
   { 'I', "no-child-restrictions", ARG_NONE, do_no_child_po_restrictions },
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
   { 'X', "no-thread-classes",     ARG_NONE, do_no_thread_classes },
#ifdef DEBUG
   { 'b', "disable-signals",       ARG_NONE, disable_signals },
   { 'd', "debug",                 ARG_MAND, do_debug },
   { 't', "trace",                 ARG_NONE, do_trace },
#endif
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
static char *get_arg(char *argv[], unsigned *i, unsigned *j, unsigned argc)
{
   if (*i >= argc)
      return NULL;
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
      // if there are no more strings then return NULL!
      if ((*i) == argc)
	 return NULL;
   }
   return &argv[*i][*j];
}

static void process_str_opt(char *argv[], unsigned *i, unsigned j, unsigned argc);

// *i is the argument position, *j is the index in the string
static int process_char_opt(char *argv[], unsigned *i, unsigned *j, unsigned argc)
{
   unsigned x;

   if (isblank(argv[*i][*j]))
   {
      do 
	 (*j)++;
      while (isblank(argv[*i][*j]));
      if (argv[*i][*j] == '-')
	 if (argv[*i][*j] == '-')
	 {
	    process_str_opt(argv, i, *j + 1, argc);
	    return 1;
	 }
	 else
	    (*j)++;
   }

   char c = argv[*i][*j];

   for (x = 0; x < NUM_OPTS; x++)
      if (options[x].short_opt == c)
      {
	 //printf("found '%c' %s (%d)\n", c, options[x].long_opt, options[x].arg);
	 if (options[x].arg == ARG_MAND || 
	     (options[x].arg == ARG_OPT && (argv[*i][(*j) + 1] == '=')))
	 {
	    char *arg;

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
	 options[x].opt_func(NULL);
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
   for (x = 2; x < strlen(argv[*i]); x++)
   {
      if (is_assign_char(argv[*i][x]))
      {
	 option_present = x + 1;
	 opt = (char *)malloc(sizeof(char) * (x - 1));
	 strncpy(opt, &argv[*i][2], x - 2);
	 opt[x - 2] = '\0';
	 break;
      }
   }
   // if the option is not in the same argument, then increment the argument pointer
   if (!option_present)
      (*i)++;

   for (x = 0; x < NUM_OPTS; x++)
      if (!strcmp(options[x].long_opt, opt))
      {
	 if (!options[x].arg)
	 {
	    if (option_present)
	       excess_option(x);
	    options[x].opt_func(NULL);
	 }
	 else if (options[x].arg == ARG_OPT)
	 {
	    char *arg = NULL;

	    if (option_present)
	       arg = get_arg(argv, i, &option_present, argc);
	    else
	       (*i)--;
	    options[x].opt_func(arg);
	 }
	 else
	 {
	    char *arg;

	    if (!(arg = get_arg(argv, i, &option_present, argc)))
	       missing_str_option(x);
	    else
	       options[x].opt_func(arg);
	 }
	 break;
      }

   // if the option is not present, then raise an error
   if (x == NUM_OPTS)
      invalid_option(opt);

   if (option_present)
      free(opt);
}

// returns either NULL or a string that must be freed with free()
// also sets up the global ARGV argument list
char *parse_command_line(unsigned argc, char *argv[])
{
   pn = basename(argv[0]);

   // file name to return, if any
   char *fn = NULL;

   unsigned i = 1;

   // check all arguments
   for (; i < argc; i++)
   {
      printd(5, "parse_command_line() %d/%d=%s\n", i, argc, argv[i]);
      if (argv[i][0] == '-')
      {
	 if (!argv[i][1])
	 {
	    i++;
	    break;
	 }
	 else
	 {
	    if (argv[i][1] == '-')
	    {
	       if (!argv[i][2])
	       {
		  i++;
		  break;
	       }
	       process_str_opt(argv, &i, 2, argc);
	    }
	    else
	    {
	       unsigned j;

	       for (j = 1; j < strlen(argv[i]); j++)		  
		  if (process_char_opt(argv, &i, &j, argc))
		     break;
	    }
	 }
      }
      else
      {
	 // only set the file name if the --exec option has not been set
	 if (!cl_pgm)
	    fn = strdup(argv[i++]);
	 break;
      }
   }

   if (i < argc)
      qore_setup_argv(i, argc, argv);

   if (opt_errors)
   {
      printe(suggest, pn);
      exit(1);
   }
   return fn;
}
