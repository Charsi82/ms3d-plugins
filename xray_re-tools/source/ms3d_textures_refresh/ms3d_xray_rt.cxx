#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_rt.h"
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
		//xr_log::instance().init("xrayMS3DInfo");
	}
	return new ms3d_xray_rt;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_rt::~ms3d_xray_rt() {}

int ms3d_xray_rt::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }

const char* ms3d_xray_rt::GetTitle() { return "X-Ray Refresh Textures"; }

int ms3d_xray_rt::Execute(msModel* model)
{
	int cnt = msModel_GetMaterialCount(model);
	char sTexture[MAX_PATH];
	xr_file_system& fs = xr_file_system::instance();
	const char* gt = fs.resolve_path(PA_GAME_TEXTURES);
	std::string full_path;
	while (cnt)
	{
		msMaterial* pM = msModel_GetMaterialAt(model, --cnt);
		msMaterial_GetDiffuseTexture(pM, sTexture, MAX_PATH);
		if (!strchr(sTexture, '\\')){
			full_path = gt;
			char mask[MAX_PATH];
			memset(mask, 0, MAX_PATH);
			char* p = strchr(sTexture, '_');
			if (p)
			{
				memcpy(mask, sTexture, p - sTexture);
				mask[p - sTexture] = 0;
			}
			full_path += mask;
			full_path += "\\";
			full_path += sTexture;
			strcpy_s(sTexture,full_path.c_str());
			msMaterial_SetDiffuseTexture(pM, sTexture);
		}
	}
	
	return 0;
}
