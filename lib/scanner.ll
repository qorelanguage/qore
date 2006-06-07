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
#include <qore/ModuleManager.h>
#include <qore/QoreRegex.h>

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
   class DateTime *dt = new DateTime();
   dt->year = years;
   dt->relative = 1;
   return dt;
}

static inline class DateTime *makeMonths(int months)
{
   class DateTime *dt = new DateTime();
   dt->month = months;
   dt->relative = 1;
   return dt;
}

static inline class DateTime *makeDays(int days)
{
   class DateTime *dt = new DateTime();
   dt->day = days;
   dt->relative = 1;
   return dt;
}

static inline class DateTime *makeHours(int hours)
{
   class DateTime *dt = new DateTime();
   dt->hour = hours;
   dt->relative = 1;
   return dt;
}

static inline class DateTime *makeMinutes(int minutes)
{
   class DateTime *dt = new DateTime();
   dt->minute = minutes;
   dt->relative = 1;
   return dt;
}

static inline class DateTime *makeSeconds(int seconds)
{
   class DateTime *dt = new DateTime();
   dt->second = seconds;
   dt->relative = 1;
   return dt;
}

//2005-03-29-10:19:27
//0123456789012345678
static inline class DateTime *makeDateTime(char *str)
{
   // move string to middle to form date string
   // move day to middle
   memmove(str+9, str+8, 2);
   // move month to middle
   memmove(str+7, str+5, 2);
   // move year to middle
   memmove(str+3, str, 4);
   // move minute to middle
   memmove(str+13, str+14, 2);
   // move second and null to middle
   memmove(str+15, str+17, 3);
   //printf("new date: %s\n", str + 3);
   return new DateTime(str + 3);
}

//2005-03-29
//0123456789
static inline class DateTime *makeDate(char *str)
{
   // move month
   memmove(str+4, str+5, 2);
   // move day and null
   memmove(str+6, str+8, 3);
   //printf("new date: %d:%s\n", strlen(str), str);
   return new DateTime(str);
}

