-- =========================================================================================
--			Basic format for addng to the custom menu
-- =========================================================================================

-- add custom menus here
-- local NameMenu = {}
-- 
-- table.insert(NameMenu, {                        Adds functionality to individual menu
--   name = "Back",
--   action = function()
--     changePauseMenu("Custom")
--   end
-- })

-- addPauseMenu("Name of Menu", NameMenu)          Adds to the table of menus

-- table.insert(CustomMenu, {                      Adds to the main custom menu
--   name = "Name of menu",
--   action = function()
--     changePauseMenu("NameMenu")
--   end
--   info = "View name of menu"
-- })

-- =========================================================================================
-- 	The following is basically a test for infinite nitro, and custom menu support
-- =========================================================================================

local NitroMenu = {}

local InfiniteBoost = false

local Original1
local Original2
local Original3
local Original4

local getOriginalConstants = function()
  print("getting OG Constants")
  Original1 = abilities.constants["single"]["hold"]["nitro"][1]
  Original2 = abilities.constants["single"]["hold"]["nitro"][2]
  Original3 = abilities.constants["single"]["hold"]["nitro"][3]
  Original4 = abilities.constants["single"]["hold"]["nitro"][4]
end

-- Actual mod function
table.insert(NitroMenu, {
  name = "Toggle Infinite Boost: off",
  action = function()
    if Original1 == nil then
      getOriginalConstants()
    end
    if InfiniteBoost == false then
      print("Toggling on")
      InfiniteBoost = true
      abilities.constants["single"]["hold"]["nitro"][1] = 0
      abilities.constants["single"]["hold"]["nitro"][2] = 0
      abilities.constants["single"]["hold"]["nitro"][3] = 0
      abilities.constants["single"]["hold"]["nitro"][4] = 0
      for k, v in pairs(NitroMenu) do
        if NitroMenu[k]["name"] == "Toggle Infinite Boost: off" then
          NitroMenu[k]["name"] = "Toggle Infinite Boost: on"
          break
        end
      end
    elseif InfiniteBoost == true then
      print("Toggling off")
      InfiniteBoost = false
      abilities.constants["single"]["hold"]["nitro"][1] = Original1
      abilities.constants["single"]["hold"]["nitro"][2] = Original2
      abilities.constants["single"]["hold"]["nitro"][3] = Original3
      abilities.constants["single"]["hold"]["nitro"][4] = Original4
      for k, v in pairs(NitroMenu) do
        if NitroMenu[k]["name"] == "Toggle Infinite Boost: on" then
          NitroMenu[k]["name"] = "Toggle Infinite Boost: off"
          break
        end
      end
    end
  end
})

-- Back button
table.insert(NitroMenu, {
  name = "Back",
  action = changePauseMenu("Custom")
})

-- Add this menu to main table of menus
addPauseMenu("Nitro Menu", NitroMenu)

-- Add entry into main custom menu
table.insert(CustomMenu, {
  name = "Nitro Menu",
  action = changePauseMenu("Nitro Menu"),
  description = "View Nitro Hax"
})

print("======================================== Custom loaded ========================================")