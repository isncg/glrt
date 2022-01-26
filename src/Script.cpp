#include <cstdio>
#include <cstring>
extern "C"
{
#include <lua540/lua.h>
#include <lua540/lauxlib.h>
#include <lua540/lualib.h>
}

#include "../include/scene/node.h"
#include "../utils/utils.h"
class LuaScript
{
public:
	lua_State* L = 0;
	void Init()
	{
		if (L)
			lua_close(L);
		L = luaL_newstate();
		luaL_openlibs(L);
		lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
		lua_writeline();
		luaL_dofile(L, "./script/script.lua");
	}
};

static LuaScript _luascript;
void InitScript()
{
	_luascript.Init();
}


class LuaScriptNode : public ScriptNode
{
public:
	int ref = 0;
	std::string className;
	virtual void OnEnterTree() override
	{
		lua_rawgeti(_luascript.L, LUA_REGISTRYINDEX, ref);
		lua_pushstring(_luascript.L, "onEnterTree");
		lua_gettable(_luascript.L, -2);
		if (lua_isnil(_luascript.L, -1))
		{
			log(string_format("failed to get function onEnterTree of class %s", className.c_str()).c_str());
			return;
		}
		lua_pcall(_luascript.L, 0, 0, 0);
	}
	virtual void Awake() override
	{

	}
	virtual void Update() override
	{

	}
};


ScriptNode* ScriptNode::CreateLuaScriptNode(std::string className)
{
	log(string_format("create script node %s", className.c_str()).c_str());
	auto node = new LuaScriptNode;
	auto ctor = string_format("%s.new()", className.c_str());
	lua_getglobal(_luascript.L, ctor.c_str());
	lua_call(_luascript.L, 0, 1);
	node->ref = luaL_ref(_luascript.L, LUA_REGISTRYINDEX);
	if (node->ref == LUA_REFNIL)
	{
		log(string_format("failed to create class %s", className.c_str()).c_str());
	}
	return node;
}