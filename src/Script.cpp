#include <cstdio>
#include <cstring>
extern "C"
{
#include <lua540/lua.h>
#include <lua540/lauxlib.h>
#include <lua540/lualib.h>
}


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
