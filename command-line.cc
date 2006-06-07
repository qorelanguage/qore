/*
  command-line.cc

  Qore Programming Language

  this whole file comes out of a very old getopt-stype implementation by me

  it should offer POSIX style command-line handling on any platform...

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols
*/

#include <qore/Qore.h>
#include "command-line.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

// global parse_option
int parse_options = 0;

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

// program text given on the command-line
char *cl_pgm = NULL;

// program name
static char *pn;

static int opt_errors = 0;

static char usage[] = "usage: %s [option(s)]... [program file]\n";
static char suggest[] = "try '%s -h' for more information.\n";

static char helpstr[] = 
#ifdef DEBUG
"  -d, --debug=arg              sets debugging level (higher number = more output)\n" \
"  -t, --trace                  turns on function tracing\n" 
#endif
"  -h, --help                   shows this help text\n" \
"  -e, --exec=arg               execute program given on command-line\n" \
"  -x, --exec-class[=arg]       instantiate class with same name as file name\n" \
"                               (override with arg, also sets --no-top-level)\n" \
"  -m, --show-module-errors     show error messages related to loading and\n" \
"                               initializing qore modules\n"
"  -c, --charset=arg            sets default character set encoding\n" \
"      --show-charsets          displays the list of known character sets\n" \
"      --show-aliases           displays the list of character sets aliases\n" \
"  -V, --version                show program version information and quit\n" \
"\n" \
" PARSE OPTIONS:\n" \
"  -G, --no-global-vars         makes global variable definitions illegal\n" \
"  -S, --no-subroutine-def      makes subroutine definitions illegal\n" \
"  -T, --no-threads             makes thread operations illegal\n" \
"  -L, --no-top-level           makes top-level statements illegal\n" \
"  -C, --no-class-defs          makes class definitions illegal\n" \
"  -D, --no-namespace-defs      makes namespace declarations illegal\n" \
"  -E, --no-external-process    makes access to external processes illegal\n" \
"  -K, --lock-options           disables changes to parse options in program\n" \
"  -P, --no-process-control     makes process control illegal (fork(), exit(), etc)\n" \
"  -F, --no-constant-defs       makes constant definitions illegal\n" \
"  -N, --no-new                 makes using the 'new' operator illegal\n" \
"  -I, --no-child-restrictions  do not restrict subprograms' parse options\n" \
"  -O, --require-our            requires global variables to be declared with 'our'\n";

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
   leave(0);
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
   printf("QORE for %s, Copyright (C) 2003 - 2006 David Nichols\nversion %s", qore_target_arch, qore_version_string);
   class charPtrNode *w = qoreFeatureList.getHead();
   if (w)
   {
      printf(" (builtin features: ");
      while (w)
      {
	 printf(w->str);
	 w = w->next;
	 if (w)
	    printf(", ");
      }
      putchar(')');
   }
   putchar('\n');
   leave(0);
}

static void set_charset(char *arg)
{
   def_charset = strdup(arg);
}

static void show_charsets(char *arg)
{
   QEM.showEncodings();
   leave(0);
}

