"2008-02-08 00:04:56 SANGWON 
"c-support에서 제공하는 설정 + 기존에 사용하던 설정
"
"http://hangang.tistory.com
"자유롭게 사용하세요.
"
"===============================================================================
"==========  load example vimrc from the distribution  =========================
"===============================================================================
"
filetype plugin on
"
"===============================================================================
"==========  CUSTOMIZATION (vimrc)  ============================================
"===============================================================================
"
" Platform specific items:
" - central backup directory (has to be created)
" - default dictionary
" Uncomment your choice.  
if  has("win16") || has("win32")     || has("win64") || 
  \ has("win95") || has("win32unix")
    "
"    runtime mswin.vim
"    set backupdir =$VIM\vimfiles\backupdir
"    set dictionary=$VIM\vimfiles\wordlists/german.list
else
"    set backupdir =$HOME/.vim.backupdir
"    set dictionary=$HOME/.vim/wordlists/german.list
endif
"
" Using a backupdir under UNIX/Linux: you may want to include a line similar to
"   find  $HOME/.vim.backupdir -name "*" -type f -mtime +60 -exec rm -f {} \;
" in one of your shell startup files (e.g. $HOME/.profile)
"
"-------------------------------------------------------------------------------
" Use of dictionaries
"-------------------------------------------------------------------------------
"
set complete+=k           " scan the files given with the 'dictionary' option
"
"-------------------------------------------------------------------------------
" Various settings
"-------------------------------------------------------------------------------
"
set autoindent
set autoread				" read open files again when changed outside Vim
set autowrite				" write a modified buffer on each :next , ...
set backspace	=2
set browsedir	=current	" which directory to use for the file browser
set cindent
set hlsearch
set incsearch				" use incremental search
set keywordprg	=man
set ls			=2
set magic
set nojoinspaces
set nocompatible
set nowrapscan
set nowrap					" do not wrap lines
set number
set ruler
set shiftwidth	=4			" number of spaces to use for each step of indent
set showcmd
set showmatch
set smartindent
set tabstop		=4			" number of spaces that a <Tab> in the file counts for
set textwidth	=200
set title
set tags=./tags,tags,../tags,../../tags
"
"-------------------------------------------------------------------------------
" Syntax
"-------------------------------------------------------------------------------
"
if has("syntax")
	syntax on
endif "
"-------------------------------------------------------------------------------
" Highlighting
"-------------------------------------------------------------------------------
"
colo desert
"
"-------------------------------------------------------------------------------
" Directory Setting
"-------------------------------------------------------------------------------
"
if exists("$HOME")
	let s:home_dir = $HOME
	let s:temp = strpart(s:home_dir,strlen(s:home_dir)-1,1)
	if s:temp == "/" || s:temp == "\\"
		let s:home_dir = strpart(s:home_dir,0,strlen(s:home_dir)-1)
	endif
"
" Set path
if has("win32")
	let s:dir_tmp = s:home_dir."/_vim/tmp"
	let s:dir_backup = s:home_dir."/_vim/backup"
else
	let s:dir_tmp = s:home_dir."/.vim/tmp"
	let s:dir_backup = s:home_dir."/.vim/backup"
endif
"
set path=.,/usr/include,/usr/local/include,/usr/include/c++/4.1.3
"
" Set temp directory
	if isdirectory(s:dir_tmp)
		set swf
		let &dir = s:dir_tmp
	else
		set noswf
		set dir=.
	endif
"
" Set backup directory
	if isdirectory(s:dir_backup)
		set bk
		let &bdir = s:dir_backup
		set bex=.bak
	else
		set nobk
	endif
"
endif "exists("$HOME")

if filereadable("cscope.out")
	cs add cscope.out
elseif $CSCOPE_DB != ""
	cs add $CSCOPE_DB
endif
set cscopeverbose 

