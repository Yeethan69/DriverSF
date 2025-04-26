//
// Created by ethan on 23/04/2025.
//
#include "d3d9hooks.h"
#include "../../safetyhook/include/safetyhook.hpp"
#include "../overrides/luaoverrides.h"
#include <d3dx9.h>

namespace d3d9::hooks {

    // Vars
    static SafetyHookInline o_end_scene{};
    static SafetyHookInline o_present{};

    bool first_call = true;
    ID3DXFont* p_font = nullptr;
    ID3DXSprite* p_sprite = nullptr;
    IDirect3DDevice9* p_my_device;

    // Functions
    bool draw_message(
        ID3DXFont* font, ID3DXSprite* font_sprite,
        unsigned int x, unsigned int y, float scale,
        int alpha, unsigned char r, unsigned char g, unsigned char b,
        LPCSTR message
        ) {
        // Begin drawing with alphablend and sort by texture
        font_sprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

        D3DXMATRIX scale_matrix, translation_matrix, world_matrix;
        // Scale x and y by the scale factor
        D3DXMatrixScaling(&scale_matrix, scale, scale, 1.0f);
        // Translate x and y by the position
        D3DXMatrixTranslation(&translation_matrix, x, y, 0.0f);
        world_matrix = scale_matrix * translation_matrix;
        font_sprite->SetTransform(&world_matrix);

        RECT text_rect = { 0, 0, 0, 0 }; // Position is now handled by SetTransform
        DWORD format = DT_NOCLIP; // Don't clip based on the (now tiny/zero) RECT
        D3DCOLOR font_color = D3DCOLOR_ARGB(alpha, r, g, b);

        // Draw the text
        font->DrawText(font_sprite, message, -1, &text_rect, format, font_color);

        D3DXMATRIX identity_matrix;
        D3DXMatrixIdentity(&identity_matrix);
        font_sprite->SetTransform(&identity_matrix);

        // End drawing
        font_sprite->End();

        return true;
    }

    HRESULT __stdcall end_scene_hook(IDirect3DDevice9* p_device) {
        // Just initialises the sprite once
        if (first_call) {
            D3DXCreateFont(p_device, 40, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                "Courier New", &p_font);
            D3DXCreateSprite(p_device, &p_sprite);
            first_call = false;
            p_my_device = p_device;
        }
        return o_end_scene.stdcall<HRESULT>(p_device);
    }

    HRESULT __stdcall present_hook(IDirect3DDevice9* p_device, const RECT* p_source_rect, const RECT* p_dest_rect,
        HWND h_dest_window_override,
        const RGNDATA* p_dirty_region) {
        if (p_device == p_my_device) {
            IDirect3DSurface9* pBackBuffer = nullptr;
            HRESULT hr = p_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

            UINT width = 0;
            UINT height = 0;

            // get window size to scale the positioning
            if (SUCCEEDED(hr) && pBackBuffer != nullptr) {
                D3DSURFACE_DESC surface_desc;
                hr = pBackBuffer->GetDesc(&surface_desc);
                if (SUCCEEDED(hr)) {
                    width = surface_desc.Width;
                    height = surface_desc.Height;
                }
                pBackBuffer->Release();
            }

            // acquire the lock to read the text
            std::shared_lock lock(lua::overrides::dev_text_mutex);
            if (!lua::overrides::dev_texts.empty()) {
                for (auto text: lua::overrides::dev_texts) {
                    draw_message(p_font, p_sprite,
                        text.second.position.x * width, text.second.position.y * height, text.second.scale,
                        255, text.second.colour.x * 255, text.second.colour.y * 255, text.second.colour.z * 255,
                        text.second.text.c_str());
                }
            }
        }
        return o_present.stdcall<HRESULT>(p_device, p_source_rect, p_dest_rect, h_dest_window_override, p_dirty_region);
    }

    void install_hooks() {
        // Get address of Direct3DCreate9
        typedef IDirect3D9*(__stdcall* t_direct_3d_create9)(UINT);
        const auto direct_3d_create9 = (t_direct_3d_create9)GetProcAddress(GetModuleHandleA("d3d9.dll"), "Direct3DCreate9");

        // Create dummy device pointer
        IDirect3D9* d3d9 = direct_3d_create9(D3D_SDK_VERSION);
        IDirect3DDevice9* d3d9device;

        // Get the window handle
        HWND h_wnd;
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            DWORD processId;
            GetWindowThreadProcessId(hwnd, &processId);
            if (processId == GetCurrentProcessId()) { // Check if the window belongs to the current process
                *(HWND*)lParam = hwnd;
                return FALSE; // Stop enumerating windows
            }
            return TRUE; // Continue enumerating windows
        }, LPARAM(&h_wnd));

        // Set up the presentation parameters
        auto params = D3DPRESENT_PARAMETERS();
        params.Windowed = TRUE;
        params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        params.hDeviceWindow = h_wnd;

        // Create the device
        auto hr = d3d9->CreateDevice(0, D3DDEVTYPE_HAL, nullptr,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &d3d9device);
        if (SUCCEEDED(hr)) {
            // If dummy device creation is successful, create hooks using the addresses in the vtable
            o_end_scene = safetyhook::create_inline((*(DWORD**)d3d9device)[42], end_scene_hook);
            o_present = safetyhook::create_inline((*(DWORD**)d3d9device)[17], present_hook);
        }

        // Once hooks are installed, release the dummy devices
        d3d9device->Release();
        d3d9->Release();
    }

    void uninstall_hooks() {
        o_end_scene = {};
        o_present = {};

        if (p_font) {
            p_font->Release();
            p_font = nullptr;
        }
        if (p_sprite) {
            p_sprite->Release();
            p_sprite = nullptr;
        }
    }
} // d3d9
