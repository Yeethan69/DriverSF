//
// Created by ethan on 23/04/2025.
//
#include "dihooks.h"
#include "../../safetyhook/include/safetyhook.hpp"
#include "../helpers/luaexec.h"
#include <dinput.h>

namespace di::hooks {
    SafetyHookInline o_get_device_data{};

    HRESULT __stdcall get_device_data_hook(IDirectInputDevice* device, DWORD size, LPDIDEVICEOBJECTDATA data,
        LPDWORD num_elements, DWORD flags) {
        const auto hr = o_get_device_data.stdcall<HRESULT>(device, size, data, num_elements, flags);
        if (SUCCEEDED(hr)) {
            if (data[0].dwOfs == DIK_RALT) { // Hardoded to right alt key
                if (data[0].dwData == 0x80 && lua::exec::execute == false) {
                    // Toggles the in game menu
                    lua::exec::execute_lua_buffer(
                        "if getUpvalueByName(originalClearPauseMenu, \"pauseMenuShown\") then clearPauseMenu() else TriggerPauseMenu() end");
                }
            }
        }
        return hr;
    }

    void install_hooks() {
        // Get address of DirectInput8Create
        typedef HRESULT(__stdcall* t_direct_input8_create)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
        const auto direct_input8_create = (t_direct_input8_create)GetProcAddress(GetModuleHandleA("dinput8.dll"), "DirectInput8Create");

        //Create dummy direct input pointer
        IDirectInput8* p_di = nullptr;
        HRESULT hr = direct_input8_create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8A,
            (LPVOID*)&p_di, nullptr);
        if (SUCCEEDED(hr)) {
            // Create dummy device pointer
            IDirectInputDevice* p_di_device = nullptr;
            hr = p_di->CreateDevice(GUID_SysKeyboard, (LPDIRECTINPUTDEVICE8A*)&p_di_device, nullptr);
            if (SUCCEEDED(hr)) {
                o_get_device_data = safetyhook::create_inline((*(DWORD**)p_di_device)[10], get_device_data_hook);
            }
            p_di_device->Release();
        }
        p_di->Release();
    }
} // di