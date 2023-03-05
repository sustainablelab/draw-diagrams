#include "imgui.h"
#include "diagram.h"

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
static void ShowMenuFile(void)
{ // Define the File menu
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {Diagram::file_quit = true;}
}
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
    ImDrawList* draw = ImGui::GetWindowDrawList();
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
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    /////////////
    // WHAT IS...
    /////////////
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImGui::Text("%d:sizeof(ImDrawList* draw): %d bytes", __LINE__, (int)sizeof(draw));
    ImGuiViewport* vp = ImGui::GetWindowViewport();
    ImGui::Text("%d:viewport top-left: (%.0f,%.0f)",__LINE__, vp->Pos.x, vp->Pos.y);
    ImGui::Text("%d:viewport size: %.0f x %.0f",__LINE__, vp->Size.x, vp->Size.y);
    ImGui::Text("%d:viewport bottom-right: (%.0f,%.0f)",__LINE__, vp->Pos.x+vp->Size.x, vp->Pos.y+vp->Size.y);
    ImGui::End();
}
