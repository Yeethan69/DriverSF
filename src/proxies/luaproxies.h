//
// Created by ethan on 23/04/2025.
//
#pragma once
#ifndef LUAPROXIES_H
#define LUAPROXIES_H
#include <cstdint>
extern "C" {
#include "lstate.h"
}

namespace lua::proxies {
    // Typedefs
    typedef void(__cdecl* t_gamelua_pushvalue)(lua_State*, int);
    typedef void(__cdecl* t_gamelua_pushcclosure)(lua_State*, lua_CFunction, int );
    typedef void(__cdecl* t_gamelua_setfield)(lua_State*, int, const char*);
    typedef void(__cdecl* tgamelua_getfield)(lua_State*, int, const char*);
    typedef int(__cdecl* t_gamelua_type)(lua_State*, int);
    typedef const char*(__cdecl* t_gamelua_tolstring)(lua_State*, int, size_t*);
    typedef int(__cdecl* t_gamelua_gettop)(lua_State*);
    typedef void(__cdecl* t_gamelua_settop)(lua_State* , int);
    typedef void*(__cdecl* t_gamelua_touserdata)(lua_State*, int);
    typedef TValue*(__cdecl* t_gamelua_index2adr)(lua_State*, int);
    typedef int(__cdecl* t_gamelua_tobooelan)(lua_State*, int);
    typedef const char*(__cdecl* t_gamelua_getupvalue)(lua_State*, int, int);
    #define gamelua_pop(L,n)		gamelua_settop(L, -(n)-1)

    // Global functions
    inline t_gamelua_pushvalue gamelua_pushvalue;
    inline t_gamelua_pushcclosure gamelua_pushcclosure;
    inline t_gamelua_setfield gamelua_setfield;
    inline tgamelua_getfield gamelua_getfield;
    inline t_gamelua_type gamelua_type;
    inline t_gamelua_tolstring gamelua_tolstring;
    inline t_gamelua_gettop gamelua_gettop;
    inline t_gamelua_settop gamelua_settop;
    inline t_gamelua_touserdata gamelua_touserdata;
    inline t_gamelua_index2adr gamelua_index2adr;
    inline t_gamelua_tobooelan gamelua_tobooelan;
    inline t_gamelua_getupvalue gamelua_getupvalue;

    // Setup
    void setup(uint32_t base_address);
}

#endif //LUAPROXIES_H
