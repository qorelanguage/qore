" Vim syntax file for Qore * mato [25-oct-2015]
" Language:	Qore
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2016 May 26

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Qore keywords
syn keyword qoreKeyword		abstract
syn keyword qoreKeyword		class
syn keyword qoreStorageClass 	const
syn keyword qoreKeyword 	constructor
syn keyword qoreKeyword		deprecated
syn keyword qoreKeyword 	destructor
syn keyword qoreKeyword		final
syn keyword qoreKeyword		inherits
syn keyword qoreKeyword		module
syn keyword qoreKeyword		namespace
syn keyword qoreStorageClass 	my our
syn keyword qoreAccess		private
syn keyword qoreAccess		public
syn keyword qoreStorageClass 	static
syn keyword qoreKeyword		sub returns
syn keyword qoreStorageClass 	synchronized

" Qore operators
syn keyword qoreOperator background
syn keyword qoreOperator cast
syn keyword qoreOperator chomp
syn keyword qoreOperator delete
syn keyword qoreOperator elements
syn keyword qoreOperator exists
syn keyword qoreOperator extract
syn keyword qoreOperator foldl
syn keyword qoreOperator foldr
syn keyword qoreOperator instanceof
syn keyword qoreOperator keys
syn keyword qoreOperator map
syn keyword qoreOperator new
syn keyword qoreOperator pop
syn keyword qoreOperator push
syn keyword qoreOperator remove
syn keyword qoreOperator select
syn keyword qoreOperator shift
syn keyword qoreOperator splice
syn keyword qoreOperator trim
syn keyword qoreOperator unshift

" Qore statements
syn keyword qoreConditional	if else
syn keyword qoreConditional	switch
syn keyword qoreLabel		case default
syn keyword qoreRepeat		for foreach in 
syn keyword qoreRepeat		find in
syn keyword qoreRepeat		while do
syn keyword qoreStatement	break continue
syn keyword qoreException	try catch
syn keyword qoreException	rethrow throw
syn keyword qoreStatement	context subcontext summarize by
syn keyword qoreStatement	where sortBy sortDescendingBy
syn keyword qoreStatement	on_exit on_success on_error
syn keyword qoreStatement	thread_exit
syn keyword qoreStatement	return

" Qore types
syn keyword qoreType binary
syn keyword qoreType bool softbool
syn keyword qoreType date softdate
syn keyword qoreType float softfloat
syn keyword qoreType int softint
syn keyword qoreType number softnumber
syn keyword qoreType string softstring
syn keyword qoreType timeout

syn keyword qoreType any
syn keyword qoreType code
syn keyword qoreType data
syn keyword qoreType hash
syn keyword qoreType list softlist
syn keyword qoreType nothing
syn keyword qoreType object
syn keyword qoreType reference

" Qore code flags
syn keyword qoreCodeFlag CONSTANT
syn keyword qoreCodeFlag DEPRECATED
syn keyword qoreCodeFlag NOOP
syn keyword qoreCodeFlag RET_VALUE_ONLY
syn keyword qoreCodeFlag RUNTIME_NOOP

" Qore parse directives
syn match qoreParseDefine	"^%define\>"
syn match qoreParseConditional	"^%else\>"
syn match qoreParseConditional	"^%endif\>"
syn match qoreParseConditional	"^%ifdef\>"
syn match qoreParseConditional	"^%ifndef\>"

syn match qoreParseInclude "^%include\>"
syn match qoreParseInclude "^%module-cmd\>"
syn match qoreParseInclude "^%requires\>"
syn match qoreParseInclude "^%try-module\>"

