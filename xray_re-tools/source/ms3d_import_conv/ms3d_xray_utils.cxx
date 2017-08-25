#include <msLib.h>
#include "ms3d_xray_utils.h"
#include "xr_file_system.h"

using namespace xray_re;

void set_texture(msMaterial* mtl, const std::string& texture)
{
	std::string path;
	xr_file_system& fs = xr_file_system::instance();
	if (!fs.resolve_path(PA_GAME_TEXTURES, texture, path))
		path = texture;
	msMaterial_SetDiffuseTexture(mtl, path.append(".dds").c_str());
	msVec4 mask = { 1, 1, 1, 1 };
	msMaterial_SetDiffuse(mtl, mask);
}
