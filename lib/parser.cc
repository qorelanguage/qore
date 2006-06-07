/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_CLASS = 258,
     TOK_NEW = 259,
     TOK_OBJECT_REF = 260,
     TOK_RETURN = 261,
     TOK_MY = 262,
     TOK_DO = 263,
     TOK_TRY = 264,
     TOK_THROW = 265,
     TOK_CATCH = 266,
     TOK_FINALLY = 267,
     TOK_WHERE = 268,
     TOK_NULL = 269,
     P_INCREMENT = 270,
     P_DECREMENT = 271,
     TOK_WHILE = 272,
     TOK_IF = 273,
     TOK_FOR = 274,
     TOK_SUB = 275,
     TOK_THREAD_EXIT = 276,
     TOK_BREAK = 277,
     TOK_CONTINUE = 278,
     TOK_NOTHING = 279,
     TOK_CONTEXT_ROW = 280,
     TOK_FIND = 281,
     TOK_AUTOCOMMIT = 282,
     TOK_FOREACH = 283,
     TOK_IN = 284,
     TOK_DELETE = 285,
     TOK_PRIVATE = 286,
     TOK_BACKGROUND = 287,
     TOK_SYNCHRONIZED = 288,
     TOK_CONTEXT = 289,
     TOK_SORT_BY = 290,
     TOK_SUB_CONTEXT = 291,
     TOK_CONST = 292,
     TOK_SUMMARIZE = 293,
     TOK_BY = 294,
     TOK_SORT_DESCENDING_BY = 295,
     TOK_NAMESPACE = 296,
     TOK_SCOPED_REF = 297,
     TOK_OUR = 298,
     TOK_RETHROW = 299,
     TOK_SWITCH = 300,
     TOK_CASE = 301,
     TOK_DEFAULT = 302,
     TOK_INHERITS = 303,
     INTEGER = 304,
     FLOAT = 305,
     IDENTIFIER = 306,
     VAR_REF = 307,
     BACKQUOTE = 308,
     SELF_REF = 309,
     KW_IDENTIFIER_OPENPAREN = 310,
     SCOPED_REF = 311,
     CONTEXT_REF = 312,
     COMPLEX_CONTEXT_REF = 313,
     DATETIME = 314,
     QUOTED_WORD = 315,
     REGEX_SUBST = 316,
     REGEX_TRANS = 317,
     BASE_CLASS_CALL = 318,
     REGEX = 319,
     IFX = 320,
     TOK_ELSE = 321,
     SHIFT_RIGHT_EQUALS = 322,
     SHIFT_LEFT_EQUALS = 323,
     XOR_EQUALS = 324,
     DIVIDE_EQUALS = 325,
     MULTIPLY_EQUALS = 326,
     MODULA_EQUALS = 327,
     OR_EQUALS = 328,
     AND_EQUALS = 329,
     MINUS_EQUALS = 330,
     PLUS_EQUALS = 331,
     TOK_SPLICE = 332,
     TOK_PUSH = 333,
     TOK_UNSHIFT = 334,
     LOGICAL_OR = 335,
     LOGICAL_AND = 336,
     REGEX_NMATCH = 337,
     REGEX_MATCH = 338,
     ABSOLUTE_NE = 339,
     ABSOLUTE_EQ = 340,
     LOGICAL_CMP = 341,
     LOGICAL_GE = 342,
     LOGICAL_LE = 343,
     LOGICAL_NE = 344,
     LOGICAL_EQ = 345,
     TOK_INSTANCEOF = 346,
     TOK_EXISTS = 347,
     SHIFT_LEFT = 348,
     SHIFT_RIGHT = 349,
     TOK_KEYS = 350,
     TOK_ELEMENTS = 351,
     TOK_POP = 352,
     TOK_SHIFT = 353,
     NEG = 354
   };
#endif
/* Tokens.  */
#define TOK_CLASS 258
#define TOK_NEW 259
#define TOK_OBJECT_REF 260
#define TOK_RETURN 261
#define TOK_MY 262
#define TOK_DO 263
#define TOK_TRY 264
#define TOK_THROW 265
#define TOK_CATCH 266
#define TOK_FINALLY 267
#define TOK_WHERE 268
#define TOK_NULL 269
#define P_INCREMENT 270
#define P_DECREMENT 271
#define TOK_WHILE 272
#define TOK_IF 273
#define TOK_FOR 274
#define TOK_SUB 275
#define TOK_THREAD_EXIT 276
#define TOK_BREAK 277
#define TOK_CONTINUE 278
#define TOK_NOTHING 279
#define TOK_CONTEXT_ROW 280
#define TOK_FIND 281
#define TOK_AUTOCOMMIT 282
#define TOK_FOREACH 283
#define TOK_IN 284
#define TOK_DELETE 285
#define TOK_PRIVATE 286
#define TOK_BACKGROUND 287
#define TOK_SYNCHRONIZED 288
#define TOK_CONTEXT 289
#define TOK_SORT_BY 290
#define TOK_SUB_CONTEXT 291
#define TOK_CONST 292
#define TOK_SUMMARIZE 293
#define TOK_BY 294
#define TOK_SORT_DESCENDING_BY 295
#define TOK_NAMESPACE 296
#define TOK_SCOPED_REF 297
#define TOK_OUR 298
#define TOK_RETHROW 299
#define TOK_SWITCH 300
#define TOK_CASE 301
#define TOK_DEFAULT 302
#define TOK_INHERITS 303
#define INTEGER 304
#define FLOAT 305
#define IDENTIFIER 306
#define VAR_REF 307
#define BACKQUOTE 308
#define SELF_REF 309
#define KW_IDENTIFIER_OPENPAREN 310
#define SCOPED_REF 311
#define CONTEXT_REF 312
#define COMPLEX_CONTEXT_REF 313
#define DATETIME 314
#define QUOTED_WORD 315
#define REGEX_SUBST 316
#define REGEX_TRANS 317
#define BASE_CLASS_CALL 318
#define REGEX 319
#define IFX 320
#define TOK_ELSE 321
#define SHIFT_RIGHT_EQUALS 322
#define SHIFT_LEFT_EQUALS 323
#define XOR_EQUALS 324
#define DIVIDE_EQUALS 325
#define MULTIPLY_EQUALS 326
#define MODULA_EQUALS 327
#define OR_EQUALS 328
#define AND_EQUALS 329
#define MINUS_EQUALS 330
#define PLUS_EQUALS 331
#define TOK_SPLICE 332
#define TOK_PUSH 333
#define TOK_UNSHIFT 334
#define LOGICAL_OR 335
#define LOGICAL_AND 336
#define REGEX_NMATCH 337
#define REGEX_MATCH 338
#define ABSOLUTE_NE 339
#define ABSOLUTE_EQ 340
#define LOGICAL_CMP 341
#define LOGICAL_GE 342
#define LOGICAL_LE 343
#define LOGICAL_NE 344
#define LOGICAL_EQ 345
#define TOK_INSTANCEOF 346
#define TOK_EXISTS 347
#define SHIFT_LEFT 348
#define SHIFT_RIGHT 349
#define TOK_KEYS 350
#define TOK_ELEMENTS 351
#define TOK_POP 352
#define TOK_SHIFT 353
#define NEG 354




/* Copy the first part of user declarations.  */
#line 1 "parser.yy"

/*
   parser.yy

   Qore Programming Language

   Copyright (C) 2003, 2004, 2005 David Nichols

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
*/

#include <qore/config.h>
#include <qore/QoreNode.h>
#include <qore/Operator.h>
#include <qore/Function.h>
#include <qore/Statement.h>
#include <qore/Variable.h>
#include <qore/support.h>
#include <qore/Context.h>
#include <qore/Object.h>
#include <qore/QoreString.h>
#include <qore/QoreClass.h>
#include <qore/thread.h>
#include <qore/QoreProgram.h>
#include <qore/Find.h>
#include <qore/Namespace.h>
#include <qore/ParserSupport.h>
#include <qore/DateTime.h>
#include <qore/RegexSubst.h>
#include <qore/QoreRegex.h>
#include <qore/RegexTrans.h>

#include "parser.h"

#include <stdio.h>
#include <string.h>

#define YYINITDEPTH 300
//#define YYDEBUG 1

/*
int yywrap()
{
   return 1;
}
*/

extern class Operator **ops;

static QoreNode *process_dot(class QoreNode *l, class QoreNode *r)
{
   if (r->type == NT_BAREWORD)
   {
      class QoreNode *rv = makeTree(OP_OBJECT_REF, l, new QoreNode(r->val.c_str));
      r->deref(NULL);
      return rv;
   }
   else if (r->type == NT_FUNCTION_CALL && r->val.fcall->type == FC_UNRESOLVED)
   {
      r->val.fcall->type = FC_METHOD;
      return makeTree(OP_OBJECT_FUNC_REF, l, r);
   }

   return makeTree(OP_OBJECT_REF, l, r);
}

// returns 0 for OK, -1 for error
static int check_lvalue(class QoreNode *node)
{
   //printd(5, "type=%s\n", node->type->name);
   if (node->type == NT_VARREF)
   {
      return 0;
   }
   if (node->type == NT_TREE)
   {
      if (node->val.tree.op == OP_LIST_REF || node->val.tree.op == OP_OBJECT_REF)
	 return check_lvalue(node->val.tree.left);
      else
	 return -1;
   }
   if (node->type == NT_SELF_VARREF)
      return 0;
   return -1;
}

static inline int check_vars(class QoreNode *n)
{
   if (n->type == NT_LIST)
   {
      for (int i = 0; i < n->val.list->size(); i++)
         if (n->val.list->retrieve_entry(i)->type != NT_VARREF)
	    return 1;
      return 0;
   }
   return check_lvalue(n);
}

// returns true if the node needs run-time evaluation, false if not
bool needsEval(class QoreNode *n)
{
   if (!n)
      return false;

   // if it's a constant
   if (n->type == NT_BAREWORD || n->type == NT_CONSTANT)
      return false;

   if (n->type == NT_FLIST)
      n->type = NT_LIST;

   if (n->type == NT_LIST)
   {
      for (int i = 0; i < n->val.list->size(); i++)
	 if (needsEval(n->val.list->retrieve_entry(i)))
	    return true;
      // here we set needs_eval to false so the list won't be evaluated again
      n->val.list->needs_eval = false;
      return false;
   }

   if (n->type == NT_HASH)
   {
      class HashIterator *hi = n->val.hash->newIterator();
      while (hi->next())
	 if (needsEval(hi->getValue()))
	 {
	    delete hi;
	    return true;
	 }
      delete hi;
      // here we set needs_eval to false so the hash won't be evaluated again
      n->val.hash->needs_eval = false;
      return false;
   }
   
   if (n->type->isValue())
      return false;

   if (n->type == NT_TREE)
   {
      if (needsEval(n->val.tree.left) || (n->val.tree.right && needsEval(n->val.tree.right)))
      {
	 return true;
      }
      return n->val.tree.op->hasEffect();
   }

   //printd(0, "needsEval() type %s = true\n", n->type->name);
   return true;
}

static bool hasEffect(class QoreNode *n)
{
   // check for statements with no effect and issue parser error
   if (n->type == NT_FUNCTION_CALL || n->type ==  NT_FIND)
      return true;

   if (n->type == NT_TREE)
   {
      // FIXME: this is not a good approach
      if (n->val.tree.op == OP_ASSIGNMENT)
      {
	 // if the statement is a single, non-nested assignment
	 // then change the operator type to avoid extra
	 // evaluations
	 n->val.tree.op = OP_SINGLE_ASSIGN;
	 return true;
      }
      return n->val.tree.op->hasEffect();
   }

   //printd(5, "hasEffect() node %08x type=%s op=%d ok=%d\n", n, n->type->name, n->type == NT_TREE ? n->val.tree.op : -1, ok);
   return false;
}

#define OFM_PRIVATE 1
#define OFM_SYNCED  2

