#include "ms3d_lua_script.h"
#include "xr_file_system.h"
#include "msLib.h"
#include <windows.h>

using namespace xray_re;
static msModel*  pModel;

int Lua_print(lua_State* L)
{
	int args = lua_gettop(L);
	while (args > 0)
	{
		const char* tmp = luaL_tolstring(L, -args, NULL);
		msg("%s", tmp);
		lua_pop(L, 1);
		args--;
	}
	return 0;
}

int Lua_printf(lua_State* L)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "string");
	lua_getfield(L, -1, "format");
	int i = 1;
	while (i <= top) lua_pushvalue(L, i++);
	lua_call(L, top, 1);
	const char* _str = lua_tostring(L, -1);
	lua_settop(L, 0);
	lua_pushstring(L, _str);
	Lua_print(L);
	return 0;
}

int Lua_msModel_GetMeshCount(lua_State* L)
{
	msModel* pModel = (msModel*) lua_touserdata(L, 1);
	int mesh_cnt = msModel_GetMeshCount(pModel);
	lua_pushnumber(L, mesh_cnt);
	return 1;
}

int Lua_GetModel(lua_State* L)
{
	if (!pModel) return 0;
	lua_pushlightuserdata(L, pModel);
	return 1;
}

int Lua_msModel_GetMeshAt(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = lua_tointeger(L, 2);
	msMesh* pMesh = msModel_GetMeshAt(pModel, idx);
	if (!pMesh) return 0;
	lua_pushlightuserdata(L, pMesh);
	return 1;
}

int Lua_msModel_GetMaterialAt(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = lua_tointeger(L, 2);
	msMaterial* pMtl = msModel_GetMaterialAt(pModel, idx);
	if (!pMtl) return 0;
	lua_pushlightuserdata(L, pMtl);
	return 1;
}

int Lua_msModel_GetBoneAt(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = lua_tointeger(L, 2);
	msBone* pBone = msModel_GetBoneAt(pModel, idx);
	if (!pBone) return 0;
	lua_pushlightuserdata(L, pBone);
	return 1;
}

int Lua_msMesh_GetVertexCount(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int cnt = msMesh_GetVertexCount(pMesh);
	lua_pushnumber(L, cnt);
	return 1;
}
int Lua_msMesh_GetVertexNormalCount(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int cnt = msMesh_GetVertexNormalCount(pMesh);
	lua_pushnumber(L, cnt);
	return 1;
}
int Lua_msMesh_GetTriangleCount(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int cnt = msMesh_GetTriangleCount(pMesh);
	lua_pushnumber(L, cnt);
	return 1;
}

int Lua_msMesh_GetVertexAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	word idx = lua_tointeger(L, 2);
	msVertex* pVert = msMesh_GetVertexAt(pMesh, idx);
	if (!pVert) return 0;
	lua_pushlightuserdata(L, pVert);
	return 1;
}

int Lua_msMesh_GetVertexExAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	word idx = lua_tointeger(L, 2);
	msVertexEx* pVert = msMesh_GetVertexExAt(pMesh, idx);
	if (!pVert) return 0;
	lua_pushlightuserdata(L, pVert);
	return 1;
}

int Lua_msMesh_GetVertexNormalAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = lua_tointeger(L, 2);
	msVec3 nrm;
	msMesh_GetVertexNormalAt(pMesh, idx, nrm);
	lua_pushnumber(L, nrm[0]);
	lua_pushnumber(L, nrm[1]);
	lua_pushnumber(L, nrm[2]);
	return 3;
}

int Lua_msMesh_SetVertexNormalAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = lua_tointeger(L, 2);
	msVec3 nrm;
	nrm[0] = lua_tonumber(L,3);
	nrm[1] = lua_tonumber(L,4);
	nrm[2] = lua_tonumber(L,5);
	msMesh_SetVertexNormalAt(pMesh, idx, nrm);
	return 0;
}
int Lua_msMesh_GetTriangleAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = lua_tointeger(L, 2);
	msTriangle* pTri = msMesh_GetTriangleAt(pMesh, idx);
	if (!pTri) return 0;
	lua_pushlightuserdata(L, pTri);
	return 1;
}

int Lua_msMesh_GetTriangleExAt(lua_State* L)
{
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = lua_tointeger(L, 2);
	msTriangleEx* pTri = msMesh_GetTriangleExAt(pMesh, idx);
	if (!pTri) return 0;
	lua_pushlightuserdata(L, pTri);
	return 1;
}

