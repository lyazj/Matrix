let s:make_args = "Equation.TEST"
let s:make_debug_args = "Equation.TEST"

func! Run_Make()
  exec "w"
  exec "!clear; cd \"%:h\" && make ".s:make_args
endfunc

func! Run_Make_Debug()
  exec "w"
  exec "!clear; cd \"%:h\" && make ".s:make_debug_args
endfunc

autocmd Filetype * imap <F5> <ESC>:call Run_Make()<CR>
autocmd Filetype * nmap <F5>      :call Run_Make()<CR>
autocmd Filetype * vmap <F5> <ESC>:call Run_Make()<CR>
autocmd Filetype * imap <C-F5> <ESC>:call Run_Make_Debug()<CR>
autocmd Filetype * nmap <C-F5>      :call Run_Make_Debug()<CR>
autocmd Filetype * vmap <C-F5> <ESC>:call Run_Make_Debug()<CR>
