//
// Created by ethan on 24/04/2025.
//
#pragma once
#ifndef LUAOVERRIDES_H
#define LUAOVERRIDES_H
#include <map>
#include <shared_mutex>
extern "C" {
#include "../../lua-5.1.2/src/lstate.h"
}

namespace lua::overrides {

    // Structs
    struct vec4 {
        uint64_t unknown;
        float x, y, z, w;
    };

    struct vec2 {
        uint64_t unknown;
        float x, y;
    };

    struct dev_text {
        std::string text;
        vec2 position{};
        vec4 colour{};
        float scale{};
    };

    typedef struct load_s {
        const char *s;
        size_t size;
    } load_s;

    // Vars
    inline std::map<int, dev_text> dev_texts{};
    inline std::shared_mutex dev_text_mutex{};
    inline bool config_menu_enable = true;

    // Override functions
    int lual_loadbuffer_override(lua_State* L, const char* buff, size_t size, const char* name);

    // Overrides
    void override_print(lua_State* L);
    void override_development(lua_State* L);
    void override_config_menu(lua_State* L);
} // lua

#endif //LUAOVERRIDES_H
