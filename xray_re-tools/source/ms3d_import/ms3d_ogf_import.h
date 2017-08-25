#pragma once
#ifndef __MS3D_OGF_IMPORT_H__
#define __MS3D_OGF_IMPORT_H__

#include "xr_ogf.h"
#include "xr_ini_ext.h"

struct msModel;

class ms3d_ogf_import {
public:
			ms3d_ogf_import(msModel* model, string path = "");
			~ms3d_ogf_import();
	int		import(const xray_re::xr_ogf* ogf);

private:
	int		import(const xray_re::xr_ogf* ogf, const int* bone_refs);
	int		import(const xray_re::xr_bone* xbone, const xray_re::xr_bone_vec& xbones, int* bone_refs);

private:
	msModel*	m_model;
	xr_ini_ext* m_data;
};

inline ms3d_ogf_import::ms3d_ogf_import(msModel* model, string path) : m_model(model) {
	m_data = new xr_ini_ext;
	m_data->add_item("src", "path", path);
}
inline ms3d_ogf_import::~ms3d_ogf_import() { m_data->clear(); delete m_data; }
#endif
