%{
/*
   parser.yy

   Qore Programming Language

   Copyright 2003 - 2009 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/BreakStatement.h>
#include <qore/intern/ContinueStatement.h>
#include <qore/intern/ReturnStatement.h>
#include <qore/intern/RethrowStatement.h>
#include <qore/intern/ThreadExitStatement.h>
#include <qore/intern/ExpressionStatement.h>
#include <qore/intern/DoWhileStatement.h>
#include <qore/intern/SummarizeStatement.h>
#include <qore/intern/ContextStatement.h>
#include <qore/intern/IfStatement.h>
#include <qore/intern/WhileStatement.h>
#include <qore/intern/ForStatement.h>
#include <qore/intern/ForEachStatement.h>
#include <qore/intern/DeleteStatement.h>
#include <qore/intern/TryStatement.h>
#include <qore/intern/ThrowStatement.h>
#include <qore/intern/StatementBlock.h>
#include <qore/intern/ParserSupport.h>
#include <qore/intern/SwitchStatement.h>
#include <qore/intern/CaseNodeWithOperator.h>
#include <qore/intern/CaseNodeRegex.h>
#include <qore/intern/OnBlockExitStatement.h>

#include "parser.h"

#include <qore/intern/QoreClassIntern.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define YYINITDEPTH 300
//#define YYDEBUG 1

#define YYLLOC_DEFAULT(Current, Rhs, N)                                \
          do                                                                  \
            if (N)                                                            \
              {                                                               \
                (Current).first_line   = YYRHSLOC(Rhs, 1).first_line;         \
                (Current).last_line    = YYRHSLOC(Rhs, N).last_line;          \
              }                                                               \
            else                                                              \
              {                                                               \
                (Current).first_line   = (Current).last_line   =              \
                  YYRHSLOC(Rhs, 0).last_line;                                 \
              }                                                               \
          while (0)

class HashElement {
   public:
      char *key;
      AbstractQoreNode *value;
 
      DLLLOCAL HashElement(AbstractQoreNode *k, AbstractQoreNode *v);
      DLLLOCAL HashElement(int tag, char *constant, AbstractQoreNode *v);
      DLLLOCAL ~HashElement();
};

static QoreTreeNode *makeErrorTree(Operator *op, AbstractQoreNode *left, AbstractQoreNode *right) {
   return new QoreTreeNode(left, op, right);
}

static AbstractQoreNode *makeTree(Operator *op, AbstractQoreNode *left, AbstractQoreNode *right)
{
   //tracein("makeTree()");
   printd(5, "makeTree(): l=%08p (%s, value=%d), r=%08p (%s, value=%d), op=%s\n", 
	  left, left->getTypeName(), left->is_value(), 
	  right, right ? right->getTypeName() : "n/a", right ? right->is_value() : false, op->getName());

   // if both nodes are values, then evaluate immediately
   if (left->is_value() && (!right || right->is_value()))
   {
      ExceptionSink xsink;

      AbstractQoreNode *n_node = op->eval(left, right, true, &xsink);
      //printd(5, "makeTree(): l=%08p (%s), r=%08p (%s), op=%s, returning %08p\n", left, left->getTypeName(), right, right ? right->getTypeName() : "n/a", op->getName(), n_node);
      left->deref(0);
      if (right)
	 right->deref(0);

      if (xsink.isEvent())
	 getProgram()->addParseException(&xsink);

      //traceout("makeTree()");
      return n_node ? n_node : nothing();
   }
   // otherwise, put nodes and operator into tree for runtime evaluation
   return new QoreTreeNode(left, op, right);
}

static QoreListNode *makeArgs(AbstractQoreNode *arg)
{
   if (!arg)
      return 0;

   QoreListNode *l;
   if (arg->getType() == NT_LIST) {
      l = reinterpret_cast<QoreListNode *>(arg);
      if (!l->isFinalized())
	 return l;
   }

   l = new QoreListNode(arg->needs_eval());
   l->push(arg);
   return l;
}

HashElement::HashElement(AbstractQoreNode *k, AbstractQoreNode *v)
{
   //tracein("HashElement::HashElement()");
   if (!k || k->getType() != NT_STRING) {
      parse_error("object member name must be a string value!");
      key = strdup("");
   }
   else
      key = strdup(reinterpret_cast<QoreStringNode *>(k)->getBuffer());
   k->deref(0);
   value = v;
   //traceout("HashElement::HashElement()");
}

HashElement::HashElement(int tag, char *constant, AbstractQoreNode *v)
{
   //tracein("HashElement::HashElement()");
   key = (char *)malloc(sizeof(char) * strlen(constant) + 2);
   key[0] = tag; // mark as constant
   strcpy(key + 1, constant);
   value = v;
   free(constant);
   //traceout("HashElement::HashElement()");
}

HashElement::~HashElement()
{
   free(key);
}

// for constant definitions
class ConstNode
{
   public:
      class NamedScope *name;
      AbstractQoreNode *value;

      DLLLOCAL inline ConstNode(char *n, AbstractQoreNode *v) { name = new NamedScope(n); value = v; }
      DLLLOCAL inline ~ConstNode() { delete name; }
};

class ObjClassDef
{
   public:
      class NamedScope *name;
      class QoreClass *oc;

      DLLLOCAL inline ObjClassDef(char *n, class QoreClass *o) { name = new NamedScope(n); oc = o; }
      DLLLOCAL inline ~ObjClassDef() { delete name; }
};

#define NSN_OCD   1
#define NSN_CONST 2
#define NSN_NS    3

struct NSNode
{
      int type;
      union {
	    class ObjClassDef *ocd;
	    class ConstNode  *cn;
	    class QoreNamespace  *ns;
      } n;
      DLLLOCAL NSNode(class ObjClassDef *o) { type = NSN_OCD; n.ocd = o; }
      DLLLOCAL NSNode(class ConstNode  *c) { type = NSN_CONST; n.cn = c; }
      DLLLOCAL NSNode(class QoreNamespace  *s) { type = NSN_NS; n.ns = s; }
};

static void addNSNode(class QoreNamespace *ns, struct NSNode *n)
{
   switch (n->type)
   {
      case NSN_OCD:
	 ns->addClass(n->n.ocd->name, n->n.ocd->oc);
	 delete n->n.cn;
	 break;
      case NSN_CONST:
	 ns->addConstant(n->n.cn->name, n->n.cn->value);
	 delete n->n.cn;
	 break;
      case NSN_NS:
	 ns->addNamespace(n->n.ns);
	 break;
   }
   delete n;
}

static QoreListNode *make_list(AbstractQoreNode *a1, AbstractQoreNode *a2)
{
   QoreListNode *l = new QoreListNode(true);
   l->push(a1);
   l->push(a2);
   return l;
}

static QoreListNode *splice_expressions(AbstractQoreNode *a1, AbstractQoreNode *a2)
{
   //tracein("splice_expressions()");
   if (a1 && a1->getType() == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(a1);
      if (!l->isFinalized()) {
	 //printd(5, "LIST x\n");
	 l->push(a2);
	 return l;
      }
   }
   return make_list(a1, a2);
}

static int checkParseOption(int o)
{
   return getParseOptions() & o;
}

class MemberList : private strset_t
{
   public:
      DLLLOCAL ~MemberList()
      {
	 strset_t::iterator i;
	 while ((i = begin()) != end())
	 {
	    char *name = *i;
	    erase(i);
	    free(name);
	 }
      }

      DLLLOCAL int add(char *name)
      {
	 if (find(name) != end())
	    return -1;
	 // add new member to list
	 insert(name);
	 return 0;
      }

      DLLLOCAL void mergePrivateMembers(class QoreClass *qc)
      {
	 strset_t::iterator i;
	 while ((i = begin()) != end())
	 {
	    char *name = *i;
	    erase(i);
	    qc->addPrivateMember(name);
	 }
      }
};

static inline void addConstant(NamedScope *name, AbstractQoreNode *value)
{
   getRootNS()->rootAddConstant(name, value);
}

static inline void addClass(NamedScope *name, QoreClass *oc)
{
   getRootNS()->rootAddClass(name, oc);
}

static inline class QoreClass *parseFindClass(char *name)
{
   QoreClass *c = getRootNS()->rootFindClass(name);
   if (!c)
      parse_error("reference to undefined class '%s'", name);

   return c;
}

static AbstractQoreNode *process_dot(AbstractQoreNode *l, AbstractQoreNode *r)
{
   qore_type_t rtype = r->getType();
   if (rtype == NT_BAREWORD) {
      BarewordNode *b = reinterpret_cast<BarewordNode *>(r);
      AbstractQoreNode *rv = makeTree(OP_OBJECT_REF, l, b->makeQoreStringNode());
      b->deref();
      return rv;
   }

   if (rtype == NT_FUNCTION_CALL) {
      FunctionCallNode *f = reinterpret_cast<FunctionCallNode *>(r);
      if (f->getFunctionType() == FC_UNRESOLVED) {
	 MethodCallNode *m = new MethodCallNode(f->takeName(), f->take_args());
	 f->deref();
	 return makeTree(OP_OBJECT_FUNC_REF, l, m);
      }
   }

   return makeTree(OP_OBJECT_REF, l, r);
}

// returns 0 for OK, -1 for error
static int check_lvalue(AbstractQoreNode *node)
{
   qore_type_t ntype = node->getType();
   //printd(5, "type=%s\n", node->getTypeName());
   if (ntype == NT_VARREF)
      return 0;

   if (ntype == NT_TREE) {
      QoreTreeNode *t = reinterpret_cast<QoreTreeNode *>(node);
      if (t->op == OP_LIST_REF || t->op == OP_OBJECT_REF)
	 return check_lvalue(t->left);
      else
	 return -1;
   }
   if (ntype == NT_SELF_VARREF)
      return 0;
   return -1;
}

static inline int check_vars(AbstractQoreNode *n)
{
   if (n && n->getType() == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(n);
      for (unsigned i = 0; i < l->size(); i++)
	 if (l->retrieve_entry(i)->getType() != NT_VARREF)
	    return 1;
      return 0;
   }
   return check_lvalue(n);
}

// returns true if the node needs run-time evaluation, false if not
bool needsEval(AbstractQoreNode *n) {
   if (!n)
      return false;

   qore_type_t ntype = n->getType();

   // if it's a constant or a function reference
   if (ntype == NT_BAREWORD || ntype == NT_CONSTANT || ntype == NT_FUNCREF)
      return false;

   if (ntype == NT_LIST) {
      QoreListNode *l = reinterpret_cast<QoreListNode *>(n);
      for (unsigned i = 0; i <l->size(); i++) {
	 if (needsEval(l->retrieve_entry(i)))
	    return true;
      }
      // here we set needs_eval to false so the list won't be evaluated again
      l->clearNeedsEval();
      return false;
   }

   if (ntype == NT_HASH) {
      QoreHashNode *h = reinterpret_cast<QoreHashNode *>(n);
      HashIterator hi(h);
      while (hi.next())
	 if (needsEval(hi.getValue()))
	    return true;
      // here we set needs_eval to false so the hash won't be evaluated again
      h->clearNeedsEval();
      return false;
   }
   
   if (ntype == NT_TREE) {
      QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);

      if (needsEval(tree->left) || (tree->right && needsEval(tree->right)))
	 return true;
      return tree->op->hasEffect();
   }

   //printd(5, "needsEval() type %s = true\n", n->getTypeName());
   // we don't return needs_eval() here because some node types are not meant to be evaluated directly but are also not values
   return !n->is_value();
}

int check_case(const char *op, AbstractQoreNode *exp) {
   // ignore if NULL (= NOTHING)
   if (exp && needsEval(exp)) {
      if (op)
	 parse_error("case expression with '%s' needs run-time evaluation", op);
      else
	 parse_error("case expression needs run-time evaluation", op);
      return -1;
   }
   return 0;
}

static bool hasEffect(AbstractQoreNode *n)
{
   // check for expressions with no effect
   qore_type_t ntype = n->getType();
   if (ntype == NT_FUNCTION_CALL || ntype == NT_STATIC_METHOD_CALL 
       || ntype == NT_FIND || ntype == NT_FUNCREFCALL)
      return true;

   if (ntype == NT_TREE)
      return reinterpret_cast<QoreTreeNode *>(n)->op->hasEffect();

   //printd(5, "hasEffect() node %08p type=%s\n", n, n->getTypeName());
   return false;
}

#define OFM_PRIVATE 1
#define OFM_SYNCED  2
#define OFM_STATIC  4

static inline void tryAddMethod(int mod, char *n, AbstractQoreNode *params, BCAList *bcal, StatementBlock *b)
{
   class NamedScope *name = new NamedScope(n);
   if (bcal && strcmp(name->getIdentifier(), "constructor")) {
      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
      if (params)
	 params->deref(0);
      delete bcal;
      if (b)
	 delete b;
   }
   else {
      QoreMethod *method = new QoreMethod(new UserFunction(strdup(name->getIdentifier()), new Paramlist(params), b, mod & OFM_SYNCED), mod & OFM_PRIVATE, mod & OFM_STATIC);
      
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
      class QoreMethod *m;
      // base class argument list for constructors
      class BCAList *bcal;

      DLLLOCAL inline MethodNode(class UserFunction *f, bool n_priv, bool n_static, class BCAList *bl) : bcal(bl)
      {
	 m = new QoreMethod(f, n_priv, n_static);
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
	 m = 0;
	 if (bcal) {
	    qc->parseAddBaseClassArgumentList(bcal);
	    bcal = 0;
	 }
	 delete this;
      }
};

%}

%pure-parser
%lex-param {yyscan_t yyscanner}
%parse-param {yyscan_t yyscanner}
%locations
%error-verbose

%union
{
      int i4;
      int64 integer;
      double decimal;
      class QoreStringNode *String;
      char *string;
      class BinaryNode *binary;
      AbstractQoreNode *node;
      QoreHashNode *hash;
      QoreListNode *list;
      AbstractStatement *statement;
      class StatementBlock *sblock;
      class ContextModList *cmods;
      class ContextMod *cmod;
      class HashElement *hashelement;
      class UserFunction *userfunc;	
      class MethodNode *methodnode;
      class MemberList *privlist;
      class QoreClass *qoreclass;
      class ConstNode *constnode;
      class QoreNamespace *ns;
      class NSNode *nsn;
      class ObjClassDef *objdef;
      class DateTimeNode *datetime;
      class RegexSubstNode *RegexSubst;
      class RegexTransNode *RegexTrans;
      class SwitchStatement *switchstmt;
      class CaseNode *casenode;
      class BCList *sclist;
      class BCNode *sclnode;
      class BCAList *bcalist;
      class BCANode *bcanode;
      class NamedScope *nscope;
      class QoreRegexNode *Regex;
      class QoreImplicitArgumentNode *implicit_arg;
}

%{

//#define YYSTYPE char const *
//#define LEX_PARAMETERS YYSTYPE *lvalp, struct YYLTYPE *llocp
//#define ERROR_PARAMETERS struct YYLTYPE *llocp, char const *s
//#define LEX_PARAMETERS YYSTYPE *lvalp, yyscan_t scanner
//#define LEX_PARAMETERS void
//#define ERROR_PARAMETERS char const *s
//int yylex();

#define LEX_PARAMETERS YYSTYPE *lvalp, YYLTYPE *loc, yyscan_t scanner

DLLLOCAL int yylex(LEX_PARAMETERS);

DLLLOCAL void yyerror(YYLTYPE *loc, yyscan_t scanner, const char *str)
{
   //printd(5, "yyerror() location: %d-%d: \"%s\"\n", loc->first_line, loc->last_line, str);
   parse_error("%s", str);
}

%}

// define string aliases for token names for more user-friendly error reporting
%token TOK_CLASS "class"
%token TOK_RETURN "return"
%token TOK_MY "my"
%token TOK_DO "do"
%token TOK_TRY "try"
%token TOK_THROW "throw"
%token TOK_CATCH "catch"
%token TOK_FINALLY "finally"
%token TOK_WHERE "where"
%token TOK_NULL "NULL"
%token TOK_WHILE "while"
%token TOK_IF "if"
%token TOK_FOR "for"
%token TOK_SUB "sub"
%token TOK_THREAD_EXIT "thread_exit" 
%token TOK_BREAK "break"
%token TOK_CONTINUE "continue"
%token TOK_NOTHING "NOTHING"
%token TOK_CONTEXT_ROW "%%"
%token TOK_FIND "find"
%token TOK_FOREACH "foreach"
%token TOK_IN "in"
%token TOK_DELETE "delete"
%token TOK_PRIVATE "private"
%token TOK_SYNCHRONIZED "synchronized"
%token TOK_CONTEXT "context"
%token TOK_SORT_BY "sortBy"
%token TOK_SORT_DESCENDING_BY "sortDescendingBy"
%token TOK_SUB_CONTEXT "subcontext"
%token TOK_CONST "const"
%token TOK_SUMMARIZE "summarize"
%token TOK_BY "by"
%token TOK_NAMESPACE "namespace"
%token TOK_OUR "our"
%token TOK_RETHROW "rethrow"
%token TOK_SWITCH "switch"
%token TOK_CASE "case"
%token TOK_DEFAULT "default"
%token TOK_INHERITS "inherits"
%token TOK_ELSE "else"
%token TOK_STATIC "static"

// operator tokens
%token P_INCREMENT "++ operator"
%token P_DECREMENT "-- operator"
%token PLUS_EQUALS "+= operator"
%token MINUS_EQUALS "-= operator"
%token AND_EQUALS "&= operator"
%token OR_EQUALS "|= operator"
%token MODULA_EQUALS "%= operator"
%token MULTIPLY_EQUALS "*= operator"
%token DIVIDE_EQUALS "/= operator"
%token XOR_EQUALS "^= operator"
%token SHIFT_LEFT_EQUALS "<<= operator"
%token SHIFT_RIGHT_EQUALS ">>= operator"
%token TOK_UNSHIFT "unshift"
%token TOK_PUSH "push"
%token TOK_POP "pop"
%token TOK_SHIFT "shift"
%token TOK_CHOMP "chomp"
%token TOK_TRIM "trim"
%token LOGICAL_AND "&& operator"
%token LOGICAL_OR "|| operator"
%token LOGICAL_EQ "== operator"
%token LOGICAL_NE "!= operator"
%token LOGICAL_LE "<= operator"
%token LOGICAL_GE ">= operator"
%token LOGICAL_CMP "<=> operator"
%token ABSOLUTE_EQ "=== operator"
%token ABSOLUTE_NE "!== operator"
%token REGEX_MATCH "=~ operator"
%token REGEX_NMATCH "!~ operator"
%token TOK_EXISTS "exists"
%token TOK_INSTANCEOF "instanceof"
%token SHIFT_RIGHT ">> operator"
%token SHIFT_LEFT "<< operator"
%token TOK_ELEMENTS "elements"
%token TOK_KEYS "keys"
%token TOK_NEW "new"
%token TOK_BACKGROUND "background"
%token TOK_ON_EXIT "on_exit"
%token TOK_ON_SUCCESS "on_success"
%token TOK_ON_ERROR "on_error"
%token TOK_MAP "map"
%token TOK_FOLDR "foldr"
%token TOK_FOLDL "foldl"
%token TOK_SELECT "select"

 // tokens returning data
%token <integer> INTEGER "integer value"
%token <decimal> QFLOAT "floating-point value"
%token <string> IDENTIFIER "identifier"
%token <string> VAR_REF "variable reference"
%token <string> BACKQUOTE "backquote expression"
%token <string> SELF_REF "in-object member reference"
%token <string> KW_IDENTIFIER_OPENPAREN "keyword used as function or method identifier"
%token <string> SCOPED_REF "namespace or class-scoped reference"
%token <string> CONTEXT_REF "context reference"
%token <string> COMPLEX_CONTEXT_REF "named context reference"
%token <datetime> DATETIME "date/time value"
%token <String> QUOTED_WORD "quoted string"
%token <binary> BINARY "binary constant value"
%token <RegexSubst> REGEX_SUBST "regular expression substitution expression"
%token <RegexTrans> REGEX_TRANS "transliteration expression"
%token <nscope> BASE_CLASS_CALL "call to base class method"
%token <Regex> REGEX "regular expression"
%token <Regex> REGEX_EXTRACT "regular expression extraction expression"
%token <implicit_arg> IMPLICIT_ARG_REF "implicit argument reference"
%token <String> DOT_KW_IDENTIFIER "keyword used as hash key or object member reference"

%nonassoc IFX SCOPED_REF
%nonassoc TOK_ELSE

// FIXME: check precedence
%right PLUS_EQUALS MINUS_EQUALS AND_EQUALS OR_EQUALS MODULA_EQUALS MULTIPLY_EQUALS DIVIDE_EQUALS XOR_EQUALS SHIFT_LEFT_EQUALS SHIFT_RIGHT_EQUALS
%right '='
%nonassoc TOK_UNSHIFT TOK_PUSH TOK_SPLICE TOK_MAP TOK_FOLDR TOK_FOLDL TOK_SELECT
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
%nonassoc TOK_SHIFT TOK_POP TOK_CHOMP TOK_TRIM
%left NEG		      // unary minus, defined for precedence
%right '~' '\\'               // binary not, reference operator
%left '!'		      // logical not
%right TOK_NEW TOK_BACKGROUND
%nonassoc P_INCREMENT P_DECREMENT
%left '{' '[' '.' '(' DOT_KW_IDENTIFIER         // list and object references, etc, defined for precedence

%type <sblock>      block
%type <sblock>      statement_or_block
%type <sblock>      statements
%type <statement>   statement
%type <statement>   return_statement
%type <statement>   try_statement
%type <node>        exp
%type <node>        myexp
%type <node>        scalar
%type <hash>        hash
%type <list>        list
%type <String>      string
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
%destructor { if ($$) delete $$; } REGEX REGEX_SUBST REGEX_EXTRACT REGEX_TRANS block statement_or_block statements statement return_statement try_statement hash_element context_mods context_mod method_definition object_def top_namespace_decl namespace_decls namespace_decl scoped_const_decl unscoped_const_decl switch_statement case_block case_code superclass base_constructor private_member_list member_list base_constructor_list base_constructors class_attributes
%destructor { if ($$) $$->deref(); } superclass_list inheritance_list string QUOTED_WORD DATETIME BINARY IMPLICIT_ARG_REF DOT_KW_IDENTIFIER
%destructor { if ($$) $$->deref(0); } exp myexp scalar hash list
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
	      parse_error("illegal constant definition \"%s\" (conflicts with parse option NO_CONSTANT_DEFS)", $1->name->ostr);
	   delete $1;
	}
        | object_outofline_function_def  // registered directly
	| statement { 
	   if ($1) {
	      if ($1->isParseDeclaration())
		 delete $1;
	      else
		 getProgram()->addStatement($1);
	   }
	}
        | '{' statements '}' {
	   // set line range
	   $2->LineNumber = @1.first_line;
	   $2->EndLineNumber = @2.last_line;
	   getProgram()->addStatement($2);
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
	{ $4->setName($2); $$ = $4; free($2); }
        | TOK_NAMESPACE IDENTIFIER ';'
        { $$ = new QoreNamespace($2); free($2); }
	;

namespace_decls:
	namespace_decl
        {
	   class QoreNamespace *ns = new QoreNamespace();
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
	   if ($4 && $4->getType() == NT_FUNCREF)
	      parse_error("constants may not be assigned to call references");
	   else if (needsEval($4))
	      parse_error("constant expression needs run-time evaluation");
	   $$ = new ConstNode($2, $4); 
	}
        ;

scoped_const_decl:
	TOK_CONST SCOPED_REF '=' exp ';'
        {
	   if ($4 && $4->getType() == NT_FUNCREF)
	      parse_error("constants may not be assigned to call references");
	   else if (needsEval($4))
	      parse_error("constant expression needs run-time evaluation");
	   $$ = new ConstNode($2, $4); 
	}
        ;

block:
	'{' statements '}'
        { $$ = $2; }
        |
        '{' /* NOTHING */ '}'
        { $$ = 0; }
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
	| block
	{ $$ = new StatementBlock($1); }
 	| statements block
        { $1->addStatement($2); $$ = $1; }
	| statements statement
        { $1->addStatement($2); $$ = $1; }
	;

optname: 
	/* empty */ 
        { $$ = 0; }
        | IDENTIFIER { $$ = $1; }
        ;

statement:
	exp ';'
        {
	   // if the expression has no effect and it's not a variable declaration
	   qore_type_t t = $1 ? $1->getType() : 0;
	   if (!hasEffect($1)
	       && (t != NT_VARREF || reinterpret_cast<VarRefNode *>($1)->type == VT_UNRESOLVED)
	       && (t != NT_LIST || !reinterpret_cast<QoreListNode *>($1)->isVariableList()))
	      parse_error("statement has no effect (%s)", $1 ? $1->getTypeName() : "NOTHING");
	   if (t == NT_TREE)
	      reinterpret_cast<QoreTreeNode *>($1)->ignoreReturnValue();
	   $$ = new ExpressionStatement(@1.first_line, @1.last_line, $1);
	}
        // this should be covered as an expression, but for some reason it isn't...
        | SCOPED_REF '(' myexp ')' ';' {
	   NamedScope *ns = new NamedScope($1);
	   assert(ns->elements > 1);
	   printd(5, "statement: parsing static method call: %s()\n", ns->ostr);
	   $$ = new ExpressionStatement(@1.first_line, @1.last_line, new StaticMethodCallNode(ns, makeArgs($3)));
	}
        | try_statement
        { $$ = $1; }
	| TOK_RETHROW ';'
	{
	   $$ = new RethrowStatement(@1.first_line, @1.last_line);
	}
        | TOK_THROW exp ';'
        {
	   $$ = new ThrowStatement(@1.first_line, @2.last_line, $2);
	}
        | TOK_ON_EXIT statement_or_block
        {
	   $$ = new OnBlockExitStatement(@1.first_line, @2.last_line, $2, OBE_Unconditional);
	}
        | TOK_ON_SUCCESS statement_or_block
        {
	   $$ = new OnBlockExitStatement(@1.first_line, @2.last_line, $2, OBE_Success);
	}
        | TOK_ON_ERROR statement_or_block
        {
	   $$ = new OnBlockExitStatement(@1.first_line, @2.last_line, $2, OBE_Error);
	}
        | TOK_SUB_CONTEXT context_mods statement_or_block
        {
	   $$ = new ContextStatement(@1.first_line, @3.last_line, 0, 0, $2, $3);
	}
        | TOK_SUMMARIZE optname '(' exp ')' TOK_BY '(' exp ')' context_mods statement_or_block
        {
	   $$ = new SummarizeStatement(@1.first_line, @11.last_line, $2, $4, $10, $11, $8);
	}
        | TOK_CONTEXT optname '(' exp ')' context_mods statement_or_block
        {
	   $$ = new ContextStatement(@1.first_line, @7.last_line, $2, $4, $6, $7);
        }
	| TOK_IF '(' exp ')' statement_or_block %prec IFX
        {	
	   $$ = new IfStatement(@1.first_line, @5.last_line, $3, $5);
	}
        | TOK_IF '(' exp ')' statement_or_block TOK_ELSE statement_or_block
        {
	   $$ = new IfStatement(@1.first_line, @7.last_line, $3, $5, $7);
	}
	| TOK_WHILE '(' exp ')' statement_or_block
        {
	   $$ = new WhileStatement(@1.first_line, @5.last_line, $3, $5);
	}
	| TOK_DO statement_or_block TOK_WHILE '(' exp ')' ';'
        {
	   $$ = new DoWhileStatement(@1.first_line, @5.last_line, $5, $2);
	}
	| TOK_FOR '(' myexp ';' myexp ';' myexp ')' statement_or_block
        {
	   $$ = new ForStatement(@1.first_line, @9.last_line, $3, $5, $7, $9);
	}
        | TOK_FOREACH exp TOK_IN '(' exp ')' statement_or_block
        {
	   $$ = new ForEachStatement(@1.first_line, @7.last_line, $2, $5, $7);
	   qore_type_t t = $2 ? $2->getType() : 0;
	   if (t != NT_VARREF && t != NT_SELF_VARREF)
	      parse_error("foreach variable expression is not a variable reference");
	}
        | return_statement ';' { $$ = $1; }
        | TOK_THREAD_EXIT ';'  
	{ 
	   // see if thread exit is allowed
	   if (checkParseOption(PO_NO_THREAD_CONTROL))
	      parse_error("illegal use of \"thread_exit\" (conflicts with parse option NO_THREAD_CONTROL)");

	   $$ = new ThreadExitStatement(@1.first_line, @1.last_line); 
	}
        | TOK_BREAK ';'
        {
	  $$ = new BreakStatement(@1.first_line, @1.last_line);
	}
        | TOK_CONTINUE ';'
        {
	  $$ = new ContinueStatement(@1.first_line, @1.last_line);
	}
        | TOK_DELETE exp ';'
        {
	   $$ = new DeleteStatement(@1.first_line, @2.last_line, $2);
	   if (check_vars($2))
	      parse_error("delete statement takes only variable references as arguments");
	}
        | switch_statement { $$ = $1; }
        | error ';'        { $$ = 0; }
	;

context_mods:
	// empty 
        { $$ = 0; }
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
        TOK_RETURN     { $$ = new ReturnStatement(@1.first_line, @1.last_line); }
	|
	TOK_RETURN exp { $$ = new ReturnStatement(@1.first_line, @2.last_line, $2); }
	;

switch_statement:
        TOK_SWITCH '(' exp ')' '{' case_block '}'
        {
	   $6->setSwitch($3);
	   $$ = $6;
	   $$->LineNumber = @1.first_line;
	   $$->EndLineNumber = @7.last_line;
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
	   check_case(">=", $3);
	   $$ = new CaseNodeWithOperator($3, $5, OP_LOG_GE);
        }
        | TOK_CASE LOGICAL_GE exp ':' // nothing
        {
	   check_case(">=", $3);
	   $$ = new CaseNodeWithOperator($3, 0, OP_LOG_GE);
        }

        | TOK_CASE LOGICAL_LE exp ':' statements
        {
	   check_case("<=", $3);
	   $$ = new CaseNodeWithOperator($3, $5, OP_LOG_LE);
        }
        | TOK_CASE LOGICAL_LE exp ':' // nothing
        {
	   check_case("<=", $3);
	   $$ = new CaseNodeWithOperator($3, 0, OP_LOG_LE);
        }

        | TOK_CASE LOGICAL_EQ exp ':' statements
        {
	   check_case("==", $3);
	   $$ = new CaseNodeWithOperator($3, $5, OP_LOG_EQ);
        }
        | TOK_CASE LOGICAL_EQ exp ':' // nothing
        {
	   check_case("==", $3);
	   $$ = new CaseNodeWithOperator($3, 0, OP_LOG_EQ);
        }

        | TOK_CASE '<' exp ':' statements
        {
	   check_case("<", $3);
	   $$ = new CaseNodeWithOperator($3, $5, OP_LOG_LT);
        }
        | TOK_CASE '<' exp ':' // nothing
        {
	   check_case("<", $3);
	   $$ = new CaseNodeWithOperator($3, 0, OP_LOG_LT);
        }

        | TOK_CASE '>' exp ':' statements
        {
	   check_case(">", $3);
	   $$ = new CaseNodeWithOperator($3, $5, OP_LOG_GT);
        }
        | TOK_CASE '>' exp ':' // nothing
        {
	   check_case(">", $3);
	   $$ = new CaseNodeWithOperator($3, 0, OP_LOG_GT);
        }

	| TOK_CASE REGEX_MATCH REGEX ':' statements
	{
	   $$ = new CaseNodeRegex($3, $5);
	}
	| TOK_CASE REGEX_MATCH REGEX ':' // nothing
	{
	   $$ = new CaseNodeRegex($3, 0);
	}

	| TOK_CASE REGEX_NMATCH REGEX ':' statements
	{
	   $$ = new CaseNodeNegRegex($3, $5);
	}
	| TOK_CASE REGEX_NMATCH REGEX ':' // nothing
	{
	   $$ = new CaseNodeNegRegex($3, 0);
	}

	| TOK_CASE REGEX ':' statements
	{
	   $$ = new CaseNodeRegex($2, $4);
	}
	| TOK_CASE REGEX ':' // nothing
	{
	   $$ = new CaseNodeRegex($2, 0);
	}

        | TOK_CASE exp ':' statements
        {
	   check_case(0, $2);
	   $$ = new CaseNode($2, $4);
	}
        | TOK_CASE exp ':' // nothing
        {
	   check_case(0, $2);
	   $$ = new CaseNode($2, 0);
	}

        | TOK_DEFAULT ':' statements
        {
	   $$ = new CaseNode(0, $3);
	}
        | TOK_DEFAULT ':' // nothing
        {
	   $$ = new CaseNode(0, 0);
	}
        ;

try_statement:
        TOK_TRY statement_or_block TOK_CATCH '(' myexp ')' statement_or_block
        {
	   char *param = 0;
	   if ($5)
	   {
	      if ($5->getType() == NT_VARREF) 
		 param = reinterpret_cast<VarRefNode *>($5)->takeName();
	      else
		 parse_error("only one parameter accepted in catch block for exception hash");
	      $5->deref(0);
	   }
	   $$ = new TryStatement(@1.first_line, @7.last_line, $2, $7, param);
	}
        ;

myexp:     /* empty */          { $$ = 0; }
        | exp                   { $$ = $1; }
        ;

//finally:   /* empty */          { $$ = 0; }
//        | TOK_FINALLY statement { $$ = $2; }
//        ;

object_def:
	TOK_CLASS IDENTIFIER inheritance_list '{' class_attributes '}'
        {
	   $$ = new ObjClassDef($2, $5); 
	   $5->setName($2);
	   $5->parseSetBaseClassList($3);
	}
        | TOK_CLASS SCOPED_REF inheritance_list '{' class_attributes '}'
        { 
	   $$ = new ObjClassDef($2, $5); 
	   $5->setName($$->name->getIdentifier()); 
	   $5->parseSetBaseClassList($3);
	}
	| TOK_CLASS IDENTIFIER inheritance_list ';'
        { 
	   class QoreClass *qc = new QoreClass($2);
	   qc->parseSetBaseClassList($3);
	   $$ = new ObjClassDef($2, qc); 	   
	}
	| TOK_CLASS SCOPED_REF inheritance_list ';'
        { 
	   class QoreClass *qc = new QoreClass();
	   $$ = new ObjClassDef($2, qc);
	   qc->setName($$->name->getIdentifier());
	   qc->parseSetBaseClassList($3);
	}
	| TOK_CLASS IDENTIFIER inheritance_list '{' '}'
        { 
	   class QoreClass *qc = new QoreClass($2);
	   qc->parseSetBaseClassList($3);
	   $$ = new ObjClassDef($2, qc); 	   
	}
	| TOK_CLASS SCOPED_REF inheritance_list '{' '}'
        { 
	   class QoreClass *qc = new QoreClass();
	   $$ = new ObjClassDef($2, qc);
	   qc->setName($$->name->getIdentifier());
	   qc->parseSetBaseClassList($3);
	}
	;

inheritance_list:
        TOK_INHERITS superclass_list
        {
	   $$ = $2;
        }
        | // NOTHING
        { $$ = 0; }
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
	   $$ = new MethodNode(new UserFunction($2, new Paramlist($4), $7, $1 & OFM_SYNCED), $1 & OFM_PRIVATE, $1 & OFM_STATIC, $6);
	}
	| IDENTIFIER '(' myexp ')' base_constructor_list block
        {
	   if ($5 && strcmp($1, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($1, new Paramlist($3), $6), false, false, $5);
	}
	| method_modifiers KW_IDENTIFIER_OPENPAREN myexp ')' base_constructor_list block
	{
	   if ($5 && strcmp($2, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($2, new Paramlist($3), $6, $1 & OFM_SYNCED), $1 & OFM_PRIVATE, $1 & OFM_STATIC, $5);
	}
	| KW_IDENTIFIER_OPENPAREN myexp ')' base_constructor_list block
        {
	   if ($4 && strcmp($1, "constructor"))
	      parse_error("base class constructor lists are only legal when defining ::constructor() methods");
	   $$ = new MethodNode(new UserFunction($1, new Paramlist($2), $5), false, false, $4);
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
	   $$ = 0;
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
        | TOK_STATIC { $$ = OFM_STATIC; }
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

list:
	exp ',' exp
        { $$ = splice_expressions($1, $3); }
        | exp ','
        {
	   QoreListNode *l;
	   if ($1 && $1->getType() == NT_LIST) 
	      l = reinterpret_cast<QoreListNode *>($1);
	   else
	   {
	      l = new QoreListNode();
	      l->push($1);
	      // parse_error("problem in parsing ',' in list: left side of comma is not a list (type: '%s')", $1 ? $1->getTypeName() : "NOTHING");
	      // so we don't insert null values in the parse tree
	      //l = new QoreListNode();
	   }
	   $$ = l;
        }
        ;

hash:
	hash_element
	{
	   $$ = new QoreHashNode(true);
	   $$->setKeyValue($1->key, $1->value, 0);
	   delete $1;
	}
	| hash ',' hash_element
	{
	   $1->setKeyValue($3->key, $3->value, 0);
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
        { $$ = $1; }
        | list
        { $$ = $1; }
	| '(' hash ')'
	{ $$ = $2; }
        | SCOPED_REF
        { $$ = new ConstantNode($1); }
        | VAR_REF
        { $$ = new VarRefNode($1, VT_UNRESOLVED); }
        | TOK_MY VAR_REF
        { $$ = new VarRefNode($2, VT_LOCAL); }
        | TOK_MY '(' list ')' 
        {
	   $3->setVariableList();
	   for (unsigned i = 0; i < $3->size(); i++)
	   {
	      AbstractQoreNode *n = $3->retrieve_entry(i);
	      if (!n || n->getType() != NT_VARREF)
		 parse_error("element %d in list following 'my' is not a variable reference (%s)", i, n ? n->getTypeName() : "NOTHING");
	      else {
		 VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
		 if (v->type != VT_UNRESOLVED)
		    parse_error("illegal variable declaration '%s' in local variable declaration list", v->name);
		 else
		    v->type = VT_LOCAL;
	      }
	   }
	   $$ = $3;
	}
        | TOK_OUR VAR_REF {
	   getProgram()->addGlobalVarDef($2);
	   $$ = new VarRefNode($2, VT_GLOBAL); 
	}
        | TOK_OUR '(' list ')' { 
	   $3->setVariableList();
	   for (unsigned i = 0; i < $3->size(); i++) {
	      AbstractQoreNode *n = $3->retrieve_entry(i);
	      if (!n || n->getType() != NT_VARREF) 
		 parse_error("element %d in list following 'our' is not a variable reference (%s)", i, n ? n->getTypeName() : "NOTHING");
	      else {
		 VarRefNode *v = reinterpret_cast<VarRefNode *>(n);
		 if (v->type != VT_UNRESOLVED) {
		    parse_error("illegal variable declaration '%s' in global variable declaration list", v->name);
		 }
		 else {
		    v->type = VT_GLOBAL;
		    getProgram()->addGlobalVarDef(v->name);
		 }
	      }
	   }
	   $$ = $3;
	}
	| IDENTIFIER
        { $$ = new BarewordNode($1); }
	| CONTEXT_REF
        { $$ = new ContextrefNode($1); }
        | TOK_CONTEXT_ROW
        { $$ = new ContextRowNode(); }
        | COMPLEX_CONTEXT_REF
        { $$ = new ComplexContextrefNode($1); } 
        | TOK_FIND exp TOK_IN exp TOK_WHERE '(' exp ')'
        { $$ = new FindNode($2, $4, $7); }
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
	   if ($1 && $1->getType() == NT_LIST) {
	      QoreListNode *l = reinterpret_cast<QoreListNode *>($1);
	      bool ok = true;
	      for (unsigned i = 0; i < l->size(); i++)
	      {
		 AbstractQoreNode *n = l->retrieve_entry(i);
		 if (check_lvalue(n))
		 {
		    parse_error("element %d in list assignment is not an lvalue (%s)", i, n->getTypeName());
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
		 parse_error("left-hand side of assignment is not an lvalue (%s)", $1->getTypeName());
		 $$ = makeErrorTree(OP_ASSIGNMENT, $1, $3);
	      }
	      else
		 $$ = makeTree(OP_ASSIGNMENT, $1, $3);
	   }
	   //print_tree($1, 0);
	}
        | TOK_EXISTS exp
        { $$ = makeTree(OP_EXISTS, $2, 0); }
        | TOK_ELEMENTS exp
        { $$ = makeTree(OP_ELEMENTS, $2, 0); }
        | exp TOK_INSTANCEOF IDENTIFIER
        {
	   $$ = makeTree(OP_INSTANCEOF, $1, new ClassRefNode($3));
	}
        | exp TOK_INSTANCEOF SCOPED_REF
        {
	   $$ = makeTree(OP_INSTANCEOF, $1, new ClassRefNode($3));
	}
        | TOK_KEYS exp
        { $$ = makeTree(OP_KEYS, $2, 0); }
        | TOK_UNSHIFT exp  // unshift list, element
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() != 2) {
	      parse_error("invalid arguments to unshift, expected: lvalue, expression (%s)", $2->getTypeName());
	      $$ = makeErrorTree(OP_UNSHIFT, $2, 0);
	   }
	   else {
	      AbstractQoreNode *lv = l->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to unshift is not an lvalue");
		 $$ = makeErrorTree(OP_UNSHIFT, lv, l->shift());
	      }
	      else
		 $$ = makeTree(OP_UNSHIFT, lv, l->shift());
	      $2->deref(0);
	   }
	}
	| TOK_SHIFT exp
	{ 
	   if (check_lvalue($2))
	   {
	      parse_error("argument to shift operator is not an lvalue");
	      $$ = makeErrorTree(OP_SHIFT, $2, 0); 
	   }
	   else
	      $$ = makeTree(OP_SHIFT, $2, 0); 
	}
        | TOK_PUSH exp  // push lvalue-list, element
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() != 2) {
	      parse_error("invalid arguments to push, expected: lvalue, expression (%s)", $2->getTypeName());
	      $$ = makeErrorTree(OP_PUSH, $2, 0);
	   }
	   else
	   {
	      AbstractQoreNode *lv = l->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to push is not an lvalue");
		 $$ = makeErrorTree(OP_PUSH, lv, l->shift());
	      }
	      else
		 $$ = makeTree(OP_PUSH, lv, l->shift());
	      $2->deref(0);
	   }
	}
	| TOK_POP exp
	{
	   if (check_lvalue($2))
	   {
	      parse_error("argument to pop operator is not an lvalue");
	      $$ = makeErrorTree(OP_POP, $2, 0); 
	   }
	   else
	      $$ = makeTree(OP_POP, $2, 0); 
	}
	| TOK_CHOMP exp
	{
	   if (check_lvalue($2))
	   {
	      parse_error("argument to chomp operator is not an lvalue (use the chomp() function instead)");
	      $$ = makeErrorTree(OP_CHOMP, $2, 0); 
	   }
	   else
	      $$ = makeTree(OP_CHOMP, $2, 0); 
	}
	| TOK_TRIM exp
	{
	   if (check_lvalue($2))
	   {
	      parse_error("argument to trim operator is not an lvalue (use the trim() function instead)");
	      $$ = makeErrorTree(OP_TRIM, $2, 0); 
	   }
	   else
	      $$ = makeTree(OP_TRIM, $2, 0); 
	}
        | TOK_SPLICE exp  // splice lvalue-list, offset, [length, list]
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() < 2 || l->size() > 4) {
	      parse_error("invalid arguments to splice, expected: lvalue, offset exp [length exp, [list exp]] (got %s)", get_type_name($2));
	      $$ = makeErrorTree(OP_SPLICE, $2, 0);
	   }
	   else
	   {
	      AbstractQoreNode *lv = l->shift();
	      if (check_lvalue(lv))
	      {
		 parse_error("first argument to splice is not an lvalue");
		 $$ = makeErrorTree(OP_SPLICE, lv, $2);
	      }
	      else
		 $$ = makeTree(OP_SPLICE, lv, $2);
	   }
	}
        | TOK_MAP exp
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   int len = l ? l->size() : 0;
	   if (!l || len < 2 || len > 3) {
	      parse_error("invalid arguments to map operator, expected: 2 or 3 element list (code expression, list argument, [select expression]), got: '%s'", get_type_name($2));
	      $$ = makeErrorTree(OP_MAP, $2, 0);
	   }
	   else if (len == 2) {
	      AbstractQoreNode *map_exp = l->shift();
	      AbstractQoreNode *arg = l->shift();
	      $$ = new QoreTreeNode(map_exp, OP_MAP, arg);
	      $2->deref(0);
	   }
	   else {
	      AbstractQoreNode *map_exp = l->shift();
	      $$ = new QoreTreeNode(map_exp, OP_MAP_SELECT, l);
	   }
	}
        | TOK_FOLDR exp
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() != 2) {
	      parse_error("invalid arguments to foldr operator, expected: 2-element list expected: 2-element list (fold expression and list expression), got: '%s'", get_type_name($2));
	      $$ = makeErrorTree(OP_FOLDR, $2, 0);
	   }
	   else {
	      AbstractQoreNode *code_exp = l->shift();
	      AbstractQoreNode *arg = l->shift();
	      $$ = new QoreTreeNode(code_exp, OP_FOLDR, arg);
	      $2->deref(0);
	   }
	}
        | TOK_FOLDL exp
        {
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() != 2) {
	      parse_error("invalid arguments to foldl operator, expected: 2-element list (fold expression and list expression), got: '%s'", get_type_name($2));
	      $$ = makeErrorTree(OP_FOLDL, $2, 0);
	   }
	   else {
	      AbstractQoreNode *code_exp = l->shift();
	      AbstractQoreNode *arg = l->shift();
	      $$ = new QoreTreeNode(code_exp, OP_FOLDL, arg);
	      $2->deref(0);
	   }
	}
        | TOK_SELECT exp
	{
	   QoreListNode *l = $2 && $2->getType() == NT_LIST ? reinterpret_cast<QoreListNode *>($2) : 0;
	   if (!l || l->size() != 2) {
	      parse_error("invalid arguments to select operator, expected: 2-element list (list expression and select expression) got: '%s'", get_type_name($2));
	      $$ = makeErrorTree(OP_SELECT, $2, 0);
	   }
	   else {
	      AbstractQoreNode *arg = l->shift();
	      AbstractQoreNode *select_exp = l->shift();
	      $$ = new QoreTreeNode(arg, OP_SELECT, select_exp);
	      $2->deref(0);
	   }
	}
        | exp '?' exp ':' exp
        { $$ = new QoreTreeNode($1, OP_QUESTION_MARK, make_list($3, $5)); } 
        | P_INCREMENT exp   // pre-increment
        {
	   if (check_lvalue($2))
	   {
	      parse_error("pre-increment expression is not an lvalue");
	      $$ = makeErrorTree(OP_PRE_INCREMENT, $2, 0);
	   }
	   else
	      $$ = makeTree(OP_PRE_INCREMENT, $2, 0);
        }
        | exp P_INCREMENT   // post-increment
        {
	   if (check_lvalue($1))
	   {
	      parse_error("post-increment expression is not an lvalue");
	      $$ = makeErrorTree(OP_POST_INCREMENT, $1, 0);
	   }
	   else
	      $$ = makeTree(OP_POST_INCREMENT, $1, 0);
        }
        | P_DECREMENT exp   // pre-decrement
        {
	   if (check_lvalue($2))
	   {
	      parse_error("pre-decrement expression is not an lvalue");
	      $$ = makeErrorTree(OP_PRE_DECREMENT, $2, 0);
	   }
	   else
	      $$ = makeTree(OP_PRE_DECREMENT, $2, 0);
        }
        | exp P_DECREMENT   // post-decrement
        {
	   if (check_lvalue($1))
	   {
	      parse_error("post-decrement expression is not an lvalue");
	      $$ = makeErrorTree(OP_POST_DECREMENT, $1, 0);
	   }
	   else
	      $$ = makeTree(OP_POST_DECREMENT, $1, 0);
        }
	| exp '(' myexp ')'
        {
	   //printd(5, "1=%s (%08p), 3=%s (%08p)\n", $1->getTypeName(), $1, $3 ? $3->getTypeName() : "n/a", $3); 
	   qore_type_t t = $1 ? $1->getType() : 0;
	   if (t == NT_BAREWORD)
	   {
	      BarewordNode *b = reinterpret_cast<BarewordNode *>($1);
	      // take string from node and delete node
	      char *str = b->takeString();
	      b->deref();
	      printd(5, "parsing call %s() args=%08p %s\n", str, $3, $3 ? $3->getTypeName() : "n/a");
	      $$ = new FunctionCallNode(str, makeArgs($3));
	   }
	   else if (t == NT_CONSTANT)
	   {
	      ConstantNode *c = reinterpret_cast<ConstantNode *>($1);
	      // take NamedScope from node and delete node
	      NamedScope *ns = c->takeName();
	      c->deref();
	      assert(ns->elements > 1);
	      printd(5, "parsing scoped call (static method or new object call) %s()\n", ns->ostr);
	      $$ = new StaticMethodCallNode(ns, makeArgs($3));
	   }
	   else if (t == NT_SELF_VARREF)
	   {
	      SelfVarrefNode *v = reinterpret_cast<SelfVarrefNode *>($1);
	      // take string from node and delete node
	      char *str = v->takeString();
	      v->deref();
	      printd(5, "parsing in-object method call %s()\n", str);
	      $$ = new FunctionCallNode(makeArgs($3), str);
	   }
	   else {
	      QoreTreeNode *tree;
	      
	      if (t == NT_TREE) {
		 tree = reinterpret_cast<QoreTreeNode *>($1);
		 if (!(tree->op == OP_OBJECT_REF && tree->right && tree->right->getType() == NT_STRING))
		    tree = 0;
	      }
	      else
		 tree = 0;

	      if (tree) {
		 // create an object method call node
		 // take the string
		 class QoreStringNode *str = reinterpret_cast<QoreStringNode *>(tree->right);
		 char *cstr = str->giveBuffer();
		 str->deref();
		 
		 //printd(5, "method call to %s: tree=%s, args=%08p %s\n", cstr, tree->left->getTypeName(), $3, $3 ? $3->getTypeName() : "n/a");

		 MethodCallNode *mc = new MethodCallNode(cstr, makeArgs($3));
		 tree->right = mc;
		 tree->op = OP_OBJECT_FUNC_REF;
		 $$ = $1;
	      }
	      else {
		 VarRefNode *r = dynamic_cast<VarRefNode *>($1);
		 if (r && r->type != VT_UNRESOLVED)
		    parseException("INVALID-CODE-REFERENCE-CALL", "%s variable '%s' declared as a function reference call", r->type == VT_GLOBAL ? "global" : "local", r->name);
		 $$ = new CallReferenceCallNode($1, makeArgs($3));
	      }
	   }
	}
        | BASE_CLASS_CALL '(' myexp ')'
        {
	   printd(5, "parsing in-object base class method call %s()\n", $1->ostr);
	   if (!strcmp($1->getIdentifier(), "copy"))
	      parse_error("illegal call to base class copy method '%s'", $1->ostr);

	   $$ = new FunctionCallNode(makeArgs($3), $1);
	}
        | KW_IDENTIFIER_OPENPAREN myexp ')'
        {
	   printd(5, "parsing call %s()\n", $1);
	   $$ = new FunctionCallNode($1, makeArgs($2));
        }
        | SELF_REF                   { $$ = new SelfVarrefNode($1); }
	| exp LOGICAL_AND exp	     { $$ = makeTree(OP_LOG_AND, $1, $3); }
	| exp LOGICAL_OR exp	     { $$ = makeTree(OP_LOG_OR, $1, $3); }
	| exp '|' exp		     { $$ = makeTree(OP_BIN_OR, $1, $3); }
	| exp '&' exp		     { $$ = makeTree(OP_BIN_AND, $1, $3); }
	| exp '^' exp		     { $$ = makeTree(OP_BIN_XOR, $1, $3); }
        | exp REGEX_MATCH REGEX
        {
	   $$ = makeTree(OP_REGEX_MATCH, $1, $3);
	}
        | exp REGEX_NMATCH REGEX
        {
	   $$ = makeTree(OP_REGEX_NMATCH, $1, $3);
	}
        | exp REGEX_MATCH REGEX_SUBST
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of regular expression substitution operator is not an lvalue");
	      $$ = makeErrorTree(OP_REGEX_SUBST, $1, $3);
	   }
	   else
	   {
	      //printf("REGEX_SUBST: '%s'\n", $3->getPattern()->getBuffer());
	      $$ = makeTree(OP_REGEX_SUBST, $1, $3);
	   }
	}
        | exp REGEX_MATCH REGEX_TRANS
        {
	   if (check_lvalue($1))
	   {
	      parse_error("left-hand side of transliteration operator is not an lvalue");
	      $$ = makeErrorTree(OP_REGEX_TRANS, $1, $3);
	   }
	   else
	   {
	      $$ = makeTree(OP_REGEX_TRANS, $1, $3);
	   }
	}
        | exp REGEX_MATCH REGEX_EXTRACT
        { $$ = makeTree(OP_REGEX_EXTRACT, $1, $3); }
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
	| '-' exp %prec NEG	     { $$ = makeTree(OP_UNARY_MINUS, $2, 0); }
        | '~' exp		     { $$ = makeTree(OP_BIN_NOT, $2, 0); }
        | '!' exp                    { $$ = makeTree(OP_NOT, $2, 0); }
        | '\\' exp
        {
	   qore_type_t t = $2 ? $2->getType() : 0;
	   //printd(5, "backslash exp line %d, type %s\n", @2.first_line, $2->getTypeName());

	   if (t == NT_FUNCTION_CALL) {
	      FunctionCallNode *f = reinterpret_cast<FunctionCallNode *>($2);
	      if (f->getArgs()) {
		 parse_error("argument given to call reference");
		 $$ = $2;
	      }
	      else {
		 if (f->getFunctionType() == FC_UNRESOLVED)
		    $$ = new UnresolvedCallReferenceNode(f->takeName());
		 else {// must be self call
		    assert(f->getFunctionType() == FC_SELF);
		    if (f->f.sfunc->name)
		       $$ = new ParseSelfMethodReferenceNode(f->f.sfunc->takeName());
		    else {
		       assert(f->f.sfunc->ns);
		       $$ = new ParseScopedSelfMethodReferenceNode(f->f.sfunc->takeNScope());
		    }
		 }
		 f->deref();
	      }
	   }
	   else if (t == NT_STATIC_METHOD_CALL) {
	      StaticMethodCallNode *m = reinterpret_cast<StaticMethodCallNode *>($2);
	      if (m->getArgs()) {
		 parse_error("argument given to static method call reference");
		 $$ = $2;
	      }
	      else {
		 $$ = new UnresolvedStaticMethodCallReferenceNode(m->takeScope());
		 m->deref();
	      }
	   }
	   else {
	      bool make_ref = true;

	      if (t == NT_TREE) {
		 QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>($2);
		 if (tree->op == OP_OBJECT_FUNC_REF) {
		    assert(tree->right->getType() == NT_METHOD_CALL);
		    MethodCallNode *m = reinterpret_cast<MethodCallNode *>(tree->right);
		    if (m->getArgs()) {
		       parse_error("argument given to call reference");
		       $$ = $2;
		    }
		    else { // rewrite as a call reference
		       // take components of tree and delete tree
		       AbstractQoreNode *exp = tree->left;
		       tree->left = 0;
		       char *meth = m->takeName();
		       m->deref();
		       tree->right = 0;
		       tree->deref();
		       $$ = new ParseObjectMethodReferenceNode(exp, meth);
		       //printd(5, "made parse object method reference: exp=%08p meth=%s (node=%08p)\n", exp, meth, $$);
		       make_ref = false;
		    }
		 }
	      }

	      if (make_ref) {
		 //printd(5, "type=%s\n", $2->getTypeName());
		 $$ = new ReferenceNode($2);
		 if (check_lvalue($2))
		    parse_error("argument to reference operator is not an lvalue or a function or method");
	      }
	   }
	}
        | TOK_NEW exp //function_call
        {
	   qore_type_t t = $2 ? $2->getType() : 0;
	   if (t == NT_STATIC_METHOD_CALL) {
	      StaticMethodCallNode *smc = reinterpret_cast<StaticMethodCallNode *>($2);
	      ScopedObjectCallNode *new_exp = new ScopedObjectCallNode(smc->takeScope(), smc->takeArgs());	      
	      smc->deref();
	      $$ = makeTree(OP_NEW, new_exp, 0); 
	      // see if new can be used
	      if (checkParseOption(PO_NO_NEW))
		 parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	   }
	   else if (t != NT_FUNCTION_CALL) {
	      parse_error("invalid expression after 'new' operator (%s)", $2 ? $2->getTypeName() : "<nothing>");
	      $$ = $2;
	   }
	   else {
	      FunctionCallNode *f = reinterpret_cast<FunctionCallNode *>($2);
	      $$ = makeTree(OP_NEW, f->parseMakeNewObject(), 0);
	      f->deref();
	      // see if new can be used
	      if (checkParseOption(PO_NO_NEW))
		 parse_error("illegal use of the \"new\" operator (conflicts with parse option NO_NEW)");
	   }
	}
	| TOK_BACKGROUND exp	     
	{
	   $$ = makeTree(OP_BACKGROUND, $2, 0);
	   // check to see if the expression is legal
	   if (checkParseOption(PO_NO_THREAD_CONTROL))
	      parse_error("illegal use of \"background\" operator (conflicts with parse option NO_THREAD_CONTROL)");
	   else if (!hasEffect($2))
	      parse_error("argument to background operator has no effect");
	}
        | BACKQUOTE
        {
	   $$ = new BackquoteNode($1);
	   if (checkParseOption(PO_NO_EXTERNAL_PROCESS))
	      parse_error("illegal use of backquote operator (conflicts with parse option NO_EXTERNAL_PROCESS)");
	}
        | exp '[' exp ']'            { $$ = makeTree(OP_LIST_REF, $1, $3); }
	| exp '{' exp '}'            { $$ = makeTree(OP_OBJECT_REF, $1, $3); }
	| exp '.' exp
	{
	   $$ = process_dot($1, $3);
	}
        | exp DOT_KW_IDENTIFIER
	{
	   $$ = makeTree(OP_OBJECT_REF, $1, $2);
	}
	| '(' exp ')'                
        { 
	   $$ = $2;
	   if ($2 && $2->getType() == NT_LIST)
	      reinterpret_cast<QoreListNode *>($2)->setFinalized(); 
	}
        | '(' ')' { QoreListNode *l = new QoreListNode(); l->setFinalized(); $$ = l; }
        | TOK_SUB '(' myexp ')' block
        { 
	   UserFunction *uf = new UserFunction(0, new Paramlist($3), $5);
	   $$ = new QoreClosureParseNode(uf);
/*
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_CLOSURES))
	      parse_error("illegal closure definition (conflicts with parse option NO_CLOSURES)");
*/
	}
	| TOK_SYNCHRONIZED TOK_SUB '(' myexp ')' block
        {
	   UserFunction *uf = new UserFunction(0, new Paramlist($4), $6, true);
	   $$ = new QoreClosureParseNode(uf);
/*
	   // make sure definition was legal
	   if (checkParseOption(PO_NO_CLOSURES))
	      parse_error("illegal closure definition (conflicts with parse option NO_CLOSURES)");
*/
	}
        | IMPLICIT_ARG_REF { $$ = $1; }
	;

string:
	QUOTED_WORD 
	{
	   $$ = $1;
	}
	| QUOTED_WORD string
	{
	   $$ = $1;
	   $$->concat($2);
	   $2->deref();
	}

scalar:
	QFLOAT        { $$ = new QoreFloatNode($1); }
        | INTEGER     { $$ = new QoreBigIntNode($1); }
        | string      { $$ = $1; }
        | DATETIME    { $$ = $1; }
        | TOK_NULL    { $$ = null(); }
        | TOK_NOTHING { $$ = nothing(); }
	;

%%