static inline void tryAddMethod(int mod, char *n, QoreNode *params, BCAList *bcal, StatementBlock *b)
{
   class NamedScope *name = new NamedScope(n);
   if (bcal && strcmp(name->getIdentifier(), "constructor"))
   {
      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
      if (params)
	 params->deref(NULL);
      bcal->deref();
      if (b)
	 delete b;
   }
   else
   {
      class Method *method = new Method(new UserFunction(strdup(name->getIdentifier()), new Paramlist(params), b, mod & OFM_SYNCED), mod & OFM_PRIVATE, bcal);
      
      if (addMethodToClass(name, method))
	 delete method;
   }
   delete name;
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 224 "parser.yy"
typedef union YYSTYPE {
      int i4;
      int64 integer;
      double decimal;
      class QoreString *String;
      char *string;
      class QoreNode *node;
      class Statement *statement;
      class StatementBlock *sblock;
      class ContextModList *cmods;
      class ContextMod *cmod;
      class HashElement *hashelement;
      class UserFunction *userfunc;	
      class Method *objectfunc;
      class MemberList *privlist;
      class QoreClass *qoreclass;
      class ConstNode *constnode;
      class Namespace *ns;
      class NSNode *nsn;
      class ObjClassDef *objdef;
      class DateTime *datetime;
      class RegexSubst *RegexSubst;
      class RegexTrans *RegexTrans;
      class SwitchStatement *switchstmt;
      class CaseNode *casenode;
      class BCList *sclist;
      class BCNode *sclnode;
      class BCAList *bcalist;
      class BCANode *bcanode;
      class NamedScope *nscope;
      class QoreRegex *Regex;
} YYSTYPE;
/* Line 196 of yacc.c.  */
#line 531 "parser.cc"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 257 "parser.yy"

//#define YYSTYPE char const *

//#define LEX_PARAMETERS YYSTYPE *lvalp, struct YYLTYPE *llocp
//#define ERROR_PARAMETERS struct YYLTYPE *llocp, char const *s
#define LEX_PARAMETERS YYSTYPE *lvalp, yyscan_t scanner
//#define LEX_PARAMETERS void
//#define ERROR_PARAMETERS char const *s
int yylex(LEX_PARAMETERS);
//int yylex();

void yyerror(yyscan_t scanner, const char *str)
{
   parse_error("%s", str);
}



/* Line 219 of yacc.c.  */
#line 560 "parser.cc"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  151
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4266

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  125
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  44
/* YYNRULES -- Number of rules. */
#define YYNRULES  209
/* YYNRULES -- Number of states. */
#define YYNSTATES  471

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   354

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   116,     2,     2,     2,   106,    86,     2,
     122,   123,   107,   104,    81,   105,   119,   108,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    83,   121,
      89,    77,    90,    82,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   118,   115,   124,    88,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   117,    87,   120,   114,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    78,    79,    80,    84,    85,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     109,   110,   111,   112,   113
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    18,
      20,    24,    26,    32,    36,    38,    41,    43,    45,    47,
      49,    55,    61,    65,    68,    70,    72,    74,    77,    78,
      80,    83,    85,    88,    92,    96,   108,   116,   122,   130,
     136,   144,   154,   162,   165,   168,   171,   174,   178,   180,
     183,   184,   187,   192,   197,   202,   204,   207,   215,   217,
     220,   225,   229,   233,   236,   244,   245,   247,   254,   261,
     266,   271,   277,   283,   286,   287,   289,   293,   295,   297,
     300,   303,   305,   307,   310,   313,   317,   319,   323,   331,
     338,   345,   351,   359,   366,   369,   370,   372,   376,   381,
     386,   388,   391,   393,   395,   402,   410,   416,   423,   428,
     432,   437,   442,   447,   451,   454,   456,   460,   463,   467,
     471,   475,   477,   479,   483,   485,   487,   490,   495,   498,
     503,   505,   507,   509,   511,   520,   524,   528,   532,   536,
     540,   544,   548,   552,   556,   560,   564,   567,   570,   574,
     578,   581,   584,   587,   590,   593,   596,   602,   605,   608,
     611,   614,   616,   618,   620,   624,   628,   632,   636,   640,
     644,   648,   652,   656,   660,   664,   668,   672,   676,   680,
     684,   688,   692,   696,   700,   704,   708,   712,   716,   720,
     724,   728,   731,   734,   737,   740,   743,   746,   749,   751,
     756,   761,   765,   769,   772,   774,   776,   778,   780,   782
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
{
     126,     0,    -1,   127,    -1,   126,   127,    -1,   160,    -1,
     146,    -1,   132,    -1,   131,    -1,   154,    -1,   137,    -1,
     117,   135,   120,    -1,   128,    -1,    41,    51,   117,   129,
     120,    -1,    41,    51,   121,    -1,   130,    -1,   129,   130,
      -1,   132,    -1,   131,    -1,   146,    -1,   128,    -1,    37,
      51,    77,   167,   121,    -1,    37,    56,    77,   167,   121,
      -1,   117,   135,   120,    -1,   117,   120,    -1,   137,    -1,
     133,    -1,   137,    -1,   135,   137,    -1,    -1,    51,    -1,
     167,   121,    -1,   144,    -1,    44,   121,    -1,    10,   167,
     121,    -1,    36,   138,   134,    -1,    38,   136,   122,   167,
     123,    39,   122,   167,   123,   138,   134,    -1,    34,   136,
     122,   167,   123,   138,   134,    -1,    18,   122,   167,   123,
     134,    -1,    18,   122,   167,   123,   134,    66,   134,    -1,
      17,   122,   167,   123,   134,    -1,     8,   134,    17,   122,
     167,   123,   121,    -1,    19,   122,   145,   121,   145,   121,
     145,   123,   134,    -1,    28,   167,    29,   122,   167,   123,
     134,    -1,   140,   121,    -1,    21,   121,    -1,    22,   121,
      -1,    23,   121,    -1,    30,   167,   121,    -1,   141,    -1,
       1,   121,    -1,    -1,   138,   139,    -1,    13,   122,   167,
     123,    -1,    35,   122,   167,   123,    -1,    40,   122,   167,
     123,    -1,     6,    -1,     6,   167,    -1,    45,   122,   167,
     123,   117,   142,   120,    -1,   143,    -1,   142,   143,    -1,
      46,   167,    83,   135,    -1,    46,   167,    83,    -1,    47,
      83,   135,    -1,    47,    83,    -1,     9,   134,    11,   122,
     145,   123,   134,    -1,    -1,   167,    -1,     3,    51,   147,
     117,   150,   120,    -1,     3,    56,   147,   117,   150,   120,
      -1,     3,    51,   147,   121,    -1,     3,    56,   147,   121,
      -1,     3,    51,   147,   117,   120,    -1,     3,    56,   147,
     117,   120,    -1,    48,   148,    -1,    -1,   149,    -1,   148,
      81,   149,    -1,    51,    -1,    56,    -1,    31,    51,    -1,
      31,    56,    -1,   153,    -1,   151,    -1,   150,   153,    -1,
     150,   151,    -1,    31,   152,   121,    -1,    54,    -1,   152,
      81,    54,    -1,   158,    51,   122,   145,   123,   155,   133,
      -1,    51,   122,   145,   123,   155,   133,    -1,   158,    55,
     145,   123,   155,   133,    -1,    55,   145,   123,   155,   133,
      -1,   158,    56,   122,   145,   123,   155,   133,    -1,    56,
     122,   145,   123,   155,   133,    -1,    83,   156,    -1,    -1,
     157,    -1,   156,    81,   157,    -1,    51,   122,   145,   123,
      -1,    56,   122,   145,   123,    -1,   159,    -1,   158,   159,
      -1,    31,    -1,    33,    -1,    20,    51,   122,   145,   123,
     133,    -1,    33,    20,    51,   122,   145,   123,   133,    -1,
      20,    55,   145,   123,   133,    -1,    33,    20,    55,   145,
     123,   133,    -1,    51,   122,   145,   123,    -1,    55,   145,
     123,    -1,    56,   122,   145,   123,    -1,    54,   122,   145,
     123,    -1,    63,   122,   145,   123,    -1,   167,    81,   167,
      -1,   167,    81,    -1,   166,    -1,   165,    81,   166,    -1,
     165,    81,    -1,   168,    83,   167,    -1,    51,    83,   167,
      -1,    56,    83,   167,    -1,   168,    -1,   164,    -1,   122,
     165,   123,    -1,    56,    -1,    52,    -1,     7,    52,    -1,
       7,   122,   164,   123,    -1,    43,    52,    -1,    43,   122,
     164,   123,    -1,    51,    -1,    57,    -1,    25,    -1,    58,
      -1,    26,   167,    29,   167,    13,   122,   167,   123,    -1,
     167,    76,   167,    -1,   167,    75,   167,    -1,   167,    74,
     167,    -1,   167,    73,   167,    -1,   167,    72,   167,    -1,
     167,    71,   167,    -1,   167,    70,   167,    -1,   167,    69,
     167,    -1,   167,    68,   167,    -1,   167,    67,   167,    -1,
     167,    77,   167,    -1,   101,   167,    -1,   110,   167,    -1,
     167,   100,    51,    -1,   167,   100,    56,    -1,   109,   167,
      -1,    80,   167,    -1,   112,   167,    -1,    79,   167,    -1,
     111,   167,    -1,    78,   167,    -1,   167,    82,   167,    83,
     167,    -1,    15,   167,    -1,   167,    15,    -1,    16,   167,
      -1,   167,    16,    -1,   161,    -1,   163,    -1,    54,    -1,
     167,    85,   167,    -1,   167,    84,   167,    -1,   167,    87,
     167,    -1,   167,    86,   167,    -1,   167,    88,   167,    -1,
     167,    92,    64,    -1,   167,    91,    64,    -1,   167,    92,
     167,    -1,   167,    91,   167,    -1,   167,    92,    61,    -1,
     167,    92,    62,    -1,   167,    90,   167,    -1,   167,    89,
     167,    -1,   167,    95,   167,    -1,   167,    99,   167,    -1,
     167,    94,   167,    -1,   167,    93,   167,    -1,   167,    98,
     167,    -1,   167,    97,   167,    -1,   167,    96,   167,    -1,
     167,   102,   167,    -1,   167,   103,   167,    -1,   167,   104,
     167,    -1,   167,   105,   167,    -1,   167,   106,   167,    -1,
     167,   108,   167,    -1,   167,   107,   167,    -1,   105,   167,
      -1,   114,   167,    -1,   116,   167,    -1,   115,   167,    -1,
       4,   161,    -1,     4,   162,    -1,    32,   167,    -1,    53,
      -1,   167,   118,   167,   124,    -1,   167,   117,   167,   120,
      -1,   167,   119,   167,    -1,   122,   167,   123,    -1,   122,
     123,    -1,    50,    -1,    49,    -1,    60,    -1,    59,    -1,
      14,    -1,    24,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   372,   372,   373,   377,   378,   387,   397,   406,   407,
     419,   423,   434,   436,   441,   447,   455,   463,   471,   479,
     489,   498,   507,   510,   515,   518,   523,   526,   532,   533,
     537,   546,   548,   552,   557,   562,   578,   584,   589,   594,
     599,   604,   609,   616,   617,   625,   629,   633,   640,   641,
     646,   648,   661,   663,   665,   670,   672,   676,   685,   689,
     697,   703,   709,   713,   720,   739,   740,   748,   754,   760,
     766,   773,   779,   789,   794,   798,   802,   809,   813,   817,
     821,   828,   833,   838,   843,   851,   858,   862,   871,   877,
     883,   889,   898,   902,   909,   914,   920,   924,   932,   936,
     943,   944,   953,   954,   958,   965,   972,   979,   989,   994,
    1002,  1010,  1015,  1026,  1028,  1038,  1045,  1051,  1056,  1058,
    1060,  1064,  1066,  1068,  1070,  1074,  1076,  1080,  1093,  1098,
    1114,  1116,  1122,  1124,  1126,  1131,  1141,  1152,  1162,  1172,
    1182,  1192,  1202,  1212,  1222,  1232,  1265,  1267,  1269,  1273,
    1277,  1279,  1299,  1309,  1330,  1340,  1359,  1361,  1371,  1381,
    1391,  1401,  1402,  1403,  1404,  1405,  1406,  1407,  1408,  1409,
    1413,  1417,  1423,  1429,  1442,  1455,  1456,  1457,  1458,  1459,
    1460,  1461,  1462,  1463,  1464,  1465,  1466,  1467,  1468,  1469,
    1470,  1471,  1472,  1473,  1474,  1481,  1491,  1498,  1507,  1514,
    1515,  1516,  1520,  1526,  1530,  1531,  1532,  1533,  1534,  1535
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_CLASS", "TOK_NEW", "TOK_OBJECT_REF",
  "TOK_RETURN", "TOK_MY", "TOK_DO", "TOK_TRY", "TOK_THROW", "TOK_CATCH",
  "TOK_FINALLY", "TOK_WHERE", "TOK_NULL", "P_INCREMENT", "P_DECREMENT",
  "TOK_WHILE", "TOK_IF", "TOK_FOR", "TOK_SUB", "TOK_THREAD_EXIT",
  "TOK_BREAK", "TOK_CONTINUE", "TOK_NOTHING", "TOK_CONTEXT_ROW",
  "TOK_FIND", "TOK_AUTOCOMMIT", "TOK_FOREACH", "TOK_IN", "TOK_DELETE",
  "TOK_PRIVATE", "TOK_BACKGROUND", "TOK_SYNCHRONIZED", "TOK_CONTEXT",
  "TOK_SORT_BY", "TOK_SUB_CONTEXT", "TOK_CONST", "TOK_SUMMARIZE", "TOK_BY",
  "TOK_SORT_DESCENDING_BY", "TOK_NAMESPACE", "TOK_SCOPED_REF", "TOK_OUR",
  "TOK_RETHROW", "TOK_SWITCH", "TOK_CASE", "TOK_DEFAULT", "TOK_INHERITS",
  "INTEGER", "FLOAT", "IDENTIFIER", "VAR_REF", "BACKQUOTE", "SELF_REF",
  "KW_IDENTIFIER_OPENPAREN", "SCOPED_REF", "CONTEXT_REF",
  "COMPLEX_CONTEXT_REF", "DATETIME", "QUOTED_WORD", "REGEX_SUBST",
  "REGEX_TRANS", "BASE_CLASS_CALL", "REGEX", "IFX", "TOK_ELSE",
  "SHIFT_RIGHT_EQUALS", "SHIFT_LEFT_EQUALS", "XOR_EQUALS", "DIVIDE_EQUALS",
  "MULTIPLY_EQUALS", "MODULA_EQUALS", "OR_EQUALS", "AND_EQUALS",
  "MINUS_EQUALS", "PLUS_EQUALS", "'='", "TOK_SPLICE", "TOK_PUSH",
  "TOK_UNSHIFT", "','", "'?'", "':'", "LOGICAL_OR", "LOGICAL_AND", "'&'",
  "'|'", "'^'", "'<'", "'>'", "REGEX_NMATCH", "REGEX_MATCH", "ABSOLUTE_NE",
  "ABSOLUTE_EQ", "LOGICAL_CMP", "LOGICAL_GE", "LOGICAL_LE", "LOGICAL_NE",
  "LOGICAL_EQ", "TOK_INSTANCEOF", "TOK_EXISTS", "SHIFT_LEFT",
  "SHIFT_RIGHT", "'+'", "'-'", "'%'", "'*'", "'/'", "TOK_KEYS",
  "TOK_ELEMENTS", "TOK_POP", "TOK_SHIFT", "NEG", "'~'", "'\\\\'", "'!'",
  "'{'", "'['", "'.'", "'}'", "';'", "'('", "')'", "']'", "$accept",
  "top_level_commands", "top_level_command", "top_namespace_decl",
  "namespace_decls", "namespace_decl", "unscoped_const_decl",
  "scoped_const_decl", "block", "statement_or_block", "statements",
  "optname", "statement", "context_mods", "context_mod",
  "return_statement", "switch_statement", "case_block", "case_code",
  "try_statement", "myexp", "object_def", "inheritance_list",
  "superclass_list", "superclass", "class_attributes",
  "private_member_list", "member_list", "method_definition",
  "object_outofline_function_def", "base_constructor_list",
  "base_constructors", "base_constructor", "method_modifiers",
  "method_modifier", "sub_def", "function_call", "scoped_object_call",
  "self_function_call", "list", "hash", "hash_element", "exp", "scalar", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,    61,   332,   333,
     334,    44,    63,    58,   335,   336,    38,   124,    94,    60,
      62,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,    43,    45,    37,    42,    47,   350,
     351,   352,   353,   354,   126,    92,    33,   123,    91,    46,
     125,    59,    40,    41,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,   125,   126,   126,   127,   127,   127,   127,   127,   127,
     127,   127,   128,   128,   129,   129,   130,   130,   130,   130,
     131,   132,   133,   133,   134,   134,   135,   135,   136,   136,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     138,   138,   139,   139,   139,   140,   140,   141,   142,   142,
     143,   143,   143,   143,   144,   145,   145,   146,   146,   146,
     146,   146,   146,   147,   147,   148,   148,   149,   149,   149,
     149,   150,   150,   150,   150,   151,   152,   152,   153,   153,
     153,   153,   154,   154,   155,   155,   156,   156,   157,   157,
     158,   158,   159,   159,   160,   160,   160,   160,   161,   161,
     162,   163,   163,   164,   164,   165,   165,   165,   166,   166,
     166,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   168,   168,   168,   168,   168,   168
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       3,     1,     5,     3,     1,     2,     1,     1,     1,     1,
       5,     5,     3,     2,     1,     1,     1,     2,     0,     1,
       2,     1,     2,     3,     3,    11,     7,     5,     7,     5,
       7,     9,     7,     2,     2,     2,     2,     3,     1,     2,
       0,     2,     4,     4,     4,     1,     2,     7,     1,     2,
       4,     3,     3,     2,     7,     0,     1,     6,     6,     4,
       4,     5,     5,     2,     0,     1,     3,     1,     1,     2,
       2,     1,     1,     2,     2,     3,     1,     3,     7,     6,
       6,     5,     7,     6,     2,     0,     1,     3,     4,     4,
       1,     2,     1,     1,     6,     7,     5,     6,     4,     3,
       4,     4,     4,     3,     2,     1,     3,     2,     3,     3,
       3,     1,     1,     3,     1,     1,     2,     4,     2,     4,
       1,     1,     1,     1,     8,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     3,     3,
       2,     2,     2,     2,     2,     2,     5,     2,     2,     2,
       2,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     2,     2,     2,     1,     4,
       4,     3,     3,     2,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,    55,     0,     0,     0,     0,   208,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   209,
     132,     0,     0,     0,   102,     0,   103,    28,    50,     0,
      28,     0,     0,     0,     0,   205,   204,   130,   125,   198,
     163,    65,   124,   131,   133,   207,   206,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     2,    11,     7,     6,     9,     0,    48,
      31,     5,     8,     0,   100,     4,   161,   162,   122,     0,
     121,    49,    74,    74,     0,     0,   195,   196,   124,    56,
     126,     0,     0,    25,     0,    24,     0,     0,   157,   159,
       0,     0,    65,     0,    65,    44,    45,    46,     0,     0,
       0,   197,     0,    29,     0,     0,     0,     0,     0,     0,
     128,     0,    32,     0,    65,    65,     0,    66,    65,    65,
     155,   153,   151,   146,   191,   150,   147,   154,   152,   192,
     194,   193,     0,    26,   130,   124,   203,     0,   115,     0,
     121,     1,     3,    43,   103,     0,   101,   158,   160,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     114,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,     0,     0,    65,   122,     0,    23,     0,     0,     0,
      33,     0,     0,     0,    65,     0,     0,     0,    47,     0,
      65,     0,     0,     0,     0,    34,    51,     0,     0,     0,
       0,    13,   122,     0,     0,     0,   109,     0,     0,    10,
      27,     0,     0,   117,   123,   202,     0,    65,   144,   143,
     142,   141,   140,   139,   138,   137,   136,   135,   145,   113,
       0,   165,   164,   167,   166,   168,   176,   175,   170,   172,
     173,   174,   169,   171,   180,   179,   177,   183,   182,   181,
     178,   148,   149,   184,   185,   186,   187,   188,   190,   189,
       0,     0,   201,     0,    77,    78,    73,    75,     0,    69,
       0,    70,     0,   127,    22,     0,    65,     0,     0,    65,
       0,     0,     0,     0,    65,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,    14,    17,    16,    18,   129,
       0,   108,   111,    95,   112,   119,   120,     0,     0,   116,
       0,   118,     0,     0,   200,   199,    79,    80,     0,   102,
       0,    65,    71,     0,    82,    81,     0,    72,     0,   110,
       0,     0,    39,    37,     0,     0,   106,     0,     0,     0,
       0,    50,     0,     0,     0,    20,    21,     0,    12,    15,
       0,     0,     0,    95,   156,    76,    86,     0,    65,     0,
      67,    84,    83,     0,    65,    68,     0,     0,     0,    65,
     104,     0,     0,     0,   107,     0,    52,    53,    54,     0,
       0,     0,     0,    58,     0,     0,    94,    96,    93,     0,
       0,    85,     0,    95,    65,     0,    40,    64,    38,     0,
       0,    42,   105,    36,     0,     0,     0,    57,    59,    65,
      65,     0,    92,    87,    95,     0,     0,    95,     0,   134,
       0,     0,     0,     0,     0,    97,     0,    91,    95,     0,
      41,    50,     0,    98,    99,    89,     0,    90,     0,    88,
      35
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,    62,    63,    64,   324,   325,    65,    66,    93,    94,
     142,   114,    95,   115,   226,    68,    69,   412,   413,    70,
     126,    71,   201,   296,   297,   353,   354,   387,   355,    72,
     382,   416,   417,   356,    74,    75,    76,    87,    77,    78,
     147,   148,    79,    80
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -360
static const short int yypact[] =
{
     755,   -92,   -20,   -29,  2264,   -39,  1457,  1457,  2264,  -360,
    2264,  2264,   -81,   -67,   -54,    30,   -47,   -30,   -28,  -360,
    -360,  2264,  2264,  2264,  -360,  2264,    79,    49,  -360,   -19,
      49,    60,   -38,    71,    35,  -360,  -360,    76,  -360,  -360,
      81,  2264,    82,  -360,  -360,  -360,  -360,    83,  2264,  2264,
    2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,
    1925,   486,   638,  -360,  -360,  -360,  -360,  -360,    85,  -360,
    -360,  -360,  -360,    -3,  -360,  -360,  -360,  -360,  -360,  3202,
    -360,  -360,   136,   136,    76,    86,  -360,  -360,  -360,  3990,
    -360,  2264,  1574,  -360,   185,  -360,   198,  3265,    33,    33,
    2264,  2264,  2264,    88,  2264,  -360,  -360,  -360,  3707,  3801,
    3328,    36,   113,  -360,    89,   872,   135,   137,    93,    62,
    -360,  2264,  -360,  2264,  2264,  2264,    94,  3990,  2264,  2264,
    3747,  3747,  3747,   315,    36,    36,    36,    36,    36,    36,
      36,    36,  1691,  -360,   -59,   133,  -360,   -64,  -360,  2383,
     138,  -360,  -360,  -360,  -360,    96,  -360,  -360,  -360,  2264,
    2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,
    2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2151,
    2038,  2264,  2264,  2264,  2264,  2264,  2264,  2264,   -18,  2264,
    2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  2264,  -360,
     -12,    70,    80,  2264,    97,  3990,  -360,  1808,   100,   101,
    -360,  2446,  2509,    98,  2264,   130,  2264,   147,  -360,   148,
    2264,  2264,   149,   151,   152,  -360,  -360,  2264,  2264,  2264,
      17,  -360,   143,  2572,   159,   160,  -360,   165,   166,  -360,
    -360,  2264,  2264,   303,  -360,  -360,  2264,  2264,  3990,  3990,
    3990,  3990,  3990,  3990,  3990,  3990,  3990,  3990,  4096,  4147,
    3864,    41,    41,  3361,  3361,  3361,   315,   315,  -360,   315,
    -360,  -360,  -360,   315,   315,   315,   315,   315,   315,   315,
     315,  -360,  -360,   -10,   -10,    55,    55,    63,    36,    36,
    3613,  2320,  -360,   -16,  -360,  -360,   209,  -360,    -8,  -360,
     144,  -360,   168,  -360,  -360,  2264,  2264,  1457,  1457,  2264,
     174,   181,  3424,  2264,  2264,   178,  2635,  2264,  2264,  2264,
    3487,  3550,  2698,  -360,     5,  -360,  -360,  -360,  -360,  -360,
     187,  -360,  -360,   225,  -360,  4053,  4053,   226,   133,  -360,
     138,  4053,   191,  2264,  -360,  -360,  -360,  -360,   -12,   256,
     194,  2264,  -360,   145,  -360,  -360,   245,  -360,   244,  -360,
    2761,   195,  -360,   253,   201,   181,  -360,   202,  2824,   200,
     181,  -360,  2887,  2950,  3013,  -360,  -360,   286,  -360,  -360,
      19,    -6,   181,   225,  4147,  -360,  -360,   -60,  2264,   203,
    -360,  -360,  -360,   206,  2264,  -360,   208,  1457,  1457,  2264,
    -360,  2264,  1457,   181,  -360,   872,  -360,  -360,  -360,   210,
    2264,   250,   -31,  -360,   212,   215,   257,  -360,  -360,   181,
     288,  -360,   220,   225,  2264,   223,  -360,  -360,  -360,   227,
    3076,  -360,  -360,  -360,  2264,  3927,   989,  -360,  -360,  2264,
    2264,    -6,  -360,  -360,   225,   181,   228,   225,  1457,  -360,
    3139,  1106,  1223,   233,   234,  -360,   181,  -360,   225,   181,
    -360,  -360,  1340,  -360,  -360,  -360,   181,  -360,   872,  -360,
    -360
};

/* YYPGOTO[NTERM-NUM].  */
static const short int yypgoto[] =
{
    -360,  -360,   287,  -223,  -360,    24,  -221,  -220,   -98,    -5,
     -91,   335,     0,  -359,  -360,  -360,  -360,  -360,   -46,  -360,
     211,  -219,   284,  -360,    21,    72,  -286,  -360,  -276,  -360,
    -103,  -360,   -68,    18,   -69,  -360,   372,  -360,  -360,   -57,
    -360,   134,    65,   -58
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -64
static const short int yytable[] =
{
      67,   207,    96,   150,   156,   157,   158,   323,     2,   326,
     327,   328,   405,    90,   120,   410,   411,   243,    73,   293,
       2,   420,    84,   349,   241,   154,    41,    85,    24,    81,
     154,    82,   116,   281,   204,   346,    83,   117,   282,   294,
     347,   100,    29,   350,   295,   414,    31,   351,   -64,   -64,
     415,   157,   158,   155,    29,   101,   157,   158,    31,   244,
     143,   421,    67,   124,   232,   410,   411,   391,   102,    89,
     157,   158,   391,    97,   105,    98,    99,   392,   157,   158,
      73,   103,   392,    91,   121,   104,   108,   109,   110,   437,
     111,   106,   143,   107,   191,   192,   193,   194,   195,   112,
     113,   323,   468,   326,   327,   328,   127,   196,   197,   198,
     225,   119,   352,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   378,   149,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   240,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   196,   197,   198,   205,   123,   196,   197,
     198,   193,   194,   195,   219,   211,   212,   127,   220,   127,
     194,   195,   196,   197,   198,   349,   349,   154,   154,   230,
     196,   197,   198,   231,   200,   340,   205,   298,   233,   127,
     127,   299,   122,   127,   127,   350,   350,   300,   124,   351,
     351,   301,   208,   125,   128,   129,   153,   240,   203,   209,
     214,   221,   227,   366,   228,   229,   242,   236,   247,   309,
     303,   246,   305,   306,   248,   249,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   269,   273,   274,   275,   276,   277,
     278,   279,   280,   311,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   357,   390,   329,   400,   127,   313,
     314,   317,   404,   318,   319,   349,    24,   154,   154,   127,
     419,   312,   331,   332,   418,   127,   316,   156,   333,   334,
     348,   359,   320,   321,   322,   350,   393,   365,    92,   351,
     394,   370,   362,   363,   380,   432,   335,   336,   381,   241,
     386,   341,   127,   213,   383,   215,   388,     9,   397,   398,
     445,   442,   399,   403,   401,   409,   423,    19,   424,   426,
     157,   158,   434,   436,   439,   234,   235,   440,   441,   237,
     238,   456,   443,   444,   459,   452,   447,   457,   379,   152,
     448,   458,    35,    36,   337,   466,   463,   464,   465,   338,
     462,   467,    45,    46,   395,   118,   438,   202,   469,   385,
     360,   127,   358,   455,   127,    86,     0,   339,   368,   127,
       0,     0,   372,   373,   374,     0,     0,     0,     0,     0,
       0,     0,   427,   428,     0,     0,     0,   431,     0,     0,
     433,     0,     0,     0,     0,     0,     0,     0,   384,     0,
       0,     0,     0,     0,   302,   188,   127,   189,   190,   191,
     192,   193,   194,   195,     0,   310,     0,     0,     0,     0,
       0,   315,   196,   197,   198,     0,   143,     0,     0,     0,
       0,     0,     0,   460,     0,     0,     0,     0,     0,     0,
       0,   143,   240,   127,     0,     0,     0,     0,   342,   127,
       0,     0,   240,   470,   127,     0,   430,     0,     0,     0,
       0,     0,     0,     0,     0,   435,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
       3,     0,     0,     5,     0,     0,     0,     0,     0,   450,
       9,    10,    11,     0,   127,   127,     0,     0,     0,     0,
      19,    20,    21,     0,     0,     0,     0,   361,    25,     0,
     364,     0,     0,     0,     0,   369,     0,     0,     0,    32,
       0,     0,     0,     0,     0,    35,    36,   144,    38,    39,
      40,    41,   145,    43,    44,    45,    46,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   389,     0,    48,    49,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    51,     0,     0,
       0,    52,     0,     0,     0,    53,    54,    55,    56,   422,
      57,    58,    59,     0,     0,   425,     0,     0,    61,   146,
     429,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   446,     0,     0,   151,     1,
       0,     2,     3,     0,     4,     5,     6,     7,     8,     0,
     453,   454,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     0,    22,     0,    23,    24,
      25,    26,    27,     0,    28,    29,    30,     0,     0,    31,
       0,    32,    33,    34,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
       0,     0,     0,    52,     0,     0,     0,    53,    54,    55,
      56,     0,    57,    58,    59,    60,     1,     0,     2,     3,
      61,     4,     5,     6,     7,     8,     0,     0,     0,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     0,    22,     0,    23,    24,    25,    26,    27,
       0,    28,    29,    30,     0,     0,    31,     0,    32,    33,
      34,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    51,     0,     0,     0,
      52,     0,     0,     0,    53,    54,    55,    56,     0,    57,
      58,    59,    60,     1,     0,     0,     3,    61,     4,     5,
       6,     7,     8,     0,     0,   222,     9,    10,    11,    12,
      13,    14,     0,    16,    17,    18,    19,    20,    21,     0,
      22,     0,    23,     0,    25,     0,    27,   223,    28,     0,
      30,     0,   224,     0,     0,    32,    33,    34,     0,     0,
       0,    35,    36,    37,    38,    39,    40,    41,    88,    43,
      44,    45,    46,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      48,    49,    50,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,    52,     0,     0,
       0,    53,    54,    55,    56,     0,    57,    58,    59,    92,
       1,     0,     0,     3,    61,     4,     5,     6,     7,     8,
       0,     0,     0,     9,    10,    11,    12,    13,    14,     0,
      16,    17,    18,    19,    20,    21,     0,    22,     0,    23,
       0,    25,     0,    27,     0,    28,     0,    30,     0,     0,
       0,     0,    32,    33,    34,   -63,   -63,     0,    35,    36,
      37,    38,    39,    40,    41,    88,    43,    44,    45,    46,
       0,     0,    47,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    48,    49,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      51,     0,     0,     0,    52,     0,     0,     0,    53,    54,
      55,    56,     0,    57,    58,    59,     0,     1,     0,   -63,
       3,    61,     4,     5,     6,     7,     8,     0,     0,     0,
       9,    10,    11,    12,    13,    14,     0,    16,    17,    18,
      19,    20,    21,     0,    22,     0,    23,     0,    25,     0,
      27,     0,    28,     0,    30,     0,     0,     0,     0,    32,
      33,    34,   -61,   -61,     0,    35,    36,    37,    38,    39,
      40,    41,    88,    43,    44,    45,    46,     0,     0,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    48,    49,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    51,     0,     0,
       0,    52,     0,     0,     0,    53,    54,    55,    56,     0,
      57,    58,    59,     0,     1,     0,   -61,     3,    61,     4,
       5,     6,     7,     8,     0,     0,     0,     9,    10,    11,
      12,    13,    14,     0,    16,    17,    18,    19,    20,    21,
       0,    22,     0,    23,     0,    25,     0,    27,     0,    28,
       0,    30,     0,     0,     0,     0,    32,    33,    34,   -62,
     -62,     0,    35,    36,    37,    38,    39,    40,    41,    88,
      43,    44,    45,    46,     0,     0,    47,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,    49,    50,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    51,     0,     0,     0,    52,     0,
       0,     0,    53,    54,    55,    56,     0,    57,    58,    59,
       0,     1,     0,   -62,     3,    61,     4,     5,     6,     7,
       8,     0,     0,     0,     9,    10,    11,    12,    13,    14,
       0,    16,    17,    18,    19,    20,    21,     0,    22,     0,
      23,     0,    25,     0,    27,     0,    28,     0,    30,     0,
       0,     0,     0,    32,    33,    34,   -60,   -60,     0,    35,
      36,    37,    38,    39,    40,    41,    88,    43,    44,    45,
      46,     0,     0,    47,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    48,    49,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    51,     0,     0,     0,    52,     0,     0,     0,    53,
      54,    55,    56,     0,    57,    58,    59,     0,     1,     0,
     -60,     3,    61,     4,     5,     6,     7,     8,     0,     0,
       0,     9,    10,    11,    12,    13,    14,     0,    16,    17,
      18,    19,    20,    21,     0,    22,     0,    23,     0,    25,
       0,    27,     0,    28,     0,    30,     0,     0,     0,     0,
      32,    33,    34,     0,     0,     0,    35,    36,    37,    38,
      39,    40,    41,    88,    43,    44,    45,    46,     0,     0,
      47,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    48,    49,    50,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    51,     0,
       0,     0,    52,     0,     0,     0,    53,    54,    55,    56,
       0,    57,    58,    59,    92,     1,     0,     0,     3,    61,
       4,     5,     6,     7,     8,     0,     0,     0,     9,    10,
      11,    12,    13,    14,     0,    16,    17,    18,    19,    20,
      21,     0,    22,     0,    23,     0,    25,     0,    27,     0,
      28,     0,    30,     0,     0,     0,     0,    32,    33,    34,
       0,     0,     0,    35,    36,    37,    38,    39,    40,    41,
      88,    43,    44,    45,    46,     0,     0,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,    49,    50,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,     0,     0,     0,    52,
       0,     0,     0,    53,    54,    55,    56,     0,    57,    58,
      59,     0,     1,     0,   206,     3,    61,     4,     5,     6,
       7,     8,     0,     0,     0,     9,    10,    11,    12,    13,
      14,     0,    16,    17,    18,    19,    20,    21,     0,    22,
       0,    23,     0,    25,     0,    27,     0,    28,     0,    30,
       0,     0,     0,     0,    32,    33,    34,     0,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    88,    43,    44,
      45,    46,     0,     0,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
      49,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    51,     0,     0,     0,    52,     0,     0,     0,
      53,    54,    55,    56,     0,    57,    58,    59,     0,     1,
       0,   239,     3,    61,     4,     5,     6,     7,     8,     0,
       0,     0,     9,    10,    11,    12,    13,    14,     0,    16,
      17,    18,    19,    20,    21,     0,    22,     0,    23,     0,
      25,     0,    27,     0,    28,     0,    30,     0,     0,     0,
       0,    32,    33,    34,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    88,    43,    44,    45,    46,     0,
       0,    47,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
       0,     0,     0,    52,     0,     0,     0,    53,    54,    55,
      56,     0,    57,    58,    59,     0,     1,     0,   304,     3,
      61,     4,     5,     6,     7,     8,     0,     0,     0,     9,
      10,    11,    12,    13,    14,     0,    16,    17,    18,    19,
      20,    21,     0,    22,     0,    23,     0,    25,     0,    27,
       0,    28,     0,    30,     0,     0,     0,     0,    32,    33,
      34,     0,     0,     0,    35,    36,    37,    38,    39,    40,
      41,    88,    43,    44,    45,    46,     0,     0,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    48,    49,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    51,     0,     0,     0,
      52,     0,     0,     0,    53,    54,    55,    56,     0,    57,
      58,    59,     3,     0,     0,     5,     0,    61,     0,     0,
       0,     0,     9,    10,    11,     0,     0,     0,     0,     0,
       0,     0,    19,    20,    21,     0,     0,     0,     0,     0,
      25,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,     0,     0,     0,     0,     0,    35,    36,    37,
      38,    39,    40,    41,    88,    43,    44,    45,    46,   270,
     271,    47,   272,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,    49,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
       0,     0,     0,    52,     0,     0,     0,    53,    54,    55,
      56,     0,    57,    58,    59,     3,     0,     0,     5,     0,
      61,     0,     0,     0,     0,     9,    10,    11,     0,     0,
       0,     0,     0,     0,     0,    19,    20,    21,     0,     0,
       0,     0,     0,    25,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    32,     0,     0,     0,     0,     0,
      35,    36,    37,    38,    39,    40,    41,    88,    43,    44,
      45,    46,     0,     0,    47,   268,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    48,
      49,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    51,     0,     0,     0,    52,     0,     0,     0,
      53,    54,    55,    56,     0,    57,    58,    59,     3,     0,
       0,     5,     0,    61,     0,     0,     0,     0,     9,    10,
      11,     0,     0,     0,     0,     0,     0,     0,    19,    20,
      21,     0,     0,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,     0,     0,
       0,     0,     0,    35,    36,    37,    38,    39,    40,    41,
      88,    43,    44,    45,    46,     0,     0,    47,     0,     0,
       0,     0,     0,     0,     0,   157,   158,     0,     0,     0,
       0,     0,    48,    49,    50,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,     0,     0,     0,    52,
       0,     0,     0,    53,    54,    55,    56,     0,    57,    58,
      59,     0,     0,     0,     0,     0,    61,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   157,   158,
       0,   170,   171,     0,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,     0,   189,   190,   191,   192,   193,   194,   195,     0,
       0,     0,     0,     0,     0,     0,     0,   196,   197,   198,
       0,     0,     0,     0,   345,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   157,   158,     0,   170,   171,     0,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,     0,   189,   190,   191,   192,   193,
     194,   195,     0,     0,     0,     0,     0,     0,     0,     0,
     196,   197,   198,     0,     0,     0,   245,     0,     0,     0,
       0,     0,     0,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   157,   158,     0,   170,   171,     0,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,     0,   189,   190,
     191,   192,   193,   194,   195,     0,     0,     0,     0,     0,
       0,     0,     0,   196,   197,   198,     0,     0,     0,   307,
       0,     0,     0,     0,     0,     0,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   157,   158,     0,
     170,   171,     0,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
       0,   189,   190,   191,   192,   193,   194,   195,     0,     0,
       0,     0,     0,     0,     0,     0,   196,   197,   198,     0,
       0,     0,   308,     0,     0,     0,     0,     0,     0,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     157,   158,     0,   170,   171,     0,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,     0,   189,   190,   191,   192,   193,   194,
     195,     0,     0,     0,     0,     0,     0,     0,     0,   196,
     197,   198,     0,     0,     0,   330,     0,     0,     0,     0,
       0,     0,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   157,   158,     0,   170,   171,     0,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,     0,   189,   190,   191,
     192,   193,   194,   195,     0,     0,     0,     0,     0,     0,
       0,     0,   196,   197,   198,     0,     0,     0,   371,     0,
       0,     0,     0,     0,     0,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   157,   158,     0,   170,
     171,     0,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,     0,
     189,   190,   191,   192,   193,   194,   195,     0,     0,     0,
       0,     0,     0,     0,     0,   196,   197,   198,     0,     0,
       0,   377,     0,     0,     0,     0,     0,     0,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   157,
     158,     0,   170,   171,     0,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,     0,   189,   190,   191,   192,   193,   194,   195,
       0,     0,     0,     0,     0,     0,     0,     0,   196,   197,
     198,     0,     0,     0,   396,     0,     0,     0,     0,     0,
       0,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   157,   158,     0,   170,   171,     0,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,     0,   189,   190,   191,   192,
     193,   194,   195,     0,     0,     0,     0,     0,     0,     0,
       0,   196,   197,   198,     0,     0,     0,   402,     0,     0,
       0,     0,     0,     0,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   157,   158,     0,   170,   171,
       0,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,     0,     0,     0,
     406,     0,     0,     0,     0,     0,     0,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   157,   158,
       0,   170,   171,     0,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,     0,   189,   190,   191,   192,   193,   194,   195,     0,
       0,     0,     0,     0,     0,     0,     0,   196,   197,   198,
       0,     0,     0,   407,     0,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   157,   158,     0,   170,   171,     0,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,     0,   189,   190,   191,   192,   193,
     194,   195,     0,     0,     0,     0,     0,     0,     0,     0,
     196,   197,   198,     0,     0,     0,   408,     0,     0,     0,
       0,     0,     0,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   157,   158,     0,   170,   171,     0,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,     0,   189,   190,
     191,   192,   193,   194,   195,     0,     0,     0,     0,     0,
       0,     0,     0,   196,   197,   198,     0,     0,     0,   449,
       0,     0,     0,     0,     0,     0,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   157,   158,     0,
     170,   171,     0,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
       0,   189,   190,   191,   192,   193,   194,   195,     0,     0,
       0,     0,     0,     0,     0,     0,   196,   197,   198,     0,
       0,     0,   461,     0,     0,     0,     0,     0,     0,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     157,   158,     0,   170,   171,     0,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,     0,   189,   190,   191,   192,   193,   194,
     195,     0,     0,     0,     0,     0,     0,     0,     0,   196,
     197,   198,     0,   199,     0,     0,     0,     0,     0,     0,
       0,     0,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   157,   158,     0,   170,   171,     0,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,     0,   189,   190,   191,
     192,   193,   194,   195,     0,     0,   157,   158,     0,     0,
       0,     0,   196,   197,   198,     0,   210,     0,     0,     0,
       0,     0,     0,     0,     0,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,     0,     0,     0,   170,
     171,     0,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,     0,
     189,   190,   191,   192,   193,   194,   195,   367,     0,   157,
     158,     0,     0,     0,     0,   196,   197,   198,     0,   218,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,     0,   189,   190,   191,   192,   193,   194,   195,
       0,     0,     0,     0,     0,     0,     0,     0,   196,   197,
     198,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   157,   158,     0,   170,   171,     0,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,     0,   189,   190,   191,   192,
     193,   194,   195,     0,     0,     0,     0,     0,     0,     0,
       0,   196,   197,   198,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   157,   158,     0,   170,   171,
       0,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,     0,   375,     0,
       0,     0,     0,     0,     0,     0,     0,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   157,   158,
       0,   170,   171,     0,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,     0,   189,   190,   191,   192,   193,   194,   195,     0,
       0,     0,     0,     0,     0,     0,     0,   196,   197,   198,
       0,   376,     0,     0,     0,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,     0,     0,     0,   170,   171,     0,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,     0,   189,   190,   191,   192,   193,
     194,   195,   157,   158,     0,     0,     0,     0,     0,     0,
     196,   197,   198,   344,     0,     0,   216,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   157,   158,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,     0,     0,     0,   170,   171,
       0,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,   157,   158,     0,     0,
       0,     0,     0,     0,   196,   197,   198,     0,   170,   171,
     217,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,     0,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   157,
     158,     0,   170,   171,     0,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,     0,   189,   190,   191,   192,   193,   194,   195,
       0,     0,     0,     0,     0,     0,     0,     0,   196,   197,
     198,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   157,   158,     0,   170,   171,   343,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,     0,   189,   190,   191,   192,
     193,   194,   195,     0,     0,     0,     0,     0,     0,     0,
       0,   196,   197,   198,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   157,   158,     0,   170,   171,
     451,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   157,   158,
       0,   170,   171,     0,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,     0,   189,   190,   191,   192,   193,   194,   195,     0,
       0,     0,     0,     0,     0,     0,     0,   196,   197,   198,
       0,   157,   158,     0,     0,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,     0,     0,     0,     0,   171,     0,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,     0,   189,   190,   191,   192,   193,
     194,   195,   157,   158,     0,     0,     0,     0,     0,     0,
     196,   197,   198,   169,     0,     0,     0,   170,   171,     0,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,     0,   189,   190,
     191,   192,   193,   194,   195,     0,     0,     0,     0,     0,
       0,     0,     0,   196,   197,   198,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
       0,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,     0,   189,
     190,   191,   192,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   197,   198
};

static const short int yycheck[] =
{
       0,    92,     7,    61,    73,    15,    16,   230,     3,   230,
     230,   230,   371,    52,    52,    46,    47,    81,     0,    31,
       3,    81,    51,    31,    83,    33,    55,    56,    31,   121,
      33,    51,    51,    51,    91,    51,    56,    56,    56,    51,
      56,   122,    37,    51,    56,    51,    41,    55,    15,    16,
      56,    15,    16,    56,    37,   122,    15,    16,    41,   123,
      60,   121,    62,   122,   121,    46,    47,   353,   122,     4,
      15,    16,   358,     8,   121,    10,    11,   353,    15,    16,
      62,    51,   358,   122,   122,    55,    21,    22,    23,   120,
      25,   121,    92,   121,   104,   105,   106,   107,   108,    20,
      51,   324,   461,   324,   324,   324,    41,   117,   118,   119,
     115,    51,   120,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,   120,    61,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   142,   102,   103,   104,   105,   106,   107,   108,
     117,   118,   119,   117,   118,   119,    91,   122,   117,   118,
     119,   106,   107,   108,    51,   100,   101,   102,    55,   104,
     107,   108,   117,   118,   119,    31,    31,    33,    33,   117,
     117,   118,   119,   121,    48,   243,   121,   117,   123,   124,
     125,   121,   121,   128,   129,    51,    51,   117,   122,    55,
      55,   121,    17,   122,   122,   122,   121,   207,   122,    11,
     122,   122,    77,   311,    77,   122,    83,   123,   122,   121,
     123,    83,   122,   122,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   123,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   120,   120,   123,   365,   203,   122,
     122,   122,   370,   122,   122,    31,    31,    33,    33,   214,
     383,   216,   123,   123,   382,   220,   221,   356,   123,   123,
      81,   123,   227,   228,   229,    51,    51,   123,   117,    55,
      55,   123,   307,   308,   117,   403,   241,   242,    83,    83,
      54,   246,   247,   102,   123,   104,   122,    14,   123,    66,
     423,   419,   121,   123,   122,    39,   123,    24,   122,   121,
      15,    16,   122,    83,   122,   124,   125,   122,    81,   128,
     129,   444,    54,   123,   447,   436,   123,   445,   324,    62,
     123,   123,    49,    50,    51,   458,   123,   123,   456,    56,
     451,   459,    59,    60,   120,    30,   412,    83,   466,   348,
     305,   306,   300,   441,   309,     3,    -1,   243,   313,   314,
      -1,    -1,   317,   318,   319,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   397,   398,    -1,    -1,    -1,   402,    -1,    -1,
     405,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   343,    -1,
      -1,    -1,    -1,    -1,   203,   100,   351,   102,   103,   104,
     105,   106,   107,   108,    -1,   214,    -1,    -1,    -1,    -1,
      -1,   220,   117,   118,   119,    -1,   436,    -1,    -1,    -1,
      -1,    -1,    -1,   448,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   451,   452,   388,    -1,    -1,    -1,    -1,   247,   394,
      -1,    -1,   462,   468,   399,    -1,   401,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   410,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   424,
       4,    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,   434,
      14,    15,    16,    -1,   439,   440,    -1,    -1,    -1,    -1,
      24,    25,    26,    -1,    -1,    -1,    -1,   306,    32,    -1,
     309,    -1,    -1,    -1,    -1,   314,    -1,    -1,    -1,    43,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   351,    -1,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,
      -1,   105,    -1,    -1,    -1,   109,   110,   111,   112,   388,
     114,   115,   116,    -1,    -1,   394,    -1,    -1,   122,   123,
     399,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   424,    -1,    -1,     0,     1,
      -1,     3,     4,    -1,     6,     7,     8,     9,    10,    -1,
     439,   440,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    28,    -1,    30,    31,
      32,    33,    34,    -1,    36,    37,    38,    -1,    -1,    41,
      -1,    43,    44,    45,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    -1,   105,    -1,    -1,    -1,   109,   110,   111,
     112,    -1,   114,   115,   116,   117,     1,    -1,     3,     4,
     122,     6,     7,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    -1,    28,    -1,    30,    31,    32,    33,    34,
      -1,    36,    37,    38,    -1,    -1,    41,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    80,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
     105,    -1,    -1,    -1,   109,   110,   111,   112,    -1,   114,
     115,   116,   117,     1,    -1,    -1,     4,   122,     6,     7,
       8,     9,    10,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    -1,    21,    22,    23,    24,    25,    26,    -1,
      28,    -1,    30,    -1,    32,    -1,    34,    35,    36,    -1,
      38,    -1,    40,    -1,    -1,    43,    44,    45,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    -1,    -1,   105,    -1,    -1,
      -1,   109,   110,   111,   112,    -1,   114,   115,   116,   117,
       1,    -1,    -1,     4,   122,     6,     7,     8,     9,    10,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    -1,
      21,    22,    23,    24,    25,    26,    -1,    28,    -1,    30,
      -1,    32,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    -1,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     101,    -1,    -1,    -1,   105,    -1,    -1,    -1,   109,   110,
     111,   112,    -1,   114,   115,   116,    -1,     1,    -1,   120,
       4,   122,     6,     7,     8,     9,    10,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    -1,    21,    22,    23,
      24,    25,    26,    -1,    28,    -1,    30,    -1,    32,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    -1,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,
      -1,   105,    -1,    -1,    -1,   109,   110,   111,   112,    -1,
     114,   115,   116,    -1,     1,    -1,   120,     4,   122,     6,
       7,     8,     9,    10,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    -1,    21,    22,    23,    24,    25,    26,
      -1,    28,    -1,    30,    -1,    32,    -1,    34,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    43,    44,    45,    46,
      47,    -1,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,   105,    -1,
      -1,    -1,   109,   110,   111,   112,    -1,   114,   115,   116,
      -1,     1,    -1,   120,     4,   122,     6,     7,     8,     9,
      10,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      -1,    21,    22,    23,    24,    25,    26,    -1,    28,    -1,
      30,    -1,    32,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   101,    -1,    -1,    -1,   105,    -1,    -1,    -1,   109,
     110,   111,   112,    -1,   114,   115,   116,    -1,     1,    -1,
     120,     4,   122,     6,     7,     8,     9,    10,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    -1,    21,    22,
      23,    24,    25,    26,    -1,    28,    -1,    30,    -1,    32,
      -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      43,    44,    45,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    -1,    -1,
      63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,
      -1,    -1,   105,    -1,    -1,    -1,   109,   110,   111,   112,
      -1,   114,   115,   116,   117,     1,    -1,    -1,     4,   122,
       6,     7,     8,     9,    10,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    -1,    21,    22,    23,    24,    25,
      26,    -1,    28,    -1,    30,    -1,    32,    -1,    34,    -1,
      36,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,    45,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,   105,
      -1,    -1,    -1,   109,   110,   111,   112,    -1,   114,   115,
     116,    -1,     1,    -1,   120,     4,   122,     6,     7,     8,
       9,    10,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    -1,    21,    22,    23,    24,    25,    26,    -1,    28,
      -1,    30,    -1,    32,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    -1,    -1,   105,    -1,    -1,    -1,
     109,   110,   111,   112,    -1,   114,   115,   116,    -1,     1,
      -1,   120,     4,   122,     6,     7,     8,     9,    10,    -1,
      -1,    -1,    14,    15,    16,    17,    18,    19,    -1,    21,
      22,    23,    24,    25,    26,    -1,    28,    -1,    30,    -1,
      32,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,
      -1,    43,    44,    45,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    -1,   105,    -1,    -1,    -1,   109,   110,   111,
     112,    -1,   114,   115,   116,    -1,     1,    -1,   120,     4,
     122,     6,     7,     8,     9,    10,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    -1,    21,    22,    23,    24,
      25,    26,    -1,    28,    -1,    30,    -1,    32,    -1,    34,
      -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    80,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
     105,    -1,    -1,    -1,   109,   110,   111,   112,    -1,   114,
     115,   116,     4,    -1,    -1,     7,    -1,   122,    -1,    -1,
      -1,    -1,    14,    15,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    -1,   105,    -1,    -1,    -1,   109,   110,   111,
     112,    -1,   114,   115,   116,     4,    -1,    -1,     7,    -1,
     122,    -1,    -1,    -1,    -1,    14,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    -1,    -1,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    -1,    -1,   105,    -1,    -1,    -1,
     109,   110,   111,   112,    -1,   114,   115,   116,     4,    -1,
      -1,     7,    -1,   122,    -1,    -1,    -1,    -1,    14,    15,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    24,    25,
      26,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    -1,    -1,    -1,
      -1,    -1,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,   105,
      -1,    -1,    -1,   109,   110,   111,   112,    -1,   114,   115,
     116,    -1,    -1,    -1,    -1,    -1,   122,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    15,    16,
      -1,    81,    82,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,   102,   103,   104,   105,   106,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,    -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    15,    16,    -1,    81,    82,    -1,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,   102,   103,   104,   105,   106,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,    -1,    -1,    -1,   123,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    15,    16,    -1,    81,    82,    -1,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,    -1,   102,   103,
     104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    15,    16,    -1,
      81,    82,    -1,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      -1,   102,   103,   104,   105,   106,   107,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,
      -1,    -1,   123,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      15,    16,    -1,    81,    82,    -1,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,    -1,   102,   103,   104,   105,   106,   107,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
     118,   119,    -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    15,    16,    -1,    81,    82,    -1,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,   102,   103,   104,
     105,   106,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   117,   118,   119,    -1,    -1,    -1,   123,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    15,    16,    -1,    81,
      82,    -1,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,    -1,
     102,   103,   104,   105,   106,   107,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,    -1,
      -1,   123,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    15,
      16,    -1,    81,    82,    -1,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    -1,   102,   103,   104,   105,   106,   107,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,    -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    15,    16,    -1,    81,    82,    -1,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,   102,   103,   104,   105,
     106,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,   118,   119,    -1,    -1,    -1,   123,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    15,    16,    -1,    81,    82,
      -1,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,
     123,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    15,    16,
      -1,    81,    82,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,   102,   103,   104,   105,   106,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    15,    16,    -1,    81,    82,    -1,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,   102,   103,   104,   105,   106,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,    -1,    -1,    -1,   123,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    15,    16,    -1,    81,    82,    -1,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,    -1,   102,   103,
     104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,   123,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    15,    16,    -1,
      81,    82,    -1,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      -1,   102,   103,   104,   105,   106,   107,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,
      -1,    -1,   123,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      15,    16,    -1,    81,    82,    -1,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,    -1,   102,   103,   104,   105,   106,   107,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,
     118,   119,    -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    15,    16,    -1,    81,    82,    -1,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,   102,   103,   104,
     105,   106,   107,   108,    -1,    -1,    15,    16,    -1,    -1,
      -1,    -1,   117,   118,   119,    -1,   121,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    -1,    -1,    -1,    81,
      82,    -1,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,    -1,
     102,   103,   104,   105,   106,   107,   108,    13,    -1,    15,
      16,    -1,    -1,    -1,    -1,   117,   118,   119,    -1,   121,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    -1,   102,   103,   104,   105,   106,   107,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    15,    16,    -1,    81,    82,    -1,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,   102,   103,   104,   105,
     106,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    15,    16,    -1,    81,    82,
      -1,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,   121,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    15,    16,
      -1,    81,    82,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,   102,   103,   104,   105,   106,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,   121,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    -1,    -1,    -1,    81,    82,    -1,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,   102,   103,   104,   105,   106,
     107,   108,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,   120,    -1,    -1,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    -1,    -1,    -1,    81,    82,
      -1,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,    81,    82,
      29,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    15,
      16,    -1,    81,    82,    -1,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,    -1,   102,   103,   104,   105,   106,   107,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    15,    16,    -1,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,   102,   103,   104,   105,
     106,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,   118,   119,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    15,    16,    -1,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    15,    16,
      -1,    81,    82,    -1,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,   102,   103,   104,   105,   106,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
      -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    -1,    -1,    -1,    -1,    82,    -1,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,    -1,   102,   103,   104,   105,   106,
     107,   108,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
     117,   118,   119,    77,    -1,    -1,    -1,    81,    82,    -1,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,    -1,   102,   103,
     104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   117,   118,   119,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,    -1,   102,
     103,   104,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,   118,   119
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,     6,     7,     8,     9,    10,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    28,    30,    31,    32,    33,    34,    36,    37,
      38,    41,    43,    44,    45,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    63,    78,    79,
      80,   101,   105,   109,   110,   111,   112,   114,   115,   116,
     117,   122,   126,   127,   128,   131,   132,   137,   140,   141,
     144,   146,   154,   158,   159,   160,   161,   163,   164,   167,
     168,   121,    51,    56,    51,    56,   161,   162,    56,   167,
      52,   122,   117,   133,   134,   137,   134,   167,   167,   167,
     122,   122,   122,    51,    55,   121,   121,   121,   167,   167,
     167,   167,    20,    51,   136,   138,    51,    56,   136,    51,
      52,   122,   121,   122,   122,   122,   145,   167,   122,   122,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   135,   137,    51,    56,   123,   165,   166,   167,
     168,     0,   127,   121,    33,    56,   159,    15,    16,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      81,    82,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   102,
     103,   104,   105,   106,   107,   108,   117,   118,   119,   121,
      48,   147,   147,   122,   164,   167,   120,   135,    17,    11,
     121,   167,   167,   145,   122,   145,    29,    29,   121,    51,
      55,   122,    13,    35,    40,   134,   139,    77,    77,   122,
     117,   121,   164,   167,   145,   145,   123,   145,   145,   120,
     137,    83,    83,    81,   123,   123,    83,   122,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,    64,   167,
      61,    62,    64,   167,   167,   167,   167,   167,   167,   167,
     167,    51,    56,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,    31,    51,    56,   148,   149,   117,   121,
     117,   121,   145,   123,   120,   122,   122,   123,   123,   121,
     145,   123,   167,   122,   122,   145,   167,   122,   122,   122,
     167,   167,   167,   128,   129,   130,   131,   132,   146,   123,
     123,   123,   123,   123,   123,   167,   167,    51,    56,   166,
     168,   167,   145,    83,   120,   124,    51,    56,    81,    31,
      51,    55,   120,   150,   151,   153,   158,   120,   150,   123,
     167,   145,   134,   134,   145,   123,   133,    13,   167,   145,
     123,   123,   167,   167,   167,   121,   121,   123,   120,   130,
     117,    83,   155,   123,   167,   149,    54,   152,   122,   145,
     120,   151,   153,    51,    55,   120,   123,   123,    66,   121,
     133,   122,   123,   123,   133,   138,   123,   123,   123,    39,
      46,    47,   142,   143,    51,    56,   156,   157,   133,   155,
      81,   121,   145,   123,   122,   145,   121,   134,   134,   145,
     167,   134,   133,   134,   122,   167,    83,   120,   143,   122,
     122,    81,   133,    54,   123,   155,   145,   123,   123,   123,
     167,    83,   135,   145,   145,   157,   155,   133,   123,   155,
     134,   123,   135,   123,   123,   133,   155,   133,   138,   133,
     134
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (yyscanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, yyscanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 51: /* "IDENTIFIER" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2446 "parser.cc"
        break;
      case 52: /* "VAR_REF" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2451 "parser.cc"
        break;
      case 53: /* "BACKQUOTE" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2456 "parser.cc"
        break;
      case 54: /* "SELF_REF" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2461 "parser.cc"
        break;
      case 55: /* "KW_IDENTIFIER_OPENPAREN" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2466 "parser.cc"
        break;
      case 56: /* "SCOPED_REF" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2471 "parser.cc"
        break;
      case 57: /* "CONTEXT_REF" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2476 "parser.cc"
        break;
      case 58: /* "COMPLEX_CONTEXT_REF" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2481 "parser.cc"
        break;
      case 59: /* "DATETIME" */
#line 365 "parser.yy"
        { if ((yyvaluep->datetime)) delete (yyvaluep->datetime); };
#line 2486 "parser.cc"
        break;
      case 60: /* "QUOTED_WORD" */
#line 365 "parser.yy"
        { if ((yyvaluep->String)) delete (yyvaluep->String); };
#line 2491 "parser.cc"
        break;
      case 61: /* "REGEX_SUBST" */
#line 365 "parser.yy"
        { if ((yyvaluep->RegexSubst)) delete (yyvaluep->RegexSubst); };
#line 2496 "parser.cc"
        break;
      case 62: /* "REGEX_TRANS" */
#line 365 "parser.yy"
        { if ((yyvaluep->RegexTrans)) delete (yyvaluep->RegexTrans); };
#line 2501 "parser.cc"
        break;
      case 64: /* "REGEX" */
#line 365 "parser.yy"
        { if ((yyvaluep->Regex)) delete (yyvaluep->Regex); };
#line 2506 "parser.cc"
        break;
      case 128: /* "top_namespace_decl" */
#line 365 "parser.yy"
        { if ((yyvaluep->ns)) delete (yyvaluep->ns); };
#line 2511 "parser.cc"
        break;
      case 129: /* "namespace_decls" */
#line 365 "parser.yy"
        { if ((yyvaluep->ns)) delete (yyvaluep->ns); };
#line 2516 "parser.cc"
        break;
      case 130: /* "namespace_decl" */
#line 365 "parser.yy"
        { if ((yyvaluep->nsn)) delete (yyvaluep->nsn); };
#line 2521 "parser.cc"
        break;
      case 131: /* "unscoped_const_decl" */
#line 365 "parser.yy"
        { if ((yyvaluep->constnode)) delete (yyvaluep->constnode); };
#line 2526 "parser.cc"
        break;
      case 132: /* "scoped_const_decl" */
#line 365 "parser.yy"
        { if ((yyvaluep->constnode)) delete (yyvaluep->constnode); };
#line 2531 "parser.cc"
        break;
      case 133: /* "block" */
#line 365 "parser.yy"
        { if ((yyvaluep->sblock)) delete (yyvaluep->sblock); };
#line 2536 "parser.cc"
        break;
      case 134: /* "statement_or_block" */
#line 365 "parser.yy"
        { if ((yyvaluep->sblock)) delete (yyvaluep->sblock); };
#line 2541 "parser.cc"
        break;
      case 135: /* "statements" */
#line 365 "parser.yy"
        { if ((yyvaluep->sblock)) delete (yyvaluep->sblock); };
#line 2546 "parser.cc"
        break;
      case 136: /* "optname" */
#line 368 "parser.yy"
        { free((yyvaluep->string)); };
#line 2551 "parser.cc"
        break;
      case 137: /* "statement" */
#line 365 "parser.yy"
        { if ((yyvaluep->statement)) delete (yyvaluep->statement); };
#line 2556 "parser.cc"
        break;
      case 138: /* "context_mods" */
#line 365 "parser.yy"
        { if ((yyvaluep->cmods)) delete (yyvaluep->cmods); };
#line 2561 "parser.cc"
        break;
      case 139: /* "context_mod" */
#line 365 "parser.yy"
        { if ((yyvaluep->cmod)) delete (yyvaluep->cmod); };
#line 2566 "parser.cc"
        break;
      case 140: /* "return_statement" */
#line 365 "parser.yy"
        { if ((yyvaluep->statement)) delete (yyvaluep->statement); };
#line 2571 "parser.cc"
        break;
      case 141: /* "switch_statement" */
#line 365 "parser.yy"
        { if ((yyvaluep->statement)) delete (yyvaluep->statement); };
#line 2576 "parser.cc"
        break;
      case 142: /* "case_block" */
#line 365 "parser.yy"
        { if ((yyvaluep->switchstmt)) delete (yyvaluep->switchstmt); };
#line 2581 "parser.cc"
        break;
      case 143: /* "case_code" */
#line 365 "parser.yy"
        { if ((yyvaluep->casenode)) delete (yyvaluep->casenode); };
#line 2586 "parser.cc"
        break;
      case 144: /* "try_statement" */
#line 365 "parser.yy"
        { if ((yyvaluep->statement)) delete (yyvaluep->statement); };
#line 2591 "parser.cc"
        break;
      case 145: /* "myexp" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2596 "parser.cc"
        break;
      case 146: /* "object_def" */
#line 365 "parser.yy"
        { if ((yyvaluep->objdef)) delete (yyvaluep->objdef); };
#line 2601 "parser.cc"
        break;
      case 147: /* "inheritance_list" */
#line 366 "parser.yy"
        { if ((yyvaluep->sclist)) (yyvaluep->sclist)->deref(); };
#line 2606 "parser.cc"
        break;
      case 148: /* "superclass_list" */
#line 366 "parser.yy"
        { if ((yyvaluep->sclist)) (yyvaluep->sclist)->deref(); };
#line 2611 "parser.cc"
        break;
      case 149: /* "superclass" */
#line 365 "parser.yy"
        { if ((yyvaluep->sclnode)) delete (yyvaluep->sclnode); };
#line 2616 "parser.cc"
        break;
      case 150: /* "class_attributes" */
#line 366 "parser.yy"
        { if ((yyvaluep->qoreclass)) (yyvaluep->qoreclass)->deref(); };
#line 2621 "parser.cc"
        break;
      case 151: /* "private_member_list" */
#line 365 "parser.yy"
        { if ((yyvaluep->privlist)) delete (yyvaluep->privlist); };
#line 2626 "parser.cc"
        break;
      case 152: /* "member_list" */
#line 365 "parser.yy"
        { if ((yyvaluep->privlist)) delete (yyvaluep->privlist); };
#line 2631 "parser.cc"
        break;
      case 153: /* "method_definition" */
#line 365 "parser.yy"
        { if ((yyvaluep->objectfunc)) delete (yyvaluep->objectfunc); };
#line 2636 "parser.cc"
        break;
      case 155: /* "base_constructor_list" */
#line 366 "parser.yy"
        { if ((yyvaluep->bcalist)) (yyvaluep->bcalist)->deref(); };
#line 2641 "parser.cc"
        break;
      case 156: /* "base_constructors" */
#line 366 "parser.yy"
        { if ((yyvaluep->bcalist)) (yyvaluep->bcalist)->deref(); };
#line 2646 "parser.cc"
        break;
      case 157: /* "base_constructor" */
#line 365 "parser.yy"
        { if ((yyvaluep->bcanode)) delete (yyvaluep->bcanode); };
#line 2651 "parser.cc"
        break;
      case 161: /* "function_call" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2656 "parser.cc"
        break;
      case 162: /* "scoped_object_call" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2661 "parser.cc"
        break;
      case 163: /* "self_function_call" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2666 "parser.cc"
        break;
      case 164: /* "list" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2671 "parser.cc"
        break;
      case 165: /* "hash" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2676 "parser.cc"
        break;
      case 166: /* "hash_element" */
#line 365 "parser.yy"
        { if ((yyvaluep->hashelement)) delete (yyvaluep->hashelement); };
#line 2681 "parser.cc"
        break;
      case 167: /* "exp" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2686 "parser.cc"
        break;
      case 168: /* "scalar" */
#line 367 "parser.yy"
        { if ((yyvaluep->node)) (yyvaluep->node)->deref(NULL); };
#line 2691 "parser.cc"
        break;

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (yyscan_t yyscanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (yyscan_t yyscanner)
#else
int
yyparse (yyscanner)
    yyscan_t yyscanner;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 379 "parser.yy"
    {
	   addClass((yyvsp[0].objdef)->name, (yyvsp[0].objdef)->oc); 
	   // see if class definitions are allowed
	   if (checkParseOption(PO_NO_CLASS_DEFS))
	      parse_error("illegal class definition \"%s\" (conflicts with parse option NO_CLASS_DEFS)",
			  (yyvsp[0].objdef)->oc->name);
	   delete (yyvsp[0].objdef);
	}
    break;

  case 6:
#line 388 "parser.yy"
    { 
	   addConstant((yyvsp[0].constnode)->name, (yyvsp[0].constnode)->value);
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  (yyvsp[0].constnode)->name->ostr);

	   delete (yyvsp[0].constnode);
	}
    break;

  case 7:
#line 398 "parser.yy"
    { 
	   getRootNS()->addConstant((yyvsp[0].constnode)->name, (yyvsp[0].constnode)->value); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  (yyvsp[0].constnode)->name->ostr);
	   delete (yyvsp[0].constnode);
	}
    break;

  case 9:
#line 408 "parser.yy"
    { 
	   // if it is a global variable declaration, then do not register
	   if ((yyvsp[0].statement) && (yyvsp[0].statement)->Type == S_EXPRESSION
	       && (((yyvsp[0].statement)->s.node->type == NT_VARREF
		    && (yyvsp[0].statement)->s.node->val.vref->type == VT_GLOBAL)
		   || ((yyvsp[0].statement)->s.node->type == NT_VLIST && 
		       (yyvsp[0].statement)->s.node->val.list->retrieve_entry(0)->val.vref->type == VT_GLOBAL)))
	      delete (yyvsp[0].statement);
	   else
	      getProgram()->addStatement((yyvsp[0].statement)); 
	}
    break;

  case 10:
#line 420 "parser.yy"
    {
	   getProgram()->addStatement(new Statement((yyvsp[-1].sblock)));
        }
    break;

  case 11:
#line 424 "parser.yy"
    {
	   getRootNS()->addNamespace((yyvsp[0].ns)); 
	   // see if ns declaration is legal
	   if (checkParseOption(PO_NO_NAMESPACE_DEFS))
	      parse_error("illegal namespace definition \"%s\" (conflicts with parse option NO_NAMESPACE_DEFINITION)",
			  (yyvsp[0].ns)->name);
	}
    break;

  case 12:
#line 435 "parser.yy"
    { (yyvsp[-1].ns)->name = (yyvsp[-3].string); (yyval.ns) = (yyvsp[-1].ns); }
    break;

  case 13:
#line 437 "parser.yy"
    { (yyval.ns) = new Namespace((yyvsp[-1].string)); free((yyvsp[-1].string)); }
    break;

  case 14:
#line 442 "parser.yy"
    {
	   class Namespace *ns = new Namespace();
	   addNSNode(ns, (yyvsp[0].nsn));
	   (yyval.ns) = ns;
        }
    break;

  case 15:
#line 448 "parser.yy"
    {  
	   addNSNode((yyvsp[-1].ns), (yyvsp[0].nsn));
	   (yyval.ns) = (yyvsp[-1].ns);
	}
    break;

  case 16:
#line 456 "parser.yy"
    { 
	   (yyval.nsn) = new NSNode((yyvsp[0].constnode)); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  (yyvsp[0].constnode)->name->ostr);
        }
    break;

  case 17:
#line 464 "parser.yy"
    { 
	   (yyval.nsn) = new NSNode((yyvsp[0].constnode)); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  (yyvsp[0].constnode)->name->ostr);
	}
    break;

  case 18:
#line 472 "parser.yy"
    { 
	   (yyval.nsn) = new NSNode((yyvsp[0].objdef)); 
	   // see if class definitions are allowed
	   if (checkParseOption(PO_NO_CLASS_DEFS))
	      parse_error("illegal class definition \"%s\" (conflicts with parse option NO_CLASS_DEFS)",
			  (yyvsp[0].objdef)->oc->name);
	}
    break;

  case 19:
#line 480 "parser.yy"
    { 
	   (yyval.nsn) = new NSNode((yyvsp[0].ns)); 
	   // see if ns declaration is legal
	   if (checkParseOption(PO_NO_NAMESPACE_DEFS))
	      parse_error("illegal namespace definition \"%s\" (conflicts with parse option NO_NAMESPACE_DEFINITION)",
			  (yyvsp[0].ns)->name);
	}
    break;

  case 20:
#line 490 "parser.yy"
    { 
	   if (needsEval((yyvsp[-1].node)))
	      parse_error("constant expression needs run-time evaluation");
	   (yyval.constnode) = new ConstNode((yyvsp[-3].string), (yyvsp[-1].node)); 
	}
    break;

  case 21:
#line 499 "parser.yy"
    {
	   if (needsEval((yyvsp[-1].node)))
	      parse_error("constant expression needs run-time evaluation");
	   (yyval.constnode) = new ConstNode((yyvsp[-3].string), (yyvsp[-1].node)); 
	}
    break;

  case 22:
#line 508 "parser.yy"
    { (yyval.sblock) = (yyvsp[-1].sblock); }
    break;

  case 23:
#line 511 "parser.yy"
    { (yyval.sblock) = NULL; }
    break;

  case 24:
#line 516 "parser.yy"
    { (yyval.sblock) = new StatementBlock((yyvsp[0].statement)); }
    break;

  case 25:
#line 519 "parser.yy"
    { (yyval.sblock) = (yyvsp[0].sblock); }
    break;

  case 26:
#line 524 "parser.yy"
    { (yyval.sblock) = new StatementBlock((yyvsp[0].statement)); }
    break;

  case 27:
#line 527 "parser.yy"
    { (yyvsp[-1].sblock)->addStatement((yyvsp[0].statement)); (yyval.sblock) = (yyvsp[-1].sblock); }
    break;

  case 28:
#line 532 "parser.yy"
    { (yyval.string) = NULL; }
    break;

  case 29:
#line 533 "parser.yy"
    { (yyval.string) = (yyvsp[0].string); }
    break;

  case 30:
#line 538 "parser.yy"
    {
	   // if the expression has no effect and it's not a variable declaration
	   if (!hasEffect((yyvsp[-1].node)) 
	       && ((yyvsp[-1].node)->type != NT_VARREF || (yyvsp[-1].node)->val.vref->type == VT_UNRESOLVED)
	       && ((yyvsp[-1].node)->type != NT_VLIST))
	      parse_error("statement has no effect (%s)", (yyvsp[-1].node)->type->name);
	   (yyval.statement) = new Statement(S_EXPRESSION, (yyvsp[-1].node));
	}
    break;

  case 31:
#line 547 "parser.yy"
    { (yyval.statement) = (yyvsp[0].statement); }
    break;

  case 32:
#line 549 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_RETHROW);
	}
    break;

  case 33:
#line 553 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_THROW);
	   (yyval.statement)->s.Throw = new ThrowStatement((yyvsp[-1].node));
	}
    break;

  case 34:
#line 558 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_SUBCONTEXT);
	   (yyval.statement)->s.SContext = new ContextStatement(NULL, NULL, (yyvsp[-1].cmods), (yyvsp[0].sblock));
	}
    break;

  case 35:
#line 563 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_SUMMARY);
	   
	   ContextModList *cml;

	   if ((yyvsp[-1].cmods))
	   {
	      (yyvsp[-1].cmods)->addContextMod(new ContextMod(CM_SUMMARIZE_BY, (yyvsp[-3].node)));
	      cml = (yyvsp[-1].cmods);
	   }
	   else
	      cml = new ContextModList(new ContextMod(CM_SUMMARIZE_BY, (yyvsp[-3].node)));

	   (yyval.statement)->s.SContext = new ContextStatement((yyvsp[-9].string), (yyvsp[-7].node), cml, (yyvsp[0].sblock));
	}
    break;

  case 36:
#line 579 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_CONTEXT);
	   
	   (yyval.statement)->s.SContext = new ContextStatement((yyvsp[-5].string), (yyvsp[-3].node), (yyvsp[-1].cmods), (yyvsp[0].sblock));
        }
    break;

  case 37:
#line 585 "parser.yy"
    {	
	   (yyval.statement) = new Statement(S_IF);
	   (yyval.statement)->s.If = new IfStatement((yyvsp[-2].node), (yyvsp[0].sblock), NULL);
	}
    break;

  case 38:
