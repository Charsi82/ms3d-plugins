#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_lua.h"
#include "xr_log.h"
#include "xr_file_system.h"
#include "msLib.h"
#include "ms3d_lua_script.h"

using namespace xray_re;
using namespace std;

static bool g_fs_inited = false;

cMsPlugIn* CreatePlugIn()
{
	if (!g_fs_inited) {
		char path[MAX_PATH + 16];
		DWORD n = GetModuleFileName(NULL, path, xr_dim(path));
		if (n && PathRemoveFileSpec(path)) {
			strcat_s(path, sizeof(path), "\\xray_path.ltx");
			xr_file_system& fs = xr_file_system::instance();
			if (!fs.initialize(path))
				MessageBox(NULL, _T("can't initialize the file system"), _T("X-Ray error"), MB_OK);
		}
		g_fs_inited = true;
		xr_log::instance().init("xrayMS3DLua");
	}
	return new ms3d_xray_lua;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_lua::~ms3d_xray_lua() {}

int ms3d_xray_lua::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }
const char* ms3d_xray_lua::GetTitle() { return "X-Ray Run LUA script"; }

//struct tri_info{
//	int self_ind;
//	word vert0, vert1, vert2;
//	float norm0, norm1, norm2;
//	int smg_id;
//	bool nosg;
//	bool adjacent(const tri_info &a){
//		if (self_ind == a.self_ind) return false;
//		int res = 0;
//		if (vert0 == a.vert0 || vert1 == a.vert0 || vert2 == a.vert0) res++;
//		if (vert0 == a.vert1 || vert1 == a.vert1 || vert2 == a.vert1) res++;
//		if (vert0 == a.vert2 || vert1 == a.vert2 || vert2 == a.vert2) res++;
//		return res == 2;
//	}
//};


