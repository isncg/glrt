#include <cstdio>
#include <cstring>
extern "C"
{
#include <lua540/lua.h>
#include <lua540/lauxlib.h>
#include <lua540/lualib.h>
}

#include "../utils/utils.h"
#include "../include/Script.h"
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

class LuaCallParam
{
public:
	IScriptContext* pContext;
	virtual int param_count() { return 1; }
	virtual bool fill_params(lua_State* L)
	{
		if (lua_gettop(L) != param_count())
			return false;
		if (!lua_islightuserdata(L, -param_count()))
			return false;
		pContext = (IScriptContext*)lua_touserdata(L, -param_count());
		return true;
	}
};

bool fill_lua_value(float* value, lua_State* L, int index)
{
	int result;
	*value = lua_tonumberx(L, index, &result);
	return result;
}

bool fill_lua_value(int* value, lua_State* L, int index)
{
	int result;
	*value = lua_tointegerx(L, index, &result);
	return result;
}

bool fill_lua_value(std::string* value, lua_State* L, int index)
{
	if (!lua_isstring(L, index))
		return false;
	*value = lua_tostring(L, index);
	return true;
}

template<typename T1>
class LuaCallParam1 : public LuaCallParam
{
public:	
	T1 arg1;
	virtual int param_count() override { return 2; }
	virtual bool fill_params(lua_State* L) override
	{
		return LuaCallParam::fill_params(L) && fill_lua_value(&arg1, L, -param_count() + 1);
	}
};
template<typename T1, typename T2>
class LuaCallParam2 : public LuaCallParam1<T1>
{
public:
	T2 arg2;
	virtual int param_count() override { return 3; }
	virtual bool fill_params(lua_State* L) override
	{
		return LuaCallParam<T1>::fill_params(L) && fill_lua_value(&arg2, L, -param_count() + 2);
	}
};

class LuaScriptContext : public IScriptContext
{
public:

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

#include "../include/scene/node.h"

static int LuaScript_GetTransformPosition(lua_State* L)
{
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pData) return 0;
	lua_pushnumber(L, ((Node*)(ctx->pData))->transform.position.x);
	lua_pushnumber(L, ((Node*)(ctx->pData))->transform.position.y);
	lua_pushnumber(L, ((Node*)(ctx->pData))->transform.position.z);
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
	if (NULL == ctx || NULL == ctx->pData) return 0;
	((Node*)(ctx->pData))->transform.position.x = x;
	((Node*)(ctx->pData))->transform.position.y = y;
	((Node*)(ctx->pData))->transform.position.z = z;
	return 0;
}

static const luaL_Reg GLRT_LUAAPI_NODE_TRANSFORM[] =
{
	{"getTransformPosition", LuaScript_GetTransformPosition},
	{"setTransformPosition", LuaScript_SetTransformPosition},
	{NULL, NULL}
};

#include "../include/Material.h"
static int LuaScript_SetMaterialColor(lua_State* L)
{
	if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2) || !lua_isnumber(L, -3) || !lua_isstring(L, -4)) return 0;
	auto z = lua_tonumber(L, -1);
	auto y = lua_tonumber(L, -2);
	auto x = lua_tonumber(L, -3);
	auto name = lua_tostring(L, -4);
	lua_pop(L, 3);
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pData) return 0;
	ctx->data<Material>()->Set(name, Color{ (float)x, (float)y, (float)z });
}

static int LuaScript_SetMaterialF3(lua_State* L)
{
	if (!lua_isnumber(L, -1) || !lua_isnumber(L, -2) || !lua_isnumber(L, -3) || !lua_isstring(L, -4)) return 0;
	auto z = lua_tonumber(L, -1);
	auto y = lua_tonumber(L, -2);
	auto x = lua_tonumber(L, -3);
	auto name = lua_tostring(L, -4);
	lua_pop(L, 3);
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pData) return 0;
	ctx->data<Material>()->Set(name, Vector3{ (float)x, (float)y, (float)z });
}

static const luaL_Reg GLRT_LUAAPI_MATERIAL[] =
{
	{"setMaterialColor", LuaScript_SetMaterialColor},
	{"setMaterialF3", LuaScript_SetMaterialF3},
	{NULL, NULL}
};

#include "../include/Shader.h"
static int LuaScript_ShaderLibLoad(lua_State* L)
{
	if (!lua_isstring(L, -1)) return 0;
	auto name = lua_tostring(L, -1);
	lua_pop(L, 1);
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pData) return 0;
	auto shader = ctx->data<ShaderLib>()->Load(name);
	if (NULL == shader) return 0;
	if (NULL == shader->pScriptContext)
	{
		IScriptContext::Alloc(shader);
		shader->pScriptContext->BindScript("Shader");
	}
	lua_getglobal(L, shader->pScriptContext->instance_name.c_str());
	return 1;
}

static int LuaScript_ShaderLibGet(lua_State* L)
{
	if (!lua_isstring(L, -1)) return 0;
	std::string name = lua_tostring(L, -1);
	lua_pop(L, 1);
	auto ctx = LuaScriptContext::Get(L);
	if (NULL == ctx || NULL == ctx->pData) return 0;
	auto shader = ctx->data<ShaderLib>()->Get(name);
	if (NULL == shader) return 0;
	if (NULL == shader->pScriptContext)
	{
		IScriptContext::Alloc(shader);
		shader->pScriptContext->BindScript("Shader");
	}

	if (shader->pScriptContext)
	{
		lua_getglobal(L, shader->pScriptContext->instance_name.c_str());
		return 1;
	}
	return 0;
}

static int LuaScript_ShaderLibGetInstance(lua_State* L)
{
	auto& inst = ShaderLib::Instance();
	if (!inst.pScriptContext)
	{
		IScriptContext::Alloc(&inst);
		inst.pScriptContext->BindScript("ShaderLib");
	}
	if (inst.pScriptContext)
	{
		lua_getglobal(L, inst.pScriptContext->instance_name.c_str());
		return 1;
	}
	return 0;
}

static const luaL_Reg GLRT_LUAAPI_SHADERLIB[] =
{
	{"shaderlibLoad", LuaScript_ShaderLibLoad},
	{"shaderlibGet", LuaScript_ShaderLibGet},
	{"shaderlibGetInstance", LuaScript_ShaderLibGetInstance},
	{NULL, NULL}
};


int GLRT_LUAAPI(lua_State* L)
{
	luaL_newlib(L, GLRT_LUAAPI_NODE_TRANSFORM);
	luaL_newlib(L, GLRT_LUAAPI_MATERIAL);
	luaL_newlib(L, GLRT_LUAAPI_SHADERLIB);
	return 3;
}

IScriptContext* IScriptContext::Alloc(IScriptable* pData)
{
	auto result = new LuaScriptContext;
	if (NULL != pData)
		pData->pScriptContext = result;
	result->pData = pData;
	return result;
}


void IScriptable::SetScript(const char* className)
{
	pScriptContext = IScriptContext::Alloc(this);
	pScriptContext->BindScript(className);
}

void IScriptable::InvokeScript(const char* method)
{
	if (NULL != pScriptContext)
		pScriptContext->Invoke(method);
}