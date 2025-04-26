//
// Created by ethan on 23/04/2025.
//
#include "luahooks.h"

#include <filesystem>
#include <fstream>

#include "../helpers/luaexec.h"
#include "../overrides/luaoverrides.h"
#include "../logger.h"

namespace lua::hooks {

    // Vars
    inline constexpr uint32_t pcall_offset = 0x1F1CA0;
    inline constexpr uint32_t protectedparser_offset = 0x207730;

    // Functions
    void dump_luac(const char *source, size_t size, std::string buffer_name) {
        if (dump_lua) {
            const std::string dump_path = ".\\dumps\\luac\\" + buffer_name;
            std::ofstream dump_file(dump_path, std::ios::noreplace | std::ios::binary);
            if (std::ifstream(dump_path).good())
                return; // File already exists, don't overwrite
            std::filesystem::create_directories(std::filesystem::path(dump_path).parent_path());
            dump_file.open(dump_path, std::ios::binary);
            dump_file.write(source, size);
            dump_file.close();
            logger::dll("luad_protectedparser_hook", "Dumped: " + dump_path);
        }
    }

    int lua_pcall_hook(lua_State* L, int n_args, int n_results, int err_func) {
        if (exec::p_game_state == nullptr)
            exec::p_game_state = L;

        const auto res = o_lua_pcall.call<int>(L, n_args, n_results, err_func);

        overrides::override_print(L); // Override lua print to log to console
        overrides::override_development(L); // Override development functions forward to the d3d9 hook

        overrides::override_config_menu(L); // Enables the config menu before game start

        exec::lua_game_running_hook_bootstrap(L); // Loads custom lua code that patches dev menu to enable custom menu

        exec::execute_lua(L, "DLL_Execute"); // Execute the code in exec::buffer

        return res;
    }

    int luad_protectedparser_hook(lua_State* L, ZIO* z, const char* name) {
        static bool safe = true;
        if (!safe || *(int*)z->data == 0 || *(int*)z->data == 1) {
            if (*(int*)z->data == 0 || *(int*)z->data == 1) {
                const auto res = o_luad_protectedparser.call<int>(L, z, name);
                return res;
            }
            return o_luad_protectedparser.call<int>(L, z, name);
        }

        const char* source = ((overrides::load_s*)z->data)->s;
        const size_t size = ((overrides::load_s*)z->data)->size;
        std::string content;
        content.resize(size);
        std::transform(source, source + size, content.begin(), [](char c) { return c == '\0' ? ' ' : c; });
        const auto start = content.find("\\\\");
        const auto end = content.find(".lua", start);
        auto buffer_name = content.substr(start + 2, end - start + 2).append("c");

        if (buffer_name == "visuals\\camera\\cameras\\camera_functions.luac") // Next one z->data is not load_s, crashes
            safe = false;

        if (buffer_name.find(".luac") != std::string::npos) { // If buffer is compiled lua script
            dump_luac(source, size, buffer_name);

            const std::string decompiled = buffer_name.substr(0, buffer_name.find(".luac")) + ".lua";
            const std::string lua_path = R"(.\dumps\lua\)" + decompiled;

            //if decompiled version exists, load that instead
            if (FILE *f = fopen(lua_path.c_str(), "r")) {
                std::ifstream ifs(lua_path.c_str());
                const std::string lua((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ((overrides::load_s*)z->data)->s = lua.c_str();
                ((overrides::load_s*)z->data)->size = lua.length();
                return o_luad_protectedparser.call<int>(L, z, name);
            }
        }

        return o_luad_protectedparser.call<int>(L, z, name);
    }

    void install_hooks(const uint32_t base_address) {
        o_lua_pcall = safetyhook::create_inline(base_address + pcall_offset, lua_pcall_hook);
        o_luad_protectedparser = safetyhook::create_inline(base_address + protectedparser_offset, luad_protectedparser_hook);
    }

    void uninstall_hooks() {
        o_lua_pcall = {};
        o_luad_protectedparser = {};
    }

} // lua