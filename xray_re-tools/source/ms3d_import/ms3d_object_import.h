#pragma once
#ifndef __MS3D_OBJECT_IMPORT_H__
#define __MS3D_OBJECT_IMPORT_H__

#include "xr_object.h"
using namespace xray_re;
using namespace std;
#include <windows.h>

struct msModel;
/*
struct xr_face
{
	UINT v0, v1, v2;
	UINT ref0, ref1, ref2;
	UINT mask;
	UINT mtl_index;
	xr_face(UINT a0, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5);
};*/

class ms3d_object_import {
public:
	ms3d_object_import(msModel* model, const char* path);
	int		import(const xray_re::xr_object* object);

private:
	int		import(const xray_re::xr_mesh* mesh);
	int		import(const xray_re::xr_surface* surface);
	int		process_chunk(xr_reader* r, UINT chunk_id, UINT chunk_size);
	void	load_tc(const char* path);
	int		load_object(const char* path);
	msModel*	m_model;

	vector<uint32_t> m_uv_map_index;
	vector<uint32_t> m_uv_map_layer;
	vector<fvector2> m_tc1, m_tc2;
};

inline ms3d_object_import::ms3d_object_import(msModel* model, const char* path): m_model(model) { load_tc(path); }

//inline xr_face::xr_face(UINT a0, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5) :v0(a0), v1(a1), v2(a2), ref0(a3), ref1(a4), ref2(a5){};
#endif
