%{
/*
  scanner.ll

  Qore Programming Language

  Copyright (C) David Nichols 2003, 2004, 2005

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Flex scanner: no string length limits, no include file depth limits, thread-safe
  
  requires flex 2.5.31 or better (2.5.4 will not work) so a thread-safe scanner can
  be generated
  see: http://lex.sourceforge.net/
*/

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreNode.h>
#include <qore/support.h>
#include <qore/Function.h>
#include <qore/params.h>
#include <qore/QoreProgram.h>
#include <qore/DateTime.h>
#include <qore/QoreString.h>
#include <qore/common.h>
#include <qore/ParserSupport.h>
#include <qore/RegexSubst.h>
#include <qore/RegexTrans.h>
#include <qore/ModuleManager.h>
#include <qore/QoreRegex.h>
#include <qore/QoreWarnings.h>

#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int yyparse(yyscan_t yyscanner);

static inline class QoreString *getIncludeFileName(char *file)
{
   //printd(5, "getIncludeFileName(%s)\n", file);
   // FIXME: UNIX-specific
   if (file[0] == '/')
      return new QoreString(file);

   QoreString *rv;
   rv = findFileInEnvPath(file, "QORE_INCLUDE_DIR");
   if (!rv)
      rv = new QoreString(file);
   return rv;
}

static inline char *remove_quotes(char *str)
{
   str[strlen(str) - 1] = '\0';
   return str + 1;
}

static inline class DateTime *makeYears(int years)
{
   return new DateTime(years, 0, 0, 0, 0, 0, 0, true);
}

static inline class DateTime *makeMonths(int months)
{
   return new DateTime(0, months, 0, 0, 0, 0, 0, true);
}

static inline class DateTime *makeDays(int days)
{
   return new DateTime(0, 0, days, 0, 0, 0, 0, true);
}

static inline class DateTime *makeHours(int hours)
{
   return new DateTime(0, 0, 0, hours, 0, 0, 0, true);
}

static inline class DateTime *makeMinutes(int minutes)
{
   return new DateTime(0, 0, 0, 0, minutes, 0, 0, true);
}

static inline class DateTime *makeSeconds(int seconds)
{
   return new DateTime(0, 0, 0, 0, 0, seconds, 0, true);
}

static inline class DateTime *makeMilliseconds(int ms)
{
   return new DateTime(0, 0, 0, 0, 0, 0, ms, true);
}

//2005-03-29-10:19:27
//0123456789012345678
static inline void do_date_time_str(char *str)
{
   // move day to middle
   memmove(str+9, str+8, 2);
   // move month to middle
   memmove(str+7, str+5, 2);
   // move year to middle
   memmove(str+3, str, 4);
   // move minute to middle
   memmove(str+13, str+14, 2);
   // move seconds and the rest + null char to middle
   memmove(str+15, str+17, strlen(str+17) + 1);
}

//2005-03-29
//0123456789
static inline void do_date_str(char *str)
{
   // move month
   memmove(str+4, str+5, 2);
   // move day and null
   memmove(str+6, str+8, 3);
}

//16:03:29
//01234567
static inline void do_time_str(char *str)
{
   // move minute back
   memmove(str+2, str+3, 2);
   // move seconds and rest + null char
   memmove(str+4, str+6, strlen(str+6) + 1);
}

static inline class DateTime *makeDateTime(char *str)
{
   // move string to middle to form date string
   do_date_time_str(str);
   //printf("new date: %s\n", str + 3);
   return new DateTime(str + 3);
}

static inline class DateTime *makeDate(char *str)
{
   do_date_str(str);
   //printf("new date: %d:%s\n", strlen(str), str);
   return new DateTime(str);
}

static inline class DateTime *makeTime(char *str)
{
   do_time_str(str);
   //printf("new time: %d:%s\n", strlen(str), str);
   return new DateTime(str);
}

static inline class DateTime *makeRelativeDateTime(char *str)
{
   // move string to middle to form date string
   do_date_time_str(str);
   //printf("new date: %s\n", str + 3);
   class DateTime *dt = new DateTime();
   dt->setRelativeDate(str + 3);
   return dt;
}

static inline class DateTime *makeRelativeDate(char *str)
{
   do_date_str(str);
   //printf("new date: %d:%s\n", strlen(str), str);
   class DateTime *dt = new DateTime();
   dt->setRelativeDate(str);
   return dt;
}

static inline class DateTime *makeRelativeTime(char *str)
{
   do_time_str(str);
   //printf("new time: %d:%s\n", strlen(str), str);
   class DateTime *dt = new DateTime();
   dt->setRelativeDate(str);
   return dt;
}

static inline char *trim(char *str)
{
   while ((*str) == ' ' || (*str) == '\t')
      str++;
   // duplicate string
   char *n = strdup(str);
   // find end of string
   int l = strlen(n);
   if (l)
   {
      char *e = n + l - 1;
      while ((*e) == ' ' || (*e) == '\t')
	 *(e--) = '\0';
   }
   if (!n[0])
   {
      free(n);
      n = NULL;
   }
   return n;
}

static inline bool isRegexModifier(class QoreRegex *qr, int c)
{
   if (c == 'i')
      qr->setCaseInsensitive();
   else if (c == 's')
      qr->setDotAll();
   else if (c == 'x')
      qr->setExtended();
   else if (c == 'm')
      qr->setMultiline();
   else
      return false;
   return true;
}

static inline bool isRegexSubstModifier(class RegexSubst *qr, int c)
{
   if (c == 'g')
      qr->setGlobal();
   else if (c == 'i')
      qr->setCaseInsensitive();
   else if (c == 's')
      qr->setDotAll();
   else if (c == 'x')
      qr->setExtended();
   else if (c == 'm')
      qr->setMultiline();
   else
      return false;
   return true;
}

%}

