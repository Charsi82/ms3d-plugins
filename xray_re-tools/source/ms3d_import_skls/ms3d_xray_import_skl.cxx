#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_import_skl.h"
#include "xr_log.h"
#include "xr_file_system.h"
#include "msLib.h"
#include <time.h>

using namespace xray_re;
using namespace std;

#include "xr_ogf_v4.h" //omf support
int process_omf(string omf_path);

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
		xr_log::instance().init("xrayMS3DImportSkl");
	}
	return new ms3d_xray_info;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_info::~ms3d_xray_info() {}

int ms3d_xray_info::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }

const char* ms3d_xray_info::GetTitle() { return "X-Ray Animation Extractor"; }

int ms3d_xray_info::Execute(msModel* model)
{

	if (msModel_GetMeshCount(model) == 0)
	{
		MessageBox(NULL, "The scene is empty!", "X-Ray Skl Importer", MB_OK | MB_ICONWARNING);
		return 0;
	}
	string info = "";

	int nNumComments = msModel_GetMeshCount(model);

	if (nNumComments){

		msMesh *pMesh = msModel_GetMeshAt(model, 0);
		int nCommentLength = msMesh_GetComment(pMesh, NULL, 0);

		if (nCommentLength){

			char *pszComment = new char[nCommentLength + 1];
			msMesh_GetComment(pMesh, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';

			info.append(pszComment);
			delete[] pszComment;
		}
	}
	if (!info.size()) return 1;// empty
	
	// определяем omf или список анимаций
	if (!strstr(info.c_str(), "\\"))
	{
		msg("anim list"); //to do...
		int anm_cnt = 1;
		int split_pos = info.find("\r\n");
		for (;;)
		{
			msg("%d '%s'", anm_cnt, info.substr(0, split_pos).c_str());
			info = info.substr(split_pos + 2);
			++anm_cnt;
			split_pos = info.find("\r\n");
			if (split_pos == -1) break;
		}

		info = "";
		int nCommentLength = msModel_GetComment(model, NULL, 0);
		if (nCommentLength > 0)
		{
			char *pszComment = new char[nCommentLength + 1];
			msModel_GetComment(model, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';

			info.append(pszComment);
			delete[] pszComment;
		}
		if (!info.size()) return 1;
		int pos2 = info.find("\r\n");
		info = info.substr(6, pos2-6);
		msg("src = '%s'", info.c_str());
		
		xr_ogf* ogf = xr_ogf::load_ogf(info);
		if (ogf) {
			int acnt = 0;
			auto ms = ogf->motions();
			xr_file_system& fs = xr_file_system::instance();
			for (auto it = ms.begin(); it != ms.end(); ++it)
			{
				string skls_path = "";
				if (fs.resolve_path(PA_IMPORT, "skls", skls_path))
				{
					string save_to = skls_path + "\\" + (*it)->name() + ".skl";
					msg("save to '%s'", save_to.c_str());
					(*it)->save_skl(save_to.c_str());
				}
				++acnt;
			}
			delete ogf;
			msg(">>%d motions", acnt);
		}
	}
	else
	{
		msg("processing omf");
		info = "";
		msMesh *pMesh = msModel_GetMeshAt(model, 0);
		int nCommentLength = msMesh_GetComment(pMesh, NULL, 0);

		if (nCommentLength){

			char *pszComment = new char[nCommentLength + 1];
			msMesh_GetComment(pMesh, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';

			info.append(pszComment);
			delete[] pszComment;
		}
		if (!info.size()) return 1;

		clock_t start = clock();
		int anim_cnt = 0;
		for (;;)
		{
			int split_pos = info.find(",");
			if (split_pos == -1 ) break;
			//msg("%s", info.substr(0, split_pos).c_str());
			anim_cnt += process_omf(info.substr(0, split_pos));
			info = info.substr(split_pos + 1);
		}
		//msg("last = %s", info.c_str());
		anim_cnt += process_omf(info);
		auto ftime = (clock() - start) / 1.0 / CLOCKS_PER_SEC;
		msg("total time: %.3fs", ftime);
		char buf[50];
		_sprintf_p(buf, 50, "Экспортировано %d анимаций за %.3f сек.", anim_cnt, ftime);
		//vprintf_s(buf, fmt, anim_cnt, ftime);
		MessageBox(NULL, buf, "Anims Extractor", MB_OK);
	}
	return 0;
}

int process_omf(string info)
{
	int acnt = 0;
	xr_file_system& fs = xr_file_system::instance();
	std::string path;
	if (fs.resolve_path(PA_GAME_MESHES, info + ".omf", path))
	{
		//import omf
		xr_ogf_v4* omf = new xr_ogf_v4;

		if (omf && omf->load_omf(path.c_str())) {
			auto ms = omf->motions();
			//msg("load omf with %d anims", ms.size());
			int zz = 0;
			for (auto it = ms.begin(); it != ms.end(); ++it)
			{
				//msg("[%02d][%s][%d]", ++zz, (*it)->name().c_str(), (*it)->fps());
				string skls_path = "";
				if (fs.resolve_path(PA_IMPORT, "skls", skls_path))
				{
					string save_to = skls_path +"\\" +(*it)->name() + ".skl";
						//msg("save to '%s'", save_to.c_str());
						(*it)->save_skl(save_to.c_str());
				}
				++acnt;
			}
			delete omf;
		}
		else{
			msg("omf '%s' not loaded", path.c_str());
		}
	}
	else
	{
		msg("omf '%s' not found", path.c_str());
	}
	return acnt;
}