# DriverSF - Lua loading DLL
## Features
- Log lua print statements.
- Enable custom lua loading in gamedir/scripts.
- Dump luac files from game to gamedir/dumps/luac.
- Override lua loading with decompiled lua files from gamedir/dumps/lua (decompile yourself - [unluac](https://sourceforge.net/projects/unluac/) works).
- Dev/Debug menus re-enabled (config on start and in game dev menu - patched to enable use for custom menus).

**Made for and tested only on 1.04.1114**
## Building
I use CLion - cmake with visual studio 32bit toolchain (Have not tested with other environments).

**Requires [DirectX SDK](https://www.microsoft.com/en-gb/download/details.aspx?id=6812) (Cmake makes assumption it is installed in default location).**

## Usage
Rename dll to "UMPDC.dll", other names may also work.

Copy scripts folder to game directory.

Run game.

***Right Alt key to toggle in game menu***

(enableCustomMenus = true in scripts/load.lua to disable custom menus, and return in game menu to default)

## TODO
- [ ] Create loader to not rely on load order hijacking
- [ ] Enable loading custom scripts earlier or later - at the moment they load when the game state is "running"
  
## Credits
- [cursey - safetyhook](https://github.com/cursey/safetyhook) - the library used for hooking functions.
