%{
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
#include <qore/qore_thread.h>
#include <qore/QoreProgram.h>
#include <qore/Find.h>
#include <qore/Namespace.h>
#include <qore/ParserSupport.h>
#include <qore/DateTime.h>
#include <qore/RegexSubst.h>
#include <qore/QoreRegex.h>
#include <qore/RegexTrans.h>
#include <qore/BinaryObject.h>
#include <qore/SwitchStatementWithOperators.h>

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

// copies keys added, deletes them in the destructor
static inline class QoreNode *splice_expressions(class QoreNode *a1, class QoreNode *a2)
{
   //tracein("splice_expressions()");
   if (a1->type == NT_LIST)
   {
      //printd(5, "LIST x\n");
      a1->val.list->push(a2);
      return a1;
   }
   //printd(5, "NODE x\n");
   class QoreNode *nl = new QoreNode(NT_LIST);
   nl->val.list = new List(1);
   nl->val.list->push(a1);
   nl->val.list->push(a2);
   //traceout("splice_expressions()");
   return nl;
}

static inline int checkParseOption(int o)
{
   return getParseOptions() & o;
}

class MemberList : private strset_t
{
   public:
      DLLLOCAL ~MemberList();
      DLLLOCAL class MemberList *copy() const;
      DLLLOCAL int add(char *name);
      DLLLOCAL inline void mergePrivateMembers(class QoreClass *qc);
};

inline void MemberList::mergePrivateMembers(class QoreClass *qc)
{
   strset_t::iterator i;
   while ((i = begin()) != end())
   {
      char *name = *i;
      erase(i);
      qc->addPrivateMember(name);
   }
}

MemberList::~MemberList()
{
   strset_t::iterator i;
   while ((i = begin()) != end())
   {
      char *name = *i;
      erase(i);
      free(name);
   }
}

class MemberList *MemberList::copy() const
{
   class MemberList *nl = new MemberList();
   for (strset_t::iterator i = begin(); i != end(); i++)
      nl->add(*i);

   return nl;
}

int MemberList::add(char *name)
{
   if (find(name) != end())
      return -1;
   // add new member to list
   insert(name);
   return 0;
}

static inline void addConstant(class NamedScope *name, class QoreNode *value)
{
   getRootNS()->rootAddConstant(name, value);
}

static inline void addClass(class NamedScope *name, class QoreClass *oc)
{
   tracein("addClass()");
   getRootNS()->rootAddClass(name, oc);
   traceout("addClass()");
}

static inline class QoreClass *parseFindClass(char *name)
{
   class QoreClass *c = getRootNS()->rootFindClass(name);
   if (!c)
      parse_error("reference to undefined class '%s'", name);

   return c;
}

static QoreNode *process_dot(class QoreNode *l, class QoreNode *r)
{
   if (r->type == NT_BAREWORD)
   {
      class QoreNode *rv = makeTree(OP_OBJECT_REF, l, new QoreNode(r->val.c_str));
      r->deref(NULL);
      return rv;
   }
   else if (r->type == NT_FUNCTION_CALL && r->val.fcall->getType() == FC_UNRESOLVED)
   {
      r->val.fcall->parseMakeMethod();
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
      n->val.list->clearNeedsEval();
      return false;
   }

   if (n->type == NT_HASH)
   {
      class HashIterator hi(n->val.hash);
      while (hi.next())
	 if (needsEval(hi.getValue()))
	    return true;
      // here we set needs_eval to false so the hash won't be evaluated again
      n->val.hash->clearNeedsEval();
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

   //printd(5, "hasEffect() node %08p type=%s op=%d ok=%d\n", n, n->type->name, n->type == NT_TREE ? n->val.tree.op : -1, ok);
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
      delete bcal;
      if (b)
	 delete b;
   }
   else
   {
      class Method *method = new Method(new UserFunction(strdup(name->getIdentifier()), new Paramlist(params), b, mod & OFM_SYNCED), mod & OFM_PRIVATE);
      
      if (getRootNS()->addMethodToClass(name, method, bcal))
      {
	 delete method;
	 if (bcal)
	    delete bcal;
      }
   }
   delete name;
}

struct MethodNode {
   public:
      // method to add to class
      class Method *m;
      // base class argument list for constructors
      class BCAList *bcal;

      DLLLOCAL inline MethodNode(class UserFunction *f, int p, class BCAList *bl) : bcal(bl)
      {
	 m = new Method(f, p);
      }
      DLLLOCAL inline ~MethodNode()
      {
	 if (m)
	    delete m;
	 if (bcal)
	    delete bcal;
      }
      DLLLOCAL inline void addAndDelete(class QoreClass *qc)
      {
	 qc->addMethod(m);
	 m = NULL;
	 if (bcal)
	 {
	    qc->parseAddBaseClassArgumentList(bcal);
	    bcal = NULL;
	 }
	 delete this;
      }
};


%}

%pure-parser
//%skeleton "lalr1.cc"
%lex-param {yyscan_t yyscanner}
%parse-param {yyscan_t yyscanner}
//%locations
%error-verbose

%union
{
      int i4;
      int64 integer;
      double decimal;
      class QoreString *String;
      char *string;
      class BinaryObject *binary;
      class QoreNode *node;
      class Statement *statement;
      class StatementBlock *sblock;
      class ContextModList *cmods;
      class ContextMod *cmod;
      class HashElement *hashelement;
      class UserFunction *userfunc;	
      class MethodNode *methodnode;
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
}

%{
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

%}

%token TOK_CLASS TOK_NEW TOK_OBJECT_REF TOK_RETURN TOK_MY
%token TOK_DO TOK_TRY TOK_THROW TOK_CATCH TOK_FINALLY
%token TOK_WHERE TOK_NULL P_INCREMENT P_DECREMENT 
%token TOK_WHILE TOK_IF TOK_FOR TOK_SUB TOK_THREAD_EXIT 
%token TOK_BREAK TOK_CONTINUE TOK_NOTHING TOK_CONTEXT_ROW
%token TOK_FIND TOK_AUTOCOMMIT TOK_FOREACH TOK_IN TOK_DELETE
%token TOK_PRIVATE TOK_BACKGROUND TOK_SYNCHRONIZED
%token TOK_CONTEXT TOK_SORT_BY TOK_SUB_CONTEXT TOK_CONST
%token TOK_SUMMARIZE TOK_BY TOK_SORT_DESCENDING_BY
%token TOK_NAMESPACE TOK_SCOPED_REF TOK_OUR TOK_RETHROW
%token TOK_SWITCH TOK_CASE TOK_DEFAULT TOK_INHERITS