syn match qoreParseDirective "^%allow-bare-refs\>"
syn match qoreParseDirective "^%allow-injection\>"
syn match qoreParseDirective "^%append-include-path\>"
syn match qoreParseDirective "^%append-module-path\>"
syn match qoreParseDirective "^%assume-global\>"
syn match qoreParseDirective "^%assume-local\>"
syn match qoreParseDirective "^%broken-int-assignments\>"
syn match qoreParseDirective "^%broken-list-parsing\>"
syn match qoreParseDirective "^%broken-operators\>"
syn match qoreParseDirective "^%disable-all-warnings\>"
syn match qoreParseDirective "^%disable-warning\>"
syn match qoreParseDirective "^%enable-all-warnings\>"
syn match qoreParseDirective "^%enable-warning\>"
syn match qoreParseDirective "^%endtry\>"
syn match qoreParseDirective "^%exec-class\>"
syn match qoreParseDirective "^%lockdown\>"
syn match qoreParseDirective "^%lock-options\>"
syn match qoreParseDirective "^%lock-warnings\>"
syn match qoreParseDirective "^%new-style\>"
syn match qoreParseDirective "^%no-class-defs\>"
syn match qoreParseDirective "^%no-child-restrictions\>"
syn match qoreParseDirective "^%no-constant-defs\>"
syn match qoreParseDirective "^%no-database\>"
syn match qoreParseDirective "^%no-external-access\>"
syn match qoreParseDirective "^%no-external-info\>"
syn match qoreParseDirective "^%no-external-process\>"
syn match qoreParseDirective "^%no-filesystem\>"
syn match qoreParseDirective "^%no-global-vars\>"
syn match qoreParseDirective "^%no-gui\>"
syn match qoreParseDirective "^%no-io\>"
syn match qoreParseDirective "^%no-locate-control\>"
syn match qoreParseDirective "^%no-modules\>"
syn match qoreParseDirective "^%no-namespace-defs\>"
syn match qoreParseDirective "^%no-network\>"
syn match qoreParseDirective "^%no-new\>"
syn match qoreParseDirective "^%no-process-control\>"
syn match qoreParseDirective "^%no-subroutine-defs\>"
syn match qoreParseDirective "^%no-terminal-io\>"
syn match qoreParseDirective "^%no-thread-classes\>"
syn match qoreParseDirective "^%no-thread-control\>"
syn match qoreParseDirective "^%no-thread-info\>"
syn match qoreParseDirective "^%no-threads\>"
syn match qoreParseDirective "^%no-top-level\>"
syn match qoreParseDirective "^%old-style\>"
syn match qoreParseDirective "^%perl-bool-eval\>"
syn match qoreParseDirective "^%push-parse-options\>"
syn match qoreParseDirective "^%require-dollar\>"
syn match qoreParseDirective "^%require-our\>"
syn match qoreParseDirective "^%require-prototypes\>"
syn match qoreParseDirective "^%require-types\>"
syn match qoreParseDirective "^%set-time-zone\>"
syn match qoreParseDirective "^%strict-args\>"
syn match qoreParseDirective "^%strict-bool-eval\>"

" Qore supplied modules
syn keyword qoreModule BulkSqlUtil
syn keyword qoreModule CsvUtil
syn keyword qoreModule Diff
syn keyword qoreModule FilePoller
syn keyword qoreModule FixedLengthUtil
syn keyword qoreModule FreetdsSqlUtil
syn keyword qoreModule HttpServer
syn keyword qoreModule HttpServerUtil
syn keyword qoreModule MailMessage
syn keyword qoreModule Mapper
syn keyword qoreModule Mime
syn keyword qoreModule MysqlSqlUtil
syn keyword qoreModule OracleSqlUtil
syn keyword qoreModule PgsqlSqlUtil
syn keyword qoreModule Pop3Client
syn keyword qoreModule Qorize
syn keyword qoreModule QUnit
syn keyword qoreModule RestClient
syn keyword qoreModule RestHandler
syn keyword qoreModule SalesforceRestClient
syn keyword qoreModule Schema
syn keyword qoreModule SmtpClient
syn keyword qoreModule SqlUtil
syn keyword qoreModule TableMapper
syn keyword qoreModule TelnetClient
syn keyword qoreModule UnitTest
syn keyword qoreModule Util
syn keyword qoreModule WebSocketClient
syn keyword qoreModule WebSocketHandler
syn keyword qoreModule WebSocketUtil
syn keyword qoreModule WebUtil

if exists("qore_highlight_all")
  if exists("qore_no_builtin_highlight")
    unlet qore_no_builtin_highlight
  endif
  if exists("qore_no_exception_highlight")
    unlet qore_no_exception_highlight
  endif
  if exists("qore_no_number_highlight")
    unlet qore_no_number_highlight
  endif
  let qore_space_error_highlight = 1
endif

" Qore built-ins
if !exists("qore_no_builtin_highlight")
  " built-in constants
  syn keyword qoreBoolean	False True
  syn keyword qoreConstant	NOTHING NULL
endif

" Qore exceptions
if !exists("qore_no_exception_highlight")
  "syn keyword qoreExceptionName
endif

if exists("qore_space_error_highlight")
  " trailing whitespace
  syn match qoreSpaceError display excludenl "\s\+$"
  " mixed tabs and spaces
  syn match qoreSpaceError display " \+\t"
  syn match qoreSpaceError display "\t\+ "
endif

syn region qoreBlock start="{" end="}" fold transparent

syn match qoreIdentifier display "\$[A-Za-z_][A-Za-z0-9_]*"
syn match qoreIdentifier display "\$\$\|\$[0-9]\+"
syn match qoreIdentifier display "\$\."he=e-1
syn match qoreIdentifier display "\$\#"

