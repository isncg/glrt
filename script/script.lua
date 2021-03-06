print("GLRT application script setup")
GLRT = require("GLRT")
-- cocos2dx-lua class
-- https://github.com/chukong/quick-cocos2d-x/blob/master/framework/functions.lua
--[[--
@param string classname 类名
@param [mixed super] 父类或者创建对象实例的函数
@return table
]]
function class(classname, super)
    local superType = type(super)
    local cls

    if superType ~= "function" and superType ~= "table" then
        superType = nil
        super = nil
    end

    if superType == "function" or (super and super.__ctype == 1) then
        -- inherited from native C++ Object
        cls = {}

        if superType == "table" then
            -- copy fields from super
            for k,v in pairs(super) do cls[k] = v end
            cls.__create = super.__create
            cls.super    = super
        else
            cls.__create = super
            cls.ctor = function() end
        end

        cls.__cname = classname
        cls.__ctype = 1

        function cls.new(...)
            local instance = cls.__create(...)
            -- copy fields from class to native object
            for k,v in pairs(cls) do instance[k] = v end
            instance.class = cls
            instance:ctor(...)
            return instance
        end

    else
        -- inherited from Lua Object
        if super then
            cls = {}
            setmetatable(cls, {__index = super})
            cls.super = super
        else
            cls = {ctor = function() end}
        end

        cls.__cname = classname
        cls.__ctype = 2 -- lua
        cls.__index = cls

        function cls.new(...)
            local instance = setmetatable({}, cls)
            instance.class = cls
            instance:ctor(...)
            return instance
        end
    end

    return cls
end


ScriptableObject = {}
function ScriptableObject.__init__(self)
end

LuaBehavior = class("LuaBehavior", ScriptableObject)
function LuaBehavior.__init__(self)
end

-- Awake is called when the script instance is being loaded.
function LuaBehavior.awake(self)
end

-- Update is called every frame, if the LuaBehavior is enabled.
function LuaBehavior.update(self)
end

function LuaBehavior.on_enter_tree(self)
end

Shader = class("Shader", ScriptableObject)

ShaderLib = class("ShaderLib", ScriptableObject)
function ShaderLib.__init__(self)
	ScriptableObject.__init__(self)
	self.load = function(filename) GLRT.shaderlibLoad(self, filename) end
	self.get  = function(filename) GLRT.shaderlibGet(self, filename) end
end

Material = class("Material", ScriptableObject)