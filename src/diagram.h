#pragma once

namespace Diagram
{
    //////////////////////
    // APPLICATION GLOBALS - defined in src/diagram.cpp
    //////////////////////
    extern bool file_quit;                              // Diagram::file_quit

    ///////////////////////
    // API for src/main.cpp
    ///////////////////////
    void UI(void);                                      // Diagram::UI()
    void Debug(void);                                   // Diagram::Debug()
}
