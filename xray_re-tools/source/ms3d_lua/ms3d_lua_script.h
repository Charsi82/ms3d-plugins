#ifndef __MS3D_XRAY_LUA_SCRIPT_H__
#define __MS3D_XRAY_LUA_SCRIPT_H__
#include "src\lua.hpp"
#include "msLib.h"

void init_lua_libs(lua_State* L, msModel* model);
void close_lua(lua_State* L);
#endif