#line 590 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_IF);
	   (yyval.statement)->s.If = new IfStatement((yyvsp[-4].node), (yyvsp[-2].sblock), (yyvsp[0].sblock));
	}
    break;

  case 39:
#line 595 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_WHILE);
	   (yyval.statement)->s.While = new WhileStatement((yyvsp[-2].node), (yyvsp[0].sblock));
	}
    break;

  case 40:
#line 600 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_DO_WHILE);
	   (yyval.statement)->s.While = new WhileStatement((yyvsp[-2].node), (yyvsp[-5].sblock));
	}
    break;

  case 41:
#line 605 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_FOR);
	   (yyval.statement)->s.For = new ForStatement((yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].sblock));
	}
    break;

  case 42:
#line 610 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_FOREACH);
	   (yyval.statement)->s.ForEach = new ForEachStatement((yyvsp[-5].node), (yyvsp[-2].node), (yyvsp[0].sblock));
	   if ((yyvsp[-5].node)->type != NT_VARREF && (yyvsp[-5].node)->type != NT_SELF_VARREF)
	      parse_error("foreach variable expression is not a variable reference");
	}
    break;

  case 43:
#line 616 "parser.yy"
    { (yyval.statement) = (yyvsp[-1].statement); }
    break;

  case 44:
#line 618 "parser.yy"
    { 
	   // see if thread exit is allowed
	   if (checkParseOption(PO_NO_THREADS))
	      parse_error("illegal use of \"thread_exit\" (conflicts with parse option NO_THREADS)");

	   (yyval.statement) = new Statement(S_THREAD_EXIT); 
	}
    break;

  case 45:
