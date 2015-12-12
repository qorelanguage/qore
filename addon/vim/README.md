# Setting up VIM for Qore & Qorus

If you use VIM, even occasionally, for editing Qore and Qorus stuff, the following might come handy.

First, make sure you have VIM configuration folder:  `mkdir -p ~/.vim`  
Later, if any subfolder mentioned in this text is missing from `~/.vim`, just create it.

## File type detection

Vim can detect the type of file that is edited.  So we just teach it to recognise the files we use.  
Here's an example of how to do it -- just save the following into file `~/.vim/filetype.vim` :

```vim
"my filetype file" * mato [03-nov-2015]

if exists("did_load_filetypes")
  finish
endif

augroup filetypedetect
  " Qore and Qorus files
  au! BufRead,BufNewFile *.q,*.qm,*.qtest       setfiletype qore
  au! BufRead,BufNewFile *.qc,*.qclass,*.qconn,*.qconst,*.qfd,*.qjob,*.ql,*.qmapper,*.qrf,*.qsd,*.qsm,*.qvmap,*.qwf       setfiletype qore
  au! BufRead,BufNewFile *.qpp                  setfiletype cpp
augroup END
```

## Syntax file

Next, we need to teach VIM about syntax of Qore language.  
Just copy `syntax` folder found here to your `~/.vim` folder for VIM to load and use it.

Please note that there are some extra highlighting groups defined in the syntax file.  
You may want to configure them by copying and editing the following lines into your colour scheme file (usually located in `~/.vim/colors/` folder):

```vim
hi SpaceError   guibg=darkGreen ctermbg=darkGreen
hi Builtin      guifg=lightGreen
hi Module       guifg=lightGreen
hi Package      guifg=lightGreen
```

## Syntax highlighting

Syntax highlighting requires a few VIM options turned on, but that is most usually already done.  
You can have a look at `$VIMRUNTIME/vimrc_example.vim` file that comes with VIM for inspiration.

## More customisation

You may want to customise VIM's behaviour (based on filetype) even more.  
An example of how to do it is included -- just copy `ftplugin` folder found here to your `~/.vim` folder for VIM to load and use it.

