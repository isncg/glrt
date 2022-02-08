ScriptingWindow = class("ScriptingWindow", ScriptableObject)

function ScriptingWindow.onCreate(self)
	print("on scripting window create")
end


--[[
Test case: frame counter
c++:
	Node* pNode = ...
	IScriptContext::Alloc(pNode)->BindScript("FrameCounter");
]]--
FrameCounter = class("FrameCounter", LuaBehavior)
function FrameCounter.awake(self)
	self.frameCount = 0
end

function FrameCounter.update(self)
	self.frameCount = self.frameCount + 1 
	print("frame count", self.frameCount)
end