%token <integer> INTEGER
%token <decimal> QFLOAT
%token <string> IDENTIFIER VAR_REF BACKQUOTE SELF_REF KW_IDENTIFIER_OPENPAREN
%token <string> SCOPED_REF CONTEXT_REF COMPLEX_CONTEXT_REF
%token <datetime> DATETIME
%token <String> QUOTED_WORD
%token <binary> BINARY
%token <RegexSubst> REGEX_SUBST
%token <RegexTrans> REGEX_TRANS
%token <nscope> BASE_CLASS_CALL
%token <Regex> REGEX REGEX_EXTRACT

%nonassoc IFX
%nonassoc TOK_ELSE

// FIXME: check precedence
%right PLUS_EQUALS MINUS_EQUALS AND_EQUALS OR_EQUALS MODULA_EQUALS MULTIPLY_EQUALS DIVIDE_EQUALS XOR_EQUALS SHIFT_LEFT_EQUALS SHIFT_RIGHT_EQUALS
%right '='
%nonassoc TOK_UNSHIFT TOK_PUSH TOK_SPLICE
%left ','
%right '?' ':'
%left LOGICAL_AND LOGICAL_OR
%left '&' '|' '^'	      // binary and, or, and xor
%left '<' '>' LOGICAL_EQ LOGICAL_NE LOGICAL_LE LOGICAL_GE LOGICAL_CMP ABSOLUTE_EQ ABSOLUTE_NE REGEX_MATCH REGEX_NMATCH
%right TOK_EXISTS TOK_INSTANCEOF
%left SHIFT_RIGHT SHIFT_LEFT  // binary shift right and left
%left '+' '-'		      // arithmetic plus and minus
%left '%'		      // modula
%left '*' '/'		      // arithmetic multiply and divide
%right TOK_ELEMENTS TOK_KEYS
%nonassoc TOK_SHIFT TOK_POP TOK_CHOMP
%left NEG		      // unary minus, defined for precedence
%right '~' '\\'               // binary not, reference operator
%left '!'		      // logical not
%right TOK_NEW TOK_BACKGROUND
%nonassoc P_INCREMENT P_DECREMENT
%left '{' '[' '.'             // list and object references, defined for precedence

%type <sblock>      block
%type <sblock>      statement_or_block
%type <sblock>      statements
%type <statement>   statement
%type <statement>   return_statement
%type <statement>   try_statement
%type <node>        exp
%type <node>        myexp
%type <node>        scalar
%type <node>        function_call
%type <node>        scoped_object_call
%type <node>        self_function_call
%type <node>        hash
%type <node>        list
%type <hashelement> hash_element
%type <cmods>       context_mods
%type <cmod>        context_mod
%type <methodnode>  method_definition
%type <privlist>    private_member_list
%type <privlist>    member_list
%type <qoreclass>   class_attributes
%type <objdef>      object_def
%type <ns>          top_namespace_decl
%type <ns>          namespace_decls
%type <nsn>         namespace_decl
%type <constnode>   scoped_const_decl
%type <constnode>   unscoped_const_decl
%type <i4>          method_modifiers
%type <i4>          method_modifier
%type <string>      optname
%type <statement>   switch_statement
%type <switchstmt>  case_block
%type <casenode>    case_code
%type <sclist>      superclass_list
%type <sclist>      inheritance_list
%type <sclnode>     superclass
%type <bcalist>     base_constructor_list
%type <bcalist>     base_constructors
%type <bcanode>     base_constructor

 // destructor actions for elements that need deleting when parse errors occur
%destructor { if ($$) delete $$; } BINARY DATETIME QUOTED_WORD REGEX REGEX_SUBST REGEX_EXTRACT REGEX_TRANS block statement_or_block statements statement return_statement try_statement hash_element context_mods context_mod method_definition object_def top_namespace_decl namespace_decls namespace_decl scoped_const_decl unscoped_const_decl switch_statement case_block case_code superclass base_constructor private_member_list member_list base_constructor_list base_constructors class_attributes
%destructor { if ($$) $$->deref(); } superclass_list inheritance_list
%destructor { if ($$) $$->deref(NULL); } exp myexp scalar function_call scoped_object_call self_function_call hash list
%destructor { free($$); } IDENTIFIER VAR_REF SELF_REF CONTEXT_REF COMPLEX_CONTEXT_REF BACKQUOTE SCOPED_REF KW_IDENTIFIER_OPENPAREN optname

%%
top_level_commands:
        top_level_command
	| top_level_commands top_level_command
	;

top_level_command:
        sub_def                      // registered directly
        | object_def		     
        {
	   addClass($1->name, $1->oc); 
	   // see if class definitions are allowed
	   if (checkParseOption(PO_NO_CLASS_DEFS))
	      parse_error("illegal class definition \"%s\" (conflicts with parse option NO_CLASS_DEFS)",
			  $1->oc->getName());
	   delete $1;
	}
	| scoped_const_decl 
        { 
	   addConstant($1->name, $1->value);
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  $1->name->ostr);

	   delete $1;
	}
        | unscoped_const_decl
        { 
	   getRootNS()->addConstant($1->name, $1->value); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  $1->name->ostr);
	   delete $1;
	}
        | object_outofline_function_def  // registered directly
	| statement                  
        { 
	   // if it is a global variable declaration, then do not register
	   if ($1 && $1->Type == S_EXPRESSION
	       && (($1->s.node->type == NT_VARREF
		    && $1->s.node->val.vref->type == VT_GLOBAL)
		   || ($1->s.node->type == NT_VLIST && 
		       $1->s.node->val.list->retrieve_entry(0)->val.vref->type == VT_GLOBAL)))
	      delete $1;
	   else
	      getProgram()->addStatement($1); 
	}
        | '{' statements '}'
        {
	   getProgram()->addStatement(new Statement($2));
        }
        | top_namespace_decl
        {
	   getRootNS()->addNamespace($1); 
	   // see if ns declaration is legal
	   if (checkParseOption(PO_NO_NAMESPACE_DEFS))
	      parse_error("illegal namespace definition \"%s\" (conflicts with parse option NO_NAMESPACE_DEFINITION)", $1->getName());
	}
	;