//16:03:29
//01234567
static inline class DateTime *makeTime(char *str)
{
   // move minute back
   memmove(str+2, str+3, 2);
   // move seconds and null
   memmove(str+4, str+6, 3);
   //printf("new time: %d:%s\n", strlen(str), str);
   class DateTime *dt = new DateTime(str);
   if (!dt->year)
      dt->year = 1970;
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

%x str_state regex_state incl check_regex regex_subst1 regex_subst2 line_comment exec_class requires

HEX_CONST       0x[0-9A-Fa-f]+
OCTAL_CONST     \\[0-7]{1,3}
DIGIT		[0-9]
WORD		[a-zA-Z][a-zA-Z0-9_]*
WS		[ \t\r]
YEAR            [0-9]{4}
MONTH           [0-9]{2}
DAY             [0-9]{2}
HOUR            [0-9]{2}
MSEC            [0-9]{2}

%%
^%no-global-vars{WS}*$                  getProgram()->parseSetParseOptions(PO_NO_GLOBAL_VARS);
^%no-subroutine-defs{WS}*$              getProgram()->parseSetParseOptions(PO_NO_SUBROUTINE_DEFS);
^%no-threads{WS}*$                      getProgram()->parseSetParseOptions(PO_NO_THREADS);
^%no-top-level{WS}*$                    getProgram()->parseSetParseOptions(PO_NO_TOP_LEVEL_STATEMENTS);
^%no-class-defs{WS}*$                   getProgram()->parseSetParseOptions(PO_NO_CLASS_DEFS);
^%no-namespace-defs{WS}*$               getProgram()->parseSetParseOptions(PO_NO_NAMESPACE_DEFS);
^%no-external-process{WS}*$             getProgram()->parseSetParseOptions(PO_NO_EXTERNAL_PROCESS);
^%lock-options{WS}*$                    getProgram()->lockOptions();
^%no-process-control{WS}*$              getProgram()->parseSetParseOptions(PO_NO_PROCESS_CONTROL);
^%no-constant-defs{WS}*$                getProgram()->parseSetParseOptions(PO_NO_CONSTANT_DEFS);
^%no-new{WS}*$                          getProgram()->parseSetParseOptions(PO_NO_NEW);
^%no-child-restrictions{WS}*$           getProgram()->parseSetParseOptions(PO_NO_CHILD_PO_RESTRICTIONS);
^%require-our{WS}*$                     getProgram()->parseSetParseOptions(PO_REQUIRE_OUR);
^%exec-class                            BEGIN(exec_class);
^%requires                              BEGIN(requires);
<requires>[^\t\n\r]+                    {
                                           char *cn = trim(yytext);
					   //printd(5, "scanner requesting feature: '%s'\n", cn);
					   if (MM.loadModule(cn, getProgram()))
					      parse_error("cannot provide feature '%s'", cn);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
^%include{WS}+				BEGIN(incl);
<exec_class>[^\t\n\r]+			{
                                           char *cn = trim(yytext);
					   //printf("setting class name to: '%s'\n", cn);
					   getProgram()->setExecClass(cn);
					   free(cn);
					   BEGIN(INITIAL);
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
					   yylval->RegexSubst->concat('\n');
                                        }
      \\\/                              yylval->RegexSubst->concat('/');
      \\.                               { yylval->RegexSubst->concat('\\'); yylval->RegexSubst->concat(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexSubst->concat(*(yptr++));
					}
}
<regex_subst1>{
      \/  	                        BEGIN(regex_subst2); yylval->RegexSubst->setDivider();
      \n				{
	                                   increment_pgm_counter();
					   yylval->RegexSubst->concat('\n');
                                        }
      \\\/                              yylval->RegexSubst->concat('/');
      \\.                               { yylval->RegexSubst->concat('\\'); yylval->RegexSubst->concat(yytext[1]); }
      [^\n\\/]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexSubst->concat(*(yptr++));
					}
}
<check_regex>{
      {WS}+                             // ignore 
      s\/                               yylval->RegexSubst = new RegexSubst(); BEGIN(regex_subst1);
      m\/                               yylval->Regex = new QoreRegex(); BEGIN(regex_state);
      \/                                yylval->Regex = new QoreRegex(); BEGIN(regex_state);
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
push{WS}*\(                             yylval->string = strdup("push"); return KW_IDENTIFIER_OPENPAREN;
pop{WS}*\(                              yylval->string = strdup("pop"); return KW_IDENTIFIER_OPENPAREN;
splice{WS}*\(                           yylval->string = strdup("splice"); return KW_IDENTIFIER_OPENPAREN;
shift{WS}*\(                            yylval->string = strdup("shift"); return KW_IDENTIFIER_OPENPAREN;
unshift{WS}*\(                          yylval->string = strdup("unshift"); return KW_IDENTIFIER_OPENPAREN;
{YEAR}-{MONTH}-{DAY}-{HOUR}:{MSEC}:{MSEC} yylval->datetime = makeDateTime(yytext); return DATETIME;
{YEAR}-{MONTH}-{DAY}                    yylval->datetime = makeDate(yytext); return DATETIME;
{HOUR}:{MSEC}:{MSEC}                    yylval->datetime = makeTime(yytext); return DATETIME;
({WORD}::)+{WORD}                       yylval->string = strdup(yytext); return SCOPED_REF;
({WORD}::)+\$\.{WORD}                   yylval->nscope = new NamedScope(strdup(yytext)); yylval->nscope->fixBCCall(); return BASE_CLASS_CALL;
{DIGIT}+"."{DIGIT}+			yylval->decimal = strtod(yytext, NULL); return FLOAT;
0[0-7]+				        yylval->integer = strtoll(yytext+1, NULL, 8); return INTEGER;
{DIGIT}+				yylval->integer = strtoll(yytext, NULL, 10); return INTEGER;
{DIGIT}+Y                               yylval->datetime = makeYears(strtol(yytext, NULL, 10));   return DATETIME;
{DIGIT}+M                               yylval->datetime = makeMonths(strtol(yytext, NULL, 10));  return DATETIME;
{DIGIT}+D                               yylval->datetime = makeDays(strtol(yytext, NULL, 10));    return DATETIME;
{DIGIT}+h                               yylval->datetime = makeHours(strtol(yytext, NULL, 10));   return DATETIME;
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
