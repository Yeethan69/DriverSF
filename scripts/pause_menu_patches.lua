globalMenus = getUpvalueByName(addPauseMenu, "menus")				-- make menus table global
globalDrawMenu = getUpvalueByName(TriggerPauseMenu, "drawMenu")			-- make draw function global

originalClearPauseMenu = clearPauseMenu						-- patches: set active to main, enable game controls
clearPauseMenu = function()
  originalClearPauseMenu()
  if enableCustomMenus then
    setUpvalueByName(originalClearPauseMenu, "activeMenu", "Main")
  end
  controller.enableGameControls()
end

originalTriggerPauseMenu = TriggerPauseMenu					-- patches: set active to main, disable game controls
TriggerPauseMenu = function()
  if enableCustomMenus then
    setUpvalueByName(originalClearPauseMenu, "activeMenu", "Main")
  end
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