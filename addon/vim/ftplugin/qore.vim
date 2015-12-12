" Vim filetype plugin file for Qore * mato [26-oct-2015]
" Language:	Qore
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2015 Dec 12

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif
let b:did_ftplugin = 1

let b:undo_ftplugin = "setl et< sw< sts< com< def< inc< sua<"

" let tab keys always be expanded to spaces
setlocal expandtab
" set shift width for indent
setlocal shiftwidth=4
" set the tab key size
setlocal softtabstop=4

" define how to recognise and format comments
setlocal comments=sO:*\ -,mO:*\ \ ,exO:*/,s1:/*,mb:*,ex:*/,:#

" patterns for finding macro definitions and include directives
setlocal define=^%define\\>
setlocal include=^%include\\>

" suffixes to use when searching for files for some commands
setlocal suffixesadd=.q,.qm,.qtest
setlocal suffixesadd+=.qc,.qclass,.qconn,.qconst,.qfd,.qjob,.ql,.qmapper,.qrf,.qsd,.qsm,.qvmap,.qwf

" vim: ts=8 sw=2
