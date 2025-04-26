//
// Created by ethan on 24/04/2025.
//
#include "luaexec.h"
#include "luahelpers.h"
#include "luaoverrides.h"
#include "luahooks.h"
#include "luaproxies.h"
#include "logger.h"

namespace lua::exec {

    // Vars
    bool game_running_hook_complete = false;

    // Functions
    void execute_lua_buffer(const std::string& input) {
        // Sets buffer to custom code
        buffer = input;
        execute = true;
    }

    void execute_lua_file(const std::string& path) {
        // Sets buffer in order to open script in gamedir\custom\path
        std::string fullpath = R"(..\\..\\scripts\\)" + path;
        buffer = "debugOpen(\"" + fullpath + "\")\n";
        execute = true;
    }

    bool execute_lua(lua_State *L, const char *chunk_name) {
        if (execute) {
            execute = false;
            const int top = proxies::gamelua_gettop(L); // Save stack top for restoration

            int status = overrides::lual_loadbuffer_override(L, buffer.c_str(), buffer.size(), chunk_name);
            if (!helpers::check_lua_status(L, status, chunk_name)) {
                proxies::gamelua_settop(L, top); // Restore stack on load error
                buffer = ""; // Clear buffer
                return false;
            }

            status = hooks::o_lua_pcall.call<int>(L, 0, 0, 0); // 0 args, 0 results, no error func
            if (!helpers::check_lua_status(L, status, chunk_name)) {
                proxies::gamelua_settop(L, top);
                buffer = "";
                return false;
            }

            proxies::gamelua_settop(L, top);
            buffer = "";
            return true;
        }
        return false;
    }

    void lua_game_running_hook_bootstrap(lua_State *L) {
        if (!game_running_hook_complete) {
            proxies::gamelua_getfield(L, LUA_GLOBALSINDEX, "GameLauncher"); // Stack: [..., GameLauncher?]
            if (proxies::gamelua_type(L, -1) == LUA_TTABLE) {
                proxies::gamelua_getfield(L, -1, "SetGameState");           // Stack: [..., GameLauncher, SetGameState?]
                if (proxies::gamelua_type(L, -1) == LUA_TFUNCTION) {
                    // Safely get the 'GameState' upvalue string
                    std::string currentGameState = helpers::get_upvalue_string(L, -1, "GameState");

                    if (currentGameState == "Game Running") {
                        logger::dll("pcall", "GameState is 'Game Running'. Loading initial custom lua...");

                        execute_lua_file("load.lua"); // Sets global buffer, sets execute=true
                        if (execute_lua(L, "load.lua")) // Executes buffer
                            game_running_hook_complete = true;
                    }
                } else {
                    logger::dll("pcall", "Skipping initial file load: GameLauncher.SetGameState not found or not a function.");
                }
                proxies::gamelua_pop(L, 1);
            }
            proxies::gamelua_pop(L, 1);
        }
    }
} // lua