static void show_charset_aliases(char *arg)
{
   QEM.showAliases();
   leave(0);
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
   { 'h', "help",                  ARG_NONE, do_help },
   { 'V', "version",               ARG_NONE, do_version },
   { 'c', "charset",               ARG_MAND, set_charset },
   { 'e', "exec",                  ARG_MAND, set_exec },
   { 'x', "exec-class",            ARG_OPT,  do_exec_class },
   { 'm', "show-module-errors",    ARG_NONE, show_module_errors },
   { '\0', "show-charsets",        ARG_NONE, show_charsets },
   { '\0', "show-aliases",         ARG_NONE, show_charset_aliases },
   { 'G', "no-global-vars",        ARG_NONE, do_no_global_vars },
   { 'S', "no-subroutine-defs",    ARG_NONE, do_no_subroutine_defs },
   { 'T', "no-threads",            ARG_NONE, do_no_threads },
   { 'L', "no-top-level",          ARG_NONE, do_no_top_level },
   { 'C', "no-class-defs",         ARG_NONE, do_no_class_defs },
   { 'D', "no-namespace-defs",     ARG_NONE, do_no_namespace_defs },
   { 'F', "no-constant-defs",      ARG_NONE, do_no_constant_defs },
   { 'N', "no-new",                ARG_NONE, do_no_new },
   { 'I', "no-child-restrictions", ARG_NONE, do_no_child_po_restrictions },
   { 'E', "no-external-process",   ARG_NONE, do_no_external_process },
   { 'P', "no-process-control",    ARG_NONE, do_no_process_control },
   { 'O', "require-our",           ARG_NONE, do_require_our  },
   { 'K', "lock-options",          ARG_NONE, do_lock_options },
#ifdef DEBUG
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

// *i is the argument position, *j is the index in the string
static int process_char_opt(char *argv[], unsigned *i, unsigned *j, int argc)
{
   unsigned x;
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
static void process_str_opt(char *argv[], unsigned *i, unsigned argc)
{
   unsigned x, option_present = 0;
   char *opt = &argv[*i][2];
   
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

static char **get_new_argv(unsigned *argc, char *argv[])
{
   // if the first argument is the file name, then
   // do nothing
   if ((*argc) < 2 || argv[1][0] != '-' || !argv[1][1])
      return argv;

   unsigned delim = 0;
   unsigned i = 0;
   while (argv[1][i])
   {
      if (argv[1][i] == ' ')
	 delim++;
      i++;
   }
   //printf("delim=%d\n", delim); fflush(stdout);
   if (!delim)
      return argv;

   (*argc) += delim;
   char **rv = new char *[*argc];
   char *l = argv[1];
   for (i = 0; i < delim; i++)
   {
      char *p = strchr(l, ' ');
      rv[i + 1] = (char *)malloc(sizeof(char) * (p - l + 1));
      strncpy(rv[i + 1], l, p - l);
      //printd(5, "get_new_argv() adding %s\n", rv[i + 1]);
      rv[i + 1][p - l] = '\0';
      l = p + 1;
   }
   //printd(5, "get_new_argv() adding %s\n", l);
   rv[i + 1] = strdup(l);

   for (i = delim + 2; i < (*argc); i++)
      rv[i] = strdup(argv[i - delim]);
   return rv;
}

static void delete_new_argv(char **nargv, int argc)
{
   for (int i = 1; i < argc; i++)
      free(nargv[i]);
   delete [] nargv;
}

// returns either NULL or a string that must be freed with free()
// also sets up the global ARGV argument list
char *parse_command_line(unsigned argc, char *argv[])
{
   pn = basename(argv[0]);

   // file name to return, if any
   char *fn = NULL;

   unsigned i = 1;

   char **cargv = get_new_argv(&argc, argv);

   // check all arguments
   for (; i < argc; i++)
   {
      printd(5, "parse_command_line() %d/%d=%s\n", i, argc, cargv[i]);
      if (cargv[i][0] == '-')
      {
	 if (!cargv[i][1])
	 {
	    i++;
	    break;
	 }
	 else
	 {
	    if (cargv[i][1] == '-')
	    {
	       if (!cargv[i][2])
	       {
		  i++;
		  break;
	       }
	       process_str_opt(cargv, &i, argc);
	    }
	    else
	    {
	       unsigned j;

	       for (j = 1; j < strlen(cargv[i]); j++)
		  if (process_char_opt(cargv, &i, &j, argc))
		     break;
	    }
	 }
      }
      else
      {
	 fn = strdup(cargv[i++]);
	 break;
      }
   }

   if (i < argc)
      qore_setup_argv(i, argc, cargv);

   if (cargv != argv)
      delete_new_argv(cargv, argc);
   if (opt_errors)
   {
      printe(suggest, pn);
      leave(1);
   }
   return fn;
}
