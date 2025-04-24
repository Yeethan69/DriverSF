//
// Created by ethan on 24/04/2025.
//
#pragma once
#ifndef LUAHELPERS_H
#define LUAHELPERS_H
#include <string>
extern "C" {
#include "../../lua-5.1.2/src/lstate.h"
}

namespace lua::helpers {
    bool check_lua_status(lua_State* L, int status, const char* contextInfo);
    void set_cfunction_in_table(lua_State* L, int tableIndex, const char* fieldName, lua_CFunction func);
    std::string get_upvalue_string(lua_State* L, int funcIndex, const char* upvalueName);
} // lua

#endif //LUAHELPERS_H