%option noyywrap nomain noyy_top_state warn
%option reentrant bison-bridge
%option stack

%x str_state regex_state incl check_regex regex_subst1 regex_subst2 line_comment exec_class requires regex_trans1 regex_trans2 regex_extract_state disable_warning enable_warning

HEX_CONST       0x[0-9A-Fa-f]+
OCTAL_CONST     \\[0-7]{1,3}
DIGIT		[0-9]
WORD		[a-zA-Z][a-zA-Z0-9_]*
WS		[ \t\r]
YEAR            [0-9]{4}
MONTH           (0[1-9])|(1[012])
DAY             ((0[1-9])|([12][0-9])|(3[01]))
HOUR            ([01][0-9])|(2[0-3])
MSEC            [0-5][0-9]
MS              [0-9]{3}
D2              [0-9]{2}

%%
^%no-global-vars{WS}*$                  getProgram()->parseSetParseOptions(PO_NO_GLOBAL_VARS);
^%no-subroutine-defs{WS}*$              getProgram()->parseSetParseOptions(PO_NO_SUBROUTINE_DEFS);
^%no-threads{WS}*$                      getProgram()->parseSetParseOptions(PO_NO_THREADS);
^%no-thread-control{WS}*$               getProgram()->parseSetParseOptions(PO_NO_THREAD_CONTROL);
^%no-thread-classes{WS}*$               getProgram()->parseSetParseOptions(PO_NO_THREAD_CLASSES);
^%no-top-level{WS}*$                    getProgram()->parseSetParseOptions(PO_NO_TOP_LEVEL_STATEMENTS);
^%no-class-defs{WS}*$                   getProgram()->parseSetParseOptions(PO_NO_CLASS_DEFS);
^%no-namespace-defs{WS}*$               getProgram()->parseSetParseOptions(PO_NO_NAMESPACE_DEFS);
^%no-external-process{WS}*$             getProgram()->parseSetParseOptions(PO_NO_EXTERNAL_PROCESS);
^%lock-options{WS}*$                    getProgram()->lockOptions();
^%lock-warnings{WS}*$                   getProgram()->parseSetParseOptions(PO_LOCK_WARNINGS);
^%no-process-control{WS}*$              getProgram()->parseSetParseOptions(PO_NO_PROCESS_CONTROL);
^%no-constant-defs{WS}*$                getProgram()->parseSetParseOptions(PO_NO_CONSTANT_DEFS);
^%no-new{WS}*$                          getProgram()->parseSetParseOptions(PO_NO_NEW);
^%no-child-restrictions{WS}*$           getProgram()->parseSetParseOptions(PO_NO_CHILD_PO_RESTRICTIONS);
^%no-database{WS}*$                     getProgram()->parseSetParseOptions(PO_NO_DATABASE);
^%require-our{WS}*$                     getProgram()->parseSetParseOptions(PO_REQUIRE_OUR);
^%enable-all-warnings{WS}*$             { 
                                           if (getProgram()->setWarningMask(-1))
					      getProgram()->makeParseWarning(QP_WARN_WARNING_MASK_UNCHANGED, "CANNOT-UPDATE-WARNING-MASK", "this program has its warning mask locked; cannot enable all warnings");
			                }
