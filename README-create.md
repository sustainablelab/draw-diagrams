[Return to README.md](README.md)

# Create my application

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
