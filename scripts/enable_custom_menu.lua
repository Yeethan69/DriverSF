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

CustomMenu = {}								        -- Sets up custom menu
table.insert(CustomMenu, {
  name = "Back",
  action = changePauseMenu("Main")
})
addPauseMenu("Custom", CustomMenu)