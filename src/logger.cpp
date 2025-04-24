//
// Created by ethan on 24/04/2025.
//
#include "logger.h"
#include <iostream>
#include <windows.h>

namespace logger {
    void setup_console() {
        AllocConsole();
        SetConsoleTitleA("Lua Console");

        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        freopen_s(&f, "CONIN$", "r", stdin);
    }

    void dll(const char* context, const std::string& message) {
        if (!log_dll)
            return;
        std::cout << "[DLL:" << context << "] " << message << std::endl;
    }

    void lua(const std::string &message) {
        if (!log_lua)
            return;
        std::cout << "[LUA] " << message << std::endl;
    }
} // logger