top_namespace_decl:
	TOK_NAMESPACE IDENTIFIER '{' namespace_decls '}'
	{ $4->setName($2); $$ = $4; }
        | TOK_NAMESPACE IDENTIFIER ';'
        { $$ = new Namespace($2); free($2); }
	;

namespace_decls:
	namespace_decl
        {
	   class Namespace *ns = new Namespace();
	   addNSNode(ns, $1);
	   $$ = ns;
        }
	| namespace_decls namespace_decl
	{  
	   addNSNode($1, $2);
	   $$ = $1;
	}
	;

namespace_decl:
	scoped_const_decl     
        { 
	   $$ = new NSNode($1); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  $1->name->ostr);
        }
        | unscoped_const_decl 
        { 
	   $$ = new NSNode($1); 
	   // see if constant definitions are allowed
	   if (checkParseOption(PO_NO_CONSTANT_DEFS))
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)",
			  $1->name->ostr);
	}
	| object_def          
        { 
	   $$ = new NSNode($1); 
	   // see if class definitions are allowed
	   if (checkParseOption(PO_NO_CLASS_DEFS))
	      parse_error("illegal class definition \"%s\" (conflicts with parse option NO_CLASS_DEFS)",
			  $1->oc->getName());
	}
	| top_namespace_decl  
        { 
	   $$ = new NSNode($1); 
	   // see if ns declaration is legal
	   if (checkParseOption(PO_NO_NAMESPACE_DEFS))
	      parse_error("illegal namespace definition \"%s\" (conflicts with parse option NO_NAMESPACE_DEFINITION)", $1->getName());
	}

unscoped_const_decl: 
	TOK_CONST IDENTIFIER '=' exp ';'
        { 
	   if (needsEval($4))
	      parse_error("constant expression needs run-time evaluation");
	   $$ = new ConstNode($2, $4); 
	}
        ;

scoped_const_decl:
	TOK_CONST SCOPED_REF '=' exp ';'
        {
	   if (needsEval($4))
	      parse_error("constant expression needs run-time evaluation");
	   $$ = new ConstNode($2, $4); 
	}
        ;

block:
	'{' statements '}'
        { $$ = $2; }
        |
        '{' /* NOTHING */ '}'
        { $$ = NULL; }
        ;

statement_or_block:
	statement
        { $$ = new StatementBlock($1); }
	|
        block
        { $$ = $1; }
	;

statements:
	statement
        { $$ = new StatementBlock($1); }
	|
	statements statement
        { $1->addStatement($2); $$ = $1; }
	;

optname: 
	/* empty */ 
        { $$ = NULL; }
        | IDENTIFIER { $$ = $1; }
        ;

statement:
	exp ';'
        {
	   // if the expression has no effect and it's not a variable declaration
	   if (!hasEffect($1) 
	       && ($1->type != NT_VARREF || $1->val.vref->type == VT_UNRESOLVED)
	       && ($1->type != NT_VLIST))
	      parse_error("statement has no effect (%s)", $1->type->name);
	   $$ = new Statement(S_EXPRESSION, $1);
	}
        | try_statement
        { $$ = $1; }
	| TOK_RETHROW ';'
	{
	   $$ = new Statement(S_RETHROW);
	}
        | TOK_THROW exp ';'
        {
	   $$ = new Statement(S_THROW);
	   $$->s.Throw = new ThrowStatement($2);
	}
        | TOK_SUB_CONTEXT context_mods statement_or_block
        {
	   $$ = new Statement(S_SUBCONTEXT);
	   $$->s.SContext = new ContextStatement(NULL, NULL, $2, $3);
	}
        | TOK_SUMMARIZE optname '(' exp ')' TOK_BY '(' exp ')' context_mods statement_or_block
        {
	   $$ = new Statement(S_SUMMARY);
	   
	   ContextModList *cml;

	   if ($10)
	   {
	      $10->addContextMod(new ContextMod(CM_SUMMARIZE_BY, $8));
	      cml = $10;
	   }
	   else
	      cml = new ContextModList(new ContextMod(CM_SUMMARIZE_BY, $8));

	   $$->s.SContext = new ContextStatement($2, $4, cml, $11);
	}
        | TOK_CONTEXT optname '(' exp ')' context_mods statement_or_block
        {
	   $$ = new Statement(S_CONTEXT);
	   
	   $$->s.SContext = new ContextStatement($2, $4, $6, $7);
        }
	| TOK_IF '(' exp ')' statement_or_block %prec IFX
        {	
	   $$ = new Statement(S_IF);
	   $$->s.If = new IfStatement($3, $5, NULL);
	}
        | TOK_IF '(' exp ')' statement_or_block TOK_ELSE statement_or_block
        {
	   $$ = new Statement(S_IF);
	   $$->s.If = new IfStatement($3, $5, $7);
	}
	| TOK_WHILE '(' exp ')' statement_or_block
        {
	   $$ = new Statement(S_WHILE);
	   $$->s.While = new WhileStatement($3, $5);
	}
	| TOK_DO statement_or_block TOK_WHILE '(' exp ')' ';'
        {
	   $$ = new Statement(S_DO_WHILE);
	   $$->s.While = new WhileStatement($5, $2);
	}
	| TOK_FOR '(' myexp ';' myexp ';' myexp ')' statement_or_block
        {
	   $$ = new Statement(S_FOR);
	   $$->s.For = new ForStatement($3, $5, $7, $9);
	}
        | TOK_FOREACH exp TOK_IN '(' exp ')' statement_or_block
        {
	   $$ = new Statement(S_FOREACH);
	   $$->s.ForEach = new ForEachStatement($2, $5, $7);
	   if ($2->type != NT_VARREF && $2->type != NT_SELF_VARREF)
	      parse_error("foreach variable expression is not a variable reference");
	}
        | return_statement ';' { $$ = $1; }
        | TOK_THREAD_EXIT ';'  
	{ 
	   // see if thread exit is allowed
	   if (checkParseOption(PO_NO_THREAD_CONTROL))
	      parse_error("illegal use of \"thread_exit\" (conflicts with parse option NO_THREAD_CONTROL)");

	   $$ = new Statement(S_THREAD_EXIT); 
	}
        | TOK_BREAK ';'
        {
	  $$ = new Statement(S_BREAK);
	}
        | TOK_CONTINUE ';'
        {
	  $$ = new Statement(S_CONTINUE);
	}
        | TOK_DELETE exp ';'
        {
	   $$ = new Statement(S_DELETE);
	   $$->s.Delete = new DeleteStatement($2);
	   if (check_vars($2))
	      parse_error("delete statement takes only variable references as arguments");
	}
        | switch_statement { $$ = $1; }
        | error ';'        { $$ = NULL; }
	;

