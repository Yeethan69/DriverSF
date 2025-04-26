enableCustomMenus = true

customPath = "..\\..\\scripts\\"
debugOpen(customPath .. "utils.lua")
debugOpen(customPath .. "pause_menu_patches.lua")
if enableCustomMenus then
  debugOpen(customPath .. "enable_custom_menu.lua")
  debugOpen(customPath .. "example_menu.lua")
end