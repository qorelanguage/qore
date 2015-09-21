" Vim syntax file
" Language: qore
" Maintainer: Adrián Lachata

" Qore statements
syn keyword qStat public private final inherits abstract self static namespace 
syn keyword qStat if else foreach in my our while do for switch continue break
syn keyword qStat case const return returns sub
syn keyword qStat try catch throw rethrow thread_exit context summarize 
syn keyword qStat synchronized subcontext deprecated by 
syn keyword qStat on_exit on_success on_error 

" Qore types
syn keyword qTyp int float number bool string date binary hash list object
syn keyword qTyp class null nothing timeout softint softfloat softnumber softbool
syn keyword qTyp softstring softdate softlist data code reference any

" Qore operators
syn keyword qOp new background detele remove shift pop chomp trim
syn keyword qOp elements keys exists instanceof unshift push splice
syn keyword qOp extract map foldl foldr select where

"Qore parse directives
syn match qParse "^[%]requires"
syn match qParse "^[%]new[-]style"
syn match qParse "^[%]require-types"
syn match qParse "^[%]allow[-]bare[-]"
syn match qParse "^[%]allow[-]injection"
syn match qParse "^[%]append[-]include[-]path"
syn match qParse "^[%]append[-]module[-]path"
syn match qParse "^[%]assume[-]global"
syn match qParse "^[%]assume[-]local"
syn match qParse "^[%]define"
syn match qParse "^[%]disable[-]all[-]warnings"
syn match qParse "^[%]disavle[-]warning"
syn match qParse "^[%]else"
syn match qParse "^[%]enable[-]all[-]warnings]"
syn match qParse "^[%]enable[-]warning"
syn match qParse "^[%]endif"
syn match qParse "^[%]endtry"
syn match qParse "^[%]exec[-]class"
syn match qParse "^[%]ifdef"
syn match qParse "^[%]ifndef"
syn match qParse "^[%]include"
syn match qParse "^[%]lockdown"
syn match qParse "^[%]lock[-]options"
syn match qParse "^[%]lock[-]warnings"
syn match qParse "^[%]old[-]style"
syn match qParse "^[%]no[-]class[-]defs"
syn match qParse "^[%]no[-]child[-]restrictions"
syn match qParse "^[%]no[-]constant[-]defs"
syn match qParse "^[%]no[-]database"
syn match qParse "^[%]no[-]external[-]access"
syn match qParse "^[%]no[-]external[-]info"
syn match qParse "^[%]no[-]external[-]process"
syn match qParse "^[%]no[-]filesystem"
syn match qParse "^[%]no[-]global[-]vars"
syn match qParse "^[%]no[-]gui"
syn match qParse "^[%]no[-]io"
syn match qParse "^[%]no[-]locate[-]control"
syn match qParse "^[%]no[-]modules"
syn match qParse "^[%]no[-]namespace[-]defs"
syn match qParse "^[%]no[-]network"
syn match qParse "^[%]no[-]new"
syn match qParse "^[%]no[-]process[-]constrol"
syn match qParse "^[%]no[-]subroutine[-]defs"
syn match qParse "^[%]no[-]terminal[-]io"
syn match qParse "^[%]no[-]thread[-]classes"
syn match qParse "^[%]no[-]thread[-]constrol"
syn match qParse "^[%]no[-]thread[-]info"
syn match qParse "^[%]no[-]threads"
syn match qParse "^[%]no[-]top[-]level"
syn match qParse "^[%]perl[-]bool[-]eval"
syn match qParse "^[%]push[-]parse[-]options"
syn match qParse "^[%]require[-]our"
syn match qParse "^[%]require[-]prototypes"
syn match qParse "^[%]requires"
syn match qParse "^[%]strict[-]args"
syn match qParse "^[%]strict[-]bool[-]eval"
syn match qParse "^[%]try[-]module"


syn match qNum '\<\d\+\>'
syn match qNum '\<[-+]\d\+\>'
" syn match qNum '\<\x\+'
" syn match qNum '\<\o\+'

syn match qNum '\<\d\+\.\d*\>'
syn match qNum '\<[-+]\d\+\.\d*\>'

syn keyword qBool True False

syn match qStr "\".\{-}\""
syn match qStr "\'.\{-}\'"

syn keyword qTodo contained TODO FIXME XXX NOTE
syn match qComment "#.*$" contains=qTodo
syn match qComment "/\*.*\*/" contains=qTodo
syn region qCommentBlock start="/\*" end="\*/"

hi def link qTodo          Todo
hi def link qComment       Comment
hi def link qCommentBlock  Comment
hi def link qStat          Statement
hi def link qOp            Statement
hi def link qTyp           Type
hi def link qNum           Constant
hi def link qBool          Constant
hi def link qStr           Constant
hi def link qParse         PreProc