"
"-------------------------------------------------------------------------------
" Plugin Settings
" Using Plugin : Taglist, EnhancedCommentify, alternate, MiniBufExplorer
"				 ShowMarks	
"
"-------------------------------------------------------------------------------
" ::EnhancedCommentify::
" Consider Indent
let	g:EnhCommentifyRespectIndent = 'Yes'
" /*hello*/ -> /* hello */
let g:EnhCommentifyPretty = 'yes'
" /*hello*/          ->   /*hello
" /*good morning*/        good morning*/
let g:EnhCommentifyCommentsOp = 'Yes'
" /*hello          ->   /*hello
" good morning*/         *good morning*/
let g:EnhCommentifyMultiPartBlocks = 'yes'
"
" ::Taglist::
 let Tlist_Ctags_Cmd = "/usr/bin/ctags-exuberant"
 let Tlist_Auto_Open = 0
 let Tlist_Auto_Update = 1
 let Tlist_WinWidth = 27
"
" ::Minibufexpl::
"let g:miniBufExplMapWindowNavVim = 1
"let g:miniBufExplMapWindowNavArrows = 1
"let g:miniBufExplMapCTabSwitchBufs = 1
"let g:miniBufExplModSelTarget = 1 
"
" ::ShowMarks::
" Show mark only abcdefgABCDEFG
let g:showmarks_include = "abcdefgABCDEFG"
"
"-------------------------------------------------------------------------------
"  some additional hot keys
"-------------------------------------------------------------------------------
"    F2   -  write file without confirmation
"    F3   -  call file explorer Ex
"    F4   -  show tag under curser in the preview window (tagfile must exist!)
"    F5   -  open quickfix error window
"    F6   -  close quickfix error window
"    F7   -  display previous error
"    F8   -  display next error   
"  S-Tab  -  Fast switching between buffers (see below)
"    C-q  -  Leave the editor with Ctrl-q (see below)
"-------------------------------------------------------------------------------
"
map   <silent> <F2>    :write<CR>
map   <silent> <F3>    :Explore<CR>
nmap  <silent> <F4>    :exe ":ptag ".expand("<cword>")<CR>
map   <silent> <F5>    :copen<CR>
map   <silent> <F6>    :cclose<CR>
map   <silent> <F7>    :cp<CR>
map   <silent> <F8>    :cn<CR>
map   <silent> <F12>   :!ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .<CR>
"
imap  <silent> <F2>    <Esc>:write<CR>
imap  <silent> <F3>    <Esc>:Explore<CR>
imap  <silent> <F4>    <Esc>:exe ":ptag ".expand("<cword>")<CR>
imap  <silent> <F5>    <Esc>:copen<CR>
imap  <silent> <F6>    <Esc>:cclose<CR>
imap  <silent> <F7>    <Esc>:cp<CR>
imap  <silent> <F8>    <Esc>:cn<CR>
imap  <silent> <F12>   <Esc>:!ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .<CR>

