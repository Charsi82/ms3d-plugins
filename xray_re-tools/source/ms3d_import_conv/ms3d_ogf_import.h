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
	int		import(const xr_ogf* ogf);

private:
	int		import(const xr_ogf* ogf, const int* bone_refs);
	int		import(const xr_bone* xbone, const xr_bone_vec& xbones, int* bone_refs);

private:
	msModel*	m_model;
	xr_ini_ext* m_data;
	bool m_toCS;
};

inline ms3d_ogf_import::ms3d_ogf_import(msModel* model, string path) : m_model(model), m_toCS(false) {
	m_data = new xr_ini_ext;
	m_data->add_item("src", "path", path);
}
inline ms3d_ogf_import::~ms3d_ogf_import() { m_data->clear(); delete m_data; }
#endif
