[Return to README.md](README.md)

# Setup

To make this project from scratch, I clone Dear ImGui as a Git
submodule with `git submodule add`. When cloning this project on
another machine (to develop and build), I clone this project,
then do a `git submodule update --init` to download the Dear
ImGui submodule.

## GitHub via SSH not HTTPS

Use SSH to avoid constantly inputting credentials.

*Note: I set up my SSH config for a "github" host:*

```
host github
	HostName github.com
	IdentityFile ~/.ssh/my_github_key
	User git
```

So for all `git` commands that take a GitHub URL, I just give
`github:` as the prefix, rather than `git@github.com:`.

## Clone

I am starting from scratch right now. I setup and clone like
this:

```
mkdir draw-diagrams
cd draw-diagrams
git init
git branch -m main
git submodule add github:ocornut/imgui.git
git commit -m 'Add Git submodule imgui'
```

And when developing on another machine, I clone this repo, then
pull in Dear ImGui as a submodule, like this:

```
git clone github:sustainablelab/draw-diagrams
cd draw-diagrams
git submodule update --init
```

## Get started with a Makefile

I like Makefiles because there's always lots of stuff to make:
the actual executable build, little test builds, actual unit
tests, and most importantly, tags! And I don't need to memorize
what all these build targets are. At the command line, I can just
type `make <Tab><Tab>` to see all the targets. And once I start
using a target a lot, I'll turn it into a Vim shortcut.

So I start with a Makefile. First I pick which of the many Dear
ImGui examples I want to use as my project's `main.cpp`.

I want to use the SDL OS (windowing, keyboard, etc) API and the
SDL software renderer backend, so I'm working with the example in
here:

```
imgui/examples/example_sdl2_sdlrenderer
```

Normally I like to start my Makefile from scratch. But for this
project, just copy the Makefile from that example folder into my
project's root folder:

```
cp imgui/examples/example_sdl2_sdlrenderer/Makefile .
```

Now I customize the Makefile. And in the process of doing that,
I'll "discover" all the little things I need to do.

### Fix paths in Makefile

I start at the top of the Makefile (making edits to my copy now,
not the one in the `imgui` submodule).

It starts off defining some variables which are used as file
paths or in the construction of file paths by Makefile string
manipulations.

The paths assume the Makefile is sitting in that `imgui` project
folder, but now it's sitting somewhere else relative to the
`imgui` project, so I know for sure I have to change something.

The first line in the Makefile defines `EXE`. I change this name
to `main` for compatibility with my Vim shortcuts/workflow. I
copy the `example_sdl2_sdlrenderer/main.cpp` source to my `src`
folder:

```
mkdir src
cp imgui/examples/example_sdl2_sdlrenderer/main.cpp src/
```

I make a bunch of little minor changes to the first few lines of
the Dear ImGui Makefile to match the way I'm used to doing
things:

```
SRC = src/main.cpp
EXE = build/$(basename $(notdir $(SRC)))
what-EXE: ; @echo $(EXE)
IMGUI_DIR = imgui
SOURCES = $(SRC) $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
```

That `what-EXE` is a quick one-liner I use for testing Makefile variables:

```
$ make what-EXE
build/main
```

The Dear Imgui Makefile throws a lot of files into a variable
name, specifically `SOURCES` and `OBJ`. So another useful quick
one-liner is to check that all the files in the variable exist.

I want to check they exist because I just messed up all the paths
by moving this Makefile! This is just a way to check if I still
have broken paths.

It's just like the other one-liner, but I use `file` instead of
`echo`.

```
SOURCES = $(SRC) $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer.cpp
what-SOURCES: ; @file $(SOURCES)
```

The `file` command prints each file name and tells me
something about it:

```
$ make what-SOURCES
src/main.cpp:                              C source, ASCII text
imgui/imgui.cpp:                           ASCII text, with very long lines (427)
imgui/imgui_demo.cpp:                      C source, ASCII text, with very long lines (393)
imgui/imgui_draw.cpp:                      C++ source, ASCII text
imgui/imgui_tables.cpp:                    ASCII text
imgui/imgui_widgets.cpp:                   C source, ASCII text
imgui/backends/imgui_impl_sdl2.cpp:        C source, ASCII text
imgui/backends/imgui_impl_sdlrenderer.cpp: C source, ASCII text, with very long lines (366)
```