int Lua_msTriangle_GetVertexIndices(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word nIndices[3];
	msTriangle_GetVertexIndices(pTriangle, nIndices);
	lua_pushinteger(L, nIndices[0]);
	lua_pushinteger(L, nIndices[1]);
	lua_pushinteger(L, nIndices[2]);
	return 3;
}

int Lua_msTriangle_SetVertexIndices(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word nIndices[3];
	nIndices[0] = lua_tointeger(L,2);
	nIndices[1] = lua_tointeger(L,3);
	nIndices[2] = lua_tointeger(L,4);
	msTriangle_SetVertexIndices(pTriangle, nIndices);
	return 0;
}

int Lua_msTriangle_GetNormalIndices(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word nIndices[3];
	msTriangle_GetNormalIndices(pTriangle, nIndices);
	lua_pushinteger(L, nIndices[0]);
	lua_pushinteger(L, nIndices[1]);
	lua_pushinteger(L, nIndices[2]);
	return 3;
}

int Lua_msTriangle_SetNormalIndices(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word nIndices[3];
	nIndices[0] = lua_tointeger(L, 2);
	nIndices[1] = lua_tointeger(L, 3);
	nIndices[2] = lua_tointeger(L, 4);
	msTriangle_SetNormalIndices(pTriangle, nIndices);
	return 3;
}

int Lua_GetTriangleNormal(lua_State* L)
{
	msMesh *pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	msTriangle *pTriangle = msMesh_GetTriangleAt(pMesh, lua_tointeger(L, 2));
	if (!pTriangle) return 0;
	word nIndices[3];
	msTriangle_GetVertexIndices(pTriangle, nIndices);
	msVertex *pVertex;
	float dx0, dy0, dz0, dx1, dy1, dz1;
	pVertex = msMesh_GetVertexAt(pMesh, nIndices[0]);
	//msg( "vert %d: %f %f %f   ", nIndices[0], pVertex->Vertex[0], pVertex->Vertex[1], pVertex->Vertex[2]);
	dx0 = pVertex->Vertex[0]; dy0 = pVertex->Vertex[1]; dz0 = pVertex->Vertex[2];
	pVertex = msMesh_GetVertexAt(pMesh, nIndices[1]);
	//msg( "vert %d: %f %f %f   ", nIndices[1], pVertex->Vertex[0], pVertex->Vertex[1], pVertex->Vertex[2]);
	dx1 = pVertex->Vertex[0]; dy1 = pVertex->Vertex[1]; dz1 = pVertex->Vertex[2];
	pVertex = msMesh_GetVertexAt(pMesh, nIndices[2]);
	//msg( "vert %d: %f %f %f\n", nIndices[2], pVertex->Vertex[0], pVertex->Vertex[1], pVertex->Vertex[2]);
	dx0 -= pVertex->Vertex[0]; dy0 -= pVertex->Vertex[1]; dz0 -= pVertex->Vertex[2];
	dx1 -= pVertex->Vertex[0]; dy1 -= pVertex->Vertex[1]; dz1 -= pVertex->Vertex[2];
	float nx, ny, nz;
	nx = dy0*dz1 - dz0*dy1;
	ny = dz0*dx1 - dx0*dz1;
	nz = dx0*dy1 - dy0*dx1;
	float norm_length = sqrt(nx*nx + ny*ny + nz*nz);
	nx /= norm_length;
	ny /= norm_length;
	nz /= norm_length;
	lua_pushnumber(L, nx);
	lua_pushnumber(L, ny);
	lua_pushnumber(L, nz);
	return 3;
}
int Lua_GetVertexPosition(lua_State* L)
{
	msVertex *pVertex = (msVertex*)lua_touserdata(L, 1);
	if (!pVertex) return 0;
	lua_pushnumber(L, pVertex->Vertex[0]);
	lua_pushnumber(L, pVertex->Vertex[1]);
	lua_pushnumber(L, pVertex->Vertex[2]);
	return 3;
}

int Lua_msTriangle_SetSmoothingGroup(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	byte sg = lua_tointeger(L, 2);
	sg &= 31;
	msTriangle_SetSmoothingGroup(pTriangle, sg);
	return 0;
}

int Lua_msTriangle_GetSmoothingGroup(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	lua_Integer sg = msTriangle_GetSmoothingGroup(pTriangle);
	lua_pushinteger(L, sg);
	return 1;
}

int Lua_msTriangle_GetFlags(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word fl = msTriangle_GetFlags(pTriangle);
	lua_pushinteger(L, fl);
	return 1;
}

int Lua_msTriangle_SetFlags(lua_State* L)
{
	msTriangle *pTriangle = (msTriangle*)lua_touserdata(L, 1);
	if (!pTriangle) return 0;
	word fl = lua_tointeger(L, 2);
	msTriangle_SetFlags(pTriangle, fl);
	return 0;
}

