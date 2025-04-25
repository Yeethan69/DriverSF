cmake_minimum_required(VERSION 3.30)
project(DriverSF LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# d3dx9 is not part of the Windows SDK anymore, so we need to link it manually
add_library(d3dx9 STATIC IMPORTED)
set_target_properties(d3dx9 PROPERTIES
        IMPORTED_LOCATION "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx9.lib"
)

target_include_directories(d3dx9 INTERFACE
        "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Include"
)

# SafetyHook submodule includes CMakeLists.txt
add_subdirectory(safetyhook)

add_library(DriverSF SHARED
        src/library.cpp
        src/hooks/d3d9hooks.cpp
        src/hooks/dihooks.cpp
        src/hooks/luahooks.cpp
        src/proxies/luaproxies.cpp
        src/overrides/luaoverrides.cpp
        src/helpers/luaexec.cpp
        src/helpers/luahelpers.cpp
        src/logger.cpp
)
target_link_libraries(DriverSF PUBLIC
        d3d9
        d3dx9
        dinput8
        dxguid
        safetyhook
)

set_target_properties(DriverSF PROPERTIES
        OUTPUT_NAME "DriverSF"
        PREFIX ""
)