#line 626 "parser.yy"
    {
	  (yyval.statement) = new Statement(S_BREAK);
	}
    break;

  case 46:
#line 630 "parser.yy"
    {
	  (yyval.statement) = new Statement(S_CONTINUE);
	}
    break;

  case 47:
#line 634 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_DELETE);
	   (yyval.statement)->s.Delete = new DeleteStatement((yyvsp[-1].node));
	   if (check_vars((yyvsp[-1].node)))
	      parse_error("delete statement takes only variable references as arguments");
	}
    break;

  case 48:
#line 640 "parser.yy"
    { (yyval.statement) = (yyvsp[0].statement); }
    break;

  case 49:
#line 641 "parser.yy"
    { (yyval.statement) = NULL; }
    break;

  case 50:
#line 646 "parser.yy"
    { (yyval.cmods) = NULL; }
    break;

  case 51:
#line 649 "parser.yy"
    { 
	   if (!(yyvsp[-1].cmods)) 
	      (yyval.cmods) = new ContextModList((yyvsp[0].cmod)); 
	   else 
	   { 
	      (yyvsp[-1].cmods)->addContextMod((yyvsp[0].cmod)); 
	      (yyval.cmods) = (yyvsp[-1].cmods); 
	   }
	}
    break;

  case 52:
#line 662 "parser.yy"
    { (yyval.cmod) = new ContextMod(CM_WHERE_NODE, (yyvsp[-1].node)); }
    break;

  case 53:
#line 664 "parser.yy"
    { (yyval.cmod) = new ContextMod(CM_SORT_ASCENDING, (yyvsp[-1].node)); }
    break;

  case 54:
#line 666 "parser.yy"
    { (yyval.cmod) = new ContextMod(CM_SORT_DESCENDING, (yyvsp[-1].node)); }
    break;

  case 55:
#line 670 "parser.yy"
    { (yyval.statement) = new Statement(S_RETURN); (yyval.statement)->s.node = NULL; }
    break;

  case 56:
#line 672 "parser.yy"
    { (yyval.statement) = new Statement(S_RETURN, (yyvsp[0].node)); }
    break;

  case 57:
#line 677 "parser.yy"
    {
	   (yyvsp[-1].switchstmt)->setSwitch((yyvsp[-4].node));
	   (yyval.statement) = new Statement(S_SWITCH);
	   (yyval.statement)->s.Switch = (yyvsp[-1].switchstmt);
        }
    break;

  case 58:
#line 686 "parser.yy"
    {
	   (yyval.switchstmt) = new SwitchStatement((yyvsp[0].casenode));
	}
    break;

  case 59:
#line 690 "parser.yy"
    {
	   (yyvsp[-1].switchstmt)->addCase((yyvsp[0].casenode));
	   (yyval.switchstmt) = (yyvsp[-1].switchstmt);
        }
    break;

  case 60:
#line 698 "parser.yy"
    {
	   if (needsEval((yyvsp[-2].node)))
	      parse_error("case expression needs run-time evaluation");
	   (yyval.casenode) = new CaseNode((yyvsp[-2].node), (yyvsp[0].sblock));
	}
    break;

  case 61:
#line 704 "parser.yy"
    {
	   if (needsEval((yyvsp[-1].node)))
	      parse_error("case expression needs run-time evaluation");
	   (yyval.casenode) = new CaseNode((yyvsp[-1].node), NULL);
	}
    break;

  case 62:
#line 710 "parser.yy"
    {
	   (yyval.casenode) = new CaseNode(NULL, (yyvsp[0].sblock));
	}
    break;

  case 63:
#line 714 "parser.yy"
    {
	   (yyval.casenode) = new CaseNode(NULL, NULL);
	}
    break;

  case 64:
#line 721 "parser.yy"
    {
	   (yyval.statement) = new Statement(S_TRY);
	   char *param = NULL;
	   if ((yyvsp[-2].node))
	   {
	      if ((yyvsp[-2].node)->type == NT_VARREF)
	      {
		 param = (yyvsp[-2].node)->val.vref->name;
		 (yyvsp[-2].node)->val.vref->name = NULL;
		 (yyvsp[-2].node)->deref(NULL);
	      }
	      else
		 parse_error("only one parameter accepted in catch block for exception hash");
	   }
	   (yyval.statement)->s.Try = new TryStatement((yyvsp[-5].sblock), (yyvsp[0].sblock), param);
	}
    break;

  case 65:
#line 739 "parser.yy"
    { (yyval.node) = NULL; }
    break;

  case 66:
#line 740 "parser.yy"
    { (yyval.node) = (yyvsp[0].node); }
    break;

  case 67:
#line 749 "parser.yy"
    {
	   (yyval.objdef) = new ObjClassDef((yyvsp[-4].string), (yyvsp[-1].qoreclass)); 
	   (yyvsp[-1].qoreclass)->name = strdup((yyvsp[-4].string));
	   (yyvsp[-1].qoreclass)->scl = (yyvsp[-3].sclist);
	}
    break;

  case 68:
#line 755 "parser.yy"
    { 
	   (yyval.objdef) = new ObjClassDef((yyvsp[-4].string), (yyvsp[-1].qoreclass)); 
	   (yyvsp[-1].qoreclass)->name = strdup((yyval.objdef)->name->getIdentifier()); 
	   (yyvsp[-1].qoreclass)->scl = (yyvsp[-3].sclist);
	}
    break;

  case 69:
#line 761 "parser.yy"
    { 
	   class QoreClass *qc = new QoreClass(strdup((yyvsp[-2].string)));
	   qc->scl = (yyvsp[-1].sclist);
	   (yyval.objdef) = new ObjClassDef((yyvsp[-2].string), qc); 	   
	}
    break;

  case 70:
#line 767 "parser.yy"
    { 
	   class QoreClass *qc = new QoreClass();
	   (yyval.objdef) = new ObjClassDef((yyvsp[-2].string), qc);
	   qc->name = strdup((yyval.objdef)->name->getIdentifier());
	   qc->scl = (yyvsp[-1].sclist);
	}
    break;

  case 71:
