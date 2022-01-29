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

int GLRT_LUAAPI(lua_State* L);

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
		luaL_requiref(L, "GLRT", GLRT_LUAAPI, 1);
		lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
		lua_writeline();
		//luaL_dofile(L, "./script/script.lua");
		const char* filename = "./script/script.lua";
		auto ret = luaL_loadfile(L, filename);
		if (ret != LUA_OK)
		{
			const char* msg = lua_tostring(L, -1);
			fprintf(stdout, "load script file error: ");
			fprintf(stdout, "%s\n", msg);
			lua_pop(L, 1);  /* remove message */
			lua_close(L);
			L = 0;
			return;
		}
		ret = lua_pcall(L, 0, LUA_MULTRET, 0);
		if (ret != LUA_OK)
		{
			const char* msg = lua_tostring(L, -1);
			fprintf(stdout, "run script file error: ");
			fprintf(stdout, "%s\n", msg);
			lua_pop(L, 1);  /* remove message */
			lua_close(L);
			L = 0;
			return;
		}
	}
};

static LuaScript _luascript;
void InitScript()
{
	_luascript.Init();
}


class LuaScriptContext : public IScriptContext
{
public:
	std::string instance_name;
	std::string class_name;
	void BindScript(const char* class_name)
	{
		auto L = _luascript.L;
		if (!L)
			return;
		this->class_name = class_name;
		auto ret = lua_getglobal(L, class_name);
		if (!lua_istable(L, -1))
		{
			fprintf(stdout, "[LuaScriptContext:Create] can not get class %s", class_name);
			lua_pop(L, 1);
			return;
		}
		lua_pushstring(L, "__init__");
		ret = lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			fprintf(stdout, "[LuaScriptContext:Create] can not get function init of class %s", class_name);
			lua_pop(L, 2);
			return;
		}
		instance_name = string_format("ctx_%p", this);
		lua_newtable(L);		
		lua_pushstring(L, "context");
		lua_pushlightuserdata(L, this);
		lua_settable(L, -3);
		lua_setglobal(L, instance_name.c_str());
		lua_getglobal(L, instance_name.c_str());
		ret = lua_pcall(L, 1, 0, 0);
		if (ret != LUA_OK)
		{
			fprintf(stdout, "[LuaScriptContext:Create][%s.init] %s", class_name, lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	void Invoke(const char* method)
	{
		auto L = _luascript.L;
		if (!L)
			return;
		lua_getglobal(L, class_name.c_str());
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			return;
		}
		lua_pushstring(L, method);
		lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 2);
			return;
		}
		lua_getglobal(L, instance_name.c_str());
		auto ret = lua_pcall(L, 1, 0, 0);
		if (ret != LUA_OK)
		{
			fprintf(stdout, "[LuaScriptContext:Invoke][%s.%s] %s", class_name.c_str(), method, lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	static LuaScriptContext* Get(lua_State* L)
	{
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			return NULL;
		}
		lua_pushstring(L, "context");
		lua_gettable(L, -2);
		if (!lua_islightuserdata(L, -1))
		{
			lua_pop(L, 2);
			return NULL;
		}
		auto result = (LuaScriptContext*)lua_touserdata(L, -1);
		lua_pop(L, 2);
		return result;
	}
};


static int LuaScript_GetTransformPosition(lua_State* L)
{
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pNode) return 0;
	lua_pushnumber(L, ctx->pNode->transform.position.x);
	lua_pushnumber(L, ctx->pNode->transform.position.y);
	lua_pushnumber(L, ctx->pNode->transform.position.z);
	return 3;
}

static int LuaScript_SetTransformPosition(lua_State* L)
{
	if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2) || !lua_isnumber(L, -3)) return 0;
	auto z = lua_tonumber(L, -1);
	auto y = lua_tonumber(L, -2);
	auto x = lua_tonumber(L, -3);
	lua_pop(L, 3);
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pNode) return 0;
	ctx->pNode->transform.position.x = x;
	ctx->pNode->transform.position.y = y;
	ctx->pNode->transform.position.z = z;
	return 0;
}

static const luaL_Reg GLRT_LUAAPI_NODE_TRANSFORM[] =
{
	{"getTransformPosition", LuaScript_GetTransformPosition},
	{"setTransformPosition", LuaScript_SetTransformPosition},
	{NULL, NULL}
};

int GLRT_LUAAPI(lua_State* L)
{
	luaL_newlib(L, GLRT_LUAAPI_NODE_TRANSFORM);
	return 1;
}

IScriptContext* IScriptContext::Alloc(Node* pNode)
{
	auto result = new LuaScriptContext;
	if (NULL != pNode)
		pNode->pScriptContext = result;
	result->pNode = pNode;
	return result;
}
