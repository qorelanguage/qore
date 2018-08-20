" Vim filetype plugin file for Qorus * mato [26-oct-2015]
" Language:	Qorus
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2016 Jan 30

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

runtime! ftplugin/qore.vim ftplugin/qore_*.vim ftplugin/qore/*.vim
let b:did_ftplugin = 1

let b:undo_ftplugin = "setl com< inex< sua< | " . b:undo_ftplugin

" suffixes to use when searching for files for some commands
setlocal suffixesadd+=.qc,.qclass,.qconn,.qconst,.qfd,.qjob,.ql,.qmapper,.qrf,.qsd,.qsm,.qvmap,.qwf

" this is for Qorus but it's very unreliable and breaks other stuff (!)
"setlocal includeexpr=substitute(v:fname,'$','-v1.0','')

" vim: ts=8 sw=2
