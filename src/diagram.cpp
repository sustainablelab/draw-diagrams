/* *************Project Setup***************
 * I use the Dear ImGui example main.cpp and treat my application like it's a library.
 *
 * This file, src/diagram.cpp, is my application.
 *
 *
 * Most everything I need happens when the main.cpp calls Diagram::UI().
 *
 * There are a few variables I make visible to the Dear ImGui main.cpp so that my
 * application (defined in Diagram::UI()) can affect the state of the program. I list
 * these variables in APPLICATION GLOBALS.
 *
 * Diagram::UI() draws "my application" as a Dear ImGui window in the OS window.
 * *******************************/
/* *************Use Dear ImGui to draw stuff***************
 * - See my notes on ImDrawList API in src/mg_draw.h
 * *******************************/
/* *************TODO***************
 * - [ ] Experiment with out-of-the-box functionality for file handling:
 *   - ImFileOpen()
 *   - ImFileClose()
 *   - ImFileGetSize()
 *   - ImFileRead()
 *   - ImFileWrite()
 *   - ImFileLoadToMemory()
 * *******************************/
#include "imgui.h"
#include "diagram.h"
#include "mg_colors.h"
#include "mg_draw.h"
constexpr bool DEBUG = true;


//////////////////////
// APPLICATION GLOBALS - declared in src/diagram.h
//////////////////////
bool Diagram::file_quit = false;

////////////////////////
// HELPERS FOR MY WINDOW
////////////////////////
/**
 * User "file" events triggered by clicking on menu.
 *
 * Move the bodies of these conditionals to functions in the Diagram namespace so that I
 * can, in the event polling in src/main.cpp, generate keyboard shortcuts as well (in
 * addition to the vanilla Ctrl+ keyboard shortcuts I define here).
 */
