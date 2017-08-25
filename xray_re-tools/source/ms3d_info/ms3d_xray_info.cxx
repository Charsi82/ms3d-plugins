#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_info.h"
#include "xr_log.h"
#include "xr_file_system.h"
#include "msLib.h"

using namespace xray_re;

static bool g_fs_inited = false;

BOOL WINAPI DllMain(HINSTANCE hinstance, ULONG reason, LPVOID reserved)
{
	return TRUE;
}

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
		xr_log::instance().init("xrayMS3DInfo");
	}
	return new ms3d_xray_info;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_info::~ms3d_xray_info() {}

int ms3d_xray_info::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }

const char* ms3d_xray_info::GetTitle() { return "X-Ray Model Information"; }

int ms3d_xray_info::Execute(msModel* model)
{
	if (msModel_GetMeshCount(model) == 0)
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
	}

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
	if(info.size()){	
		msg(info.c_str());
		xr_file_system& fs = xr_file_system::instance();
		std::string path;
		if (fs.resolve_path(PA_LOGS, "xrayMS3DInfo", path))
			ShellExecute(NULL, "", path.append(".log").c_str(), NULL, NULL, SW_SHOWNORMAL );
	}
	else
		MessageBox(NULL, "No information about this scene!", "X-Ray Model Information", MB_OK | MB_ICONWARNING);

	return 0;
}
