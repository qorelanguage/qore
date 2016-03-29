" Vim syntax file for Qorus * mato [30-jan-2016]
" Language:	Qorus
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2016 Jan 30

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Read in Qore syntax file
if version < 600
  so <sfile>:p:h/qore.vim
else
  runtime! syntax/qore.vim
endif

"let b:current_syntax = "qorus"

" vim: ts=8 sw=2
