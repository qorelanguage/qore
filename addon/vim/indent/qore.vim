" Vim indent file for Qore * mato [12-dec-2015]
" Language:	Qore
" Based on:
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2005 Mar 27

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
   finish
endif
let b:did_indent = 1

" C indenting is built-in, thus this is very simple
setlocal cindent

" Qore adjustments
setlocal cinoptions=#1
setlocal cinkeys-=0#
setlocal indentkeys-=0#

let b:undo_indent = "setl cin< cino< cink< indk<"
