#include "stdio.h"
extern "C"
{
     #include "lua.h"
     #include "lauxlib.h"
     #include "lualib.h"
}
#include <stdarg.h>
#include <string.h>
/* #include <conio.h> */
 
class lua_A
{
     typedef struct __functable
     {
         char* name;
         int (lua_A::*func)(lua_State*);
     }FuncTable;
 
     typedef struct __userdata
     {
         lua_A* pObj;
     }Userdata_lua;
 
public:
     lua_A() {printf("lua_A()\n");}
     ~lua_A() {printf("~lua_A()\n");}
 
public:
     void hello(const char* str)
     {
         printf("Hello %s!\n", str);
     }
 
     int hello_lua(lua_State* state)
     {
         const char* str = luaL_checkstring(state, -1);
         hello(str);
         return 0;
     }
 
//adapter for lua
     static lua_A* check_lua(lua_State* state, int narg)
     {
         Userdata_lua *ud =
              static_cast<Userdata_lua*>(luaL_checkudata(state, narg, className));
         if(!ud) luaL_typerror(state, narg, className);
         return ud->pObj;
     }
 
     static int thunk_lua(lua_State* state)
     {
         lua_A *obj = check_lua(state, 1);
         lua_remove(state, 1);
         FuncTable *l = static_cast<FuncTable*>(lua_touserdata(state, lua_upvalueindex(1)));
         return (obj->*(l->func))(state);
     }
 
     static int tostring_lua(lua_State* state)
     {
         char buff[32];
         Userdata_lua *ud = static_cast<Userdata_lua*>(lua_touserdata(state, 1));
         lua_A *obj = ud->pObj;
         sprintf(buff, "%p", obj);
         lua_pushfstring(state, "%s (%s)", className, buff);
         return 1;
     }
 
     static int gc_lua(lua_State* state)
     {
         Userdata_lua *ud = static_cast<Userdata_lua*>(lua_touserdata(state, 1));
         lua_A *obj = ud->pObj;
         delete obj;
         return 0;
     }
 
     static int new_lua(lua_State* state)
     {
         lua_remove(state, 1);
         lua_A *obj = new lua_A();
         Userdata_lua *ud = static_cast<Userdata_lua*>(lua_newuserdata(state, sizeof(Userdata_lua)));
         ud->pObj = obj;
         luaL_getmetatable(state, className);
         lua_setmetatable(state, -2);
         return 1;
     }
 
//register
     static void Register_lua(lua_State* state)
     {
         lua_newtable(state);
         int methods = lua_gettop(state);
 
         luaL_newmetatable(state, className);
         int metatable = lua_gettop(state);
 
         lua_pushstring(state, className);
         lua_pushvalue(state, methods);
         lua_settable(state, LUA_GLOBALSINDEX);
 
         lua_pushliteral(state, "__metatable");
         lua_pushvalue(state, methods);
         lua_settable(state, metatable);
 
         lua_pushliteral(state, "__index");
         lua_pushvalue(state, methods);
         lua_settable(state, metatable);
 
         lua_pushliteral(state, "__tostring");
         lua_pushcfunction(state, tostring_lua);
         lua_settable(state, metatable);
 
         lua_pushliteral(state, "__gc");
         lua_pushcfunction(state, gc_lua);
         lua_settable(state, metatable);
 
         lua_newtable(state);
         int mt = lua_gettop(state);
         lua_pushliteral(state, "__call");
         lua_pushcfunction(state, new_lua);
         lua_pushliteral(state, "new");
         lua_pushvalue(state, -2);
         lua_settable(state, methods);
         lua_settable(state, mt);
         lua_setmetatable(state, methods);
 
         for (FuncTable *l = g_functable; l->name; l++)
         {
              lua_pushstring(state, l->name);
              lua_pushlightuserdata(state, (void*)l);
              lua_pushcclosure(state, thunk_lua, 1);
              lua_settable(state, methods);
         }
 
         lua_pop(state, 2);
     }
 
//table like
     static char className[];
     static FuncTable g_functable[];
};
 
char lua_A::className[] = "lua_A";
lua_A::FuncTable lua_A::g_functable[] = {
     {"hello", &lua_A::hello_lua},
     {NULL, NULL}
};
 
lua_State* luaInit(char* filename)
{
     lua_State* L = lua_open();
     luaopen_base(L);
     luaopen_string(L);
     luaopen_math(L);
 
     if (filename && *filename)
     {
         if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
         {
              printf("can not open file: %s", lua_tostring(L, -1));
         }
     }
 
     return L;
}
 
void luaUninit(lua_State* L)
{
     lua_close(L);
}
 
int main()
{
     lua_State *state = luaInit(NULL);
 
     lua_A::Register_lua(state);
     if (luaL_loadfile(state, "table2.lua") || lua_pcall(state, 0, 0, 0))
     {
         printf("can not open file: %s", lua_tostring(state, -1));
     }
 
     luaUninit(state);
     return 0;
}
 
