/* *************TODO***************
 * - [ ] What out-of-the-box functionality is in the ImDrawList API?
 * - [ ] What out-of-the-box functionality is there for file handling?
 *   - ImFileOpen()
 *   - ImFileClose()
 *   - ImFileGetSize()
 *   - ImFileRead()
 *   - ImFileWrite()
 *   - ImFileLoadToMemory()
 * *******************************/
#include "imgui.h"
#include "diagram.h"
constexpr bool DEBUG = true;

typedef struct
{
    ImVec2 tl;
    ImVec2 br;
    ImVec2 sz;
} Rect;

/**
 * Get extents of the area I can draw in.
 *
 * \returns drawing area as Rect {ImVec2 topleft, ImVec2 botright} in screen coordinates
 *
 * This is the area left for drawing in. So if I call this before and after drawing stuff or
 * writing text in the window, I get different return values.
 *
 * Details using Dear ImGui API:
 *
 * ImDrawList API uses screen coordinates. Screen coordinates means 0,0 is top left of my
 * whole application window (the window I requested from the OS), regardless of where my
 * Dear ImGui window within my application window.
 * 
 * To get the remaining area available in the window:
 *
 * GetContentRegionAvail()
 * - Returns the ImVec2 size of the largest, unused rectangle in the window.
 *
 * GetCursorScreenPos()
 * - Returns the top left of the window.
 *
 * Add usable area to top left to get screen coordinates of bottom right.
 *
 * GetWindowSize()
 * - (Not what I want) Returns size of entire window including menu-bar and tab-bar
 */
Rect get_empty_extents(void)
{
    ImVec2 sz = ImGui::GetContentRegionAvail();
    ImVec2 tl = ImGui::GetCursorScreenPos();
    ImVec2 br = {tl.x + sz.x, tl.y + sz.y};
    return Rect {tl, br, sz};
}
///////////////////////
// BADWOLF COLOR SCHEME as a Dear ImGui datatype
///////////////////////
namespace Colors
{
    /* *************Colors from Steve Losh badwolf.vim***************
     *      _               _                 _  __
     *     | |__   __ _  __| | __      _____ | |/ _|
     *     | '_ \ / _` |/ _` | \ \ /\ / / _ \| | |_
     *     | |_) | (_| | (_| |  \ V  V / (_) | |  _|
     *     |_.__/ \__,_|\__,_|   \_/\_/ \___/|_|_|
     * 
     *      I am the Bad Wolf. I create myself.
     *       I take the words. I scatter them in time and space.
     *        A message to lead myself here.
     * 
     * A Vim colorscheme pieced together by Steve Losh.
     * Available at http://stevelosh.com/projects/badwolf/
     * *******************************/

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

//////////////////
// DIAGRAM GLOBALS - declared in src/diagram.h
//////////////////
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
    // Define a 10x10 grid
    /* int */ 
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
        }
        // Show extents less the above text
        Rect r_less = get_empty_extents();
        draw->AddLine(r_less.tl, r_less.br, Colors::deepgravel);
    }
    { // Use drawing extents to draw grid lines across entire window
    }
    ImVec2 bl = {r.tl.x, r.br.y}; ImVec2 tr = {r.br.x, r.tl.y};
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
    if(0)
    { // Available space to draw
        ImVec2 sz = ImGui::GetContentRegionAvail();
        ImGui::Text("%d:Available space to draw: %.0f x %.0f",__LINE__, sz.x, sz.y);
    }
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGuiViewport* vp = ImGui::GetWindowViewport();
    ImGui::Text("ImGui::GetWindowViewport()->Size is size of WHOLE application window");
    ImGui::Text("%d:Application window size: %.0f x %.0f",__LINE__, vp->Size.x, vp->Size.y);
    /////////////
    // WHAT IS...
    /////////////
    /* ImDrawList* draw = ImGui::GetWindowDrawList(); */
    /* ImGui::Text("%d:sizeof(ImDrawList* draw): %d bytes", __LINE__, (int)sizeof(draw)); */
    if(0)
    { // Size and screen coordinates of whole application window
        ImGuiViewport* vp = ImGui::GetWindowViewport();
        ImGui::Text("ImGui::GetWindowViewport()->Size is x,y size of whole application window");
        // (0,0)
        ImGui::Text("%d:viewport top-left: (%.0f,%.0f)",__LINE__, vp->Pos.x, vp->Pos.y);
        ImGui::Text("%d:viewport size: %.0f x %.0f",__LINE__, vp->Size.x, vp->Size.y);
        // (0 + Size.x, 0 + Size.y)
        ImGui::Text("%d:viewport bottom-right: (%.0f,%.0f)",__LINE__, vp->Pos.x+vp->Size.x, vp->Pos.y+vp->Size.y);
    }
    if(1)
    { // window size (including menus and tabs)
        ImGui::Text("ImGui::GetWindowSize() returns size of THIS window");
        ImVec2 sz = ImGui::GetWindowSize();
        ImGui::Text("%d:window size: %.0f x %.0f",__LINE__, sz.x, sz.y);
    }
    if(0)
    { // Get empty extents of THIS window: ImDrawList API uses screen coordinates!
        // Viewport coordinates top-left
        ImVec2 tl = ImGui::GetCursorScreenPos();      // 
        ImGui::Text("%d:Screen position top-left: (%.1f, %.1f)\n",__LINE__, tl.x, tl.y);
        ImVec2 sz = ImGui::GetContentRegionAvail();
        // Viewport coordinates bottom-right
        ImVec2 br = {tl.x + sz.x, tl.y + sz.y};
        ImGui::Text("%d:Screen position bottom-right: (%.1f, %.1f)\n",__LINE__, br.x, br.y);
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