static const luaL_Reg _msFlags[] = {
	{ "eSelected", NULL },
	{ "eSelected2", NULL },
	{ "eHidden", NULL },
	{ NULL, NULL },
};

LUAMOD_API int init_msFlags(lua_State* L)
{
	luaL_newlib(L, _msFlags);
	lua_pushnumber(L, eSelected);
	lua_setfield(L, -2, "eSelected");
	lua_pushnumber(L, eSelected2);
	lua_setfield(L, -2, "eSelected2");
	lua_pushnumber(L, eHidden);
	lua_setfield(L, -2, "eHidden");
	lua_pushnumber(L, eDirty);
	lua_setfield(L, -2, "eDirty");
	lua_pushnumber(L, eAveraged);
	lua_setfield(L, -2, "eAveraged");
	lua_pushnumber(L, eKeepVertex);
	lua_setfield(L, -2, "eKeepVertex");
	lua_pushnumber(L, eSphereMap);
	lua_setfield(L, -2, "eSphereMap");
	lua_pushnumber(L, eHasAlpha);
	lua_setfield(L, -2, "eHasAlpha");
	lua_pushnumber(L, eCombineAlpha);
	lua_setfield(L, -2, "eCombineAlpha");
	lua_pushnumber(L, eIgnoreAlpha);
	lua_setfield(L, -2, "eIgnoreAlpha");
	return 1;
}

int Lua_msVertex_GetFlags(lua_State* L)
{
	msVertex *pVertex = (msVertex*)lua_touserdata(L, 1);
	if (!pVertex) return 0;
	word fl = msVertex_GetFlags(pVertex);
	lua_pushinteger(L, fl);
	return 1;
}

int Lua_msVertex_SetFlags(lua_State* L)
{
	msVertex *pVertex = (msVertex*)lua_touserdata(L, 1);
	if (!pVertex) return 0;
	word fl = lua_tointeger(L, 2);
	msVertex_SetFlags(pVertex, fl);
	return 0;
}

int Lua_msMesh_GetFlags(lua_State* L)
{
	msMesh *pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	word fl = msMesh_GetFlags(pMesh);
	lua_pushinteger(L, fl);
	return 1;
}

int Lua_msMesh_SetFlags(lua_State* L)
{
	msMesh *pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	word fl = lua_tointeger(L, 2);
	msMesh_SetFlags(pMesh, fl);
	return 0;
}

int Lua_SendText(lua_State* L)
{
	HWND hwnd = GetForegroundWindow();
	const char* lpString = lua_tostring(L, 1);
	if (hwnd)
	{
		SetWindowText(hwnd, lpString);
	}
	return 0;
}

int Lua_msModel_Destroy(lua_State* L)
{
	if (pModel)
	{
		msModel_Destroy(pModel);
		pModel = NULL;
	}
	return 0;
}

