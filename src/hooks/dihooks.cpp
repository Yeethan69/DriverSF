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
                    lua::exec::buffer = "if getUpvalueByName(originalClearPauseMenu, \"pauseMenuShown\") then clearPauseMenu() else TriggerPauseMenu() end";
                    lua::exec::execute = true;
                }
            }
        }
        return hr;
    }

    void install_hooks() {
        // Get address of DirectInput8Create
        typedef HRESULT(__stdcall* t_direct_input8_create)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
        auto DirectInput8Create = (t_direct_input8_create)GetProcAddress(GetModuleHandleA("dinput8.dll"), "DirectInput8Create");

        // Create dummy device pointer
        IDirectInput8* p_di = nullptr;
        HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8A,
            (LPVOID*)&p_di, nullptr);
        if (SUCCEEDED(hr)) {
            // if the device is
            IDirectInputDevice* pDIDevice = nullptr;
            hr = p_di->CreateDevice(GUID_SysKeyboard, (LPDIRECTINPUTDEVICE8A*)&pDIDevice, nullptr);
            if (SUCCEEDED(hr)) {
                o_get_device_data = safetyhook::create_inline((*(DWORD**)pDIDevice)[10], get_device_data_hook);
            }
            pDIDevice->Release();
        }
        p_di->Release();
    }
} // di