" Vim indent file for Qorus * mato [30-jan-2016]
" Language:	Qorus
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2016 Jan 30

" Only load this indent file when no other was loaded
if exists("b:did_indent")
   finish
endif

" Behave just like Qore
runtime! indent/qore.vim
