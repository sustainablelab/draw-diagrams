[Return to README.md](README.md)

# Create my application

## Title

In the SDL setup code in `src/main.cpp`, set the title of the
Window. This is the first argument in the call
`SDL_CreateWindow()`:

```c
    SDL_Window* window = SDL_CreateWindow("Draw Diagrams", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
```

## Docking

I want to use Docking. Combined with the `imgui.ini` (saves
whatever stuff the user does, like window positions) the docking
feature is a real easy way to mouse drag-and-drop the GUI layout
and just be done with that part of "creating" the GUI.

Dear ImGui `master` branch does not include `docking` yet, but I
tried `docking` and it works, so I use it.

Checkout the `docking` branch:

```
git checkout -b docking origin/docking
```

That command clones the remote branch, starts tracking it, and
switches me over to it.

In the part of the `src/main.cpp` that is setup, there is a
section `// Setup Dear ImGui context` with examples setting
`io.ConfigFlags` for enabling keyboard and gamepad. Docking is
another one of these `io.ConfigFlags`.

Add this line:

```c
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
```

Build and run.

Now try dragging one window inside of another. Docking icons show
up (drop on the icon to dock to that part of the window). If the
docking "covers" the window, it becomes a tab, if it just takes
over one part, now it is a split pane with adjustable dividers.
Very nice! And the `imgui.ini` remembers whatever configuration I
last used.

There is one more feature that makes docking even nicer. This is
`Dockspace`. This does a lot, but I just use it in the simplest
way possible: turn the whole application window into a "dockable"
space. This instantly turns the application from a random
assortment of windows to something that looks like a finished
application. I just add this one line:

```c
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
```

Add it near the top of the main loop, something like this:

```c
    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // ...
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ////////////
        // DOCKSPACE
        ////////////
        // Dock all windows in the main application window
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
```

Note: only windows created *after* I call
`DockSpaceOverViewport()` are dockable. In thise case, all of the
windows are created later, so everything is dockable.

Run again -- now I can dock the windows to the application window
itself. Play with this for a second using a few windows, and the
application suddenly looks like a "normal" GUI application.

## Debug window

Debug prints are handy. Since there's a GUI, instead of doing
`printf()`, make a window dedicated to this stuff, dock it to the
bottom of the screen or wherever, and use `ImGui::Text()` in
place of `printf()`. Note this is not a log. The "immediate mode"
nature of things means this is actually updating every frame. So,
for example, the code below displays the size of the main
application window. When I resize the main application window by
dragging it with my mouse or whatever, I'll see the printed size
value update in my debug window.

```c
///////////////////////
// DEBUG MY APPLICATION by calling ImGui::Text() in this window
///////////////////////
void Diagram::Debug(void)
{
    ImGui::Begin("Debug");
    //////////
    // VERSION
    //////////
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    /////////////
    // WHAT IS...
    /////////////
    ImGuiViewport* vp = ImGui::GetWindowViewport();
    ImGui::Text("%d:viewport top-left: (%.0f,%.0f)",__LINE__, vp->Pos.x, vp->Pos.y);
    ImGui::Text("%d:viewport size: %.0f x %.0f",__LINE__, vp->Size.x, vp->Size.y);
    ImGui::Text("%d:viewport bottom-right: (%.0f,%.0f)",__LINE__, vp->Pos.x+vp->Size.x, vp->Pos.y+vp->Size.y);
    ImGui::End();
}
```

## Actual window

Start with an empty window. All the application will live here.

```c
void Diagram::UI(void)
{
    ImGui::Begin("Draw Diagrams");
    ImGui::End();
}
```

## Draw windows in main

I need to add my windows to `src/main.cpp`.

I have two windows I am creating, plus there are the two
example/demo windows Dear ImGui makes for me.

My two windows are:

- my actual application
- a window for debug prints