nmap <C-[>s :cs find s <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>g :cs find g <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>c :cs find c <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>t :cs find t <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>e :cs find e <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>f :cs find f <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>i :cs find i <C-R>=expand("<cword>")<CR> <CR>
nmap <C-[>d :cs find d <C-R>=expand("<cword>")<CR> <CR>
"
"-------------------------------------------------------------------------------
" Key Mappings
"
"-------------------------------------------------------------------------------
" Date & Time
iab xdebug <C-R>=strftime("%Y-%m-%d %H:%M:%S SANGWON")<CR>
iab xdate <C-R>=strftime("%Y-%m-%d %H:%M:%S")<CR>
iab xtime <C-R>=strftime("%H:%M:%S")<CR>
"
" Special mapping
" ,e Excute
if has("gui_running")
	map ,e :!%<
elseif has("unix")
	map ,e :!./
endif
"
" ,d Debug with gdb
if has("gui_running")
	" no setting
elseif has("unix")
	map ,d :!gdb 
endif
"
" ,c Compile current file
if has("gui_running")
	map ,c :!gcc %<
elseif has("unix")
	map ,c :!gcc -o %< %<CR>
endif
"
" ,x Excute current file
if has("gui_running")
	map ,x :!%<
elseif has("unix")
	map ,x :!./%<<CR>
endif
"
" ,t Toggle Taglist
map ,t :TlistToggle<CR>
" ,b Toggle Minibufexpl
map ,b :TMiniBufExplorer<CR>
" ,m Toggle ShowMarks
map ,m :ShowMarksToggle<CR>
"
"-------------------------------------------------------------------------------
" Fast switching between buffers
" The current buffer will be saved before switching to the next one.
" Choose :bprevious or :bnext
"-------------------------------------------------------------------------------
"
 map  <silent> <s-tab>  <Esc>:if &modifiable && !&readonly && 
     \                  &modified <CR> :write<CR> :endif<CR>:bprevious<CR>
imap  <silent> <s-tab>  <Esc>:if &modifiable && !&readonly && 
     \                  &modified <CR> :write<CR> :endif<CR>:bprevious<CR>
"
"-------------------------------------------------------------------------------
" Leave the editor with Ctrl-q : Write all changed buffers and exit Vim
"-------------------------------------------------------------------------------
nmap  <C-q>    :wqa<CR>
"
"-------------------------------------------------------------------------------
" autocomplete parenthesis, brackets and braces
"-------------------------------------------------------------------------------
" inoremap ( ()<Left>
" inoremap [ []<Left>
" inoremap { {}<Left>

" vnoremap ( s()<Esc>P<Right>%
" vnoremap [ s[]<Esc>P<Right>%
" vnoremap { s{}<Esc>P<Right>%
"
"-------------------------------------------------------------------------------
" Change the working directory to the directory containing the current file
"-------------------------------------------------------------------------------
if has("autocmd")
  autocmd BufEnter * :lchdir %:p:h
endif " has("autocmd")
"
"-------------------------------------------------------------------------------
" Filename completion
" 
"   wildmenu : command-line completion operates in an enhanced mode
" wildignore : A file that matches with one of these
"              patterns is ignored when completing file or directory names.
"-------------------------------------------------------------------------------
" 
set wildmenu
set wildignore=*.bak,*.o,*.e,*~
"
"-------------------------------------------------------------------------------
" print options  (pc = percentage of the media size)
"-------------------------------------------------------------------------------
set printoptions=left:8pc,right:3pc
"                         
"-------------------------------------------------------------------------------
" taglist.vim : toggle the taglist window
" taglist.vim : define the title texts for make
" taglist.vim : define the title texts for qmake
"-------------------------------------------------------------------------------
 noremap <silent> <F11>  <Esc><Esc>:Tlist<CR>
inoremap <silent> <F11>  <Esc><Esc>:Tlist<CR>

let tlist_make_settings  = 'make;m:makros;t:targets'

let tlist_qmake_settings = 'qmake;t:SystemVariables'

if has("autocmd")
  " ----------  qmake : set filetype for *.pro  ----------
  autocmd BufNewFile,BufRead *.pro  set filetype=qmake
endif " has("autocmd")

" REQUIRED. This makes vim invoke Latex-Suite when you open a tex file.
filetype plugin on
" " IMPORTANT: win32 users will need to have 'shellslash' set so that latex
" " can be called correctly.
set shellslash
"
" " IMPORTANT: grep will sometimes skip displaying the file name if you
" " search in a singe file. This will confuse Latex-Suite. Set your grep
" " program to always generate a file-name.
set grepprg=grep\ -nH\ $*
"
" " OPTIONAL: This enables automatic indentation as you type.
filetype indent on
"
" " OPTIONAL: Starting with Vim 7, the filetype of empty .tex files defaults to
" " 'plaintex' instead of 'tex', which results in vim-latex not being loaded.
" " The following changes the default filetype back to 'tex':
let g:tex_flavor='latex'

function! InsertTabWrapper()
	let col = col('.') - 1
	if !col || getline('.')[col-1]!~'\k'
		return "\<TAB>"
	else
		if pumvisible()
			return "\<C-N>"
		else
			return "\<C-N>\<C-P>"
		end
	endif
endfunction

inoremap <tab> <c-r>=InsertTabWrapper()<cr>

hi Pmenu ctermbg=blue
hi PmenuSel ctermbg=yellow ctermfg=black
hi PmenuSbar ctermbg=blue

map <c-w><c-f> :FirstExplorerWindow<cr>
map <c-w><c-b> :BottomExplorerWindow<cr>
map <c-w><c-t> :WMToggle<cr> 

if has("autocmd")
 " Enabled file type detection
 " Enabled file language-dependent indenting
filetype plugin on
filetype indent on
endif " has ("autocmd")
let b:verilog_indent_modules = 1


"source ~/.vim/plugin/vdb.vim
