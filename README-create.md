[Return to README.md](README.md)

# Create my application

## Vim shortcuts

Set up Vim shortcuts to make it as easy as possible to rebuild
and run:

- `;<Space>` to build:
    - call `make`
    - open Vim quickfix window if there are errors
- `;r<Space>` to run the build
    - call `build/main`

A few lines of Vimscript can test for build errors:

```vim
nnoremap <leader><Space> :call Build()<CR><CR><CR>
function Build()
    if (&filetype == 'c') || (&filetype == 'cpp')
        exec "make"
        let l:build_failed = CheckIfBuildFailed()
        if l:build_failed | copen | endif
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

And here's my shortcut to run the program whether I'm on Linux or
Windows:

```vim
function! CheckOs()
    if has("X11")
        return 'linux'
    else
        return 'windows'
    endif
endfunction

if CheckOs() == 'linux'
    nnoremap <leader>r<Space> :!./build/main<CR>
endif
if CheckOs() == 'windows'
    nnoremap <leader>r<Space> :!./build/main.exe<CR>
endif
```

## Docking

I want to use Docking. Checkout the `docking` branch:

```
git checkout -b docking origin/docking
```

In the part of the `src/main.cpp` that is setup, there is a
section `// Setup Dear ImGui context` with examples setting
`io.ConfigFlags` for enabling keyboard and gamepad.

Add this line:

```c
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
```

Build and run.

Now try dragging one window inside of another. Docking icons show
up (drop on the icon to dock to that part of the window). If the
docking "covers" the window, it becomes a tab, if it just takes
over one part, now it is a split pane with adjustable dividers.
Very nice!

There is one more feature that makes docking even nicer. This is
`Dockspace`. This does a lot, but I just use it in the simplest
way possible: turn the whole application window into a "dockable"
space. This instantly turns the application from a random
assortment of windows to something that looks like a finished
application. I just add this one line to the top of my
application code:

```c
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
```

Like this:

```c
void Diagram::UI(void)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImGui::Begin("Draw Diagrams");
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    ImGui::End();
}
```

Run again -- now I can dock the windows to the application window
itself. Play with this for a second using a few windows, and the
application suddenly looks like a "normal" GUI application.

## Drawing

In the *Dear ImGui Demo* window, click on *Examples*, then
*Custom Rendering*. Then click on the *Canvas* tab. There's a
drawing application right there.