^%disable-all-warnings{WS}*$            { 
                                           if (getProgram()->setWarningMask(0))
					      getProgram()->makeParseWarning(QP_WARN_WARNING_MASK_UNCHANGED, "CANNOT-UPDATE-WARNING-MASK", "this program has its warning mask locked; cannot disable all warnings");
			                }
^%disable-warning{WS}*$                 { parse_error("missing argument to %%disable-warning"); }
^%disable-warning{WS}+                  BEGIN(disable_warning);
<disable_warning>[^\t\n\r]+             {
                                           char *cn = trim(yytext);
					   //printd(5, "scanner: disable warning '%s'\n", cn);
					   int code = get_warning_code(cn);
					   if (!code)
					      getProgram()->makeParseWarning(QP_WARN_UNKNOWN_WARNING, "UNKNOWN-WARNING", "cannot disable unknown warning '%s'", cn);
					   else if (getProgram()->disableWarning(code))
					      getProgram()->makeParseWarning(QP_WARN_WARNING_MASK_UNCHANGED, "CANNOT-UPDATE-WARNING-MASK", "this program has its warning mask locked; cannot disable warning '%s'", cn);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
^%ensable-warning{WS}*$                 { parse_error("missing argument to %%enable-warning"); }
^%enable-warning{WS}+                   BEGIN(enable_warning);
<enable_warning>[^\t\n\r]+              {
                                           char *cn = trim(yytext);
					   //printd(5, "scanner: enable warning '%s'\n", cn);
					   int code = get_warning_code(cn);
					   if (!code)
					      getProgram()->makeParseWarning(QP_WARN_UNKNOWN_WARNING, "UNKNOWN-WARNING", "cannot enable unknown warning '%s'", cn);
					   else if (getProgram()->enableWarning(code))
					      getProgram()->makeParseWarning(QP_WARN_WARNING_MASK_UNCHANGED, "CANNOT-UPDATE-WARNING-MASK", "this program has its warning mask locked; cannot enable warning '%s'", cn);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
^%exec-class{WS}*$                      { parse_error("missing argument to %%exec-class"); }
^%exec-class{WS}*                       BEGIN(exec_class);
^%requires                              BEGIN(requires);
<requires>[^\t\n\r]+                    {
                                           char *cn = trim(yytext);
					   //printd(5, "scanner requesting feature: '%s'\n", cn);
					   QoreString *err = MM.loadModule(cn, getProgram());
					   if (err)
					      getProgram()->cannotProvideFeature(err);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
^%include{WS}*$                         { parse_error("missing argument to %%include"); }
^%include{WS}+				BEGIN(incl);
<exec_class>{
   [^\t\n\r]+$   	                {
                                           char *cn = trim(yytext);
					   //printf("setting class name to: '%s'\n", cn);
	 				   getProgram()->setExecClass(cn);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
}
<incl>{WS}*				// ignore white space
<incl>[^\t\n\r]+			{
                                           FILE *save_yyin = yyin;
					   QoreString *fname = getIncludeFileName(yytext);
					   yyin = fopen(fname->getBuffer(), "r");
					   
					   if (!yyin)
					   {
					      parse_error("cannot open include file \"%s\"", yytext);
					      yyin = save_yyin;
					      BEGIN(INITIAL);
					   }
					   else
					   {
					      // save file name string in QoreProgram's list
					      getProgram()->fileList.insert(fname->getBuffer());
					      // "give away" string - it will be deleted when the QoreProgram object
					      // is deleted
					      beginParsing(fname->giveBuffer(), (void *)YY_CURRENT_BUFFER);
					      yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE, yyscanner), yyscanner);
					      BEGIN(INITIAL);
					   }
					   delete fname;
                                        }
<<EOF>>                                 {
					   // delete current buffer
					   yy_delete_buffer(YY_CURRENT_BUFFER, yyscanner);
					   // get last parse state
					   YY_BUFFER_STATE yybs = (YY_BUFFER_STATE)endParsing();
					   if (yybs)
					   {
					      // need to close file and switch to previous buffer
					      fclose(yyin);
					      if (yybs)
						 yy_switch_to_buffer(yybs, yyscanner);
					   }
					   else
					      yyterminate();
                                        }
\/\*                                    {
                                            int c;
                                            while ((c = yyinput(yyscanner)))
					       if (c == '\n')
						  increment_pgm_counter();
					       else if (c == '*')
					       {
						  if (yyinput(yyscanner) == '/') break;
						  else unput(c);
					       }
                                               else if (c == EOF)
                                               {
						  parse_error("EOF reached in block comment");
						  break;
					       }
                                        }
\"					yylval->String = new QoreString(); BEGIN(str_state);
<str_state>{
      \"				BEGIN(INITIAL); return QUOTED_WORD;
      \n				{
                                           increment_pgm_counter();
					   yylval->String->concat('\n');
                                        }
      {OCTAL_CONST}			{
					   int result;
					   sscanf(yytext + 1, "%o", &result);
					   if (result > 0xff)
					      parse_error("octal constant out of bounds\n");
					   yylval->String->concat((char)result);
					}
      \\{DIGIT}+			parse_error("bad escape sequence");
      \\n				yylval->String->concat('\n');
      \\t				yylval->String->concat('\t');
      \\r				yylval->String->concat('\r');
      \\b				yylval->String->concat('\b');
      \\f				yylval->String->concat('\f');
      \\.				yylval->String->concat(yytext[1]);
      \\\n                              {
	                                   increment_pgm_counter();
					   yylval->String->concat('\n');
                                        }
      [^\\\n\"]+			{
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->String->concat(*(yptr++));
					}
}
<regex_subst2>{
      \/				{
                                           // get regex modifiers
	                                   int c;
	                                   do {
					      c = yyinput(yyscanner);
					   } while (isRegexSubstModifier(yylval->RegexSubst, c));
					   unput(c);
					   BEGIN(INITIAL); 
					   yylval->RegexSubst->parse(); 
					   return REGEX_SUBST;
                                        }
      \n				{
	                                   increment_pgm_counter();
					   yylval->RegexSubst->concatTarget('\n');
                                        }
      \\\/                              yylval->RegexSubst->concatTarget('/');
      \\.                               { yylval->RegexSubst->concatTarget('\\'); yylval->RegexSubst->concatTarget(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexSubst->concatTarget(*(yptr++));
					}
}
<regex_subst1>{
      \/  	                        BEGIN(regex_subst2);
      \n				{
	                                   increment_pgm_counter();
					   yylval->RegexSubst->concatSource('\n');
                                        }
      \\\/                              yylval->RegexSubst->concatSource('/');
      \\.                               { yylval->RegexSubst->concatSource('\\'); yylval->RegexSubst->concatSource(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexSubst->concatSource(*(yptr++));
					}
}
<regex_trans2>{
      -                                 yylval->RegexTrans->setTargetRange();
      \/				{
					   BEGIN(INITIAL); 
					   yylval->RegexTrans->finishTarget(); 
					   return REGEX_TRANS;
                                        }
      \n				{
	                                   increment_pgm_counter();
					   yylval->RegexTrans->concatTarget('\n');
                                        }
      \\n				yylval->RegexTrans->concatTarget('\n');
      \\t				yylval->RegexTrans->concatTarget('\t');
      \\r				yylval->RegexTrans->concatTarget('\r');
      \\b				yylval->RegexTrans->concatTarget('\b');
      \\f				yylval->RegexTrans->concatTarget('\f');
      \\\n                              {
	                                   increment_pgm_counter();
					   yylval->RegexTrans->concatTarget('\n');
                                        }
      \\\/                              yylval->RegexTrans->concatTarget('/');
      \\.				yylval->RegexTrans->concatTarget(yytext[1]);
      [^\n\\/\-]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexTrans->concatTarget(*(yptr++));
					}
}
<regex_trans1>{
      -                                 yylval->RegexTrans->setSourceRange();
      \/  	                        BEGIN(regex_trans2); yylval->RegexTrans->finishSource();
      \n				{
	                                   increment_pgm_counter();
					   yylval->RegexTrans->concatSource('\n');
                                        }
      \\n				yylval->RegexTrans->concatSource('\n');
      \\t				yylval->RegexTrans->concatSource('\t');
      \\r				yylval->RegexTrans->concatSource('\r');
      \\b				yylval->RegexTrans->concatSource('\b');
      \\f				yylval->RegexTrans->concatSource('\f');
      \\\n                              {
	                                   increment_pgm_counter();
					   yylval->RegexTrans->concatSource('\n');
                                        }


      \\\/                              yylval->RegexTrans->concatSource('/');
      \\.				yylval->RegexTrans->concatSource(yytext[1]);
      [^\n\\/\-]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexTrans->concatSource(*(yptr++));
					}
}
<check_regex>{
      {WS}+                             // ignore 
      s\/                               yylval->RegexSubst = new RegexSubst(); BEGIN(regex_subst1);
      x\/                               yylval->Regex = new QoreRegex(); BEGIN(regex_extract_state);
      m\/                               yylval->Regex = new QoreRegex(); BEGIN(regex_state);
      \/                                yylval->Regex = new QoreRegex(); BEGIN(regex_state);
      tr\/                              yylval->RegexTrans = new RegexTrans(); BEGIN(regex_trans1);
      [^\/]                             BEGIN(INITIAL);
}
<regex_state>{
      \/				{
                                           // get regex modifiers
	                                   int c;
	                                   do {
					      c = yyinput(yyscanner);
					   } while (isRegexModifier(yylval->Regex, c));
					   unput(c);
					   BEGIN(INITIAL); 
					   yylval->Regex->parse(); 
					   return REGEX;
                                        }
      \n				{
	                                   increment_pgm_counter();
					   yylval->Regex->concat('\n');
                                        }
      \\\/                              yylval->Regex->concat('/');
      \\.                               { yylval->Regex->concat('\\'); yylval->Regex->concat(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->Regex->concat(*(yptr++));
					}
}
<regex_extract_state>{
      \/				{
                                           // get regex modifiers
	                                   int c;
	                                   do {
					      c = yyinput(yyscanner);
					   } while (isRegexModifier(yylval->Regex, c));
					   unput(c);
					   BEGIN(INITIAL); 
					   yylval->Regex->parse(); 
					   return REGEX_EXTRACT;
                                        }
      \n				{
	                                   increment_pgm_counter();
					   yylval->Regex->concat('\n');
                                        }
      \\\/                              yylval->Regex->concat('/');
      \\.                               { yylval->Regex->concat('\\'); yylval->Regex->concat(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->Regex->concat(*(yptr++));
					}
}
<line_comment>{
   .*$					BEGIN(INITIAL);
}
\#                                      BEGIN(line_comment);
where					return TOK_WHERE;
NULL					return TOK_NULL;
NOTHING					return TOK_NOTHING;
class                                   return TOK_CLASS;
private                                 return TOK_PRIVATE;
new                                     return TOK_NEW;
shift                                   return TOK_SHIFT;
unshift                                 return TOK_UNSHIFT;
do					return TOK_DO;
while					return TOK_WHILE;
if					return TOK_IF;
else					return TOK_ELSE;
for					return TOK_FOR;
foreach                                 return TOK_FOREACH;
in                                      return TOK_IN;
elements                                return TOK_ELEMENTS;
keys                                    return TOK_KEYS;
sub                                     return TOK_SUB;
const                                   return TOK_CONST;
namespace				return TOK_NAMESPACE;
return					return TOK_RETURN;
my					return TOK_MY;
our 					return TOK_OUR;
break					return TOK_BREAK;
continue				return TOK_CONTINUE;
try                                     return TOK_TRY;
throw                                   return TOK_THROW;
rethrow                                 return TOK_RETHROW;
catch                                   return TOK_CATCH;
finally                                 return TOK_FINALLY;
find                                    return TOK_FIND;
delete                                  return TOK_DELETE;
background                              return TOK_BACKGROUND;
synchronized                            return TOK_SYNCHRONIZED;
thread_exit                             return TOK_THREAD_EXIT;
exists                                  return TOK_EXISTS;
context				        return TOK_CONTEXT;
summarize				return TOK_SUMMARIZE;
subcontext				return TOK_SUB_CONTEXT;
sortBy				        return TOK_SORT_BY;
sortDescendingBy			return TOK_SORT_DESCENDING_BY;
by					return TOK_BY;
switch                                  return TOK_SWITCH;
case                                    return TOK_CASE;
default                                 return TOK_DEFAULT;
inherits                                return TOK_INHERITS;
push                                    return TOK_PUSH;
pop                                     return TOK_POP;
splice                                  return TOK_SPLICE;
instanceof                              return TOK_INSTANCEOF;
chomp					return TOK_CHOMP;
chomp\(                            yylval->string = strdup("chomp"); return KW_IDENTIFIER_OPENPAREN;
push\(                             yylval->string = strdup("push"); return KW_IDENTIFIER_OPENPAREN;
pop\(                              yylval->string = strdup("pop"); return KW_IDENTIFIER_OPENPAREN;
splice\(                           yylval->string = strdup("splice"); return KW_IDENTIFIER_OPENPAREN;
shift\(                            yylval->string = strdup("shift"); return KW_IDENTIFIER_OPENPAREN;
unshift\(                          yylval->string = strdup("unshift"); return KW_IDENTIFIER_OPENPAREN;
{YEAR}-{MONTH}-{DAY}[T-]{HOUR}:{MSEC}:{MSEC}(\.{MS})?   yylval->datetime = makeDateTime(yytext); return DATETIME;
{YEAR}-{MONTH}-{DAY}                    yylval->datetime = makeDate(yytext); return DATETIME;
{HOUR}:{MSEC}:{MSEC}(\.{MS})?           yylval->datetime = makeTime(yytext); return DATETIME;
P{YEAR}-{D2}-{D2}T{D2}:{D2}:{D2}(\.{MS})?   yylval->datetime = makeRelativeDateTime(yytext+1); return DATETIME;
P{YEAR}-{D2}-{D2}                       yylval->datetime = makeRelativeDate(yytext+1); return DATETIME;
PT{D2}:{D2}:{D2}(\.{MS})?               yylval->datetime = makeRelativeTime(yytext+2); return DATETIME;
P{D2}:{D2}:{D2}(\.{MS})?                yylval->datetime = makeRelativeTime(yytext+1); return DATETIME;
({WORD}::)+{WORD}                       yylval->string = strdup(yytext); return SCOPED_REF;
({WORD}::)+\$\.{WORD}                   yylval->nscope = new NamedScope(strdup(yytext)); yylval->nscope->fixBCCall(); return BASE_CLASS_CALL;
{DIGIT}+"."{DIGIT}+			yylval->decimal = strtod(yytext, NULL); return QFLOAT;
0[0-7]+				        yylval->integer = strtoll(yytext+1, NULL, 8); return INTEGER;
{DIGIT}+				yylval->integer = strtoll(yytext, NULL, 10); return INTEGER;
{DIGIT}+Y                               yylval->datetime = makeYears(strtol(yytext, NULL, 10));   return DATETIME;
{DIGIT}+M                               yylval->datetime = makeMonths(strtol(yytext, NULL, 10));  return DATETIME;
{DIGIT}+D                               yylval->datetime = makeDays(strtol(yytext, NULL, 10));    return DATETIME;
{DIGIT}+h                               yylval->datetime = makeHours(strtol(yytext, NULL, 10));   return DATETIME;
{DIGIT}+ms                              yylval->datetime = makeMilliseconds(strtol(yytext, NULL, 10)); return DATETIME;
{DIGIT}+m                               yylval->datetime = makeMinutes(strtol(yytext, NULL, 10)); return DATETIME;
{DIGIT}+s                               yylval->datetime = makeSeconds(strtol(yytext, NULL, 10)); return DATETIME;
{HEX_CONST}				yylval->integer = strtoll(yytext, NULL, 16); return INTEGER;
\$\.{WORD}                              yylval->string = strdup(yytext + 2); return SELF_REF;
\${WORD}				yylval->string = strdup(yytext + 1); return VAR_REF;
{WORD}					yylval->string = strdup(yytext); return IDENTIFIER;
\%{WORD}                                yylval->string = strdup(yytext + 1); return CONTEXT_REF;
\%{WORD}\:{WORD}                        yylval->string = strdup(yytext + 1); return COMPLEX_CONTEXT_REF;
\%\%                                    return TOK_CONTEXT_ROW;
\`[^`]*\`                               yylval->string = strdup(remove_quotes(yytext)); return BACKQUOTE;
\'[^']*\'				yylval->String = new QoreString(remove_quotes(yytext)); return QUOTED_WORD;
\<{WS}*=				return LOGICAL_LE;
\>{WS}*=				return LOGICAL_GE;
\!{WS}*=				return LOGICAL_NE;
\<{WS}*\>				return LOGICAL_NE;
={WS}*={WS}*=                           return ABSOLUTE_EQ;
\!{WS}*={WS}*=                          return ABSOLUTE_NE;
={WS}*= 				return LOGICAL_EQ;
\<{WS}*={WS}*\>                         return LOGICAL_CMP;
&{WS}*& 	 			return LOGICAL_AND;
\|{WS}*\|				return LOGICAL_OR;
\>{WS}*\>				return SHIFT_RIGHT;
\<{WS}*\<				return SHIFT_LEFT;
\+{WS}*\+				return P_INCREMENT;
-{WS}*- 				return P_DECREMENT;
\+{WS}*=				return PLUS_EQUALS;
-{WS}*= 	 			return MINUS_EQUALS;
&{WS}*= 				return AND_EQUALS;
\|{WS}*=				return OR_EQUALS;
\%{WS}*=                                return MODULA_EQUALS;
\*{WS}*=                                return MULTIPLY_EQUALS;
\/{WS}*=                                return DIVIDE_EQUALS;
\^{WS}*=                                return XOR_EQUALS;
\>{WS}*\>{WS}*=				return SHIFT_RIGHT_EQUALS;
\<{WS}*\<{WS}*=				return SHIFT_LEFT_EQUALS;
=\~                                     BEGIN(check_regex); return REGEX_MATCH;
\!\~                                    BEGIN(check_regex); return REGEX_NMATCH;
{WS}+					/* ignore whitespace */
\n					{
                                           increment_pgm_counter();
                                        }
.					return yytext[0];
%%
