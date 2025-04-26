//
// Created by ethan on 23/04/2025.
//
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
#include "library.h"
#include "logger.h"
#include "helpers/luaexec.h"
#include "proxies/luaproxies.h"
#include "hooks/d3d9hooks.h"
#include "hooks/dihooks.h"
#include "hooks/luahooks.h"
#include <windows.h>
#include <psapi.h>
#include <iostream>

#include "overrides/luaoverrides.h"

MODULEINFO get_module_info(const char* module)
{
    MODULEINFO module_info = { nullptr };
    const HMODULE module_handle = GetModuleHandle(module);
    if (module_handle == nullptr)
        return module_info;

    GetModuleInformation(GetCurrentProcess(), module_handle, &module_info, sizeof(MODULEINFO));
    return module_info;
}

int hack_main() {
    // Formatted quite poorly right now, using namespaces as opposed to classes, but that could be changed.
    // Was one big file, so first step in refactoring was to split it.

    logger::setup_console();

    logger::log_dll = true;
    logger::log_lua = true; // Set false to disable lua logging, can be annoying

    lua::hooks::dump_lua = true; // Set false to disable lua dumping - outputs to ./dumps/luac/
    lua::overrides::config_menu_enable = false; // Set true to enable config menu on start - overrides save file

    MODULEINFO game_info = get_module_info("Driver.exe");
    const auto base_address = (uint32_t)game_info.lpBaseOfDll;

    // Hardcoded, can just be set on first call, game only uses one lua thread/state
    // Also, not really used at all
    lua::exec::p_game_state = *(lua_State**)(base_address + 0x122B498);
    lua::proxies::setup(base_address);

    lua::hooks::install_hooks(base_address);  // Includes execution of custom lua code and dumping if enabled
    d3d9::hooks::install_hooks(); // Draws dev menu on screen
    di::hooks::install_hooks(); // Handles showing/hiding dev menu

    while (!lua::exec::p_game_state) {
        Sleep(100);
    }
    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        lua::exec::execute_lua_buffer(input);
        input.clear();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    const DWORD ul_reason_for_call,
    LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            CreateThread(NULL,
                0,
                (LPTHREAD_START_ROUTINE)hack_main,
                NULL,
                0,
                NULL);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        default:
            break;
    }
    return TRUE;
}