globalMenus = getUpvalueByName(addPauseMenu, "menus")				-- make menus table global
globalDrawMenu = getUpvalueByName(TriggerPauseMenu, "drawMenu")			-- make draw function global

originalClearPauseMenu = clearPauseMenu						-- patches: set active to main, enable game controls
clearPauseMenu = function()
  originalClearPauseMenu()
  setUpvalueByName(originalClearPauseMenu, "activeMenu", "Main")
  controller.enableGameControls()
end

originalTriggerPauseMenu = TriggerPauseMenu					-- patches: set active to main, disable game controls
TriggerPauseMenu = function()  
  setUpvalueByName(originalClearPauseMenu, "activeMenu", "Main")
  originalTriggerPauseMenu()
  controller.disableGameControls()
end

local OriginalPauseMenuResume = PauseMenu.resume				-- patches: clear the menu on unpause
PauseMenu.resume = function()
  OriginalPauseMenuResume()
  clearPauseMenu()
end

originalStepMenuSelect = PauseMenuCallbacks["Menu_Select"]["JustPressed"][1] 	-- patches: allow menu refresh on select (toggle function on/off)
customStepMenuSelect = function()
  originalStepMenuSelect()
  if getUpvalueByName(originalClearPauseMenu, "pauseMenuShown") then
    globalDrawMenu()
  end
end
PauseMenuCallbacks["Menu_Select"]["JustPressed"][1] = customStepMenuSelect
controlHandler:registerState(0, "Pause", PauseMenuCallbacks)
controlHandler:registerState(1, "Pause", PauseMenuCallbacks)
controlHandler:resetState("Pause")

local Main = {}									-- option for default debug menu
table.insert(Main, {
      name = "Dev Menu",
      action = changePauseMenu("Pause"),
      info = "View Dev Menu"
})
table.insert(Main, {								-- option for custom menu
      name = "Custom Menu",
      action = changePauseMenu("Custom"),
      info = "View Custom Menu"
})
addPauseMenu("Main", Main)							-- effectively make main the new default menu
globalMenus["Pause"]["mediaVersion"] = nil
globalMenus["Pause"]["title"] = "/Dev"
globalMenus["Main"]["mediaVersion"] = "Media version: " .. Network.getVersionString()

local CustomMenu = {}								-- Sets up custom menu
table.insert(CustomMenu, {
  name = "Back",
  action = changePauseMenu("Main")
})
addPauseMenu("Custom", CustomMenu)