I add the calls to draw my windows *after* the Dear ImGui
example/demo windows (eventually I'll erase those).

```c
// src/main.cpp

        /////////////
        // MY WINDOWS -- put mine last to be default active tab (if tabbed)
        /////////////
        Diagram::UI();
        Diagram::Debug();
```

## Menus

Back to `src/diagram.cpp`. I probably want some standard file
menu in my application. There are two ways to add a menu-bar: I
can do it for the entire application or just on a
window-by-window basis, or both.

- a menu-bar for the entire application window:
    - use `ImGui::BeginMainMenuBar()` and `ImGui::EndMainMenuBar()`
    - call this anywhere, doesn't matter
- a menu-bar only for this window within the application:
    - use `ImGui::BeginMenuBar()` and `ImGui::EndMenuBar()`
    - call this within an `ImGui::Begin()` / `ImGui::End()` block
    - pass flag `ImGuiWindowFlags_MenuBar` to `flags` arg in `ImGui::Begin()`
    - the menu bar is in the window created by that call to `ImGui::Begin()`

For now, I add a menu bar to *my* application window (not to the
entire application):

```c
void Diagram::UI(void)
{
    // Set up this window with a menu-bar
    bool show_this_window = true;
    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Draw Diagrams", &show_this_window, flags);
    ShowMenu();                                         // Add menu bar to this window
    ImGui::End();
}
```

`ShowMenu()` is a function I define that is visible only within
this translation unit. I'm not going to repeat the entire code
here, just a few examples to show the idea.

```c
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
    ...
```

So, I just use the Dear ImGui API to organize the menus (File,
Edit, etc.) in the menu-bar.

Then, for each menu, I write a dedicated function that populates
the items in that menu (like File has a New, Save,
Quit, etc.). For example:

```c
static void ShowMenuFile(void)
{ // Define the File menu
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}
```

Each menu item is a conditional -- if the item is clicked,
execute whatever is in the body of that conditional.

So, to make the `Quit` actually quit the application:

```c
    if (ImGui::MenuItem("Quit", "Alt+F4")) {Diagram::file_quit = true;}
```

Of course `Diagram::file_quit` has to exist. Define this in
`src/diagram.cpp`:

```c
// src/diagram.cpp
bool Diagram::file_quit = false;
```

And declare it in `src/diagram.h` so that I can access it from
`src/main.cpp`:

```c
// src/diagram.h
namespace Diagram
{
    extern bool file_quit;
    void UI(void);
    void Debug(void);
}
```

And in `src/main.cpp`, the main loop exits if `done` is `true`,
so after the call to make my two windows, I use a logical `OR` to
update global `done` with global `Diagram::file_quit` without
trampling over other quit mechanisms that might have set `done`
to `true` earlier in the main loop:

```c
// src/main.cpp

    // Main loop
    bool done = false;
    while (!done)
    {
        ...
        Diagram::UI();
        Diagram::Debug();
        done = done or Diagram::file_quit;
        ...
```

Yes, C++ recognizes `or` -- it is an alternative to writing `||`
and the behavior is identical. Same goes for `and` as an
alternative to `&&`.

## Colors

Back to `src/diagram.cpp`.

My default go-to color scheme is the badwolf color scheme by
Steve Losh.

Here it is as a `Colors` namespace, using the Dear ImGui datatype
`ImColor`:

```c
namespace Colors
{
    // Greys
    ImColor plain          = {0xf8,0xf6,0xf2,0xff};
    ImColor snow           = {0xff,0xff,0xff,0xff};
    ImColor coal           = {0x00,0x00,0x00,0xff};
    ImColor brightgravel   = {0xd9,0xce,0xc3,0xff};
    ImColor lightgravel    = {0x99,0x8f,0x84,0xff};
    ImColor gravel         = {0x85,0x7f,0x78,0xff};
    ImColor mediumgravel   = {0x66,0x64,0x62,0xff};
    ImColor deepgravel     = {0x45,0x41,0x3b,0xff};
    ImColor deepergravel   = {0x35,0x32,0x2d,0xff};
    ImColor darkgravel     = {0x24,0x23,0x21,0xff};
    ImColor blackgravel    = {0x1c,0x1b,0x1a,0xff};
    ImColor blackestgravel = {0x14,0x14,0x13,0xff};

    // Colors
    ImColor dalespale      = {0xfa,0xde,0x3e,0xff};
    ImColor dirtyblonde    = {0xf4,0xcf,0x86,0xff};
    ImColor taffy          = {0xff,0x2c,0x4b,0xff};
    ImColor saltwatertaffy = {0x8c,0xff,0xba,0xff};
    ImColor tardis         = {0x0a,0x9d,0xff,0xff};
    ImColor orange         = {0xff,0xa7,0x24,0xff};
    ImColor lime           = {0xae,0xee,0x00,0xff};
    ImColor dress          = {0xff,0x9e,0xb8,0xff};
    ImColor toffee         = {0xb8,0x88,0x53,0xff};
    ImColor coffee         = {0xc7,0x91,0x5b,0xff};
    ImColor darkroast      = {0x88,0x63,0x3f,0xff};
}
```

## Drawing

In the *Dear ImGui Demo* window, click on *Examples*, then
*Custom Rendering*. Then click on the *Canvas* tab. There's a
drawing application right there.

A quick way to start drawing stuff is to get a pointer, `draw`,
to the `ImDrawList` for my `Diagram::UI` window and then call any
of the `ImDrawList` methods in `imgui/imgui_draw.cpp` as
`draw->AddBlah()`:

```c
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 p0 = {0.f,0.f}; ImVec2 p1 = {64.0f,128.0f};
    draw->AddLine(p0,p1,Colors::lightgravel);
```

And like everything else, that goes in my `Diagram::UI()`
function, sandwiched between the `ImGui::Begin()` and
`ImGui::End()` calls.


