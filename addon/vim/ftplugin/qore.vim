" Vim filetype plugin file for Qore * mato [26-oct-2015]
" Language:	Qore
" Maintainer:	Martin Otto <martin@qore.org>
" Last Change:	2016 Mar 07

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif
let b:did_ftplugin = 1

let b:undo_ftplugin = "setl et< sw< sts< com< def< inc< sua< path< ofu<"

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
setlocal include=^%\\(include\\\|requires\\)\\>

" suffixes to use when searching for files for some commands
setlocal suffixesadd=.q,.qm,.qtest

" Set this once, globally.
if !exists("qorepath")
  let qorepath = ".,,"
  if exists("$QORE_INCLUDE_DIR")
    let qorepath .= substitute($QORE_INCLUDE_DIR, ':', ',', 'g') .','
  endif
  if exists("$QORE_MODULE_DIR")
    let qorepath .= substitute($QORE_MODULE_DIR, ':', ',', 'g') .','
  endif
  if executable("qore")
    try
      let qorepath .= substitute(system("qore --module-path"), ':', ',', 'g')
    catch /E145:/
      " ignore
    endtry
  endif
endif

" directories to be searched for files for some commands
let &l:path = qorepath

" enable syntax code completion
setlocal omnifunc=syntaxcomplete#Complete

" vim: ts=8 sw=2