static void ShowMenuFile(void)
{ // Define the File menu
    if (ImGui::MenuItem("New", "Ctrl+N")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {Diagram::file_quit = true;}
}
/**
 * User "edit" events triggered by Ctrl+Z and Ctrl+Y.
 *
 * Move the bodies of these conditionals to functions in the Diagram namespace so that I
 * can, in the event polling in src/main.cpp, generate keyboard shortcuts as well (in
 * addition to the vanilla Ctrl+ keyboard shortcuts I define here).
 */
static void ShowMenuEdit(void)
{ // Define the Edit menu
    if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
    if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
}
/**
 * Make a main menu.
 *
 * Developer: pick one of these options
 * - menu-bar is for the entire application window
 *      - use ImGui::BeginMainMenuBar() and ImGui::EndMainMenuBar()
 *      - call this anywhere, doesn't matter
 * - menu-bar is only for the current window within the application
 *      - use ImGui::BeginMenuBar() and ImGui::EndMenuBar()
 *      - call this within an ImGui::Begin / ImGui::End block
 *      - pass flag ImGuiWindowFlags_MenuBar to flags arg in ImGui::Begin()
 *      - the menu bar is in the window created by that call to ImGui::Begin()
 */
static void ShowMenu(void)
{ // Organize the menus in the menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ShowMenuEdit();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

////////////////////////
// MY APPLICATION WINDOW
////////////////////////
void Diagram::UI(void)
{
    // Set up this window with a menu-bar
    bool show_this_window = true;
    ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Draw Diagrams", &show_this_window, flags);
    ShowMenu();                                         // Add menu bar to this window
    ///////
    // LOAD - latest state of diagram
    ///////
    
    /////////
    // UPDATE - use user input to modify the diagram
    /////////
    // Define an MxN grid
    int M = 20, N = 10;
    ////////////
    // TRANSFORM - map items from diagram space to window space
    ////////////
    /* Rect r = get_empty_extents(); */
    /////////
    // RENDER - list the items to draw
    /////////
    /**
     * draw : Call draw->AddBlah() to draw Blah in this window
     */
    ImDrawList* draw = ImGui::GetWindowDrawList();
    /**
     * Draw diagonal lines to show empty area for drawing in.
     */
    Rect r = get_empty_extents();
    { // show_drawing_extents
        // Show full extents left after menu-bar and tab-bar
        draw->AddLine(r.tl, r.br, Colors::lightgravel); // Full extents
        if(DEBUG)
        {
            ImGui::Text("%d:Available space to draw: %.1f x %.1f\n",__LINE__, r.sz.x, r.sz.y);
            ImGui::Text("%d:Screen coordinates top-left: (%.1f, %.1f)\n",__LINE__, r.tl.x, r.tl.y);
            ImGui::Text("%d:Screen coordinates bottom-right: (%.1f, %.1f)\n",__LINE__, r.br.x, r.br.y);
            ImGui::Text("%d:M x N grid : %d x %d\n",__LINE__, M, N);
        }
        // Show extents less the above text
        Rect r_less = get_empty_extents();
        draw->AddLine(r_less.tl, r_less.br, Colors::deepgravel);
    }
    { // Use drawing extents to draw grid lines across entire window
    }
    ImVec2 bl = {r.tl.x, r.br.y}; ImVec2 tr = {r.br.x, r.tl.y};
    // TODO: change hardcoded step to calculated step based on grid dimensions
    for(float x=r.tl.x; x<=r.br.x; x += 32.0f)
    {
        ImVec2 t = {x,r.tl.y}; ImVec2 b = {x,r.br.y};
        draw->AddLine(t, b, Colors::deepgravel);        // vertical line
    }
    draw->AddLine(r.tl, bl, Colors::lime);              // left most
    draw->AddLine(tr, r.br, Colors::lime);              // right most
    if(0)
    { // show_window_extents
        ImVec2 sz = ImGui::GetWindowSize();
        /* constexpr float STEP = 32.0f; */
        ImVec2 p0 = {0,0}; ImVec2 p1 = {sz.x, sz.y};
        draw->AddLine(p0, p1, Colors::deepgravel);
    }
    { // green box
        ImVec2 topL = {128.f,128.f}; ImVec2 botR = {384.0f,256.0f};
        draw->AddRect(topL,botR,Colors::saltwatertaffy, 10.0f, 0, 2.0f);
    }
    { // green box 2
        ImVec2 topL = {448.f,128.f}; ImVec2 botR = {704.0f,256.0f};
        draw->AddRect(topL,botR,Colors::saltwatertaffy, 10.0f, 0, 2.0f);
    }
    { // grey line
        ImVec2 p0 = {384.f,192.f}; ImVec2 p1 = {448.0f,192.0f};
        draw->AddLine(p0,p1,Colors::lightgravel, 2.0f);
    }
    { // orange box
        ImVec2 topL = {128.f,256.f}; ImVec2 botR = {384.0f,384.0f};
        draw->AddRect(topL,botR,Colors::orange, 10.0f, 0, 2.0f);
    }
    ImGui::End();
}

///////////////////////
// DEBUG MY APPLICATION by calling ImGui::Text() in this window
///////////////////////
void Diagram::Debug(void)
{
    ImGui::Begin("Debug");
    //////////
    // VERSION
    //////////
    if(1)
    { // Version, OS window size
        ImGui::Text("VERSION: Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGuiViewport* vp = ImGui::GetWindowViewport();
        // vp->Size , vp->Pos
        ImGui::Text("%d:OS window size: %.0f x %.0f",__LINE__, vp->Size.x, vp->Size.y);
    }
    if(1)
    { // window size (including menus and tabs)
        ImVec2 sz = ImGui::GetWindowSize();
        ImGui::Text("%d:THIS window size: %.0f x %.0f",__LINE__, sz.x, sz.y);
    }
    /////////////
    // WHAT IS...
    /////////////
    /* ImDrawList* draw = ImGui::GetWindowDrawList(); */
    /* ImGui::Text("%d:sizeof(ImDrawList* draw): %d bytes", __LINE__, (int)sizeof(draw)); */
    if(0)
    { // Empty (availalable) area in THIS window
        // Viewport coordinates top-left
        ImVec2 tl = ImGui::GetCursorScreenPos();      // 
        ImGui::Text("%d:Screen position top-left: (%.1f, %.1f)\n",__LINE__, tl.x, tl.y);
        ImVec2 sz = ImGui::GetContentRegionAvail();
        // Viewport coordinates bottom-right
        ImVec2 br = {tl.x + sz.x, tl.y + sz.y};
        ImGui::Text("%d:Screen position bottom-right: (%.1f, %.1f)\n",__LINE__, br.x, br.y);
        { // Did top left change? Yes
            ImVec2 tl = ImGui::GetCursorScreenPos();      // 
            ImGui::Text("%d:Screen position top-left: (%.1f, %.1f)\n",__LINE__, tl.x, tl.y);
        }
    }
    if(0)
    { // not sure what Max tells me
        ImVec2 sz = ImGui::GetWindowContentRegionMax();
        ImGui::Text("%d:window content region max: %.0f x %.0f",__LINE__, sz.x, sz.y);
    }
    if(0)
    { // not sure what Min tells me
        ImVec2 sz = ImGui::GetWindowContentRegionMin();
        ImGui::Text("%d:window content region min: %.0f x %.0f",__LINE__, sz.x, sz.y);
    }
    ImGui::End();
}
