function getUpvalueByName(func, upvalueName)
  local i = 1
  while true do
    local name, value = debug.getupvalue(func, i)

    if not name then
      break
    end

    if name == upvalueName then
      return value, true
    end

    i = i + 1
  end

  return nil, false
end

function setUpvalueByName(func, upvalueName, newValue)
  local i = 1
  while true do
    local name = debug.getupvalue(func, i) -- Only need the name here

    if not name then
      break
    end

    if name == upvalueName then 
      local successName = debug.setupvalue(func, i, newValue)
      if successName then
        return true
      else
        return false
      end
    end


    i = i + 1
  end

  return false
end