//
// Created by ethan on 23/04/2025.
//
#include "luaproxies.h"

namespace lua::proxies {

    // Vars
    inline constexpr uint32_t pushvalue_offset = 0x1E93E0;
    inline constexpr uint32_t pushcclosure_offset = 0x201540;
    inline constexpr uint32_t setfield_offset = 0x1EF6D0;
    inline constexpr uint32_t getfield_offset = 0x1EF640;
    inline constexpr uint32_t type_offset = 0x1E9410;
    inline constexpr uint32_t tolstring_offset = 0x1F2D20;
    inline constexpr uint32_t gettop_offset = 0x1E9300;
    inline constexpr uint32_t settop_offset = 0x1E9310;
    inline constexpr uint32_t touserdata_offset = 0x1E9540;
    inline constexpr uint32_t index2adr_offset = 0x1E91C0;
    inline constexpr uint32_t toboolean_offset = 0x1E94E0;
    inline constexpr uint32_t getupvalue_offset = 0x01E9850;

    // Functions
    void setup(uint32_t base_address) {
        gamelua_pushvalue = (t_gamelua_pushvalue)(base_address + pushvalue_offset);
        gamelua_pushcclosure = (t_gamelua_pushcclosure)(base_address + pushcclosure_offset);
        gamelua_setfield = (t_gamelua_setfield)(base_address + setfield_offset);
        gamelua_getfield = (tgamelua_getfield)(base_address + getfield_offset);
        gamelua_type = (t_gamelua_type)(base_address + type_offset);
        gamelua_tolstring = (t_gamelua_tolstring)(base_address + tolstring_offset);
        gamelua_gettop = (t_gamelua_gettop)(base_address + gettop_offset);
        gamelua_settop = (t_gamelua_settop)(base_address + settop_offset);
        gamelua_touserdata = (t_gamelua_touserdata)(base_address + touserdata_offset);
        gamelua_index2adr = (t_gamelua_index2adr)(base_address + index2adr_offset);
        gamelua_tobooelan = (t_gamelua_tobooelan)(base_address + toboolean_offset);
        gamelua_getupvalue = (t_gamelua_getupvalue)(base_address + getupvalue_offset);
    }
} // lua