#line 774 "parser.yy"
    { 
	   class QoreClass *qc = new QoreClass(strdup((yyvsp[-3].string)));
	   qc->scl = (yyvsp[-2].sclist);
	   (yyval.objdef) = new ObjClassDef((yyvsp[-3].string), qc); 	   
	}
    break;

  case 72:
#line 780 "parser.yy"
    { 
	   class QoreClass *qc = new QoreClass();
	   (yyval.objdef) = new ObjClassDef((yyvsp[-3].string), qc);
	   qc->name = strdup((yyval.objdef)->name->getIdentifier());
	   qc->scl = (yyvsp[-2].sclist);
	}
    break;

  case 73:
#line 790 "parser.yy"
    {
	   (yyval.sclist) = (yyvsp[0].sclist);
        }
    break;

  case 74:
#line 794 "parser.yy"
    { (yyval.sclist) = NULL; }
    break;

  case 75:
#line 799 "parser.yy"
    {
	   (yyval.sclist) = new BCList((yyvsp[0].sclnode));
	}
    break;

  case 76:
#line 803 "parser.yy"
    {
	   (yyvsp[-2].sclist)->add((yyvsp[0].sclnode));
        }
    break;

  case 77:
#line 810 "parser.yy"
    {
	   (yyval.sclnode) = new BCNode((yyvsp[0].string), false);
	}
    break;

  case 78:
#line 814 "parser.yy"
    {
	   (yyval.sclnode) = new BCNode(new NamedScope((yyvsp[0].string)), false);
	}
    break;

  case 79:
#line 818 "parser.yy"
    {
	   (yyval.sclnode) = new BCNode((yyvsp[0].string), true);
	}
    break;

  case 80:
#line 822 "parser.yy"
    {
	   (yyval.sclnode) = new BCNode(new NamedScope((yyvsp[0].string)), true);
	}
    break;

  case 81:
#line 829 "parser.yy"
    { 
           (yyval.qoreclass) = new QoreClass();
	   (yyval.qoreclass)->addMethod((yyvsp[0].objectfunc)); 
	}
    break;

  case 82:
#line 834 "parser.yy"
    {
	   (yyval.qoreclass) = new QoreClass();
	   (yyval.qoreclass)->mergePrivateMembers((yyvsp[0].privlist));
	}
    break;

  case 83:
#line 839 "parser.yy"
    { 
	   (yyvsp[-1].qoreclass)->addMethod((yyvsp[0].objectfunc)); 
	   (yyval.qoreclass) = (yyvsp[-1].qoreclass); 
	}
    break;

  case 84:
#line 844 "parser.yy"
    { 
	   (yyvsp[-1].qoreclass)->mergePrivateMembers((yyvsp[0].privlist)); 
	   (yyval.qoreclass) = (yyvsp[-1].qoreclass); 
	}
    break;

  case 85:
#line 852 "parser.yy"
    {
	   (yyval.privlist) = (yyvsp[-1].privlist);
	}
    break;

  case 86:
#line 859 "parser.yy"
    {
	   (yyval.privlist) = new MemberList((yyvsp[0].string));
        }
    break;

  case 87:
#line 863 "parser.yy"
    {
	   if ((yyvsp[-2].privlist)->add((yyvsp[0].string)))
	      free((yyvsp[0].string));
	   (yyval.privlist) = (yyvsp[-2].privlist);
	}
    break;

  case 88:
#line 872 "parser.yy"
    {
	   if ((yyvsp[-1].bcalist) && strcmp((yyvsp[-5].string), "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   (yyval.objectfunc) = new Method(new UserFunction((yyvsp[-5].string), new Paramlist((yyvsp[-3].node)), (yyvsp[0].sblock), (yyvsp[-6].i4) & OFM_SYNCED), (yyvsp[-6].i4) & OFM_PRIVATE, (yyvsp[-1].bcalist));
	}
    break;

  case 89:
#line 878 "parser.yy"
    {
	   if ((yyvsp[-1].bcalist) && strcmp((yyvsp[-5].string), "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   (yyval.objectfunc) = new Method(new UserFunction((yyvsp[-5].string), new Paramlist((yyvsp[-3].node)), (yyvsp[0].sblock)), 0, (yyvsp[-1].bcalist));
	}
    break;

  case 90:
#line 884 "parser.yy"
    {
	   if ((yyvsp[-1].bcalist) && strcmp((yyvsp[-4].string), "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   (yyval.objectfunc) = new Method(new UserFunction((yyvsp[-4].string), new Paramlist((yyvsp[-3].node)), (yyvsp[0].sblock), (yyvsp[-5].i4) & OFM_SYNCED), (yyvsp[-5].i4) & OFM_PRIVATE, (yyvsp[-1].bcalist));
	}
    break;

  case 91:
#line 890 "parser.yy"
    {
	   if ((yyvsp[-1].bcalist) && strcmp((yyvsp[-4].string), "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   (yyval.objectfunc) = new Method(new UserFunction((yyvsp[-4].string), new Paramlist((yyvsp[-3].node)), (yyvsp[0].sblock)), 0, (yyvsp[-1].bcalist));
	}
    break;

  case 92:
#line 899 "parser.yy"
    {
	   tryAddMethod((yyvsp[-6].i4), (yyvsp[-5].string), (yyvsp[-3].node), (yyvsp[-1].bcalist), (yyvsp[0].sblock));
	}
    break;

  case 93:
#line 903 "parser.yy"
    {
	   tryAddMethod(0, (yyvsp[-5].string), (yyvsp[-3].node), (yyvsp[-1].bcalist), (yyvsp[0].sblock));
	}
    break;

  case 94:
#line 910 "parser.yy"
    {
	   (yyval.bcalist) = (yyvsp[0].bcalist);
	}
    break;

  case 95:
#line 914 "parser.yy"
    {
	   (yyval.bcalist) = NULL;
	}
    break;

  case 96:
#line 921 "parser.yy"
    {
	   (yyval.bcalist) = new BCAList((yyvsp[0].bcanode));
	}
    break;

  case 97:
#line 925 "parser.yy"
    {
	   (yyvsp[-2].bcalist)->add((yyvsp[0].bcanode));
	   (yyval.bcalist) = (yyvsp[-2].bcalist);
	}
    break;

  case 98:
#line 933 "parser.yy"
    {
	   (yyval.bcanode) = new BCANode((yyvsp[-3].string), makeArgs((yyvsp[-1].node)));
	}
    break;

  case 99:
#line 937 "parser.yy"
    {
	   (yyval.bcanode) = new BCANode(new NamedScope((yyvsp[-3].string)), makeArgs((yyvsp[-1].node)));
	}
    break;

  case 100:
#line 943 "parser.yy"
    { (yyval.i4) = (yyvsp[0].i4); }
    break;

  case 101:
#line 945 "parser.yy"
    {
	   if (((yyvsp[-1].i4) | (yyvsp[0].i4)) == (yyvsp[-1].i4))
	      parse_error("double object method modifier");
	   (yyval.i4) = (yyvsp[-1].i4) | (yyvsp[0].i4); 
	}
    break;

  case 102:
#line 953 "parser.yy"
    { (yyval.i4) = OFM_PRIVATE; }
    break;

  case 103:
#line 954 "parser.yy"
    { (yyval.i4) = OFM_SYNCED; }
    break;

  case 104:
#line 959 "parser.yy"
    { 
	   getProgram()->registerUserFunction(new UserFunction((yyvsp[-4].string), new Paramlist((yyvsp[-2].node)), (yyvsp[0].sblock))); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", (yyvsp[-4].string));
	}
    break;

  case 105:
#line 966 "parser.yy"
    {
	   getProgram()->registerUserFunction(new UserFunction((yyvsp[-4].string), new Paramlist((yyvsp[-2].node)), (yyvsp[0].sblock), 1)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", (yyvsp[-4].string));
	}
    break;

  case 106:
#line 973 "parser.yy"
    { 
	   getProgram()->registerUserFunction(new UserFunction((yyvsp[-3].string), new Paramlist((yyvsp[-2].node)), (yyvsp[0].sblock))); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", (yyvsp[-3].string));
	}
    break;

  case 107:
#line 980 "parser.yy"
    {
	   getProgram()->registerUserFunction(new UserFunction((yyvsp[-3].string), new Paramlist((yyvsp[-2].node)), (yyvsp[0].sblock), 1)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", (yyvsp[-3].string));
	}
    break;

  case 108:
#line 990 "parser.yy"
    {
	   printd(5, "parsing call %s()\n", (yyvsp[-3].string));
	   (yyval.node) = new QoreNode((yyvsp[-3].string), makeArgs((yyvsp[-1].node)));
        }
    break;

  case 109:
#line 995 "parser.yy"
    {
	   printd(5, "parsing call %s()\n", (yyvsp[-2].string));
	   (yyval.node) = new QoreNode((yyvsp[-2].string), makeArgs((yyvsp[-1].node)));
        }
    break;

  case 110:
#line 1003 "parser.yy"
    {
	   printd(5, "parsing scoped class call (for new) %s()\n", (yyvsp[-3].string));
	   (yyval.node) = new QoreNode(new NamedScope((yyvsp[-3].string)), makeArgs((yyvsp[-1].node)));
        }
    break;

  case 111:
#line 1011 "parser.yy"
    {
	   printd(5, "parsing in-object method call %s()\n", (yyvsp[-3].string));
	   (yyval.node) = new QoreNode(makeArgs((yyvsp[-1].node)), (yyvsp[-3].string));
        }
    break;

  case 112:
#line 1016 "parser.yy"
    {
	   printd(5, "parsing in-object base class method call %s()\n", (yyvsp[-3].nscope)->ostr);
	   if (!strcmp((yyvsp[-3].nscope)->getIdentifier(), "copy"))
	      parse_error("illegal call to base class copy method '%s'", (yyvsp[-3].nscope)->ostr);

	   (yyval.node) = new QoreNode(makeArgs((yyvsp[-1].node)), (yyvsp[-3].nscope));
	}
    break;

  case 113:
#line 1027 "parser.yy"
    { (yyval.node) = splice_expressions((yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 114:
#line 1029 "parser.yy"
    {
	   if ((yyvsp[-1].node)->type != NT_LIST) {
	      parse_error("problem in parsing ',' in list: left is no list!");
	   }
	   (yyval.node) = (yyvsp[-1].node);
        }
    break;

  case 115:
#line 1039 "parser.yy"
    {
	   class Hash *h = new Hash(1);
	   h->setKeyValue((yyvsp[0].hashelement)->key, (yyvsp[0].hashelement)->value, NULL);
	   delete (yyvsp[0].hashelement);
	   (yyval.node) = new QoreNode(h);
	}
    break;

  case 116:
#line 1046 "parser.yy"
    {
	   (yyvsp[-2].node)->val.hash->setKeyValue((yyvsp[0].hashelement)->key, (yyvsp[0].hashelement)->value, NULL);
	   delete (yyvsp[0].hashelement);
	   (yyval.node) = (yyvsp[-2].node);
	}
    break;

  case 117:
#line 1052 "parser.yy"
    { /* empty ',' on end of hash */ }
    break;

  case 118:
#line 1057 "parser.yy"
    { (yyval.hashelement) = new HashElement((yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 119:
#line 1059 "parser.yy"
    { (yyval.hashelement) = new HashElement(HE_TAG_CONST, (yyvsp[-2].string), (yyvsp[0].node)); }
    break;

  case 120:
#line 1061 "parser.yy"
    { (yyval.hashelement) = new HashElement(HE_TAG_SCOPED_CONST, (yyvsp[-2].string), (yyvsp[0].node)); }
    break;

  case 121:
#line 1065 "parser.yy"
    { (yyval.node) = (yyvsp[0].node); }
    break;

  case 122:
#line 1067 "parser.yy"
    { (yyval.node) = (yyvsp[0].node); }
    break;

  case 123:
#line 1069 "parser.yy"
    { (yyval.node) = (yyvsp[-1].node); }
    break;

  case 124:
#line 1071 "parser.yy"
    { 
	   (yyval.node) = new QoreNode(new NamedScope((yyvsp[0].string))); 
	}
    break;

  case 125:
#line 1075 "parser.yy"
    { (yyval.node) = new QoreNode(NT_VARREF); (yyval.node)->val.vref = new VarRef((yyvsp[0].string), VT_UNRESOLVED); }
    break;

  case 126:
#line 1077 "parser.yy"
    {
	   (yyval.node) = new QoreNode(new VarRef((yyvsp[0].string), VT_LOCAL)); 
	}
    break;

  case 127:
#line 1081 "parser.yy"
    {
	   (yyvsp[-1].node)->type = NT_VLIST;
	   for (int i = 0; i < (yyvsp[-1].node)->val.list->size(); i++)
	   {
	      class QoreNode *n = (yyvsp[-1].node)->val.list->retrieve_entry(i);
	      if (n->type != NT_VARREF)
		 parse_error("element %d in list following 'my' is not a variable reference (%s)", i, n->type->name);
	      else
		 n->val.vref->type = VT_LOCAL;
	   }
	   (yyval.node) = (yyvsp[-1].node);
	}
    break;

  case 128:
#line 1094 "parser.yy"
    {
	   getProgram()->addGlobalVarDef(strdup((yyvsp[0].string)));
	   (yyval.node) = new QoreNode(new VarRef((yyvsp[0].string), VT_GLOBAL)); 
	}
    break;

  case 129:
#line 1099 "parser.yy"
    { 
	   (yyvsp[-1].node)->type = NT_VLIST;
	   for (int i = 0; i < (yyvsp[-1].node)->val.list->size(); i++)
	   {
	      class QoreNode *n = (yyvsp[-1].node)->val.list->retrieve_entry(i);
	      if (n->type != NT_VARREF)
		 parse_error("element %d in list following 'our' is not a variable reference (%s)", i, n->type->name);
	      else
	      {
		 n->val.vref->type = VT_GLOBAL;
		 getProgram()->addGlobalVarDef(strdup(n->val.vref->name));
	      }
	   }
	   (yyval.node) = (yyvsp[-1].node);
	}
    break;

  case 130:
#line 1115 "parser.yy"
    { (yyval.node) = new QoreNode(NT_BAREWORD); (yyval.node)->val.c_str = (yyvsp[0].string); }
    break;

  case 131:
#line 1117 "parser.yy"
    { 
	   (yyval.node) = new QoreNode(NT_CONTEXTREF); 
	   (yyval.node)->val.c_str = (yyvsp[0].string);
	   //printd(5, "context ref, %s, %08x, %08x, create\n", $1, $$, $1);
	}
    break;

  case 132:
#line 1123 "parser.yy"
    { (yyval.node) = new QoreNode(NT_CONTEXT_ROW); }
    break;

  case 133:
#line 1125 "parser.yy"
    { (yyval.node) = new QoreNode(NT_COMPLEXCONTEXTREF); (yyval.node)->val.complex_cref = new ComplexContextRef((yyvsp[0].string)); }
    break;

  case 134:
#line 1127 "parser.yy"
    {
	   (yyval.node) = new QoreNode(NT_FIND);
	   (yyval.node)->val.find = new Find((yyvsp[-6].node), (yyvsp[-4].node), (yyvsp[-1].node));
	}
    break;

  case 135:
#line 1132 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of plus-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_PLUS_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_PLUS_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 136:
#line 1142 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of minus-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_MINUS_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_MINUS_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));

	}
    break;

  case 137:
#line 1153 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of and-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_AND_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_AND_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 138:
#line 1163 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of or-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_OR_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_OR_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 139:
#line 1173 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of modula-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_MODULA_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_MODULA_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 140:
#line 1183 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of multiply-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_MULTIPLY_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_MULTIPLY_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 141:
#line 1193 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of divide-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_DIVIDE_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_DIVIDE_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 142:
#line 1203 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of xor-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_XOR_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_XOR_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 143:
#line 1213 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of shift-left-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_SHIFT_LEFT_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_SHIFT_LEFT_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 144:
#line 1223 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of shift-right-equals operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_SHIFT_RIGHT_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	      (yyval.node) = makeTree(OP_SHIFT_RIGHT_EQUALS, (yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 145:
#line 1233 "parser.yy"
    {
	   if ((yyvsp[-2].node)->type == NT_FLIST)
	      (yyvsp[-2].node)->type = NT_LIST;
	   if ((yyvsp[-2].node)->type == NT_LIST)
	   {
	      bool ok = true;
	      for (int i = 0; i < (yyvsp[-2].node)->val.list->size(); i++)
	      {
		 QoreNode *n = (yyvsp[-2].node)->val.list->retrieve_entry(i);
		 if (check_lvalue(n))
		 {
		    parse_error("element %d in list assignment is not an lvalue (%s)", i, n->type->name);
		    ok = false;
		 }
	      }
	      if (ok)
		 (yyval.node) = makeTree(OP_LIST_ASSIGNMENT, (yyvsp[-2].node), (yyvsp[0].node));
	      else
		 (yyval.node) = makeErrorTree(OP_LIST_ASSIGNMENT, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   else
	   {
	      if (check_lvalue((yyvsp[-2].node)))
	      {
		 parse_error("left-hand side of assignment is not an lvalue (%s)", (yyvsp[-2].node)->type->name);
		 (yyval.node) = makeErrorTree(OP_ASSIGNMENT, (yyvsp[-2].node), (yyvsp[0].node));
	      }
	      else
		 (yyval.node) = makeTree(OP_ASSIGNMENT, (yyvsp[-2].node), (yyvsp[0].node));
	   }
	   //print_tree($1, 0);
	}
    break;

  case 146:
#line 1266 "parser.yy"
    { (yyval.node) = makeTree(OP_EXISTS, (yyvsp[0].node), NULL); }
    break;

  case 147:
#line 1268 "parser.yy"
    { (yyval.node) = makeTree(OP_ELEMENTS, (yyvsp[0].node), NULL); }
    break;

  case 148:
#line 1270 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_INSTANCEOF, (yyvsp[-2].node), new QoreNode(new ClassRef(new NamedScope((yyvsp[0].string)))));
	}
    break;

  case 149:
#line 1274 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_INSTANCEOF, (yyvsp[-2].node), new QoreNode(new ClassRef(new NamedScope((yyvsp[0].string)))));
	}
    break;

  case 150:
#line 1278 "parser.yy"
    { (yyval.node) = makeTree(OP_KEYS, (yyvsp[0].node), NULL); }
    break;

  case 151:
#line 1280 "parser.yy"
    {
	   if ((yyvsp[0].node)->type != NT_LIST || (yyvsp[0].node)->val.list->size() != 2)
	   {
	      parse_error("invalid arguments to unshift, expected: lvalue, expression (%s)", (yyvsp[0].node)->type->name);
	      (yyval.node) = makeErrorTree(OP_UNSHIFT, (yyvsp[0].node), NULL);
	   }
	   else
	   {
	      QoreNode *lv = (yyvsp[0].node)->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to unshift is not an lvalue");
		 (yyval.node) = makeErrorTree(OP_UNSHIFT, lv, (yyvsp[0].node)->val.list->shift());
	      }
	      else
		 (yyval.node) = makeTree(OP_UNSHIFT, lv, (yyvsp[0].node)->val.list->shift());
	      (yyvsp[0].node)->deref(NULL);
	   }
	}
    break;

  case 152:
#line 1300 "parser.yy"
    { 
	   if (check_lvalue((yyvsp[0].node)))
	   {
	      parse_error("argument to shift operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_SHIFT, (yyvsp[0].node), NULL); 
	   }
	   else
	      (yyval.node) = makeTree(OP_SHIFT, (yyvsp[0].node), NULL); 
	}
    break;

  case 153:
#line 1310 "parser.yy"
    {
	   if ((yyvsp[0].node)->type != NT_LIST || (yyvsp[0].node)->val.list->size() != 2)
	   {
	      parse_error("invalid arguments to push, expected: lvalue, expression (%s)", (yyvsp[0].node)->type->name);
	      (yyval.node) = makeErrorTree(OP_PUSH, (yyvsp[0].node), NULL);
	   }
	   else
	   {
	      
	      QoreNode *lv = (yyvsp[0].node)->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to push is not an lvalue");
		 (yyval.node) = makeErrorTree(OP_PUSH, lv, (yyvsp[0].node)->val.list->shift());
	      }
	      else
		 (yyval.node) = makeTree(OP_PUSH, lv, (yyvsp[0].node)->val.list->shift());
	      (yyvsp[0].node)->deref(NULL);
	   }
	}
    break;

  case 154:
#line 1331 "parser.yy"
    {
	   if (check_lvalue((yyvsp[0].node)))
	   {
	      parse_error("argument to pop operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_POP, (yyvsp[0].node), NULL); 
	   }
	   else
	      (yyval.node) = makeTree(OP_POP, (yyvsp[0].node), NULL); 
	}
    break;

  case 155:
#line 1341 "parser.yy"
    {
	   if ((yyvsp[0].node)->type != NT_LIST || (yyvsp[0].node)->val.list->size() < 2 || (yyvsp[0].node)->val.list->size() > 4)
	   {
	      parse_error("invalid arguments to splice, expected: lvalue, offset exp [length exp, [list exp]] (%s)", (yyvsp[0].node)->type->name);
	      (yyval.node) = makeErrorTree(OP_SPLICE, (yyvsp[0].node), NULL);
	   }
	   else
	   {
	      QoreNode *lv = (yyvsp[0].node)->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to splice is not an lvalue");
		 (yyval.node) = makeErrorTree(OP_SPLICE, lv, (yyvsp[0].node));
	      }
	      else
		 (yyval.node) = makeTree(OP_SPLICE, lv, (yyvsp[0].node));
	   }
	}
    break;

  case 156:
#line 1360 "parser.yy"
    { (yyval.node) = new QoreNode((yyvsp[-4].node), OP_QUESTION_MARK, splice_expressions((yyvsp[-2].node), (yyvsp[0].node))); }
    break;

  case 157:
#line 1362 "parser.yy"
    {
	   if (check_lvalue((yyvsp[0].node)))
	   {
	      parse_error("pre-increment expression is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_PRE_INCREMENT, (yyvsp[0].node), NULL);
	   }
	   else
	      (yyval.node) = makeTree(OP_PRE_INCREMENT, (yyvsp[0].node), NULL);
        }
    break;

  case 158:
#line 1372 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-1].node)))
	   {
	      parse_error("post-increment expression is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_POST_INCREMENT, (yyvsp[-1].node), NULL);
	   }
	   else
	      (yyval.node) = makeTree(OP_POST_INCREMENT, (yyvsp[-1].node), NULL);
        }
    break;

  case 159:
#line 1382 "parser.yy"
    {
	   if (check_lvalue((yyvsp[0].node)))
	   {
	      parse_error("pre-decrement expression is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_PRE_DECREMENT, (yyvsp[0].node), NULL);
	   }
	   else
	      (yyval.node) = makeTree(OP_PRE_DECREMENT, (yyvsp[0].node), NULL);
        }
    break;

  case 160:
#line 1392 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-1].node)))
	   {
	      parse_error("post-decrement expression is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_POST_DECREMENT, (yyvsp[-1].node), NULL);
	   }
	   else
	      (yyval.node) = makeTree(OP_POST_DECREMENT, (yyvsp[-1].node), NULL);
        }
    break;

  case 161:
#line 1401 "parser.yy"
    { (yyval.node) = (yyvsp[0].node); }
    break;

  case 162:
#line 1402 "parser.yy"
    { (yyval.node) = (yyvsp[0].node); }
    break;

  case 163:
#line 1403 "parser.yy"
    { (yyval.node) = new QoreNode(NT_SELF_VARREF); (yyval.node)->val.c_str = (yyvsp[0].string); }
    break;

  case 164:
#line 1404 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 165:
#line 1405 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 166:
#line 1406 "parser.yy"
    { (yyval.node) = makeTree(OP_BIN_OR, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 167:
#line 1407 "parser.yy"
    { (yyval.node) = makeTree(OP_BIN_AND, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 168:
#line 1408 "parser.yy"
    { (yyval.node) = makeTree(OP_BIN_XOR, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 169:
#line 1410 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_REGEX_MATCH, (yyvsp[-2].node), new QoreNode((yyvsp[0].Regex)));
	}
    break;

  case 170:
#line 1414 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_REGEX_NMATCH, (yyvsp[-2].node), new QoreNode((yyvsp[0].Regex)));
	}
    break;

  case 171:
#line 1418 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_REGEX_MATCH, (yyvsp[-2].node), new QoreNode(new QoreRegex((yyvsp[0].node)->val.String)));
	   (yyvsp[0].node)->type = NT_INT;
	   (yyvsp[0].node)->deref(NULL);
	}
    break;

  case 172:
#line 1424 "parser.yy"
    { 
	   (yyval.node) = makeTree(OP_REGEX_NMATCH, (yyvsp[-2].node), new QoreNode(new QoreRegex((yyvsp[0].node)->val.String))); 
	   (yyvsp[0].node)->type = NT_INT;
	   (yyvsp[0].node)->deref(NULL);
	}
    break;

  case 173:
#line 1430 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of regular expression substitution operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_REGEX_SUBST, (yyvsp[-2].node), new QoreNode((yyvsp[0].RegexSubst)));
	   }
	   else
	   {
	      //printf("REGEX_SUBST: '%s'\n", $3->getPattern()->getBuffer());
	      (yyval.node) = makeTree(OP_REGEX_SUBST, (yyvsp[-2].node), new QoreNode((yyvsp[0].RegexSubst)));
	   }
	}
    break;

  case 174:
#line 1443 "parser.yy"
    {
	   if (check_lvalue((yyvsp[-2].node)))
	   {
	      parse_error("left-hand side of transliteration operator is not an lvalue");
	      (yyval.node) = makeErrorTree(OP_REGEX_TRANS, (yyvsp[-2].node), new QoreNode((yyvsp[0].RegexTrans)));
	   }
	   else
	   {
	      //printf("REGEX_SUBST: '%s'\n", $3->getPattern()->getBuffer());
	      (yyval.node) = makeTree(OP_REGEX_TRANS, (yyvsp[-2].node), new QoreNode((yyvsp[0].RegexTrans)));
	   }
	}
    break;

  case 175:
#line 1455 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_GT, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 176:
#line 1456 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_LT, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 177:
#line 1457 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_CMP, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 178:
#line 1458 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_EQ, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 179:
#line 1459 "parser.yy"
    { (yyval.node) = makeTree(OP_ABSOLUTE_EQ, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 180:
#line 1460 "parser.yy"
    { (yyval.node) = makeTree(OP_ABSOLUTE_NE, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 181:
#line 1461 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_NE, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 182:
#line 1462 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_LE, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 183:
#line 1463 "parser.yy"
    { (yyval.node) = makeTree(OP_LOG_GE, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 184:
#line 1464 "parser.yy"
    { (yyval.node) = makeTree(OP_SHIFT_LEFT, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 185:
#line 1465 "parser.yy"
    { (yyval.node) = makeTree(OP_SHIFT_RIGHT, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 186:
#line 1466 "parser.yy"
    { (yyval.node) = makeTree(OP_PLUS, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 187:
#line 1467 "parser.yy"
    { (yyval.node) = makeTree(OP_MINUS, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 188:
#line 1468 "parser.yy"
    { (yyval.node) = makeTree(OP_MODULA, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 189:
#line 1469 "parser.yy"
    { (yyval.node) = makeTree(OP_DIV, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 190:
#line 1470 "parser.yy"
    { (yyval.node) = makeTree(OP_MULT, (yyvsp[-2].node), (yyvsp[0].node)); }
    break;

  case 191:
#line 1471 "parser.yy"
    { (yyval.node) = makeTree(OP_UNARY_MINUS, (yyvsp[0].node), NULL); }
    break;

  case 192:
#line 1472 "parser.yy"
    { (yyval.node) = makeTree(OP_BIN_NOT, (yyvsp[0].node), NULL); }
    break;

  case 193:
#line 1473 "parser.yy"
    { (yyval.node) = makeTree(OP_NOT, (yyvsp[0].node), NULL); }
    break;

  case 194:
#line 1475 "parser.yy"
    { 
	   (yyval.node) = new QoreNode(NT_REFERENCE);
	   (yyval.node)->val.lvexp = (yyvsp[0].node);
	   if (check_lvalue((yyvsp[0].node)))
	      parse_error("argument to reference operator is not an lvalue");
	}
    break;

  case 195:
#line 1482 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_NEW, new QoreNode(new NamedScope((yyvsp[0].node)->val.fcall->f.c_str), (yyvsp[0].node)->val.fcall->args), NULL); 
	   (yyvsp[0].node)->val.fcall->f.c_str = NULL;
	   (yyvsp[0].node)->val.fcall->args = NULL;	   
	   (yyvsp[0].node)->deref(NULL);
	   // see if new can be used
	   if (checkParseOption(PO_NO_NEW))
	      parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	}
    break;

  case 196:
#line 1492 "parser.yy"
    { 
	   (yyval.node) = makeTree(OP_NEW, (yyvsp[0].node), NULL); 
	   // see if new can be used
	   if (checkParseOption(PO_NO_NEW))
	      parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	}
    break;

  case 197:
#line 1499 "parser.yy"
    {
	   (yyval.node) = makeTree(OP_BACKGROUND, (yyvsp[0].node), NULL);
	   // check to see if the expression is legal
	   if (checkParseOption(PO_NO_THREADS))
	      parse_error("illegal use of \"background\" operator (conflicts with parse option NO_THREADS)");
	   else if (!hasEffect((yyvsp[0].node)))
	      parse_error("argument to background operator has no effect");
	}
    break;

  case 198:
#line 1508 "parser.yy"
    {
	   (yyval.node) = new QoreNode(NT_BACKQUOTE);
	   (yyval.node)->val.c_str = (yyvsp[0].string);
	   if (checkParseOption(PO_NO_EXTERNAL_PROCESS))
	      parse_error("illegal use of backquote operator (conflicts with parse option NO_EXTERNAL_PROCESS)");
	}
    break;

  case 199:
#line 1514 "parser.yy"
    { (yyval.node) = makeTree(OP_LIST_REF, (yyvsp[-3].node), (yyvsp[-1].node)); }
    break;

  case 200:
#line 1515 "parser.yy"
    { (yyval.node) = makeTree(OP_OBJECT_REF, (yyvsp[-3].node), (yyvsp[-1].node)); }
    break;

  case 201:
#line 1517 "parser.yy"
    {
	   (yyval.node) = process_dot((yyvsp[-2].node), (yyvsp[0].node));
	}
    break;

  case 202:
#line 1521 "parser.yy"
    { 
	   (yyval.node) = (yyvsp[-1].node); 
	   if ((yyvsp[-1].node)->type == NT_LIST) 
	      (yyvsp[-1].node)->type = NT_FLIST; 
	}
    break;

  case 203:
#line 1526 "parser.yy"
    { (yyval.node) = new QoreNode(NT_FLIST); (yyval.node)->val.list = new List(); }
    break;

  case 204:
#line 1530 "parser.yy"
    { (yyval.node) = new QoreNode(NT_FLOAT); (yyval.node)->val.floatval = (yyvsp[0].decimal); }
    break;

  case 205:
#line 1531 "parser.yy"
    { (yyval.node) = new QoreNode(NT_INT); (yyval.node)->val.intval = (yyvsp[0].integer); }
    break;

  case 206:
#line 1532 "parser.yy"
    { (yyval.node) = new QoreNode((yyvsp[0].String)); }
    break;

  case 207:
#line 1533 "parser.yy"
    { (yyval.node) = new QoreNode((yyvsp[0].datetime)); }
    break;

  case 208:
#line 1534 "parser.yy"
    { (yyval.node) = new QoreNode(NT_NULL); }
    break;

  case 209:
#line 1535 "parser.yy"
    { (yyval.node) = new QoreNode(NT_NOTHING); }
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 4685 "parser.cc"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yyscanner, yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (yyscanner, YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (yyscanner, YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (yyscanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1538 "parser.yy"