int Lua_msModel_FindMeshByName(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	const char* mesh_name = lua_tostring(L, 2);
	int idx = msModel_FindMeshByName(pModel, mesh_name);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_FindMaterialByName(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	const char* mtl_name = lua_tostring(L, 2);
	int idx = msModel_FindMaterialByName(pModel, mtl_name);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_FindBoneByName(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	const char* bone_name = lua_tostring(L, 2);
	int idx = msModel_FindBoneByName(pModel, bone_name);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_GetMaterialCount(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = msModel_GetMaterialCount(pModel);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_GetBoneCount(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = msModel_GetBoneCount(pModel);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_AddMaterial(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = msModel_AddMaterial(pModel);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_AddBone(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int idx = msModel_AddBone(pModel);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msModel_SetFrame(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int nFrame = lua_tointeger(L, 2);
	int res = msModel_SetFrame(pModel, nFrame);
	lua_pushinteger(L, res);
	return 1;
}

int Lua_msModel_GetFrame(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int res = msModel_GetFrame(pModel);
	lua_pushinteger(L, res);
	return 1;
}

int Lua_msModel_SetTotalFrames(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int nFrame = lua_tointeger(L, 2);
	int res = msModel_SetFrame(pModel, nFrame);
	lua_pushinteger(L, res);
	return 1;
}

int Lua_msModel_GetTotalFrames(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int res = msModel_GetFrame(pModel);
	lua_pushinteger(L, res);
	return 1;
}

int Lua_msModel_SetPosition(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	msVec3 v;
	v[0] = lua_tonumber(L, 2);
	v[1] = lua_tonumber(L, 3);
	v[2] = lua_tonumber(L, 4);
	msModel_SetPosition(pModel, v);
	return 0;
}

int Lua_msModel_GetPosition(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	msVec3 v;
	msModel_GetPosition(pModel, v);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);
	return 3;
}
int Lua_msModel_SetRotation(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	msVec3 v;
	v[0] = lua_tonumber(L, 2);
	v[1] = lua_tonumber(L, 3);
	v[2] = lua_tonumber(L, 4);
	msModel_SetRotation(pModel, v);
	return 0;
}

int Lua_msModel_GetRotation(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	msVec3 v;
	msModel_GetRotation(pModel, v);
	lua_pushnumber(L, v[0]);
	lua_pushnumber(L, v[1]);
	lua_pushnumber(L, v[2]);
	return 3;
}

int Lua_msModel_SetComment(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	const char* comment = lua_tostring(L, 2);
	msModel_SetComment(pModel, comment);
	return 0;
}

int Lua_msModel_GetComment(lua_State* L)
{
	msModel* pModel = (msModel*)lua_touserdata(L, 1);
	if (!pModel) return 0;
	int comment_size = msModel_GetComment(pModel, 0, 0);
	char* comment = new char[comment_size+1];
	msModel_GetComment(pModel, comment, comment_size);
	lua_pushstring(L, comment);
	delete[] comment;
	return 1;
}

int Lua_msMesh_Destroy(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (pMesh) msMesh_Destroy(pMesh);
	return 0;
}

int Lua_msMesh_SetName(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	const char* name = lua_tostring(L, 2);
	msMesh_SetName(pMesh, name);
	return 0;
}

int Lua_msMesh_GetName(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	char name[MS_MAX_NAME] = { 0 };
	msMesh_GetName(pMesh, name, MS_MAX_NAME);
	lua_pushstring(L, name);
	return 1;
}

int Lua_msMesh_SetComment(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	const char* comment = lua_tostring(L, 2);
	msMesh_SetComment(pMesh, comment);
	return 0;
}

int Lua_msMesh_GetComment(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int size = msMesh_GetComment(pMesh, 0, 0);
	char* comment = new char[size+1];
	msMesh_GetComment(pMesh, comment, size);
	comment[size] = '\0';
	lua_pushstring(L, comment);
	delete[] comment;
	return 1;
}

int Lua_msMesh_SetMaterialIndex(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = lua_tointeger(L, 2);
	msMesh_SetMaterialIndex(pMesh, idx);
	return 0;
}

int Lua_msMesh_GetMaterialIndex(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = msMesh_GetMaterialIndex(pMesh);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msMesh_AddVertex(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = msMesh_AddVertex(pMesh);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msMesh_AddTriangle(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = msMesh_AddTriangle(pMesh);
	lua_pushinteger(L, idx);
	return 1;
}

int Lua_msMesh_AddVertexNormal(lua_State* L)
{	
	msMesh* pMesh = (msMesh*)lua_touserdata(L, 1);
	if (!pMesh) return 0;
	int idx = msMesh_AddVertexNormal(pMesh);
	lua_pushinteger(L, idx);
	return 1;
}
void init_lua_libs(lua_State* L, msModel* model)
{
	pModel = model;
	luaL_openlibs(L);

	luaL_requiref(L, "msFlags", init_msFlags, 1);

	lua_register(L, "print", &Lua_print);
	lua_register(L, "printf", &Lua_printf);

	// model
	lua_register(L, "GetModel", &Lua_GetModel);
	lua_register(L, "msModel_Destroy", &Lua_msModel_Destroy);
	lua_register(L, "msModel_GetMeshCount", &Lua_msModel_GetMeshCount);
	lua_register(L, "msModel_GetMeshAt", &Lua_msModel_GetMeshAt);
	lua_register(L, "msModel_FindMeshByName", &Lua_msModel_FindMeshByName);

	lua_register(L, "msModel_GetMaterialCount", &Lua_msModel_GetMaterialCount);
	lua_register(L, "msModel_AddMaterial", &Lua_msModel_AddMaterial);
	lua_register(L, "msModel_GetMaterialAt", &Lua_msModel_GetMaterialAt);
	lua_register(L, "msModel_FindMaterialByName", &Lua_msModel_FindMaterialByName);
	
	lua_register(L, "msModel_GetBoneCount", &Lua_msModel_GetBoneCount);
	lua_register(L, "msModel_AddBone", &Lua_msModel_AddBone);
	lua_register(L, "msModel_GetBoneAt", &Lua_msModel_GetBoneAt);
	lua_register(L, "msModel_FindBoneByName", &Lua_msModel_FindBoneByName);

	lua_register(L, "msModel_SetFrame", &Lua_msModel_SetFrame);
	lua_register(L, "msModel_GetFrame", &Lua_msModel_GetFrame);
	lua_register(L, "msModel_SetTotalFrames", &Lua_msModel_SetTotalFrames);
	lua_register(L, "msModel_GetTotalFrames", &Lua_msModel_GetTotalFrames);
	lua_register(L, "msModel_SetPosition", &Lua_msModel_SetPosition);
	lua_register(L, "msModel_GetPosition", &Lua_msModel_GetPosition);
	lua_register(L, "msModel_SetRotation", &Lua_msModel_SetRotation);
	lua_register(L, "msModel_GetRotation", &Lua_msModel_GetRotation);
	lua_register(L, "msModel_SetComment", &Lua_msModel_SetComment);
	lua_register(L, "msModel_GetComment", &Lua_msModel_GetComment);

	//mesh
	lua_register(L, "msMesh_Destroy", &Lua_msMesh_Destroy);
	lua_register(L, "msMesh_SetFlags", &Lua_msMesh_SetFlags);
	lua_register(L, "msMesh_GetFlags", &Lua_msMesh_GetFlags);
	lua_register(L, "msMesh_SetName", &Lua_msMesh_SetName);
	lua_register(L, "msMesh_GetName", &Lua_msMesh_GetName);
	lua_register(L, "msMesh_SetMaterialIndex", &Lua_msMesh_SetMaterialIndex);
	lua_register(L, "msMesh_GetMaterialIndex", &Lua_msMesh_GetMaterialIndex);

	lua_register(L, "msMesh_GetVertexCount", &Lua_msMesh_GetVertexCount); 
	lua_register(L, "msMesh_AddVertex", &Lua_msMesh_AddVertex);
	lua_register(L, "msMesh_GetVertexAt", &Lua_msMesh_GetVertexAt);
	lua_register(L, "msMesh_GetVertexExAt", &Lua_msMesh_GetVertexExAt);

	lua_register(L, "msMesh_GetTriangleCount", &Lua_msMesh_GetTriangleCount);
	lua_register(L, "msMesh_AddTriangle", &Lua_msMesh_AddTriangle);
	lua_register(L, "msMesh_GetTriangleAt", &Lua_msMesh_GetTriangleAt);
	lua_register(L, "msMesh_GetTriangleExAt", &Lua_msMesh_GetTriangleExAt);

	lua_register(L, "msMesh_GetVertexNormalCount", &Lua_msMesh_GetVertexNormalCount);
	lua_register(L, "msMesh_AddVertexNormal", &Lua_msMesh_AddVertexNormal);
	lua_register(L, "msMesh_SetVertexNormalAt", &Lua_msMesh_SetVertexNormalAt);
	lua_register(L, "msMesh_GetVertexNormalAt", &Lua_msMesh_GetVertexNormalAt);
	lua_register(L, "msMesh_SetComment", &Lua_msMesh_SetComment);
	lua_register(L, "msMesh_GetComment", &Lua_msMesh_GetComment);

	lua_register(L, "GetTriangleNormal", &Lua_GetTriangleNormal);

	//triangle
	lua_register(L, "msTriangle_SetFlags", &Lua_msTriangle_SetFlags);
	lua_register(L, "msTriangle_GetFlags", &Lua_msTriangle_GetFlags);
	lua_register(L, "msTriangle_SetVertexIndices", &Lua_msTriangle_SetVertexIndices);
	lua_register(L, "msTriangle_GetVertexIndices", &Lua_msTriangle_GetVertexIndices);
	lua_register(L, "msTriangle_SetNormalIndices", &Lua_msTriangle_SetNormalIndices);
	lua_register(L, "msTriangle_GetNormalIndices", &Lua_msTriangle_GetNormalIndices);
	lua_register(L, "msTriangle_SetSmoothingGroup", &Lua_msTriangle_SetSmoothingGroup);
	lua_register(L, "msTriangle_GetSmoothingGroup", &Lua_msTriangle_GetSmoothingGroup);
	lua_register(L, "GetVertexPosition", &Lua_GetVertexPosition);
	
	//vertexes
	lua_register(L, "msVertex_SetFlags", &Lua_msVertex_SetFlags);
	lua_register(L, "msVertex_GetFlags", &Lua_msVertex_GetFlags);

	lua_register(L, "SendText", &Lua_SendText);
}

void close_lua(lua_State* L)
{
	pModel = NULL;
	lua_close(L);
}