context_mods:
	// empty 
        { $$ = NULL; }
        | 
	context_mods context_mod
        { 
	   if (!$1) 
	      $$ = new ContextModList($2); 
	   else 
	   { 
	      $1->addContextMod($2); 
	      $$ = $1; 
	   }
	}
	;

context_mod:
	TOK_WHERE '(' exp ')'
        { $$ = new ContextMod(CM_WHERE_NODE, $3); }
        | TOK_SORT_BY '(' exp ')'
        { $$ = new ContextMod(CM_SORT_ASCENDING, $3); }
        | TOK_SORT_DESCENDING_BY '(' exp ')'
        { $$ = new ContextMod(CM_SORT_DESCENDING, $3); }
	;

return_statement:
	TOK_RETURN     { $$ = new Statement(S_RETURN); $$->s.node = NULL; }
	|
	TOK_RETURN exp { $$ = new Statement(S_RETURN, $2); }
	;

switch_statement:
        TOK_SWITCH '(' exp ')' '{' case_block '}'
        {
	   $6->setSwitch($3);
	   $$ = new Statement(S_SWITCH);
	   $$->s.Switch = $6;
        }
        ;

case_block:
        case_code
        {
	   $$ = new SwitchStatement($1);
	}
        | case_block case_code
        {
	   $1->addCase($2);
	   $$ = $1;
        }
        ;

