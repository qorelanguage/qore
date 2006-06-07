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
     BASE_CLASS_CALL = 317,
     REGEX = 318,
     IFX = 319,
     TOK_ELSE = 320,
     SHIFT_RIGHT_EQUALS = 321,
     SHIFT_LEFT_EQUALS = 322,
     XOR_EQUALS = 323,
     DIVIDE_EQUALS = 324,
     MULTIPLY_EQUALS = 325,
     MODULA_EQUALS = 326,
     OR_EQUALS = 327,
     AND_EQUALS = 328,
     MINUS_EQUALS = 329,
     PLUS_EQUALS = 330,
     TOK_SPLICE = 331,
     TOK_PUSH = 332,
     TOK_UNSHIFT = 333,
     LOGICAL_OR = 334,
     LOGICAL_AND = 335,
     REGEX_NMATCH = 336,
     REGEX_MATCH = 337,
     ABSOLUTE_NE = 338,
     ABSOLUTE_EQ = 339,
     LOGICAL_CMP = 340,
     LOGICAL_GE = 341,
     LOGICAL_LE = 342,
     LOGICAL_NE = 343,
     LOGICAL_EQ = 344,
     TOK_INSTANCEOF = 345,
     TOK_EXISTS = 346,
     SHIFT_LEFT = 347,
     SHIFT_RIGHT = 348,
     TOK_KEYS = 349,
     TOK_ELEMENTS = 350,
     TOK_POP = 351,
     TOK_SHIFT = 352,
     NEG = 353
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
#define BASE_CLASS_CALL 317
#define REGEX 318
#define IFX 319
#define TOK_ELSE 320
#define SHIFT_RIGHT_EQUALS 321
#define SHIFT_LEFT_EQUALS 322
#define XOR_EQUALS 323
#define DIVIDE_EQUALS 324
#define MULTIPLY_EQUALS 325
#define MODULA_EQUALS 326
#define OR_EQUALS 327
#define AND_EQUALS 328
#define MINUS_EQUALS 329
#define PLUS_EQUALS 330
#define TOK_SPLICE 331
#define TOK_PUSH 332
#define TOK_UNSHIFT 333
#define LOGICAL_OR 334
#define LOGICAL_AND 335
#define REGEX_NMATCH 336
#define REGEX_MATCH 337
#define ABSOLUTE_NE 338
#define ABSOLUTE_EQ 339
#define LOGICAL_CMP 340
#define LOGICAL_GE 341
#define LOGICAL_LE 342
#define LOGICAL_NE 343
#define LOGICAL_EQ 344
#define TOK_INSTANCEOF 345
#define TOK_EXISTS 346
#define SHIFT_LEFT 347
#define SHIFT_RIGHT 348
#define TOK_KEYS 349
#define TOK_ELEMENTS 350
#define TOK_POP 351
#define TOK_SHIFT 352
#define NEG 353




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 223 "parser.yy"
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
      class SwitchStatement *switchstmt;
      class CaseNode *casenode;
      class BCList *sclist;
      class BCNode *sclnode;
      class BCAList *bcalist;
      class BCANode *bcanode;
      class NamedScope *nscope;
      class QoreRegex *Regex;
} YYSTYPE;
/* Line 1447 of yacc.c.  */
#line 266 "parser.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





