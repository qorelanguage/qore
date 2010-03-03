%{
/*
  scanner.ll

  Qore Programming Language

  Copyright David Nichols 2003 - 2009

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

  Flex scanner: 
    no string length limits, 
    no include file depth limits, 
    thread-safe

  requires flex 2.5.31 or better (2.5.35 recommended, 2.5.4 will not work) 
  so a thread-safe scanner can be generated

  see: http://flex.sourceforge.net/
*/

#include <qore/Qore.h>
#include <qore/intern/ParserSupport.h>

#include "parser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

QoreParserLocation::QoreParserLocation() : explicit_first(false), first_line(0) {
}

void QoreParserLocation::updatePosition(int f) {
   if (!explicit_first) {
      first_line = f;
      update_parse_location(f, f);
   }
   else {
      update_parse_location(first_line, f);
      explicit_first = false;
   }
   last_line = f;
}

#define YY_USER_ACTION { yylloc->updatePosition(yylineno); }

int yyparse(yyscan_t yyscanner);

// the return value of this function must be freed if non-0
static char *trim(const char *str) {
   while ((*str) == ' ' || (*str) == '\t')
      str++;
   // duplicate string
   char *n = strdup(str);
   // find end of string
   int l = strlen(n);
   if (l) {
      char *e = n + l - 1;
      while ((*e) == ' ' || (*e) == '\t')
	 *(e--) = '\0';
   }
   if (!n[0]) {
      free(n);
      n = 0;
   }
   return n;
}

static QoreString *getIncludeFileName(char *file) {
   //printd(5, "getIncludeFileName(%s)\n", file);
   // FIXME: UNIX-specific
   if (file[0] == '/')
      return new QoreString(file);

   QoreString *rv;

   // check in current directory of script first
   const char *sp = getProgram()->parseGetScriptDir();
   if (sp) {
      rv = findFileInPath(file, sp);
      if (rv)
	 return rv;
   }

   rv = findFileInEnvPath(file, "QORE_INCLUDE_DIR");
   if (!rv) {
       const char *pp = getProgram()->parseGetIncludePath();
       if (pp) 
	   rv = findFileInPath(file, pp);
       if (!rv)
	   rv = new QoreString(file);
   }

   return rv;
}

void setIncludePath(const char *path) {
    char *ip = trim(path);
    if (!ip)
	return;

    QoreString npath; // for new path

    // scan through paths and:
    // 1: do environment variable substitution
    // 2: remove the paths that don't exist
    char *lp, *p;
    p = lp = ip;
    
    while (true) {
	if (*p == ':' || !*p) {
	    // skip if at beginning of new path
	    if (p != lp) {
		QoreString tpath;
		tpath.concat(lp, p - lp);
		const char *sp = tpath.getBuffer();
		//printd(5, "got path string: '%s'\n", sp);

		const char *i, *spt;
		spt = sp;

		while ((i = strchr(spt, '$'))) {
		    // find end of environment variable
		    char *ep = (char *)++i;
		    while (*ep && (*ep == '_' || isalnum(*ep)))
			++ep;
		    spt = i + 1;
		    // perform variable substitution
		    if (ep != i) {
			char save = *ep;
			*ep = '\0';
			//printd(5, "found variable '$%s'\n", i);
			TempString val(SystemEnvironment::get(i));
			*ep = save;
			if (val) {
			    // replace with value
			    int pos = i - sp;			    
			    //printd(5, "got $%s = '%s' (replacing %d char(s))\n", i, val->getBuffer(), ep - i + 1);
			    tpath.replace(pos - 1, ep - i + 1, *val);			    
			    // re-assign sp in case it's changed
			    int diff = pos + val->strlen();
			    sp = tpath.getBuffer();
			    spt = sp + diff;
			    //printd(5, "new string = '%s' ('%s')\n", sp, spt);
			}
		    }
		}
		struct stat sb;
	    
		//printd(5, "trying '%s'\n", sp);
		// add to path list if the directory exists
		if (!stat(sp, &sb)) {
		    //printd(5, "OK: adding '%s' to path list\n", sp);
		    if (npath.strlen()) 
			npath.concat(':');
		    npath.concat(&tpath);
		}

		if (!*p)
		    break;
	    }
	    lp = ++p;
	    continue;
	}

	++p;
    }
    
    //printd(5, "setting include path: '%s'\n", npath.getBuffer());
    getProgram()->parseSetIncludePath(npath.getBuffer());

    free(ip);
}

static inline char *remove_quotes(char *str) {
   str[strlen(str) - 1] = '\0';
   return str + 1;
}

static inline DateTimeNode *makeYears(int years) {
   return new DateTimeNode(years, 0, 0, 0, 0, 0, 0, true);
}

static inline DateTimeNode *makeMonths(int months) {
   return new DateTimeNode(0, months, 0, 0, 0, 0, 0, true);
}

static inline DateTimeNode *makeDays(int days)
{
   return new DateTimeNode(0, 0, days, 0, 0, 0, 0, true);
}

static inline DateTimeNode *makeHours(int hours)
{
   return new DateTimeNode(0, 0, 0, hours, 0, 0, 0, true);
}

static inline DateTimeNode *makeMinutes(int minutes)
{
   return new DateTimeNode(0, 0, 0, 0, minutes, 0, 0, true);
}

static inline DateTimeNode *makeSeconds(int seconds)
{
   return new DateTimeNode(0, 0, 0, 0, 0, seconds, 0, true);
}

static inline DateTimeNode *makeMilliseconds(int ms)
{
   return new DateTimeNode(0, 0, 0, 0, 0, 0, ms, true);
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

static inline DateTimeNode *makeDateTime(char *str)
{
   // move string to middle to form date string
   do_date_time_str(str);
   //printf("new date: %s\n", str + 3);
   return new DateTimeNode(str + 3);
}

static inline DateTimeNode *makeDate(char *str)
{
   do_date_str(str);
   //printf("new date: %d:%s\n", strlen(str), str);
   return new DateTimeNode(str);
}

static inline DateTimeNode *makeTime(char *str)
{
   do_time_str(str);
   //printf("new time: %d:%s\n", strlen(str), str);
   return new DateTimeNode(str);
}

static inline DateTimeNode *makeRelativeDateTime(char *str)
{
   // move string to middle to form date string
   do_date_time_str(str);
   //printf("new date: %s\n", str + 3);
   DateTimeNode *dt = new DateTimeNode();
   dt->setRelativeDate(str + 3);
   return dt;
}

static inline DateTimeNode *makeRelativeDate(char *str)
{
   do_date_str(str);
   //printf("new date: %d:%s\n", strlen(str), str);
   DateTimeNode *dt = new DateTimeNode();
   dt->setRelativeDate(str);
   return dt;
}

static inline DateTimeNode *makeRelativeTime(char *str)
{
   do_time_str(str);
   //printf("new time: %d:%s\n", strlen(str), str);
   DateTimeNode *dt = new DateTimeNode();
   dt->setRelativeDate(str);
   return dt;
}

static inline bool isRegexModifier(QoreRegexNode *qr, int c) {
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

static inline bool isRegexSubstModifier(RegexSubstNode *qr, int c) {
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
%option reentrant bison-bridge bison-locations
%option stack
%option yylineno
%option noyy_push_state
%option noyy_pop_state

%x str_state regex_state incl case_state regex_googleplex regex_negative_universe regex_subst1 regex_subst2 line_comment exec_class_state requires regex_trans1 regex_trans2 regex_extract_state disable_warning enable_warning append_path_state

HEX_DIGIT       [0-9A-Fa-f]
HEX_CONST       0x{HEX_DIGIT}+
OCTAL_CONST     \\[0-7]{1,3}
DIGIT		[0-9]
WORD		[a-zA-Z][a-zA-Z0-9_]*
WS		[ \t]
WSNL		[ \t\r\n]
YEAR            [0-9]{4}
MONTH           (0[1-9])|(1[012])
DAY             ((0[1-9])|([12][0-9])|(3[01]))
HOUR            ([01][0-9])|(2[0-3])
MSEC            [0-5][0-9]
MS              [0-9]{3}
D2              [0-9]{2}
BINARY          <({HEX_DIGIT}{HEX_DIGIT})+>

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
^%no-filesystem{WS}*$                   getProgram()->parseSetParseOptions(PO_NO_FILESYSTEM);
^%lock-options{WS}*$                    getProgram()->lockOptions();
^%lock-warnings{WS}*$                   getProgram()->parseSetParseOptions(PO_LOCK_WARNINGS);
^%no-process-control{WS}*$              getProgram()->parseSetParseOptions(PO_NO_PROCESS_CONTROL);
^%no-constant-defs{WS}*$                getProgram()->parseSetParseOptions(PO_NO_CONSTANT_DEFS);
^%no-new{WS}*$                          getProgram()->parseSetParseOptions(PO_NO_NEW);
^%no-network{WS}*$                      getProgram()->parseSetParseOptions(PO_NO_NETWORK);
^%no-child-restrictions{WS}*$           getProgram()->parseSetParseOptions(PO_NO_CHILD_PO_RESTRICTIONS);
^%no-database{WS}*$                     getProgram()->parseSetParseOptions(PO_NO_DATABASE);
^%no-gui{WS}*$                          getProgram()->parseSetParseOptions(PO_NO_GUI);
^%no-terminal-io{WS}*$                  getProgram()->parseSetParseOptions(PO_NO_TERMINAL_IO);
^%require-our{WS}*$                     getProgram()->parseSetParseOptions(PO_REQUIRE_OUR);
^%append-include-path{WS}+              BEGIN(append_path_state);
<append_path_state>[^\t\n\r]+           {
					   setIncludePath(yytext);
					   BEGIN(INITIAL);
                                        }
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
^%exec-class{WS}*                       BEGIN(exec_class_state);
^%requires                              BEGIN(requires);
<requires>[^\t\n\r]+                    {
                                           char *cn = trim(yytext);
					   //printd(5, "scanner requesting feature: '%s'\n", cn);
					   QoreStringNode *err = MM.parseLoadModule(cn, getProgram());
					   if (err)
					      getProgram()->cannotProvideFeature(err);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
^%include{WS}*$                         { parse_error("missing argument to %%include"); }
^%include{WS}+				BEGIN(incl);
<exec_class_state>{
   [^\t\n\r]+$   	                {
                                           char *cn = trim(yytext);
					   //printf("setting class name to: '%s'\n", cn);
	 				   getProgram()->setExecClass(cn);
					   getProgram()->parseSetParseOptions(PO_NO_TOP_LEVEL_STATEMENTS);
					   free(cn);
					   BEGIN(INITIAL);
                                        }
}
<incl>{WS}*				// ignore white space
<incl>[^\t\n\r]+			{
					   TempString fname(getIncludeFileName(yytext));
					   const char *fn = fname->getBuffer();
					   // remove enclosing quotes if any
					   if (fname->strlen() 
					       && ((fn[0] == '\"' && fn[fname->strlen() - 1] == '\"')
						   || (fn[0] == '\'' && fn[fname->strlen() - 1] == '\''))) {
					      fname->trim(fn[0]);
					   }

					   if (!fname->strlen()) {
					      parse_error("missing argument to %%include", yytext);
					      BEGIN(INITIAL);
					   }
					   else {
					      // re-get the buffer pointer
					      fn = fname->getBuffer();
					      // check if regular file
					      struct stat sbuf;
					      int rc = stat(fn, &sbuf);
					      if (rc) {
						 parse_error("stat() failed on include file: \"%s\": %s", fn, strerror(errno));
						 BEGIN(INITIAL);
					      }
					      else {

						 //printd(0, "%s: mode=%o, s_ifmt=%o, &=%o, reg=%o comp=%s\n", fname->getBuffer(), sbuf.st_mode, S_IFMT, sbuf.st_mode & S_IFMT, S_IFREG, (sbuf.st_mode & S_IFMT) != S_IFREG ? "true" : "false");
						 if ((sbuf.st_mode & S_IFMT) != S_IFREG) {
						    parse_error("cannot include \"%s\"; is not a regular file", fn);
						    BEGIN(INITIAL);
						 }
						 else {
						    FILE *save_yyin = yyin;
						    yyin = fopen(fn, "r");
						    
						    if (!yyin) {
						       parse_error("cannot open include file \"%s\"", fn);
						       yyin = save_yyin;
						       BEGIN(INITIAL);
						    }
						    else {
						       // take string from buffer
						       char *str = fname->giveBuffer();
						       // save file name string in QoreProgram's list - the list now owns the string memory
						       getProgram()->addFile(str);
						       beginParsing(str, (void *)YY_CURRENT_BUFFER);
						       yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE, yyscanner), yyscanner);
						       BEGIN(INITIAL);
						    }
						 }
					      }
					   }
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
\"					yylval->String = new QoreStringNode(); yylloc->setExplicitFirst(yylineno); BEGIN(str_state);
<str_state>{
      \"				BEGIN(INITIAL); return QUOTED_WORD;
      \n				yylval->String->concat('\n');
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
      \\v				yylval->String->concat('\v');
      \\.				yylval->String->concat(yytext[1]);
      \\\n                              yylval->String->concat('\n');
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
      \n				yylval->RegexSubst->concatSource('\n');
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
      \n				yylval->RegexTrans->concatTarget('\n');
      \\n				yylval->RegexTrans->concatTarget('\n');
      \\t				yylval->RegexTrans->concatTarget('\t');
      \\r				yylval->RegexTrans->concatTarget('\r');
      \\b				yylval->RegexTrans->concatTarget('\b');
      \\f				yylval->RegexTrans->concatTarget('\f');
      \\\n                              yylval->RegexTrans->concatTarget('\n');
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
      \n				yylval->RegexTrans->concatSource('\n');
      \\n				yylval->RegexTrans->concatSource('\n');
      \\t				yylval->RegexTrans->concatSource('\t');
      \\r				yylval->RegexTrans->concatSource('\r');
      \\b				yylval->RegexTrans->concatSource('\b');
      \\f				yylval->RegexTrans->concatSource('\f');
      \\\n                              yylval->RegexTrans->concatSource('\n');
      \\\/                              yylval->RegexTrans->concatSource('/');
      \\.				yylval->RegexTrans->concatSource(yytext[1]);
      [^\n\\/\-]+			        {
					   char *yptr = yytext;
					   while (*yptr)
					      yylval->RegexTrans->concatSource(*(yptr++));
					}
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
      \n				yylval->Regex->concat('\n');
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
      \n				yylval->Regex->concat('\n');
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
case                                    BEGIN(case_state); return TOK_CASE;
<case_state>{
   \/                                   yylval->Regex = new QoreRegexNode(); yylloc->setExplicitFirst(yylineno); BEGIN(regex_state);
   {WS}+                                /* ignore */
   [^\/]                                yyless(0); BEGIN(INITIAL);
}
default                                 return TOK_DEFAULT;
inherits                                return TOK_INHERITS;
push                                    return TOK_PUSH;
pop                                     return TOK_POP;
splice                                  return TOK_SPLICE;
instanceof                              return TOK_INSTANCEOF;
chomp					return TOK_CHOMP;
trim					return TOK_TRIM;
on_exit 		 		return TOK_ON_EXIT;
on_success				return TOK_ON_SUCCESS;
on_error 				return TOK_ON_ERROR;
map                                     return TOK_MAP;
foldr                                   return TOK_FOLDR;
foldl                                   return TOK_FOLDL;
select                                  return TOK_SELECT;
static                                  return TOK_STATIC;
class\(                                 yylval->string = strdup("class"); return KW_IDENTIFIER_OPENPAREN;
private\(                               yylval->string = strdup("private"); return KW_IDENTIFIER_OPENPAREN;
new\(                                   yylval->string = strdup("new"); return KW_IDENTIFIER_OPENPAREN;
delete\(                                yylval->string = strdup("delete"); return KW_IDENTIFIER_OPENPAREN;
case\(                                  yylval->string = strdup("case"); return KW_IDENTIFIER_OPENPAREN;
chomp\(                                 yylval->string = strdup("chomp"); return KW_IDENTIFIER_OPENPAREN;
trim\(                                  yylval->string = strdup("trim"); return KW_IDENTIFIER_OPENPAREN;
push\(                                  yylval->string = strdup("push"); return KW_IDENTIFIER_OPENPAREN;
pop\(                                   yylval->string = strdup("pop"); return KW_IDENTIFIER_OPENPAREN;
splice\(                                yylval->string = strdup("splice"); return KW_IDENTIFIER_OPENPAREN;
shift\(                                 yylval->string = strdup("shift"); return KW_IDENTIFIER_OPENPAREN;
unshift\(                               yylval->string = strdup("unshift"); return KW_IDENTIFIER_OPENPAREN;
background\(	   			yylval->string = strdup("background"); return KW_IDENTIFIER_OPENPAREN;
exists\(                                yylval->string = strdup("exists"); return KW_IDENTIFIER_OPENPAREN;
map\(                                   yylval->string = strdup("map"); return KW_IDENTIFIER_OPENPAREN;
foldr\(                                 yylval->string = strdup("foldr"); return KW_IDENTIFIER_OPENPAREN;
foldl\(                                 yylval->string = strdup("foldl"); return KW_IDENTIFIER_OPENPAREN;
select\(                                yylval->string = strdup("select"); return KW_IDENTIFIER_OPENPAREN;
inherits\(                              yylval->string = strdup("inherits"); return KW_IDENTIFIER_OPENPAREN;
default{WS}*\(                          yylval->string = strdup("default"); return KW_IDENTIFIER_OPENPAREN;
static{WS}*\(                           yylval->string = strdup("static"); return KW_IDENTIFIER_OPENPAREN;
\.new[^A-Za-z_0-9]                      {
                                           yylval->String = new QoreStringNode("new"); 
					   if (yytext[4])
					      unput(yytext[4]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.private[^A-Za-z_0-9]                  {
                                           yylval->String = new QoreStringNode("private"); 
					   if (yytext[8])
					      unput(yytext[8]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.class[^A-Za-z_0-9]                    {
                                           yylval->String = new QoreStringNode("class"); 
					   if (yytext[6])
					      unput(yytext[6]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.elements[^A-Za-z_0-9]                 {
                                           yylval->String = new QoreStringNode("elements"); 
					   if (yytext[9])
					      unput(yytext[9]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.delete[^A-Za-z_0-9]                   {
                                           yylval->String = new QoreStringNode("delete"); 
					   if (yytext[7])
					      unput(yytext[7]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.case[^A-Za-z_0-9]                     {
                                           yylval->String = new QoreStringNode("case"); 
					   if (yytext[5])
					      unput(yytext[5]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.default[^A-Za-z_0-9]                  {
                                           yylval->String = new QoreStringNode("default"); 
					   if (yytext[8])
					      unput(yytext[8]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.map[^A-Za-z_0-9]                      {
                                           yylval->String = new QoreStringNode("map"); 
					   if (yytext[4])
					      unput(yytext[4]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.select[^A-Za-z_0-9]                   {
                                           yylval->String = new QoreStringNode("select"); 
					   if (yytext[7])
					      unput(yytext[7]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.keys[^A-Za-z_0-9]                     {
                                           yylval->String = new QoreStringNode("keys"); 
					   if (yytext[5])
					      unput(yytext[5]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.chomp[^A-Za-z_0-9]                    {
                                           yylval->String = new QoreStringNode("chomp"); 
					   if (yytext[6])
					      unput(yytext[6]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.trim[^A-Za-z_0-9]                     {
                                           yylval->String = new QoreStringNode("trim"); 
					   if (yytext[5])
					      unput(yytext[5]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.push[^A-Za-z_0-9]                     {
                                           yylval->String = new QoreStringNode("push"); 
					   if (yytext[5])
					      unput(yytext[5]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.pop[^A-Za-z_0-9]                      {
                                           yylval->String = new QoreStringNode("pop"); 
					   if (yytext[4])
					      unput(yytext[4]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.splice[^A-Za-z_0-9]                   {
                                           yylval->String = new QoreStringNode("splice"); 
					   if (yytext[7])
					      unput(yytext[7]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.shift[^A-Za-z_0-9]                    {
                                           yylval->String = new QoreStringNode("shift"); 
					   if (yytext[6])
					      unput(yytext[6]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.unshift[^A-Za-z_0-9]                  {
                                           yylval->String = new QoreStringNode("unshift"); 
					   if (yytext[8])
					      unput(yytext[8]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.background[^A-Za-z_0-9]               {
                                           yylval->String = new QoreStringNode("background"); 
					   if (yytext[11])
					      unput(yytext[11]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.find[^A-Za-z_0-9]                     {
                                           yylval->String = new QoreStringNode("find"); 
					   if (yytext[5])
					      unput(yytext[5]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.exists[^A-Za-z_0-9]                   {
                                           yylval->String = new QoreStringNode("exists"); 
					   if (yytext[7])
					      unput(yytext[7]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.foldr[^A-Za-z_0-9]                    {
                                           yylval->String = new QoreStringNode("foldr"); 
					   if (yytext[6])
					      unput(yytext[6]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.foldl[^A-Za-z_0-9]                    {
                                           yylval->String = new QoreStringNode("foldl"); 
					   if (yytext[6])
					      unput(yytext[6]);
					   return DOT_KW_IDENTIFIER;
                                        }
\.static[^A-Za-z_0-9]                   {
                                           yylval->String = new QoreStringNode("static"); 
					   if (yytext[7])
					      unput(yytext[7]);
					   return DOT_KW_IDENTIFIER;
                                        }
{YEAR}-{MONTH}-{DAY}[T-]{HOUR}:{MSEC}:{MSEC}(\.{MS})?   yylval->datetime = makeDateTime(yytext); return DATETIME;
{YEAR}-{MONTH}-{DAY}                    yylval->datetime = makeDate(yytext); return DATETIME;
{HOUR}:{MSEC}:{MSEC}(\.{MS})?           yylval->datetime = makeTime(yytext); return DATETIME;
P{YEAR}-{D2}-{D2}T{D2}:{D2}:{D2}(\.{MS})?   yylval->datetime = makeRelativeDateTime(yytext+1); return DATETIME;
P{YEAR}-{D2}-{D2}                       yylval->datetime = makeRelativeDate(yytext+1); return DATETIME;
PT{D2}:{D2}:{D2}(\.{MS})?               yylval->datetime = makeRelativeTime(yytext+2); return DATETIME;
P{D2}:{D2}:{D2}(\.{MS})?                yylval->datetime = makeRelativeTime(yytext+1); return DATETIME;
({WORD}::)+{WORD}                       yylval->string = strdup(yytext); return SCOPED_REF;
({WORD}::)+\$\.{WORD}                   yylval->nscope = new NamedScope(strdup(yytext)); yylval->nscope->fixBCCall(); return BASE_CLASS_CALL;
{DIGIT}+"."{DIGIT}+			yylval->decimal = strtod(yytext, 0); return QFLOAT;
0[0-7]+				        yylval->integer = strtoll(yytext+1, 0, 8); return INTEGER;
{DIGIT}+				yylval->integer = strtoll(yytext, 0, 10); return INTEGER;
{DIGIT}+Y                               yylval->datetime = makeYears(strtol(yytext, 0, 10));   return DATETIME;
{DIGIT}+M                               yylval->datetime = makeMonths(strtol(yytext, 0, 10));  return DATETIME;
{DIGIT}+D                               yylval->datetime = makeDays(strtol(yytext, 0, 10));    return DATETIME;
{DIGIT}+h                               yylval->datetime = makeHours(strtol(yytext, 0, 10));   return DATETIME;
{DIGIT}+ms                              yylval->datetime = makeMilliseconds(strtol(yytext, 0, 10)); return DATETIME;
{DIGIT}+m                               yylval->datetime = makeMinutes(strtol(yytext, 0, 10)); return DATETIME;
{DIGIT}+s                               yylval->datetime = makeSeconds(strtol(yytext, 0, 10)); return DATETIME;
{HEX_CONST}				yylval->integer = strtoll(yytext, 0, 16); return INTEGER;
{BINARY}                                yylval->binary = parseHex(yytext + 1, strlen(yytext + 1) - 1); return BINARY;
\$\.{WORD}                              yylval->string = strdup(yytext + 2); return SELF_REF;
\${WORD}				yylval->string = strdup(yytext + 1); return VAR_REF;
{WORD}					yylval->string = strdup(yytext); return IDENTIFIER;
\%{WORD}                                yylval->string = strdup(yytext + 1); return CONTEXT_REF;
\%{WORD}\:{WORD}                        yylval->string = strdup(yytext + 1); return COMPLEX_CONTEXT_REF;
\%\%                                    return TOK_CONTEXT_ROW;
\`[^`]*\`                               yylval->string = strdup(remove_quotes(yytext)); return BACKQUOTE;
\'[^\']*\'				yylval->String = new QoreStringNode(remove_quotes(yytext)); return QUOTED_WORD;
\$\$                                    yylval->implicit_arg = new QoreImplicitArgumentNode(-1); return IMPLICIT_ARG_REF;
\$[0-9][0-9]*                           yylval->implicit_arg = new QoreImplicitArgumentNode(strtol(yytext + 1, 0, 0)); return IMPLICIT_ARG_REF;
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

\/\*                                    {
                                           int c;
					   while ((c = yyinput(yyscanner))) {
					      if (c == '*') {
						 do
						    c = yyinput(yyscanner);
						 while (c == '*');
						 if (c == '/') 
						    break;
					      }
					      if (c == EOF)
						 yyterminate();
					   }
                                        }
<regex_googleplex>{
   s\/                                  yylval->RegexSubst = new RegexSubstNode(); yylloc->setExplicitFirst(yylineno); BEGIN(regex_subst1);
   x\/                                  yylval->Regex      = new QoreRegexNode();  yylloc->setExplicitFirst(yylineno); BEGIN(regex_extract_state);
   tr\/                                 yylval->RegexTrans = new RegexTransNode(); yylloc->setExplicitFirst(yylineno); BEGIN(regex_trans1);
   m\/                                  yylval->Regex      = new QoreRegexNode();  yylloc->setExplicitFirst(yylineno); BEGIN(regex_state); 
   \/                                   yylval->Regex      = new QoreRegexNode();  yylloc->setExplicitFirst(yylineno); BEGIN(regex_state);
   {WSNL}+                              /* ignore whitespace */
   [^sxmt\/]                            parse_error("missing regular expression after =~"); BEGIN(INITIAL);
}
<regex_negative_universe>{
   m\/                                  yylval->Regex      = new QoreRegexNode();  yylloc->setExplicitFirst(yylineno); BEGIN(regex_state); 
   \/                                   yylval->Regex      = new QoreRegexNode();  yylloc->setExplicitFirst(yylineno); BEGIN(regex_state);
   {WSNL}+                              /* ignore whitespace */
   [^m\/]                               parse_error("missing regular expression after !~"); BEGIN(INITIAL);
}
=\~                                     BEGIN(regex_googleplex); return REGEX_MATCH;
\!\~                                    BEGIN(regex_negative_universe); return REGEX_NMATCH;
{WSNL}+					/* ignore whitespace */
.					return yytext[0];
%%