case_code:
        TOK_CASE LOGICAL_GE exp ':' statements
        {
          if (needsEval($3)) parse_error("case expression with '>=' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, $5, CaseNodeWithOperator::GreaterOrEqual);
        }
        | TOK_CASE LOGICAL_LE exp ':' statements
        {
         if (needsEval($3)) parse_error("case expression with '<=' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, $5, CaseNodeWithOperator::LessOrEqual);
        }
        | TOK_CASE '<' exp ':' statements
        {
          if (needsEval($3)) parse_error("case expression with '>' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, $5, CaseNodeWithOperator::Less);
        }
        | TOK_CASE '>' exp ':' statements
        {
          if (needsEval($3)) parse_error("case expression with '<' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, $5, CaseNodeWithOperator::Greater);
        }
        | TOK_CASE exp ':' statements
        {
	   if (needsEval($2))
	      parse_error("case expression needs run-time evaluation");
	   $$ = new CaseNode($2, $4);
	}
        | TOK_CASE LOGICAL_GE exp ':' // nothing
        {
          if (needsEval($3)) parse_error("case expression with '>=' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, NULL, CaseNodeWithOperator::GreaterOrEqual);
        }
        | TOK_CASE LOGICAL_LE exp ':' // nothing
        {
         if (needsEval($3)) parse_error("case expression with '<=' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, NULL, CaseNodeWithOperator::LessOrEqual);
        }
        | TOK_CASE '<' exp ':' // nothing
        {
          if (needsEval($3)) parse_error("case expression with '>' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, NULL, CaseNodeWithOperator::Less);
        }
        | TOK_CASE '>' exp ':' // nothing
        {
          if (needsEval($3)) parse_error("case expression with '<' needs run-time evaluation");
          $$ = new CaseNodeWithOperator($3, NULL, CaseNodeWithOperator::Greater);
        }
        | TOK_CASE exp ':' // nothing
        {
	   if (needsEval($2))
	      parse_error("case expression needs run-time evaluation");
	   $$ = new CaseNode($2, NULL);
	}
        | TOK_DEFAULT ':' statements
        {
	   $$ = new CaseNode(NULL, $3);
	}
        | TOK_DEFAULT ':' // nothing
        {
	   $$ = new CaseNode(NULL, NULL);
	}
        ;

try_statement:
        TOK_TRY statement_or_block TOK_CATCH '(' myexp ')' statement_or_block
        {
	   $$ = new Statement(S_TRY);
	   char *param = NULL;
	   if ($5)
	   {
	      if ($5->type == NT_VARREF)
	      {
		 param = $5->val.vref->name;
		 $5->val.vref->name = NULL;
		 $5->deref(NULL);
	      }
	      else
		 parse_error("only one parameter accepted in catch block for exception hash");
	   }
	   $$->s.Try = new TryStatement($2, $7, param);
	}
        ;

myexp:     /* empty */          { $$ = NULL; }
        | exp                   { $$ = $1; }
        ;

//finally:   /* empty */          { $$ = NULL; }
//        | TOK_FINALLY statement { $$ = $2; }
//        ;

object_def:
	TOK_CLASS IDENTIFIER inheritance_list '{' class_attributes '}'
        {
	   $$ = new ObjClassDef($2, $5); 
	   $5->setName(strdup($2));
	   $5->parseSetBaseClassList($3);
	}
        | TOK_CLASS SCOPED_REF inheritance_list '{' class_attributes '}'
        { 
	   $$ = new ObjClassDef($2, $5); 
	   $5->setName(strdup($$->name->getIdentifier())); 
	   $5->parseSetBaseClassList($3);
	}
	| TOK_CLASS IDENTIFIER inheritance_list ';'
        { 
	   class QoreClass *qc = new QoreClass(strdup($2));
	   qc->parseSetBaseClassList($3);
	   $$ = new ObjClassDef($2, qc); 	   
	}
	| TOK_CLASS SCOPED_REF inheritance_list ';'
        { 
	   class QoreClass *qc = new QoreClass();
	   $$ = new ObjClassDef($2, qc);
	   qc->setName(strdup($$->name->getIdentifier()));
	   qc->parseSetBaseClassList($3);
	}
	| TOK_CLASS IDENTIFIER inheritance_list '{' '}'
        { 
	   class QoreClass *qc = new QoreClass(strdup($2));
	   qc->parseSetBaseClassList($3);
	   $$ = new ObjClassDef($2, qc); 	   
	}
	| TOK_CLASS SCOPED_REF inheritance_list '{' '}'
        { 
	   class QoreClass *qc = new QoreClass();
	   $$ = new ObjClassDef($2, qc);
	   qc->setName(strdup($$->name->getIdentifier()));
	   qc->parseSetBaseClassList($3);
	}
	;

inheritance_list:
        TOK_INHERITS superclass_list
        {
	   $$ = $2;
        }
        | // NOTHING
        { $$ = NULL; }
        ;

superclass_list:
        superclass
        {
	   $$ = new BCList($1);
	}
        | superclass_list ',' superclass
        {
	   $1->push_back($3);
	   $$ = $1;
        }
        ;

superclass:
        IDENTIFIER
        {
	   $$ = new BCNode($1, false);
	}
        | SCOPED_REF
        {
	   $$ = new BCNode(new NamedScope($1), false);
	}
        | TOK_PRIVATE IDENTIFIER
        {
	   $$ = new BCNode($2, true);
	}
        | TOK_PRIVATE SCOPED_REF
        {
	   $$ = new BCNode(new NamedScope($2), true);
	}
	;

class_attributes:
	method_definition
        { 
           $$ = new QoreClass();
	   $1->addAndDelete($$);
	}
        | private_member_list
        {
	   $$ = new QoreClass();
	   $1->mergePrivateMembers($$);
	   delete $1;
	}
	| class_attributes method_definition
        { 
	   $2->addAndDelete($1);
	   $$ = $1; 
	}
	| class_attributes private_member_list
        { 
	   $2->mergePrivateMembers($1);
	   $$ = $1; 
	}
        ;

private_member_list:
	TOK_PRIVATE member_list ';'
        {
	   $$ = $2;
	}
        ;

member_list:
	SELF_REF
        {
	   $$ = new MemberList();
	   $$->add($1);
        }
        | member_list ',' SELF_REF
        {
	   if ($1->add($3))
	      free($3);
	   $$ = $1;
	}
	;

method_definition:
	method_modifiers IDENTIFIER '(' myexp ')' base_constructor_list block
	{
	   if ($6 && strcmp($2, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($2, new Paramlist($4), $7, $1 & OFM_SYNCED), $1 & OFM_PRIVATE, $6);
	}
	| IDENTIFIER '(' myexp ')' base_constructor_list block
        {
	   if ($5 && strcmp($1, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($1, new Paramlist($3), $6), 0, $5);
	}
	| method_modifiers KW_IDENTIFIER_OPENPAREN myexp ')' base_constructor_list block
	{
	   if ($5 && strcmp($2, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($2, new Paramlist($3), $6, $1 & OFM_SYNCED), $1 & OFM_PRIVATE, $5);
	}
	| KW_IDENTIFIER_OPENPAREN myexp ')' base_constructor_list block
        {
	   if ($4 && strcmp($1, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($1, new Paramlist($2), $5), 0, $4);
	}
	;

object_outofline_function_def:
	method_modifiers SCOPED_REF '(' myexp ')' base_constructor_list block
	{
	   tryAddMethod($1, $2, $4, $6, $7);
	}
	| SCOPED_REF '(' myexp ')' base_constructor_list block
        {
	   tryAddMethod(0, $1, $3, $5, $6);
	}
	;

base_constructor_list:
        ':' base_constructors
        {
	   $$ = $2;
	}
	| // nothing
        {
	   $$ = NULL;
	}
	;

base_constructors:
        base_constructor
	{
	   $$ = new BCAList($1);
	}
	| base_constructors ',' base_constructor
	{
	   $1->push_back($3);
	   $$ = $1;
	}
	;

base_constructor:
        IDENTIFIER '(' myexp ')'
        {
	   $$ = new BCANode($1, makeArgs($3));
	}
	| SCOPED_REF '(' myexp ')'
        {
	   $$ = new BCANode(new NamedScope($1), makeArgs($3));
	}
	;

method_modifiers:
        method_modifier { $$ = $1; }
        | method_modifiers method_modifier 
        {
	   if (($1 | $2) == $1)
	      parse_error("double object method modifier");
	   $$ = $1 | $2; 
	}
        ;

method_modifier:
	TOK_PRIVATE { $$ = OFM_PRIVATE; }
        | TOK_SYNCHRONIZED { $$ = OFM_SYNCED; }
        ;

sub_def:
        TOK_SUB IDENTIFIER '(' myexp ')' block
        { 
	   getProgram()->registerUserFunction(new UserFunction($2, new Paramlist($4), $6)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", $2);
	}
	| TOK_SYNCHRONIZED TOK_SUB IDENTIFIER '(' myexp ')' block
        {
	   getProgram()->registerUserFunction(new UserFunction($3, new Paramlist($5), $7, 1)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", $3);
	}
	| TOK_SUB KW_IDENTIFIER_OPENPAREN myexp ')' block
        { 
	   getProgram()->registerUserFunction(new UserFunction($2, new Paramlist($3), $5)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", $2);
	}
	| TOK_SYNCHRONIZED TOK_SUB KW_IDENTIFIER_OPENPAREN myexp ')' block
        {
	   getProgram()->registerUserFunction(new UserFunction($3, new Paramlist($4), $6, 1)); 
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_SUBROUTINE_DEFS))
	      parse_error("subroutine \"%s\" defined (conflicts with parse option NO_SUBROUTINE_DEFS)", $3);
	}
	;

function_call:
	IDENTIFIER '(' myexp ')'
        {
	   printd(5, "parsing call %s()\n", $1);
	   $$ = new QoreNode($1, makeArgs($3));
        }
        | KW_IDENTIFIER_OPENPAREN myexp ')'
        {
	   printd(5, "parsing call %s()\n", $1);
	   $$ = new QoreNode($1, makeArgs($2));
        }
        ;

scoped_object_call:
	SCOPED_REF '(' myexp ')'
        {
	   printd(5, "parsing scoped class call (for new) %s()\n", $1);
	   $$ = new QoreNode(new NamedScope($1), makeArgs($3));
        }
        ;

self_function_call:
	SELF_REF '(' myexp ')'
        {
	   printd(5, "parsing in-object method call %s()\n", $1);
	   $$ = new QoreNode(makeArgs($3), $1);
        }
        | BASE_CLASS_CALL '(' myexp ')'
        {
	   printd(5, "parsing in-object base class method call %s()\n", $1->ostr);
	   if (!strcmp($1->getIdentifier(), "copy"))
	      parse_error("illegal call to base class copy method '%s'", $1->ostr);

	   $$ = new QoreNode(makeArgs($3), $1);
	}
        ;

list:
	exp ',' exp
        { $$ = splice_expressions($1, $3); }
        | exp ','
        {
	   if ($1->type != NT_LIST) {
	      parse_error("problem in parsing ',' in list: left is no list!");
	   }
	   $$ = $1;
        }
        ;

hash:
	hash_element
	{
	   class Hash *h = new Hash(1);
	   h->setKeyValue($1->key, $1->value, NULL);
	   delete $1;
	   $$ = new QoreNode(h);
	}
	| hash ',' hash_element
	{
	   $1->val.hash->setKeyValue($3->key, $3->value, NULL);
	   delete $3;
	   $$ = $1;
	}
        | hash ','
        { /* empty ',' on end of hash */ $$=$1; }
	;

hash_element:
	scalar ':' exp
	{ $$ = new HashElement($1, $3); }
        | IDENTIFIER ':' exp     // for constants
        { $$ = new HashElement(HE_TAG_CONST, $1, $3); }
        | SCOPED_REF ':' exp     // for scoped constants
        { $$ = new HashElement(HE_TAG_SCOPED_CONST, $1, $3); }
	;

exp:    scalar
        { $$ = $1; }
        | BINARY
        { $$ = new QoreNode($1); }
        | list
        { $$ = $1; }
	| '(' hash ')'
	{ $$ = $2; }
        | SCOPED_REF
        { 
	   $$ = new QoreNode(new NamedScope($1)); 
	}
        | VAR_REF
        { $$ = new QoreNode(NT_VARREF); $$->val.vref = new VarRef($1, VT_UNRESOLVED); }
        | TOK_MY VAR_REF
        {
	   $$ = new QoreNode(new VarRef($2, VT_LOCAL)); 
	}
        | TOK_MY '(' list ')' 
        {
	   $3->type = NT_VLIST;
	   for (int i = 0; i < $3->val.list->size(); i++)
	   {
	      class QoreNode *n = $3->val.list->retrieve_entry(i);
	      if (n->type != NT_VARREF)
		 parse_error("element %d in list following 'my' is not a variable reference (%s)", i, n->type->name);
	      else
		 n->val.vref->type = VT_LOCAL;
	   }
	   $$ = $3;
	}
        | TOK_OUR VAR_REF
        {
	   getProgram()->addGlobalVarDef($2);
	   $$ = new QoreNode(new VarRef($2, VT_GLOBAL)); 
	}
        | TOK_OUR '(' list ')'
        { 
	   $3->type = NT_VLIST;
	   for (int i = 0; i < $3->val.list->size(); i++)
	   {
	      class QoreNode *n = $3->val.list->retrieve_entry(i);
	      if (n->type != NT_VARREF)
		 parse_error("element %d in list following 'our' is not a variable reference (%s)", i, n->type->name);
	      else
	      {
		 n->val.vref->type = VT_GLOBAL;
		 getProgram()->addGlobalVarDef(n->val.vref->name);
	      }
	   }
	   $$ = $3;
	}
	| IDENTIFIER
        { $$ = new QoreNode(NT_BAREWORD); $$->val.c_str = $1; }
	| CONTEXT_REF
        { 
	   $$ = new QoreNode(NT_CONTEXTREF); 
	   $$->val.c_str = $1;
	   //printd(5, "context ref, %s, %08p, %08p, create\n", $1, $$, $1);
	}
        | TOK_CONTEXT_ROW
        { $$ = new QoreNode(NT_CONTEXT_ROW); }
        | COMPLEX_CONTEXT_REF
        { $$ = new QoreNode(NT_COMPLEXCONTEXTREF); $$->val.complex_cref = new ComplexContextRef($1); } 
        | TOK_FIND exp TOK_IN exp TOK_WHERE '(' exp ')'
        {
	   $$ = new QoreNode(NT_FIND);
	   $$->val.find = new Find($2, $4, $7);
	}
	| exp PLUS_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of plus-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_PLUS_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_PLUS_EQUALS, $1, $3);
	}
        | exp MINUS_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of minus-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_MINUS_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_MINUS_EQUALS, $1, $3);

	}
        | exp AND_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of and-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_AND_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_AND_EQUALS, $1, $3);
	}
        | exp OR_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of or-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_OR_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_OR_EQUALS, $1, $3);
	}
        | exp MODULA_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of modula-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_MODULA_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_MODULA_EQUALS, $1, $3);
	}
        | exp MULTIPLY_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of multiply-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_MULTIPLY_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_MULTIPLY_EQUALS, $1, $3);
	}
        | exp DIVIDE_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of divide-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_DIVIDE_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_DIVIDE_EQUALS, $1, $3);
	}
        | exp XOR_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of xor-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_XOR_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_XOR_EQUALS, $1, $3);
	}
        | exp SHIFT_LEFT_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of shift-left-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_SHIFT_LEFT_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_SHIFT_LEFT_EQUALS, $1, $3);
	}
        | exp SHIFT_RIGHT_EQUALS exp
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of shift-right-equals operator is not an lvalue");
	      $$ = makeErrorTree(OP_SHIFT_RIGHT_EQUALS, $1, $3);
	   }
	   else
	      $$ = makeTree(OP_SHIFT_RIGHT_EQUALS, $1, $3);
	}
	| exp '=' exp
	{
	   if ($1->type == NT_FLIST)
	      $1->type = NT_LIST;
	   if ($1->type == NT_LIST)
	   {
	      bool ok = true;
	      for (int i = 0; i < $1->val.list->size(); i++)
	      {
		 QoreNode *n = $1->val.list->retrieve_entry(i);
		 if (check_lvalue(n))
		 {
		    parse_error("element %d in list assignment is not an lvalue (%s)", i, n->type->name);
		    ok = false;
		 }
	      }
	      if (ok)
		 $$ = makeTree(OP_LIST_ASSIGNMENT, $1, $3);
	      else
		 $$ = makeErrorTree(OP_LIST_ASSIGNMENT, $1, $3);
	   }
	   else
	   {
	      if (check_lvalue($1))
	      {
		 parse_error("left-hand side of assignment is not an lvalue (%s)", $1->type->name);
		 $$ = makeErrorTree(OP_ASSIGNMENT, $1, $3);
	      }
	      else
		 $$ = makeTree(OP_ASSIGNMENT, $1, $3);
	   }
	   //print_tree($1, 0);
	}
        | TOK_EXISTS exp
        { $$ = makeTree(OP_EXISTS, $2, NULL); }
        | TOK_ELEMENTS exp
        { $$ = makeTree(OP_ELEMENTS, $2, NULL); }
        | exp TOK_INSTANCEOF IDENTIFIER
        {
	   $$ = makeTree(OP_INSTANCEOF, $1, new QoreNode(new ClassRef(new NamedScope($3))));
	}
        | exp TOK_INSTANCEOF SCOPED_REF
        {
	   $$ = makeTree(OP_INSTANCEOF, $1, new QoreNode(new ClassRef(new NamedScope($3))));
	}
        | TOK_KEYS exp
        { $$ = makeTree(OP_KEYS, $2, NULL); }
        | TOK_UNSHIFT exp  // unshift list, element
        {
	   if ($2->type != NT_LIST || $2->val.list->size() != 2)
	   {
	      parse_error("invalid arguments to unshift, expected: lvalue, expression (%s)", $2->type->name);
	      $$ = makeErrorTree(OP_UNSHIFT, $2, NULL);
	   }
	   else
	   {
	      QoreNode *lv = $2->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to unshift is not an lvalue");
		 $$ = makeErrorTree(OP_UNSHIFT, lv, $2->val.list->shift());
	      }
	      else
		 $$ = makeTree(OP_UNSHIFT, lv, $2->val.list->shift());
	      $2->deref(NULL);
	   }
	}
	| TOK_SHIFT exp
	{ 
	   if (check_lvalue($2))
	   {
	      parse_error("argument to shift operator is not an lvalue");
	      $$ = makeErrorTree(OP_SHIFT, $2, NULL); 
	   }
	   else
	      $$ = makeTree(OP_SHIFT, $2, NULL); 
	}
        | TOK_PUSH exp  // push lvalue-list, element
        {
	   if ($2->type != NT_LIST || $2->val.list->size() != 2)
	   {
	      parse_error("invalid arguments to push, expected: lvalue, expression (%s)", $2->type->name);
	      $$ = makeErrorTree(OP_PUSH, $2, NULL);
	   }
	   else
	   {
	      
	      QoreNode *lv = $2->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to push is not an lvalue");
		 $$ = makeErrorTree(OP_PUSH, lv, $2->val.list->shift());
	      }
	      else
		 $$ = makeTree(OP_PUSH, lv, $2->val.list->shift());
	      $2->deref(NULL);
	   }
	}
	| TOK_POP exp
	{
	   if (check_lvalue($2))
	   {
	      parse_error("argument to pop operator is not an lvalue");
	      $$ = makeErrorTree(OP_POP, $2, NULL); 
	   }
	   else
	      $$ = makeTree(OP_POP, $2, NULL); 
	}
	| TOK_CHOMP exp
	{
	   if (check_lvalue($2))
	   {
	      parse_error("argument to chomp operator is not an lvalue");
	      $$ = makeErrorTree(OP_CHOMP, $2, NULL); 
	   }
	   else
	      $$ = makeTree(OP_CHOMP, $2, NULL); 
	}
        | TOK_SPLICE exp  // splice lvalue-list, offset, [length, list]
        {
	   if ($2->type != NT_LIST || $2->val.list->size() < 2 || $2->val.list->size() > 4)
	   {
	      parse_error("invalid arguments to splice, expected: lvalue, offset exp [length exp, [list exp]] (%s)", $2->type->name);
	      $$ = makeErrorTree(OP_SPLICE, $2, NULL);
	   }
	   else
	   {
	      QoreNode *lv = $2->val.list->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to splice is not an lvalue");
		 $$ = makeErrorTree(OP_SPLICE, lv, $2);
	      }
	      else
		 $$ = makeTree(OP_SPLICE, lv, $2);
	   }
	}
        | exp '?' exp ':' exp
        { $$ = new QoreNode($1, OP_QUESTION_MARK, splice_expressions($3, $5)); } 
        | P_INCREMENT exp   // pre-increment
        {
	   if (check_lvalue($2))
	   {
	      parse_error("pre-increment expression is not an lvalue");
	      $$ = makeErrorTree(OP_PRE_INCREMENT, $2, NULL);
	   }
	   else
	      $$ = makeTree(OP_PRE_INCREMENT, $2, NULL);
        }
        | exp P_INCREMENT   // post-increment
        {
	   if (check_lvalue($1))
	   {
	      parse_error("post-increment expression is not an lvalue");
	      $$ = makeErrorTree(OP_POST_INCREMENT, $1, NULL);
	   }
	   else
	      $$ = makeTree(OP_POST_INCREMENT, $1, NULL);
        }
        | P_DECREMENT exp   // pre-decrement
        {
	   if (check_lvalue($2))
	   {
	      parse_error("pre-decrement expression is not an lvalue");
	      $$ = makeErrorTree(OP_PRE_DECREMENT, $2, NULL);
	   }
	   else
	      $$ = makeTree(OP_PRE_DECREMENT, $2, NULL);
        }
        | exp P_DECREMENT   // post-decrement
        {
	   if (check_lvalue($1))
	   {
	      parse_error("post-decrement expression is not an lvalue");
	      $$ = makeErrorTree(OP_POST_DECREMENT, $1, NULL);
	   }
	   else
	      $$ = makeTree(OP_POST_DECREMENT, $1, NULL);
        }
        | function_call		     { $$ = $1; }
        | self_function_call         { $$ = $1; }
        | SELF_REF                   { $$ = new QoreNode(NT_SELF_VARREF); $$->val.c_str = $1; }
	| exp LOGICAL_AND exp	     { $$ = makeTree(OP_LOG_AND, $1, $3); }
	| exp LOGICAL_OR exp	     { $$ = makeTree(OP_LOG_OR, $1, $3); }
	| exp '|' exp		     { $$ = makeTree(OP_BIN_OR, $1, $3); }
	| exp '&' exp		     { $$ = makeTree(OP_BIN_AND, $1, $3); }
	| exp '^' exp		     { $$ = makeTree(OP_BIN_XOR, $1, $3); }
        | exp REGEX_MATCH REGEX
        {
	   $$ = makeTree(OP_REGEX_MATCH, $1, new QoreNode($3));
	}
        | exp REGEX_NMATCH REGEX
        {
	   $$ = makeTree(OP_REGEX_NMATCH, $1, new QoreNode($3));
	}
        | exp REGEX_MATCH exp
        {
	   $$ = makeTree(OP_REGEX_MATCH, $1, new QoreNode(new QoreRegex($3->val.String)));
	   $3->type = NT_INT;
	   $3->deref(NULL);
	}
        | exp REGEX_NMATCH exp
        { 
	   $$ = makeTree(OP_REGEX_NMATCH, $1, new QoreNode(new QoreRegex($3->val.String))); 
	   $3->type = NT_INT;
	   $3->deref(NULL);
	}
        | exp REGEX_MATCH REGEX_SUBST
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of regular expression substitution operator is not an lvalue");
	      $$ = makeErrorTree(OP_REGEX_SUBST, $1, new QoreNode($3));
	   }
	   else
	   {
	      //printf("REGEX_SUBST: '%s'\n", $3->getPattern()->getBuffer());
	      $$ = makeTree(OP_REGEX_SUBST, $1, new QoreNode($3));
	   }
	}
        | exp REGEX_MATCH REGEX_TRANS
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of transliteration operator is not an lvalue");
	      $$ = makeErrorTree(OP_REGEX_TRANS, $1, new QoreNode($3));
	   }
	   else
	   {
	      //printf("REGEX_SUBST: '%s'\n", $3->getPattern()->getBuffer());
	      $$ = makeTree(OP_REGEX_TRANS, $1, new QoreNode($3));
	   }
	}
        | exp REGEX_MATCH REGEX_EXTRACT
        {
	   //printd(5, "REGEX_EXTRACT: '%s'\n", (new QoreNode($3))->type->name);
	   $$ = makeTree(OP_REGEX_EXTRACT, $1, new QoreNode($3));
	}
	| exp '>' exp		     { $$ = makeTree(OP_LOG_GT, $1, $3); }
	| exp '<' exp		     { $$ = makeTree(OP_LOG_LT, $1, $3); }
	| exp LOGICAL_CMP exp	     { $$ = makeTree(OP_LOG_CMP, $1, $3); }
	| exp LOGICAL_EQ exp	     { $$ = makeTree(OP_LOG_EQ, $1, $3); }
        | exp ABSOLUTE_EQ exp        { $$ = makeTree(OP_ABSOLUTE_EQ, $1, $3); }
        | exp ABSOLUTE_NE exp        { $$ = makeTree(OP_ABSOLUTE_NE, $1, $3); }
	| exp LOGICAL_NE exp	     { $$ = makeTree(OP_LOG_NE, $1, $3); }
	| exp LOGICAL_LE exp	     { $$ = makeTree(OP_LOG_LE, $1, $3); }
	| exp LOGICAL_GE exp	     { $$ = makeTree(OP_LOG_GE, $1, $3); }
        | exp SHIFT_LEFT exp	     { $$ = makeTree(OP_SHIFT_LEFT, $1, $3); }
        | exp SHIFT_RIGHT exp	     { $$ = makeTree(OP_SHIFT_RIGHT, $1, $3); }
        | exp '+' exp		     { $$ = makeTree(OP_PLUS, $1, $3); }
	| exp '-' exp		     { $$ = makeTree(OP_MINUS, $1, $3); }
        | exp '%' exp		     { $$ = makeTree(OP_MODULA, $1, $3); }
	| exp '/' exp		     { $$ = makeTree(OP_DIV, $1, $3); }
	| exp '*' exp		     { $$ = makeTree(OP_MULT, $1, $3); }
	| '-' exp %prec NEG	     { $$ = makeTree(OP_UNARY_MINUS, $2, NULL); }
        | '~' exp		     { $$ = makeTree(OP_BIN_NOT, $2, NULL); }
        | '!' exp                    { $$ = makeTree(OP_NOT, $2, NULL); }
        | '\\' exp                   
        { 
	   $$ = new QoreNode(NT_REFERENCE);
	   $$->val.lvexp = $2;
	   if (check_lvalue($2))
	      parse_error("argument to reference operator is not an lvalue");
	}
	| TOK_NEW function_call      
        {
	   $$ = makeTree(OP_NEW, $2->val.fcall->parseMakeNewObject(), NULL);
	   $2->deref(NULL);
	   // see if new can be used
	   if (checkParseOption(PO_NO_NEW))
	      parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	}
        | TOK_NEW scoped_object_call 
        { 
	   $$ = makeTree(OP_NEW, $2, NULL); 
	   // see if new can be used
	   if (checkParseOption(PO_NO_NEW))
	      parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	}
	| TOK_BACKGROUND exp	     
	{
	   $$ = makeTree(OP_BACKGROUND, $2, NULL);
	   // check to see if the expression is legal
	   if (checkParseOption(PO_NO_THREAD_CONTROL))
	      parse_error("illegal use of \"background\" operator (conflicts with parse option NO_THREAD_CONTROL)");
	   else if (!hasEffect($2))
	      parse_error("argument to background operator has no effect");
	}
        | BACKQUOTE
        {
	   $$ = new QoreNode(NT_BACKQUOTE);
	   $$->val.c_str = $1;
	   if (checkParseOption(PO_NO_EXTERNAL_PROCESS))
	      parse_error("illegal use of backquote operator (conflicts with parse option NO_EXTERNAL_PROCESS)");
	}
        | exp '[' exp ']'            { $$ = makeTree(OP_LIST_REF, $1, $3); }
	| exp '{' exp '}'            { $$ = makeTree(OP_OBJECT_REF, $1, $3); }
	| exp '.' exp
	{
	   $$ = process_dot($1, $3);
	}
	| '(' exp ')'                
        { 
	   $$ = $2; 
	   if ($2->type == NT_LIST) 
	      $2->type = NT_FLIST; 
	}
        | '(' ')' { $$ = new QoreNode(NT_FLIST); $$->val.list = new List(); }
	;

scalar:
	QFLOAT         { $$ = new QoreNode(NT_FLOAT); $$->val.floatval = $1; }
	| INTEGER     { $$ = new QoreNode(NT_INT); $$->val.intval = $1; }
        | QUOTED_WORD { $$ = new QoreNode($1); }
        | DATETIME    { $$ = new QoreNode($1); }
        | TOK_NULL    { $$ = new QoreNode(NT_NULL); }
        | TOK_NOTHING { $$ = new QoreNode(NT_NOTHING); }
	;

%%
