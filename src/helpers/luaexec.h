//
// Created by ethan on 24/04/2025.
//
#pragma once
#ifndef LUAEXEC_H
#define LUAEXEC_H
#include <string>
extern "C" {
#include "../../lua-5.1.2/src/lstate.h"
}

namespace lua::exec {
    // Vars
    inline std::string buffer;
    inline bool execute = false;
    inline extern lua_State* p_game_state = nullptr;

    // Functions
    void execute_lua_buffer(std::string input);
    void execute_lua_file(std::string path);
    bool execute_lua(lua_State* L, const char* chunk_name = "load buffer");

    void lua_game_running_hook_bootstrap(lua_State *L);
} // lua

#endif //LUAEXEC_H
