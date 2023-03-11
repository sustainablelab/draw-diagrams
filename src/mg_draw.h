#pragma once

typedef struct
{
    ImVec2 tl;                                          // Rect TOP LEFT
    ImVec2 br;                                          // Rect BOTTOM RIGHT
    ImVec2 sz;                                          // Rect SIZE
} Rect;

/**
 * Get extents of the remaining area available for drawing.
 *
 * \returns drawing area as Rect {ImVec2 topleft, ImVec2 botright} in screen coordinates
 *
 * If I call this before I draw anything, I get back a Rect that is the window size
 * minus room taken up by stuff Dear ImGui adds for me: scrollbars on bottom and right,
 * and the tab-bar and menu-bar on top.
 *
 * If I then draw stuff (lines, shapes, text, etc.) and call this again, I get a smaller
 * Rect.
 *
 * Dear ImGui API calls:
 * - ImGui::GetContentRegionAvail()
 * - ImGui::GetCursorScreenPos()
 *
 * ImDrawList API uses screen coordinates. Screen coordinates means 0,0 is top left of
 * my whole application window (the window I requested from the OS), regardless of where
 * this particular Dear ImGui window sits within my application window.
 *
 * Note:: GetWindowSize() is NOT what I want.
 * -  Returns size of entire window including menu-bar and tab-bar
 */
static Rect get_empty_extents(void)
{
    // sz : largest, unused rectangle in the window
    ImVec2 sz = ImGui::GetContentRegionAvail();

    // tl : TOP LEFT of the window
    ImVec2 tl = ImGui::GetCursorScreenPos();

    // br : Add unused area (sz) to top left (tl)
    //      to get screen coordinates of BOTTOM RIGHT
    ImVec2 br = {tl.x + sz.x, tl.y + sz.y};
    return Rect {tl, br, sz};
}

