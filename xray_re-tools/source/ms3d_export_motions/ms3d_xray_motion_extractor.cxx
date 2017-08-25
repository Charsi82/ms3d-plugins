#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_motion_extractor.h"
#include "xr_log.h"
#include "xr_file_system.h"
#include "msLib.h"
#include <time.h>
#include "xr_ini_ext.h"

using namespace xray_re;
using namespace std;

#include "xr_ogf_v4.h" //omf support

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
		xr_log::instance().init("xrayMS3DExtractSkl");
	}
	return new ms3d_xray_export_skl;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_export_skl::~ms3d_xray_export_skl() {}

int ms3d_xray_export_skl::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }

const char* ms3d_xray_export_skl::GetTitle() { return "X-Ray Motion Extractor"; }

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
			msg("load omf with %d anims", ms.size());
			int zz = 0;
			for (auto it = ms.begin(); it != ms.end(); ++it)
			{
				msg("[%02d][%s][%d]", ++zz, (*it)->name().c_str(), (*it)->fps());
				string skls_path = "";
				if (fs.resolve_path(PA_IMPORT, "skls", skls_path))
				{
					string save_to = skls_path + "\\" + (*it)->name() + ".skl";
					msg("save to '%s'", save_to.c_str());
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

int ms3d_xray_export_skl::Execute(msModel* model)
{
	if (msModel_GetMeshCount(model) == 0)
	{
		MessageBox(NULL, "The scene is empty!", "X-Ray Extractor skl", MB_OK | MB_ICONWARNING);
		return 0;
	}

	string info = "";
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

	if (!info.size()) return 0;// empty
	xr_ini_ext mdata;
	mdata.parse(info.c_str(), info.size());

	// определяем omf или список анимаций
	if (mdata.section_exist("motions"))
	{
		info = mdata.r_line("src", "path");
		//msg("src = '%s'", info.c_str());
		
		xr_ogf* ogf = xr_ogf::load_ogf(info);
		if (ogf) {
			int acnt = 0;
			auto ms = ogf->motions();
			xr_file_system& fs = xr_file_system::instance();			
			string skls_path = "";
			fs.resolve_path(PA_IMPORT, "skls", skls_path);
			clock_t start = clock();
			for (auto it = ms.begin(); it != ms.end(); ++it)
			{
				string save_to = skls_path + "\\" + (*it)->name() + ".skl";
				//msg("save to '%s'", save_to.c_str());
				(*it)->save_skl(save_to.c_str());
				++acnt;
			}
			auto ftime = (clock() - start) / 1.0 / CLOCKS_PER_SEC;
			msg("motions count %d; total time: %.3fs", acnt, ftime);
			//char buf[50];
			//sprintf_s(buf, sizeof(buf), "Экспортировано %d анимаций за %.3f сек.", acnt, ftime);
			//MessageBox(NULL, buf, "Anims Extractor", MB_OK);
			delete ogf;
		}
	}
	else
	{
		msg("processing omf");
		info = mdata.r_line("omfs","list");
		if (info=="default") return 0;

		clock_t start = clock();
		int anim_cnt = 0;
		for (;;)
		{
			int split_pos = info.find(",");
			if (split_pos == -1 ) break;
			anim_cnt += process_omf(info.substr(0, split_pos));
			info = info.substr(split_pos + 1);
		}
		anim_cnt += process_omf(info);
		auto ftime = (clock() - start) / 1.0 / CLOCKS_PER_SEC;
		msg("motions count %d, total time: %.3fs", anim_cnt, ftime);
		//char buf[50];
		//sprintf_s(buf, sizeof(buf), "Экспортировано %d анимаций за %.3f сек.", anim_cnt, ftime);
		//MessageBox(NULL, buf, "Motion Extractor", MB_OK);
	}
	return 0;
}
