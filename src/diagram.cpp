#include "imgui.h"
#include "diagram.h"

void Diagram::UI(void)
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImGui::Begin("Draw Diagrams");
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

    ImGui::End();
}
