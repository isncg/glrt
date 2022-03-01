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

const int MAGIC_LuaScriptContext = 365905;
class LuaScriptContext : public IScriptContext
{
public:
	int magic = MAGIC_LuaScriptContext;
	void BindScript(const char* class_name)
	{
		auto L = _luascript.L;
		if (!L)
			return;
		this->class_name = class_name;
		auto ret = lua_getglobal(L, class_name);
		if (!lua_istable(L, -1))
		{
			fprintf(stdout, "[LuaScriptContext:Create] can not get class %s\n", class_name);
			lua_pop(L, 1);
			return;
		}
		lua_pushstring(L, "__init__");
		ret = lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			fprintf(stdout, "[LuaScriptContext:Create] can not get function init of class %s\n", class_name);
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
			fprintf(stdout, "[LuaScriptContext:Create][%s.init] %s\n", class_name, lua_tostring(L, -1));
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
			fprintf(stdout, "[LuaScriptContext:Invoke][%s.%s] %s\n", class_name.c_str(), method, lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
};

int LuaGetScriptContext(lua_State* L, IScriptContext* pScriptContext)
{
	if (pScriptContext)
	{
		lua_getglobal(L, pScriptContext->instance_name.c_str());
		return 1;
	}
	return 0;
}

int LuaGetScriptContext(lua_State* L, IScriptable* pScriptable, const char* className)
{
	if (pScriptable)
	{
		auto ctx = pScriptable->GetOrCreateScriptContext(className);
		return LuaGetScriptContext(L, ctx);
	}
	return 0;
}

int LuaGetScriptContext(lua_State* L, IScriptable& pScriptable, const char* className)
{
	auto ctx = pScriptable.GetOrCreateScriptContext(className);
	return LuaGetScriptContext(L, ctx);
}

class LuaCallParam
{
public:
	LuaScriptContext* pContext;
	virtual int param_count() { return 1; }
	virtual bool fill_params(lua_State* L)
	{
		if (lua_gettop(L) != param_count())
		{
			luaL_error(L, "expected %d params, got %d", param_count(), lua_gettop(L));
			return false;
		}

		if (!lua_istable(L, -param_count()))
		{
			luaL_typeerror(L, -param_count(), "script_context");
			return false;
		}
		lua_pushstring(L, "context");
		lua_gettable(L, -param_count() - 1);
		if (!lua_islightuserdata(L, -1))
		{
			luaL_typeerror(L, -param_count(), "script_context with context field");
			lua_pop(L, 1);
			return NULL;
		}
		pContext = (LuaScriptContext*)lua_touserdata(L, -1);
		lua_pop(L, 1);
		if (NULL == pContext)
		{
			luaL_error(L, "invalid context");
			return false;
		}
		if (pContext->magic != MAGIC_LuaScriptContext)
		{
			luaL_error(L, "bad magic %d", pContext->magic);
			return false;
		}
		return true;
	}
};

bool fill_lua_value(float* value, lua_State* L, int index)
{
	int result;
	*value = lua_tonumberx(L, index, &result);
	if (!result)
		luaL_typeerror(L, index, "float");
	return result;
}

bool fill_lua_value(int* value, lua_State* L, int index)
{
	int result;
	*value = lua_tointegerx(L, index, &result);
	if (!result)
		luaL_typeerror(L, index, "int");
	return result;
}

bool fill_lua_value(std::string* value, lua_State* L, int index)
{
	if (!lua_isstring(L, index))
	{
		luaL_typeerror(L, index, "string");
		return false;
	}
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
		return LuaCallParam1<T1>::fill_params(L) && fill_lua_value(&arg2, L, -param_count() + 2);
	}
};

template<typename T1, typename T2, typename T3>
class LuaCallParam3 : public LuaCallParam2<T1, T2>
{
public:
	T3 arg3;
	virtual int param_count() override { return 4; }
	virtual bool fill_params(lua_State* L) override
	{
		return LuaCallParam2<T1, T2>::fill_params(L) && fill_lua_value(&arg3, L, -param_count() + 3);
	}
};

template<typename T1, typename T2, typename T3, typename T4>
class LuaCallParam4 : public LuaCallParam3<T1, T2, T3>
{
public:
	T3 arg4;
	virtual int param_count() override { return 5; }
	virtual bool fill_params(lua_State* L) override
	{
		return LuaCallParam3<T1, T2, T3>::fill_params(L) && fill_lua_value(&arg4, L, -param_count() + 4);
	}
};



#include "../include/scene/node.h"

static int LuaScript_GetTransformPosition(lua_State* L)
{
	LuaCallParam param;
	if (!param.fill_params(L))
		return 0;
	lua_pushnumber(L, param.pContext->pData->as<Node>()->transform.position.x);
	lua_pushnumber(L, param.pContext->pData->as<Node>()->transform.position.y);
	lua_pushnumber(L, param.pContext->pData->as<Node>()->transform.position.z);
	return 3;
}

static int LuaScript_SetTransformPosition(lua_State* L)
{
	LuaCallParam3<float, float, float> params;
	if (params.fill_params(L))
	{
		auto ctx = params.pContext->data<Node>();
		ctx->transform.position.x = params.arg1;
		ctx->transform.position.y = params.arg2;
		ctx->transform.position.z = params.arg3;
	}
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
	LuaCallParam4<std::string, float, float, float> param;
	if (!param.fill_params(L))
		return 0;
	param.pContext->pData->as<Material>()->Set(param.arg1, Color{ param.arg2, param.arg3, param.arg4 });
}

static int LuaScript_SetMaterialF3(lua_State* L)
{
	LuaCallParam4<std::string, float, float, float> param;
	if (!param.fill_params(L))
		return 0;
	param.pContext->pData->as<Material>()->Set(param.arg1, Vector3{ param.arg2, param.arg3, param.arg4 });
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
	LuaCallParam1<std::string> param;
	if (!param.fill_params(L))
		return 0;
	return LuaGetScriptContext(L, param.pContext->data<ShaderLib>()->Load(std::move(param.arg1)), "Shader");
}

static int LuaScript_ShaderLibGet(lua_State* L)
{
	LuaCallParam1<std::string> param;
	if (!param.fill_params(L))
		return 0;
	return LuaGetScriptContext(L, param.pContext->data<ShaderLib>()->Get(param.arg1), "Shader");
}

static int LuaScript_ShaderLibGetInstance(lua_State* L)
{
	return LuaGetScriptContext(L, ShaderLib::Instance(), "ShaderLib");
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
	return 1;
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

IScriptContext* IScriptable::GetOrCreateScriptContext(const char* className)
{
	if (nullptr != pScriptContext)
		return pScriptContext;
	if (nullptr == className)
		return nullptr;
	SetScript(className);
	return pScriptContext;
}

void LoadScriptFile(std::string&& fname)
{
	luaL_dofile(_luascript.L, fname.c_str());
}
