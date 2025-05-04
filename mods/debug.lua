-- Debug mod 
-- Dump data section if 'O' is pressed
-- Restore data section if 'P' is pressed
local DATA_SECTION_BEGIN = 0x517290
local DATA_SECTION_END = 0x99EE58
local DATA_SECTION_SIZE = DATA_SECTION_END - DATA_SECTION_BEGIN

local dataSectionDump = nil

print("Debug mod")

re.subscribe(HookKind.tick, function()
	local oIsDown, pIsDown = re.getInputState()
    
    if oIsDown then
		print("o pressed")
		dataSectionDump = unsafe.read(DATA_SECTION_BEGIN, DATA_SECTION_SIZE)
    end
	if pIsDown then
		print("p pressed")		
		if dataSectionDump ~= nil then
			unsafe.write(DATA_SECTION_BEGIN, dataSectionDump)
		end
    end
end)