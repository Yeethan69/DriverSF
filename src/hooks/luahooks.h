//
// Created by ethan on 23/04/2025.
//
#pragma once
#ifndef LUA_H
#define LUA_H
#include "../../safetyhook/include/safetyhook.hpp"
namespace lua::hooks {

    // Vars
    inline bool dump_lua = false;

    // Hook objects
    inline SafetyHookInline o_lua_pcall{};
    inline SafetyHookInline o_luad_protectedparser{};

    // Install and uninstall hooks
    void install_hooks(const uint32_t base_address);
    void uninstall_hooks();
} // lua

#endif //LUA_H
