" Vim filetype plugin file for Qore * mato [26-oct-2015]
" Language:	Qore
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2015 Nov 03

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif
let b:did_ftplugin = 1

let b:undo_ftplugin = "setl et< sw< sts<"

" let tab keys always be expanded to spaces
setlocal expandtab
" set shift width for indent
setlocal shiftwidth=4
" set the tab key size
setlocal softtabstop=4

if exists("c_space_errors")
  let qore_space_error_highlight = 1
  let qore_highlight_all = 1
endif

" vim: ts=8 sw=2
