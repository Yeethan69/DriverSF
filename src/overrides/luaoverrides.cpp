//
// Created by ethan on 24/04/2025.
//
#include "luaoverrides.h"
#include "../logger.h"
#include "../helpers/luaexec.h"
#include "../helpers/luahelpers.h"
#include "../proxies/luaproxies.h"
#include "../hooks/luahooks.h"

namespace lua::overrides {

    // Vars
    bool print_overrridden = false;
    bool development_overridden = false;

    // Functions
    void luaz_init_override(lua_State *L, ZIO *z, lua_Reader reader, void *data) {
        z->L = L;
        z->reader = reader;
        z->data = data;
        z->n = 0;
        z->p = nullptr;
    }

    const char* get_s_override(lua_State *L, void *ud, size_t *size) {
        // Reimplementation of gets to use our loadS
        load_s* ls = (load_s*)ud;
        if (ls->size == 0)
            return nullptr;
        *size = ls->size;
        ls->size = 0;
        return ls->s;
    }

    int lua_load_override(lua_State *L, lua_Reader reader, void *data, const char *chunk_name) {
        ZIO z;
        lua_lock(L);
        if (!chunk_name) chunk_name = "?";
        luaz_init_override(L, &z, reader, data);
        const int status = hooks::o_luad_protectedparser.call<int>(L, &z, chunk_name);
        lua_unlock(L);
        return status;
    }

    int lual_loadbuffer_override(lua_State *L, const char *buff, size_t size, const char *name) {
        load_s ls;
        ls.s = buff;
        ls.size = size;
        return lua_load_override(L, get_s_override, &ls, name);
    }

    int luab_print_override (lua_State* L) {
        //from lbaselib.c
        const int n = proxies::gamelua_gettop(L);  /* number of arguments */
        proxies::gamelua_getfield(L, LUA_GLOBALSINDEX, "tostring");
        std::string result;
        for (int i = 1; i<=n; i++) {
            proxies::gamelua_pushvalue(L, -1);  /* function to be called */
            proxies::gamelua_pushvalue(L, i);   /* value to print */
            hooks::o_lua_pcall.call<int>(L, 1, 1, 0);
            const char* s = proxies::gamelua_tolstring(L, -1, NULL);  /* get result */
            if (s != 0) result += s;
            if (i>1) result += '\t';
            proxies::gamelua_pop(L, 1);  /* pop result */
        }

        if (!result.empty()) {
            logger::lua(result);
            // if (writeToFile) {
            //     printFileStream << result << std::endl;
            // }
        }
        return 0;
    }

    int development_start_batch(lua_State* L) {
        std::lock_guard lock(dev_text_mutex);
        dev_texts.clear();
        return 0;
    }

    int development_remove_batch_override(lua_State* L) {
        std::lock_guard lock(dev_text_mutex);
        dev_texts.clear();
        return 0;
    }

    int development_clear_screen_override(lua_State* L) {
        std::lock_guard lock(dev_text_mutex);
        dev_texts.clear();
        return 0;
    }

    int development_erase_text_override(lua_State* L) {
        std::lock_guard lock(dev_text_mutex);
        const int index = std::stoi(proxies::gamelua_tolstring(L, 2, NULL));
        auto it = dev_texts.find(index);
        if (it != dev_texts.end()) {
            dev_texts.erase(it);
        }
        return 0;
    }

    int development_add_2d_text_override(lua_State* L) {
        // Params:
        // 1. self? this?
        // 2. index
        // 3. text
        // 4. position (vec2)
        // 5. colour (vec4)
        // 6. scale (number)
        // 7. unknown, always -1, ignore
        std::lock_guard lock(dev_text_mutex);
        const int n = proxies::gamelua_gettop(L);

        // Check if the number of arguments is correct
        if (n != 7) {
            logger::dll("add2DText", "Invalid number of arguments for add2DText");
            return 0;
        }

        // Check if the first text to be printed is a number or string
        if (proxies::gamelua_type(L, 3) != LUA_TNUMBER && proxies::gamelua_type(L, 3) != LUA_TSTRING) {
            logger::dll("add2DText", "Invalid text");
            return 0;
        }

        dev_text text = {};
        const int index = std::stoi(proxies::gamelua_tolstring(L, 2, nullptr));
        text.text = std::string(proxies::gamelua_tolstring(L, 3, NULL));
        text.position = *(vec2*) proxies::gamelua_touserdata(L, 4);
        text.colour = *(vec4*) proxies::gamelua_touserdata(L, 5);
        text.scale = std::stof(proxies::gamelua_tolstring(L, 6, NULL));
        dev_texts[index] = text;
        return 0;
    }

    void override_print(lua_State* L) {
        if (!print_overrridden) {
            helpers::set_cfunction_in_table(L, LUA_GLOBALSINDEX, "print", luab_print_override);
            logger::dll("pcall", "Applied custom 'print' function.");
            print_overrridden = true;
        }
    }

    void override_development(lua_State *L) {
        if (!development_overridden) {
            proxies::gamelua_getfield(L, LUA_GLOBALSINDEX, "Development");              // Stack: [..., Development?]
            if (proxies::gamelua_type(L, -1) == LUA_TTABLE) {
                helpers::set_cfunction_in_table(L, -1, "removeBatch", development_remove_batch_override);
                helpers::set_cfunction_in_table(L, -1, "startBatch", development_start_batch);
                helpers::set_cfunction_in_table(L, -1, "clearScreen", development_clear_screen_override);
                helpers::set_cfunction_in_table(L, -1, "eraseText", development_erase_text_override);
                helpers::set_cfunction_in_table(L, -1, "add2DText", development_add_2d_text_override);
                logger::dll("pcall", "Applied custom 'Development' functions.");
                development_overridden = true;
            }
            proxies::gamelua_pop(L, 1);                                                 // Stack: [...]
        }
    }

    void override_config_menu(lua_State *L) {
        if (config_menu_enable) {
            exec::execute_lua_buffer("flagGameDevMenu()");
            if (exec::execute_lua(L)) {
                logger::dll("pcall", "GameDevMenu flag set.");
                config_menu_enable = false;
            }
        }
    }
} // lua