int ms3d_xray_lua::Execute(msModel* model)
{
	char WndCaption[1024] = { 0 };
	HWND hwnd = GetForegroundWindow();
	if (hwnd)
	{
		GetWindowText(hwnd, WndCaption, 1024);
	}
	lua_State* L = luaL_newstate();
	init_lua_libs(L, model);
	char script_path[MAX_PATH + 16];
	DWORD n = GetModuleFileName(NULL, script_path, xr_dim(script_path));
	if (n && PathRemoveFileSpec(script_path)) {
		strcat_s(script_path, sizeof(script_path), "\\plugin.lua");
		int rslt = luaL_dofile(L, script_path);
		if (rslt) 
			msg("res = %s",lua_tostring(L, -1));
	}
	close_lua(L);
	SetWindowText(hwnd, WndCaption);
	//xr_file_system& fs = xr_file_system::instance();
	//std::string path;
	//if (fs.resolve_path(PA_LOGS, "xrayMS3DInfo", path))
	//	ShellExecute(NULL, "", path.append(".log").c_str(), NULL, NULL, SW_SHOWNORMAL);

	//msModel_Destroy(model);
	MessageBox(NULL, "Готово!", "MS3D Lua Plugin", MB_OK|MB_ICONINFORMATION);
	return 0;
}
#ifdef qwe
int ms3d_xray_lua::Execute(msModel* model)
{
	int mesh_cnt = msModel_GetMeshCount(model);
	msg("mesh_cnt: %d", mesh_cnt);
	vector<tri_info> vtri;
	
	for (int i = 0; i < msModel_GetMeshCount(model); i++)
	{
		msMesh *pMesh = msModel_GetMeshAt(model, i);
		char szName[64];
		msMesh_GetName(pMesh, szName, 64);
		msg("submesh_name: %s", szName);
		vtri.reserve(msMesh_GetTriangleCount(pMesh));
		for (int j = 0; j < msMesh_GetTriangleCount(pMesh); j++)
		{
			msTriangle *pTriangle = msMesh_GetTriangleAt(pMesh, j);
			word nIndices[3];
			msTriangle_GetVertexIndices(pTriangle, nIndices);
			//msg("face %d: ", j);
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

			//if (j % 2 == 0){
			//	byte fl = msVertex_GetFlags(pVertex);
			//	fl |= eSelected;
			//	msVertex_SetFlags(pVertex, fl);
			//}
			tri_info _tri;
			_tri.self_ind = j;
			_tri.smg_id = 1;
			_tri.vert0 = nIndices[0];
			_tri.vert1 = nIndices[1];
			_tri.vert2 = nIndices[2];
			_tri.norm0 = dy0*dz1 - dz0*dy1;
			_tri.norm1 = dz0*dx1 - dx0*dz1;
			_tri.norm2 = dx0*dy1 - dy0*dx1;
			_tri.nosg = true;
			vtri.push_back(_tri);
			msTriangle_SetSmoothingGroup(pTriangle, 1);
		}

		msTriangle *pTriangle;
		//for (auto tmp_tri : vtri)
		//{
		//	pTriangle = msMesh_GetTriangleAt(pMesh, tmp_tri.self_ind);
		//	msTriangle_SetSmoothingGroup(pTriangle, 1);
		//}
		byte sg = 1;
		
		//for (auto tmp_tri : vtri){
		//	msg(" norm >>[%d] %f %f %f", tmp_tri.self_ind, tmp_tri.norm0, tmp_tri.norm1, tmp_tri.norm2);
		//	byte sg_id = 1;
		//	float cp = -1;
		//	for (int j = 0; j < 26; ++j){
		//		fvector3 m0, m1;
		//		m0.set(tmp_tri.norm0, tmp_tri.norm1, tmp_tri.norm2);
		//		m1.set(sgnorm[j]);
		//		float cp1 = m0.dot_product(m1);
		//		if (cp1>cp)
		//		{
		//			cp = cp1;
		//			sg_id = j+1;
		//		}
		//		pTriangle = msMesh_GetTriangleAt(pMesh, tmp_tri.self_ind);
		//		msTriangle_SetSmoothingGroup(pTriangle, sg_id);
		//		tmp_tri.smg_id = sg_id;
		//	}
		//}
		for (auto tmp_tri : vtri)
		for (auto tmp_tri2 : vtri)
		{
			if (tmp_tri2.nosg && tmp_tri2.adjacent(tmp_tri))
			{
				//sg = msTriangle_GetSmoothingGroup(msMesh_GetTriangleAt(pMesh, tmp_tri.self_ind));
				//if (tmp_tri.smg_id == -1) tmp_tri.smg_id = sg;
				msg("%d (sg = %d) adjacent with %d", tmp_tri.self_ind, sg, tmp_tri2.self_ind);
				//if (tmp_tri2.smg_id == -1  /*&& tmp_tri2.smg_id != tmp_tri.smg_id*/)
				//{
				//	fvector3 m, m2;
				//	m.set(tmp_tri.norm0, tmp_tri.norm1, tmp_tri.norm2);
				//	m2.set(tmp_tri2.norm0, tmp_tri2.norm1, tmp_tri2.norm2);
				//	m.normalize();
				//	m2.normalize();
				//	msg("m0: %f %f %f   ", m.x, m.y, m.z);
				//	msg("m1: %f %f %f   ", m2.x, m2.y, m2.z);
				//	pTriangle = msMesh_GetTriangleAt(pMesh, tmp_tri2.self_ind);
				//	if (m.dot_product(m2) > .7f){
				//		tmp_tri2.smg_id = tmp_tri.smg_id;						
				//		msg("set sg %d",tmp_tri2.smg_id);
				//		msTriangle_SetSmoothingGroup(pTriangle, tmp_tri.smg_id);
				//	}
				//	else{
				//		tmp_tri2.smg_id = ++sg;	msg("set_ sg %d",tmp_tri2.smg_id);					
				//		msTriangle_SetSmoothingGroup(pTriangle, sg);
				//	}

					//pTriangle = msMesh_GetTriangleAt(pMesh, tmp_tri2.self_ind);
					//sg = msTriangle_GetSmoothingGroup(pTriangle);
					//if (tmp_tri2.flat(tmp_tri)) sg++;
					//if (sg > 32) { sg = 32; msg("model to complex"); };
					//
					//msg("set sg %d to face %d", sg, tmp_tri.self_ind);
					//msTriangle_SetSmoothingGroup(pTriangle, sg);
				//}
			}
		}	
	}
	//sort(vtri.begin(), vtri.end(), [](const tri_info &a, const tri_info &b){return a.vert0 < b.vert0 && a.vert1 < b.vert1 && a.vert2 < b.vert2; });
	//foreach(vtri.begin());
	//for (int i = 0;i< mesh_cnt;++i)
	//{
	//	msMesh* pMesh = msModel_GetMeshAt(model, i);
	//	//int tri_cnt = msMesh_GetTriangleCount(pMesh);
	//	int j = 0;
	//	while (msMesh_GetTriangleAt(pMesh, j++));
	//	msg("mesh: %u, triangles: %u", i, j);
	//}
/*	if (msModel_GetMeshCount(model) == 0) //--
	{
		MessageBox(NULL, "The scene is empty!", "X-Ray Model Information", MB_OK | MB_ICONWARNING);
		return 0;
	}
	
	std::string info = "";
	char *pszComment = 0;
	int nCommentLength = msModel_GetComment(model, NULL, 0);
	if (nCommentLength > 0)
	{
		pszComment = new char[nCommentLength + 1];
		msModel_GetComment(model, pszComment, nCommentLength);
		pszComment[nCommentLength] = '\0';

		info.append(pszComment);
		delete[] pszComment;
	}


	msMesh *pMesh = msModel_GetMeshAt(model, 0);
	nCommentLength = msMesh_GetComment(pMesh, NULL, 0);

	if (nCommentLength){

		pszComment = new char[nCommentLength + 1];
		msMesh_GetComment(pMesh, pszComment, nCommentLength);
		pszComment[nCommentLength] = '\0';

		info.append("[userdata]\r\n").append(pszComment);
		delete[] pszComment;
	}*/

	/*int nNumComments = 0;
	int i = 0;
	// model comments
	{
		nNumComments = 0;
		int nCommentLength = msModel_GetComment(model, NULL, 0);
		if (nCommentLength > 0)
		{
			char *pszComment = new char[nCommentLength + 1];
			msModel_GetComment(model, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';

			info.append("\r\n~~~ Userdata ~~~\r\n\r\n ").append(pszComment);
			delete[] pszComment;
		}
	}

	// material comments
	{
		nNumComments = 0;
		for (i = 0; i < msModel_GetMaterialCount(model); i++)
		{
			msMaterial *pMaterial = msModel_GetMaterialAt(model, i);
			int nCommentLength = msMaterial_GetComment(pMaterial, NULL, 0);
			if (nCommentLength > 0)
				++nNumComments;
		}
		if (nNumComments){
			info.append("\r\n\r\n~~~ Materials ~~~");
		}

		for (i = 0; i < msModel_GetMaterialCount(model); i++)
		{
			msMaterial *pMaterial = msModel_GetMaterialAt(model, i);
			int nCommentLength = msMaterial_GetComment(pMaterial, NULL, 0);
			if (nCommentLength > 0)
			{
				char *pszComment = new char[nCommentLength + 1];
				msMaterial_GetComment(pMaterial, pszComment, nCommentLength);
				pszComment[nCommentLength] = '\0';
				
				char* buf = new char[260 + 1];
				msMaterial_GetName(pMaterial, buf, MS_MAX_NAME);
				info.append("\r\n\r\n ").append(buf).append(":\r\ntexture = ");
				
				msMaterial_GetDiffuseTexture(pMaterial, buf, 260);
				std::string tmp = "";
				tmp.append(buf);
				int pos = tmp.find("textures\\");
				if (pos > -1)
				{
				tmp = tmp.substr(pos + 9);
				tmp = tmp.substr(0, tmp.length()-4);
				info.append(tmp);
				}
				else
					info.append(buf);
				info.append("\r\n").append(pszComment);
				delete[] buf;
				delete[] pszComment;
			}
		}
	}

	// joint comments
	{
		nNumComments = 0;
		for (i = 0; i < msModel_GetBoneCount(model); i++)
		{
			msBone *pBone = msModel_GetBoneAt(model, i);
			int nCommentLength = msBone_GetComment(pBone, NULL, 0);
			if (nCommentLength > 0)
				++nNumComments;
		}
		if (nNumComments){
			info.append("\r\n\r\n~~~ Joints ~~~\r\n");
		}

		for (i = 0; i < msModel_GetBoneCount(model); i++)
		{
			
			msBone *pBone = msModel_GetBoneAt(model, i);
			int nCommentLength = msBone_GetComment(pBone, NULL, 0);
			if (nCommentLength > 0)
			{
				//fprintf(file, "%d\n", i);
				char *pszComment = new char[nCommentLength + 1];
				msBone_GetComment(pBone, pszComment, nCommentLength);
				pszComment[nCommentLength] = '\0';

				char* bname = new char[128];
				msBone_GetName(pBone,bname, 128);
				char* buf = new char[128];
				
				sprintf_s(buf,128, "\r\n %02d: %s > %s", i+1, bname, pszComment);
				info.append(buf);
				delete[] buf;
				delete[] bname;
				delete[] pszComment;
			}
		}
	}

	// animations
	{
		nNumComments = msModel_GetMeshCount(model);

		if (nNumComments){

			msMesh *pMesh = msModel_GetMeshAt(model, 0);
			int nCommentLength = msMesh_GetComment(pMesh, NULL, 0);

			if(nCommentLength){

				char *pszComment = new char[nCommentLength + 1];
				msMesh_GetComment(pMesh, pszComment, nCommentLength);
				pszComment[nCommentLength] = '\0';

				info.append("\r\n\r\n~~~ Animations ~~~\r\n\r\n");
				info.append(pszComment);
				delete[] pszComment;
			}
		}
	}
	*/
	/////////////////////////////////////
/*	if(info.size()){	//--
		msg(info.c_str());
		xr_file_system& fs = xr_file_system::instance();
		std::string path;
		if (fs.resolve_path(PA_LOGS, "xrayMS3DInfo", path))
			ShellExecute(NULL, "", path.append(".log").c_str(), NULL, NULL, SW_SHOWNORMAL );
	}
	else
		MessageBox(NULL, "No information about this scene!", "X-Ray Model Information", MB_OK | MB_ICONWARNING);
	*/
xr_file_system& fs = xr_file_system::instance();
std::string path;
if (fs.resolve_path(PA_LOGS, "xrayMS3DInfo", path))
ShellExecute(NULL, "", path.append(".log").c_str(), NULL, NULL, SW_SHOWNORMAL);
	return 0;
}
#endif