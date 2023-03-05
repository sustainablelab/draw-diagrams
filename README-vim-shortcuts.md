# Vim shortcuts

Set up Vim shortcuts to make it as easy as possible to rebuild
and run:

- `;<Space>` to build:
    - call `make`
    - if there are build warnings/errors, open the Vim quickfix window
    - if there are no build warnings/errors, run the program
- `;r<Space>` to run the program
    - call `build/main` (Linux) or `build/main.exe` (Windows)

## Build

If the build goes fine, just print a little message at the Vim
command line and run the program.

But if the build fails, I want Vim to open the *quickfix* window.

The quickfix window lists all the build errors. There is one line
per build output error (and compiler warning count as build
errors in this context).

Vim commands `:cnext` and `:cprevious` cycle through the errors
and jump the cursor to the exact location of the error. If the
error is something like a missing semicolon, Vim opens the file
and puts my cursor right where I need it.

For linker errors these commands just open an empty buffer (in a
failed attempt to jump me to the problem). Linker errors do not
identify problems in source code; linker errors only identify
link-time problems, like multiple definitions of the same symbol.

I map commands `:cnext` and `:cprevious` to `Alt+Right` and `Alt+Left`.

Vim automatically puts build output in the quickfix window. The
trick is: how do I test for build errors (since I only want the
"noise" of opening another window if there is some problem I need
to address).

These few lines of Vimscript test for build errors:

```vim
let l:all_the_valids = []
for qfdict in getqflist()
    call add(l:all_the_valids, qfdict['valid'])
endfor
```

And to put that in an actual Vimscript function:

```vim
nnoremap <leader><Space> :call Build()<CR><CR><CR>
function Build()
    if (&filetype == 'c') || (&filetype == 'cpp')
        exec "make"
        let l:build_failed = CheckIfBuildFailed()
        if l:build_failed
            copen
            execute 'normal ;hh'
        else
            call RunMain(CheckOs())
        endif
    else
        echomsg "This file is not C or C++"
    endif
endfunction
function! CheckIfBuildFailed()
    let l:all_the_valids = []
    for qfdict in getqflist()
        call add(l:all_the_valids, qfdict['valid'])
    endfor
    return str2nr(join(l:all_the_valids,''))==0 ? 0 : 1
endfunction
```

How that works:

- use Vim `getqflist()` to get a list of Vim dictionaries, one
  dictionary for each line of build output:
    - build output "Error" lines are marked as {...,'valid': 1,...}
    - all other build output lines are marked  {...,'valid': 0,...}
- for each dictionary, pull out the value of key `valid` (either
  0 or 1) into a list of all "valids"
- look at the join (the boolean OR) of that list:
    - if any value is 1, the join is 1, indicating an error: open
      the quickfix window
    - if all 0, there is no error: do *not* open the quickfix
      window

## Run

I go between Linux and Windows a lot. Here's my shortcut to run
the program whether I'm on Linux or Windows:

```vim
function! CheckOs()
    if has("X11")
        return 'linux'
    else
        return 'windows'
    endif
endfunction

nnoremap <leader>r<Space> :call RunMain(CheckOs())<CR>

function! RunMain(os)
    if a:os == 'linux'
        echomsg s:happy_kitty "Running build/main ("..a:os..")"
        exe "!./build/main"
    endif
    if a:os == 'windows'
        echomsg s:trippy_kitty "Running build/main.exe ("..a:os..")"
        exe "!./build/main.exe"
    endif
endfunction
```