Finally, I'll do the same trick with the `OBJS` variable (this
time I just want to echo because I know these files will not
exist yet):

```make
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
what-OBJS: ; @echo $(OBJS)
```

```
$ make what-OBJS
main.o imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_impl_sdl2.o imgui_impl_sdlrenderer.o
```

That's no good, I want those in a `build` folder. Use `addprefix`
so that I add a `build/` prefix to *every* string in the `OBJS`
variable:

```
OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
```

And check it again:

```
$ make what-OBJS 
build/main.o build/imgui.o build/imgui_demo.o build/imgui_draw.o build/imgui_tables.o build/imgui_widgets.o build/imgui_impl_sdl2.o build/imgui_impl_sdlrenderer.o
```

Success.

### Recipe for build folder

I like to add a recipe to create the `build` folder
automatically. I could make it manually, but I want to ignore the
`build` folder in my `.gitignore`. Then I have to make it
manually on every development machine because cloning the repo
won't create this `build` folder. This little recipe avoids that
initial annoyance when building on a new machine:

```
build:
	@mkdir -p build
```

The `-p` flag is usually for making directories with subfolders
in one shot, but in this case I'm using it because it will
silence the "folder already exists" error (it won't overwrite
anything, it just avoids that annoying error message).

### Order-only prerequisite

To guarantee the build folder gets created, I add it as an
"order-only prerequisite" to any recipes that put things in the
build folder.

The syntax for an "order-only prerequisite" is the pipe symbol
`|`. This just separates the prerequisites into two groups:
anything *before* the `|` is fair game for the `$^` variable and
similar variables that grab all the prerequisites; anything
*after* the `|` is required to exist before running the recipe,
but is not used in variables like `$^`. I imagine this exists for
exactly this use-case: I need a folder to exist (because I want
the output to dump its files there), but I don't want `make` to
think that this folder name is a prerequisite file for compiling
or linking or whatever.

Logically, I only need the `build` prerequisite in recipes that
creates the relocatable object files. The Dear ImGui follows
Makefile and C programming conventions: the object file recipes
use the *pattern rule* syntax:

*To build target `%.o`, use source `%.cpp`.*

There are three such "pattern rule" recipes; add the `| build` to
all of them:

```
%.o:%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<
```

### Pattern rules

But now I broke the pattern rules. Remember I changed the
`$(OBJS)` (list of object files) to have prefix `build/`.

Fix the pattern-match recipes so that the pattern matching is
only done to the *file stem* (e.g., the `main` in
`build/main.o`), not the *full path* (e.g., I want `%` to be
`main` not `build/main`).

To fix the pattern rules, I explicitly include the folder prefix
for everything:

- every `%.o` with `build/`
- every `%.cpp` with `src` or whatever directory it's in

The recipes that use `.cpp` files in `imgui/` already have a
folder prefix. I just need to add the `src/` folder prefix for
any `.cpp` files in my `src` folder.

```
build/%.o:src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(IMGUI_DIR)/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(IMGUI_DIR)/backends/%.cpp | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<
```

Test any of those by getting the `$(OBJS)` file name with `make
what-OBJS` and then doing `make build/blah.o -n` to see an
example of what the pattern rule looks like for that particular
file name.

If the Makefile is correct, `make` prints the recipe (but does
not actually run the recipe). If there is *no* matching recipe,
`make` will report an error such as `make: *** No rule to make
target 'build/blah.o'. Stop.`

### Build all

That's it! The Dear ImGui Makefile is nice and straight-ahead
with all the cross-platform goodies already setup.

For my Vim shorcut/workflow, at the very top I add this:

```
default-target: all
```

This way `make` on its own runs the `all` recipe. The first
target in a Makefile is the default target, and I make this extra
explicit by naming it `default-target`.

