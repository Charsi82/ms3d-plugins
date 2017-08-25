#define NOMINMAX
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "ms3d_xray_efc.h"
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
	return new ms3d_xray_conf;
}

void DestroyPlugIn(cMsPlugIn* plugin) { delete plugin; }

ms3d_xray_conf::~ms3d_xray_conf() {}

int ms3d_xray_conf::GetType() { return eTypeTool|eNormalsAndTexCoordsPerTriangleVertex; }

const char* ms3d_xray_conf::GetTitle() { return "X-Ray Edit xray_path.ltx"; }

int ms3d_xray_conf::Execute(msModel* model)
{
	char path[MAX_PATH + 16];
	DWORD n = GetModuleFileName(NULL, path, xr_dim(path));
	if (n && PathRemoveFileSpec(path)) {
		strcat_s(path, sizeof(path), "\\xray_path.ltx");
		ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWNORMAL);
	}
	return 0;
}
