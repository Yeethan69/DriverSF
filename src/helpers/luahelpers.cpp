//
// Created by ethan on 24/04/2025.
//
#include "luahelpers.h"
#include "../proxies/luaproxies.h"
#include "../logger.h"

namespace lua::helpers {
    int absindex(lua_State *L, int index) {
        // Converts relative index to absolute index
        if (index < 0 && index > LUA_REGISTRYINDEX)
            index += proxies::gamelua_gettop(L) + 1;
        return index;
    }

    bool check_lua_status(lua_State *L, int status, const char *contextInfo) {
        if (status != 0) {
            // If there's an error, log it
            const char* errorMsg = proxies::gamelua_tolstring(L, -1, NULL);
            std::string message("Lua Error [");
            message += contextInfo;
            message += "]: ";
            message += errorMsg ? errorMsg : "Unknown";
            logger::dll("LuaStatus", message);
            proxies::gamelua_pop(L, 1); // Pop error message
            return false;
        }
        return true;
    }

    void set_cfunction_in_table(lua_State *L, int tableIndex, const char *fieldName, lua_CFunction func) {
        // pushes a cfunction into a table
        int index = absindex(L, tableIndex);                //                | index of table
        proxies::gamelua_pushcclosure(L, func, 0);          // Stack: [..., table, ..., c_function]
        proxies::gamelua_setfield(L, index, fieldName);     // Stack: [..., table, ...] (pops function)
    }

    std::string get_upvalue_string(lua_State *L, int funcIndex, const char *upvalueName) {
        // Upvalues are local variables that a given function accesses
        std::string result = "";
        int index = absindex(L, funcIndex);

        // Needs to be a function
        if (proxies::gamelua_type(L, index) != LUA_TFUNCTION) {
            return result;
        }

        int i = 1;
        const char* currentName = nullptr;
        int originalTop = proxies::gamelua_gettop(L);

        while ((currentName = proxies::gamelua_getupvalue(L, index, i)) != nullptr) {
            if (strcmp(currentName, upvalueName) == 0) {
                // Upvalue is found by name
                if (proxies::gamelua_type(L, -1) == LUA_TSTRING) {
                    // Gets the string value of the upvalue if it is a string or number
                    const char* str = proxies::gamelua_tolstring(L, -1, NULL);
                    result = str ? str : "";
                }
                break;
            }
            proxies::gamelua_pop(L, 1);
            i++;
        }

        proxies::gamelua_settop(L, originalTop);
        return result;
    }
} // lua