Before doing anything else, I build to make sure this is all good
so far:

```
make
```

And run it!

```
./build/main
```

Beautiful.

### Tags

Now I put in recipes for tags. I use the compiler flag `-M` to
generate a list of dependencies and `-MF` to tell the compiler
what file to output the dependency list to. By convention, the
output file is `.d` (`d` for *dependency*). The file is just a
list of all the headers that are pulled in. Not just the ones I
explicitly have in my files, but the files those headers include,
and so on.

I make a `$(HEADER_LIST)` variable -- it's just like `$(EXE)` but
it ends in `.d`:

```make
HEADER_LIST = build/$(basename $(notdir $(SRC))).d
```

And a recipe to generate this file:

```make
.PHONY: $(HEADER_LIST)
$(HEADER_LIST): $(SRC)
	$(CXX) $(CXXFLAGS) -M $^ -MF $@
```

I wrote a `C` program that converts the list of headers to a list
that `ctags` understands. This is a fun exercise that I've done
several times and it comes out a bit different each time. The
program is `src/ctags-dlist.cpp`. I need a recipe to build that:

```make
build/ctags-dlist: src/ctags-dlist.cpp
	$(CXX) $(CXXFLAGS_BASE) $^ -o $@
```

And finally, the `tags` recipe that runs my `ctags-dlist` program
on the dependency list. I run `ctags` twice -- first on the
dependency list, then on the actual source files.

```make
.PHONY: tags
tags: $(HEADER_LIST) build/ctags-dlist
	build/ctags-dlist $(HEADER_LIST)
	ctags --c-kinds=+p+x --extra=+q -L build/headers.txt
	ctags -a --extra=+q $(SOURCES)
```

Putting that all together, there are three recipes. Make sure to
put these after `CXXFLAGS` is defined so that `CXXFLAGS` includes
the `pkg-config` flags for SDL:

```make
######
# TAGS
######
.PHONY: $(HEADER_LIST)
$(HEADER_LIST): $(SRC)
	$(CXX) $(CXXFLAGS) -M $^ -MF $@

build/ctags-dlist: src/ctags-dlist.cpp
	$(CXX) $(CXXFLAGS_BASE) $^ -o $@

.PHONY: tags
tags: $(HEADER_LIST) build/ctags-dlist
	build/ctags-dlist $(HEADER_LIST)
	ctags --c-kinds=+p+x --extra=+q -L build/headers.txt
	ctags -a --extra=+q $(SOURCES)
```

Now make the tags file:

```
make tags
```

## Add my application

I put my diagram code in namespace `Diagram` and follow
conventional `.h` and `.cpp` division of code:

- define namespace `Diagram` in `src/diagram.h`
- declare my functions in this `Diagram` namespace
- put these function declarations in `src/diagram.h`
- put the function definitions in `src/diagram.cpp` using the
  `Diagram::` namespace notation.

Here is the basic skeleton:

```c
// src/diagram.h

#pragma once

namespace Diagram
{
    void UI(void);
}
```

```c
// src/diagram.cpp

#include "imgui.h"
#include "diagram.h"

void Diagram::UI(void)
{
}
```

Open `src/main.cpp`. This is a standard immediate-mode game-type
application:

- some setup for the backend
- then the game loop starts
    - everything is recalculated and re-rendered on every
      iteration of the game loop

In the includes at the top, I add an include for my application
code:

```c
// src/main.cpp

#include "diagram.h"
```

In the Makefile, I add my application code to the list of
sources:

```
SOURCES = $(SRC) src/diagram.cpp $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
```

Find the part of the game loop where the Dear ImGui frame is
created (it's near the beginning of the game loop). Or just
search for `ImGui::ShowDemoWindow`. Instead of the ImGui demo
window, I call `Diagram::UI()`:

```c
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        Diagram::UI();              // CALL MY DIAGRAM DRAWING CODE
```

That's it for setup: the ImGui `main.cpp` now runs my UI code
(empty so far) on every frame. Now I'm ready to create my
application.


