#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <map>
#include <cstdio>

#include <ctype.h>
#include <stdlib.h>


#include "SegmenterManager.h"
#include "Segmenter.h"
#include "csr_utils.h"

// using namespace std;
using namespace css;

extern "C" {

#include <lua.h>
#include <lauxlib.h>
// #include <lualib.h>

}
css::SegmenterManager mgr;// = NULL;

/*
typedef struct __userdata{
  css::SegmenterManager* pObj;
}Userdata_lua;

static const char* className = "mmseg";

static css::SegmenterManager* check_lua(lua_State* state, int narg)
{
  Userdata_lua *ud = static_cast<Userdata_lua*>(luaL_checkudata(state, narg, className));
  if(!ud) luaL_typerror(state, narg, className);
  return ud->pObj;
}
 
// static int thunk_lua(lua_State* state)
// {
//   css::SegmenterManager *obj = check_lua(state, 1);
//   lua_remove(state, 1);
//   FuncTable *l = static_cast<FuncTable*>(lua_touserdata(state, lua_upvalueindex(1)));
//   return (obj->*(l->func))(state);
// }
 
static int tostring_lua(lua_State* state)
{
  char buff[32];
  Userdata_lua *ud = static_cast<Userdata_lua*>(lua_touserdata(state, 1));
  css::SegmenterManager *obj = ud->pObj;
  sprintf(buff, "%p", obj);
  lua_pushfstring(state, "%s (%s)", className, buff);
  return 1;
}
 
static int gc_lua(lua_State* state)
{
  Userdata_lua *ud = static_cast<Userdata_lua*>(lua_touserdata(state, 1));
  css::SegmenterManager *obj = ud->pObj;
  delete obj;
  return 0;
}
 
static int new_lua(lua_State* state)
{
  lua_remove(state, 1);
  css::SegmenterManager *obj = new css::SegmenterManager();
  Userdata_lua *ud = static_cast<Userdata_lua*>(lua_newuserdata(state, sizeof(Userdata_lua)));
  ud->pObj = obj;
  luaL_getmetatable(state, className);
  lua_setmetatable(state, -2);
  return 1;
}

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
 
  // for (FuncTable *l = g_functable; l->name; l++)
  // 	{
  // 	  lua_pushstring(state, l->name);
  // 	  lua_pushlightuserdata(state, (void*)l);
  // 	  lua_pushcclosure(state, thunk_lua, 1);
  // 	  lua_settable(state, methods);
  // 	}
 
  lua_pop(state, 2);
}
*/


//static const char *dictpath;

static int initdict (lua_State *L) {
  // dictpath = luaL_checkstring(L, 1);
  static const char *dictpath = luaL_checkstring(L, 1);
  // mgr = new css::SegmenterManager();
  // if(mgr == NULL){
  //   mgr = new css::SegmenterManager();
  // }
  int ret = mgr.init(dictpath);
  if (ret == 0){
	//成功
  }
  else {
	//失败
  }
  return 0;
}

static int segment (lua_State *L) {
  // if(mgr == NULL){
  //   mgr = new css::SegmenterManager();
  // }
  // css::SegmenterManager* mgr = new css::SegmenterManager();
  // int ret = mgr->init(dictpath);
  // if (ret == 0){
  //       //成功
  // }
  // else {
  //       //失败
  // }

  unsigned int n = 1;
  Segmenter *seg = mgr.getSegmenter(); 
  const char *txt = luaL_checkstring(L, 1);
  seg->setBuffer((u1*)txt, (u4)strlen(txt));

  lua_newtable(L);
  while(1) {
	u2 len = 0, symlen = 0;
	char* tok = (char*)seg->peekToken(len,symlen);
	if(!tok || !*tok || !len){
	  break;
	}
	// add to lua string
	lua_pushlstring(L, tok, len);
	// add to return table
	lua_rawseti(L, -2, n);
	n++;

	seg->popToken(len);
  }
  mgr.clear();
  // delete mgr;
  // delete seg;
  // mgr = NULL;
  return 1;
}
  
static const struct luaL_Reg mmseglib [] = {
  {"initdict", initdict},
  {"segment", segment},
  {NULL, NULL}
};

extern "C" {
  int luaopen_mmseg (lua_State *L) {
	luaL_register(L, "mmseg", mmseglib);
	return 1;
  }
}

// int main(){
//   lua_State *L;

//   L = luaL_newstate();
//   luaL_openlibs(L);
//   luaL_dofile(L, "test.lua");
//   lua_close(L);
//   return 0;
// }