syn match qoreInteger display "\(\<\|[-+]\)\d\+n\?\>"
"floating point number, with dot, optional exponent
syn match qoreFloat display "\(\<\|[-+]\)\d\+\.\d*\(e[-+]\=\d\+\)\=n\?"
"floating point number, starting with a dot, optional exponent
syn match qoreFloat display "[-+]\?\.\d\+\(e[-+]\=\d\+\)\=n\?\>"
"floating point number, without dot, with exponent
syn match qoreFloat display "\(\<\|[-+]\)\d\+e[-+]\=\d\+n\?\>"

"absolute date/time
syn match qoreDateTime "\<\d\{4}-\d\{2}-\d\{2}\([T-]\d\{2}:\d\{2}\(:\d\{2}\(\.\d\{1,6}\)\?\)\?\)\?\(Z\|[+-]\?\d\{2}\(:\d\{2}\(:\d\{2}\)\?\)\?\)\?\>"
"relative date/time
syn match qoreDateTime "\<\d\+\([YMDhms]\|ms\|us\)\>"
syn match qoreDateTime "\<P\(T\?[0-9]\)\@=\(\d\+Y\)\?\(\d\+M\)\?\(\d\+D\)\?\(T\(\d\+H\)\?\(\d\+M\)\?\(\d\+S\)\?\(\d\+u\)\?\)\?\>"
syn match qoreDateTime "\<P\d\{4}-\d\{2}-\d\{2}T\d\{2}:\d\{2}:\d\{2}\>"

syn match qoreStringEscape '\\\(\\\|[bfnrt"]\|\o\{1,3}\)' contained display
syn region qoreString start='"' skip='\\"' end='"' contains=qoreStringEscape,@Spell fold
syn region qoreString start="'" end="'" fold

syn match qoreRegexp "[=!]\~\s*\zs[mx]\?/.\{-}\\\@<!/[imsx]*"
syn match qoreRegexp "s/.\{-}\\\@<!/.\{-}\\\@<!/[gimsx]*"
syn match qoreRegexp "tr/.\{-}\\\@<!/.\{-}\\\@<!/"

syn keyword qoreTodo TODO NOTE XXX FIXME DEBUG contained

syn match qoreComment "#.*" contains=qoreTodo,qoreSpaceError,@Spell
syn region qoreComment start="/\*" end="\*/" contains=qoreTodo,qoreSpaceError,@Spell fold

if !exists("qore_minlines")
  let qore_minlines = 100
endif
"exec "syn sync minlines=" . qore_minlines
exec "syn sync ccomment qoreComment minlines=" . qore_minlines

" Define the default highlighting.
" For version 5.x and earlier, only when not done already.
" For version 5.8 and later, only when an item doesn't have highlighting yet.
if version >= 508 || !exists("did_qore_syn_inits")
  if version < 508
    let did_qore_syn_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  HiLink qoreComment		Comment
  HiLink qoreIdentifier		Identifier
  HiLink qoreTodo		Todo

  HiLink qoreConstant		Constant
  HiLink qoreBoolean		Boolean
  HiLink qoreDateTime		Constant
  HiLink qoreRegexp		Constant
  HiLink qoreString		String
  HiLink qoreStringEscape	SpecialChar

  HiLink qoreType		Type
  HiLink qoreStorageClass	StorageClass
  HiLink qoreStructure		Structure

  HiLink qoreStatement		Statement
  HiLink qoreConditional	Conditional
  HiLink qoreException		Exception
  HiLink qoreKeyword		Keyword
  HiLink qoreLabel		Label
  HiLink qoreOperator		Operator
  HiLink qoreRepeat		Repeat
  HiLink qoreAccess		Keyword

  HiLink qoreCodeFlag		Keyword
  HiLink qoreParseConditional	PreCondit
  HiLink qoreParseDefine	Define
  HiLink qoreParseInclude	Include
  HiLink qoreParseDirective	PreProc

  if !exists("qore_no_builtin_highlight")
    "HiLink Builtin		Function
    "HiLink Module		Function
    HiLink qoreBuiltin		Builtin
    HiLink qoreModule		Module
  endif
  if !exists("qore_no_exception_highlight")
    HiLink qoreExceptionName	Structure
  endif
  if !exists("qore_no_number_highlight")
    HiLink qoreFloat		Float
    HiLink qoreInteger		Number
  endif
  if exists("qore_space_error_highlight")
    HiLink qoreSpaceError	SpaceError
  endif

  delcommand HiLink
endif

let b:current_syntax = "qore"

" vim: ts=8 sw=2
