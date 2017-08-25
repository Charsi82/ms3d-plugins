#include <msLib.h>
#include "ms3d_object_import.h"
//#include "xr_string_utils.h"
#include "ms3d_xray_utils.h"

#include "xr_file_system.h"
#include "xr_object_format.h"

using namespace xray_re;

int ms3d_object_import::import(const xr_object *object)
{
	int status = 0;

	for (xr_surface_vec_cit it = object->surfaces().begin(),
		end = object->surfaces().end(); it != end; ++it) {
		if ((status = import(*it)) < 0)
			break;
	}

	for (xr_mesh_vec_cit it = object->meshes().begin(),
		end = object->meshes().end(); it != end; ++it) {
		if ((status = import(*it)) < 0)
			break;
	}

	return status;
}

int ms3d_object_import::import(const xr_surface* surface)
{
	int mtl_idx = msModel_AddMaterial(m_model);
	msMaterial* mtl = msModel_GetMaterialAt(m_model, mtl_idx);
	if (mtl == 0)
		return -1;

	msMaterial_SetName(mtl, surface->name().c_str());
	set_texture(mtl, surface->texture());
	std::string comment;
	comment.append("eshader = ").append(surface->eshader());
	comment.append("\r\ncshader = ").append(surface->cshader());
	comment.append("\r\ngamemtl = ").append(surface->gamemtl());
	if (surface->two_sided())
		comment.append("\r\ntwo_sided = true");
	else
		comment.append("\r\ntwo_sided = false");
	msMaterial_SetComment(mtl, comment.c_str());

	return 0;
}

int ms3d_object_import::import(const xr_mesh* mesh)
{
	const lw_face_vec& faces = mesh->faces();
	auto p = mesh->points();
	int qwe =  0;
	char name[MS_MAX_NAME];
	//	xr_assert(points.size() < MAX_VERTICES);
	//	xr_assert(faces.size() < MAX_TRIANGLES);

	auto sg = mesh->sgroups();
	uint32_t face_index = 0;

	for (xr_surfmap_vec_cit it = mesh->surfmaps().begin(), end = mesh->surfmaps().end();
		it != end; ++it) {
		int submesh_idx = msModel_AddMesh(m_model);
		msMesh* submesh = msModel_GetMeshAt(m_model, submesh_idx);
		if (submesh == 0)
			return -1;

		//msg("create submesh [%s] %d",mesh->name().c_str(), submesh_idx);
		sprintf_s(name, sizeof(name), "mesh%2.2d", submesh_idx);
		msMesh_SetName(submesh, name);

		auto srf_name = (*it)->surface->name();
		int mtl_idx = msModel_FindMaterialByName(m_model, srf_name.c_str());
		msMesh_SetMaterialIndex(submesh, mtl_idx);

		// add points
		for (auto it3 = p.begin(), end3 = p.end(); it3 != end3; ++it3) {
			int vert_idx = msMesh_AddVertex(submesh);
			msVertex* vert = msMesh_GetVertexAt(submesh, vert_idx);
			fvector3 pos = *it3;
			pos.z = -pos.z;
			msVertex_SetVertex(vert, pos.xyz);
		}

		// add faces		
		auto v = (*it)->faces;
		for (auto it3 = v.begin(), end3 = v.end(); it3 != end3; ++it3) {
			auto f = faces[(*it3)];

			word inds[3];
			inds[2] = word(f.v0 & UINT16_MAX);
			inds[1] = word(f.v1 & UINT16_MAX);
			inds[0] = word(f.v2 & UINT16_MAX);


			int tri_idx = msMesh_AddTriangle(submesh);
			msTriangle* tri = msMesh_GetTriangleAt(submesh, tri_idx);
			msTriangle_SetVertexIndices(tri, inds);

			//set sm group
			msTriangle_SetSmoothingGroup(tri, (byte)sg[face_index++]);

			// texturing
			msTriangleEx* tri2 = msMesh_GetTriangleExAt(submesh, tri_idx);

			auto m_tc = (m_uv_map_layer[f.ref0]==0) ? m_tc1 : m_tc2;
			auto ind = m_uv_map_index[f.ref0];
			msTriangleEx_SetTexCoord(tri2, 2, m_tc[ind].xy);

			m_tc = (m_uv_map_layer[f.ref1]==0) ? m_tc1 : m_tc2;
			ind = m_uv_map_index[f.ref1];
			msTriangleEx_SetTexCoord(tri2, 1, m_tc[ind].xy);

			m_tc = (m_uv_map_layer[f.ref2]==0) ? m_tc1 : m_tc2;
			ind = m_uv_map_index[f.ref2];
			msTriangleEx_SetTexCoord(tri2, 0, m_tc[ind].xy);
		};
	}
	return 0;
}

/////////////////////////////

// data chunks processing
int ms3d_object_import::process_chunk(xr_reader* r, UINT chunk_id, UINT chunk_size)
{
	//msg("try process chunk[%04x][%d]", chunk_id, chunk_size);
	switch (chunk_id){
/*		case EOBJ_CHUNK_VERSION:
		{
			m_chunk_revision = r->r_u16();
			msg("chunk_version = %d", m_chunk_revision);
			return 1;
		}

		case EOBJ_CHUNK_USERDATA:
		{
			r->r_sz(m_szUserData);
			msg("userdata = %s", m_szUserData.c_str());
			return 1;
		}

		case EOBJ_CHUNK_LOD_REF:
		{
								   r->r_sz(m_szLodReference);
								   msg("lod ref = %s", m_szLodReference.c_str());
								   return 1;
		}

		case EOBJ_CHUNK_FLAGS:
			m_object_type = r->r_u32();
			msg("object_type = %08x", m_object_type);
			return 1;

		case EOBJ_CHUNK_TRANSFORM:
			r->r_fvector3(m_position);
			msg("pos[%.4f][%.4f][%.4f]", m_position.x, m_position.y, m_position.z);
			r->r_fvector3(m_orientation);
			msg("dir[%.4f][%.4f][%.4f]", m_orientation.x, m_orientation.y, m_orientation.z);
			return 1;
*/
/*
		case EOBJ_CHUNK_REVISION:
		{
									std::string value;
									r->r_sz(value);
									msg("author = %s", value.c_str());
									msg("creation time = %d", r->r_u32());
									r->r_sz(value);
									msg("last_modifier = %s", value.c_str());
									msg("modify_time = %d", r->r_u32());
									return 1;
		}
*/
/*	case EOBJ_CHUNK_SURFACES_2://surfaces
	{
		UINT surf_cnt = r->r_u32();
		//m_surfaces.
		std::string value;
		for (UINT i = 0; i < surf_cnt;i++)
		{
			xr_surface* sf = new xr_surface;
			r->r_sz(sf->name());
			msg("name = %s\n", sf->name().c_str());
			r->r_sz(sf->eshader());
			msg("eshader = %s\n", sf->eshader().c_str());
			r->r_sz(sf->cshader());
			msg("cshader = %s\n", sf->cshader().c_str());
			r->r_sz(sf->gamemtl());
			msg("game mtl = %s\n", sf->gamemtl().c_str());
			r->r_sz(sf->texture());
			msg("texture = %s\n", sf->texture().c_str());
			r->r_sz(value);
			msg("Texture = %s\n", value.c_str()); //skip "Texture"
			if (r->r_u32()) sf->set_two_sided();
			msg("two sided flags = %d", sf->two_sided()?1:0);
			msg("m_fvf = %d", r->r_u32());
			msg("tc = %d", r->r_u32());
			m_surfaces.push_back(sf);
		}
		msg("surf_size:%d", m_surfaces.size());
		return 1;
	}
	case EOBJ_CHUNK_MESHES: //meshes
	{
			UINT mesh_id = r->r_u32();
			UINT mesh_size = r->r_u32();
			while (mesh_size > 0){
				UINT block_id = r->r_u32();
				UINT block_size = r->r_u32();
				if (!process_chunk(r, block_id, block_size))
				{
					msg("unhandled geometry block [%04x][%d]", block_id, block_size);
					r->skip<uint8_t>(block_size);
				}
				mesh_size -= block_size + 8;
			}
			return true;
	}

	// geometry
	case EMESH_CHUNK_VERSION:
	{
		m_mesh_version = r->r_u16();
		msg("mesh version %d", m_mesh_version);
		return 1;
	}

	case EMESH_CHUNK_MESHNAME:
	{
		r->r_sz(m_mesh_name);
		msg("mesh version %s", m_mesh_name.c_str());
		return 1;
	}

	case EMESH_CHUNK_FLAGS:
	{
		m_mesh_flags = r->r_u8();
		msg("mesh flags %x", m_mesh_flags);
		return 1;
	}

	case EMESH_CHUNK_BBOX:
	{
		r->r_fvector3(m_bbox_min);
		msg("bbox min [%.4f][%.4f][%.4f]", m_bbox_min.x, m_bbox_min.y, m_bbox_min.z);
		r->r_fvector3(m_bbox_max);
		msg("bbox max[%.4f][%.4f][%.4f]", m_bbox_max.x, m_bbox_max.y, m_bbox_max.z);
		return 1;
	}

	case EMESH_CHUNK_OPTIONS:
	{
		msg("options [%d][%d]", r->r_u32(), r->r_u32());
		return 1;
	}

	case EMESH_CHUNK_VERTS:
	{
		UINT vert_cnt = r->r_u32();
		msg("vert count [%d]", vert_cnt);
		fvector3 pos;
		for (UINT i = 0; i < vert_cnt; i++)
		{
			r->r_fvector3(pos);
			msg("vert[%d] = [%.4f][%.4f][%.4f]", i, pos.x, pos.y, pos.z);
			m_vert.push_back(fvector3(pos));
		}
		return 1;
	}

	case EMESH_CHUNK_FACES:
	{
		UINT f_cnt = r->r_u32();
		msg("faces count [%d]", f_cnt);
		for (UINT i = 0; i < f_cnt; i++)
		{
			//msg("face[%d] = [%d][%d][%d][%d][%d][%d]", i, r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32());
			msg("face %d", i);
			m_faces.push_back(xr_face(r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32(), r->r_u32()));
		}
		return 1;
	}

	case EMESH_CHUNK_SG:
	{
		msg("smoothing groups");
		for (UINT i = 0; i < chunk_size / 4; i++)
		{
			UINT mask = r->r_u32();
			//msg("face [%d] : sg mask [%d]", i, mask);
			m_faces[i].mask = mask;
		} 
		return 1;
	}

	case EMESH_CHUNK_SFACE:
	{
							  UINT m_cnt = r->r_u16();
							  //msg("mtls count [%d]", m_cnt);
							  std::string value;
							  for (UINT i = 0; i < m_cnt; i++)
							  {
								  r->r_sz(value);
								  msg("mtl name[%d] = %s", i, value.c_str());
								  UINT f_cnt = r->r_u32();
								  for (UINT j = 0; j < f_cnt; j++)
								  {
									  UINT ind = r->r_u32();
									  msg("face[%d] = [%d]", j, ind);
									  m_faces[ind].mtl_index = i;
								  }
							  }
							  return 1;
	}*/

	case EMESH_CHUNK_VMREFS:
	{
							   UINT el = chunk_size;
							   UINT uv_cnt = r->r_u32();
							   // msg("uv map count [%d]", uv_cnt);
							   for (UINT i = 0; i < uv_cnt; i++)
							   {
								   r->r_u8(); //msg("unk[%d] = [%d]", i, r->r_u8());
								   UINT x = r->r_u32();
								   //msg("table number[%d] = [%d]", i, x);
								   m_uv_map_layer.push_back(x);
								   x = r->r_u32();
								   //msg("index in table[%d] = [%d]", i, x);
								   m_uv_map_index.push_back(x);
							   }

							   //el -= 4 + uv_cnt*(1 + 4 + 4);
							   //if (el > 0){ msg("tc skip %u", el); r->skip<uint8_t>(el); };

							   return 1;
	}

	case EMESH_CHUNK_VMAPS_2:
	{
								//msg("texture coordinates:");
								UINT uv_tbl_cnt = r->r_u32();
								//msg("uv_tbl_cnt [%d]", uv_tbl_cnt);
								// layer1
								std::string value;
								r->r_sz(value);
								//msg("value %s", value.c_str());
								r->r_u8(); // msg("unk %d",r->r_u8() );
								r->r_u16(); // msg("layer_id %d", r->r_u16());
								int s = r->r_u32();
								//msg("uvs %d", s);
								int i = 0;
								for (; i < s; i++)
								{
									float u = r->r_float();
									float v = r->r_float();
									//msg("[%d]:[%.5f][%.5f]", i, u, v);
									m_tc1.push_back(fvector2().set(u, v));
								}								
								for (i = 0; i < s; i++)
								{
									uint32_t x = r->r_u32();
									//msg("[%d] index [%d]",i , x);
								}

								// layer2
								//std::string value;
								r->r_sz(value);
								//msg("value %s", value.c_str());
								r->r_u8(); //msg("unk %d", r->r_u8());
								r->r_u16(); // msg("layer_id %d", r->r_u16());
								s = r->r_u32();
								//msg("uvs %d", s);
								for (i = 0; i < s; i++)
								{
									float u = r->r_float();
									float v = r->r_float();
									//msg("[%d]:[%.5f][%.5f]", i, u, v);
									m_tc2.push_back(fvector2().set(u, v));
								}
								for (i = 0; i < s; i++)
								{
									uint32_t x = r->r_u32();
									//msg("[%d] index [%d]", i, x);
								}

								for (i = 0; i < s; i++)
								{
									r->r_u32(); // msg("[%d] :: [%d]", i, r->r_u32());
								}
								return 1;
	}
	//surfaces
/*	case EOBJ_CHUNK_SURFACES_2:
	{
								   UINT surf_cnt = r->r_u32();
								   //m_surfaces.
								   std::string value;
								   for (UINT i = 0; i < surf_cnt; i++)
								   {
									   xr_surface* sf = new xr_surface;
									   r->r_sz(sf->name());
									   msg("name = %s\n", sf->name().c_str());
									   r->r_sz(sf->eshader());
									   msg("eshader = %s\n", sf->eshader().c_str());
									   r->r_sz(sf->cshader());
									   msg("cshader = %s\n", sf->cshader().c_str());
									   r->r_sz(sf->gamemtl());
									   msg("game mtl = %s\n", sf->gamemtl().c_str());
									   r->r_sz(sf->texture());
									   msg("texture = %s\n", sf->texture().c_str());
									   r->r_sz(value);
									   msg("Texture = %s\n", value.c_str()); //skip "Texture"
									   if (r->r_u32()) sf->set_two_sided();
									   msg("two sided flags = %d", sf->two_sided() ? 1 : 0);
									   msg("m_fvf = %d", r->r_u32());
									   msg("tc = %d", r->r_u32());
									   m_surfaces.push_back(sf);
								   }
								   msg("surf_size:%d", m_surfaces.size());
								   return 1;
	}*/

	// geometry
	case EOBJ_CHUNK_MESHES:
	{
								UINT mesh_id = r->r_u32();
								UINT mesh_size = r->r_u32();
								while (mesh_size > 0){
									UINT block_id = r->r_u32();
									UINT block_size = r->r_u32();
									if (!process_chunk(r, block_id, block_size))
									{
										msg("unhandled geometry block [%04x][%d]", block_id, block_size);
										r->skip<uint8_t>(block_size);
									}
									mesh_size -= block_size + 8;
								}
								return true;
	}

	//////////////////////////////////////////////////
	case EMESH_CHUNK_VERSION:
	case EMESH_CHUNK_MESHNAME:
	case EMESH_CHUNK_FLAGS:
	case EMESH_CHUNK_BBOX:
	case EMESH_CHUNK_OPTIONS:
	case EMESH_CHUNK_VERTS:
	case EMESH_CHUNK_FACES:
	case EMESH_CHUNK_SG:
	case EMESH_CHUNK_SFACE:
	case EOBJ_CHUNK_VERSION:
	case EOBJ_CHUNK_USERDATA:
	case EOBJ_CHUNK_LOD_REF:
	case EOBJ_CHUNK_FLAGS:
	case EOBJ_CHUNK_TRANSFORM:
	case EOBJ_CHUNK_SURFACES_2:
	case EOBJ_CHUNK_BONES_1:
	case EOBJ_CHUNK_REVISION:
	default:
		r->skip <uint8_t>(chunk_size);
		return 1;
	}
	return 0;
}

void ms3d_object_import::load_tc(const char* path)
{
	xr_file_system& fs = xr_file_system::instance();
	xr_reader* r = fs.r_open(path);
	if (r == 0)	return;
	UINT chunk_id = r->r_u32();
	UINT chunk_size = r->r_u32();
	//msg("main[%04x] size[%d]", chunk_id, chunk_size);
	while (!r->eof())
	{
		chunk_id = r->r_u32();
		chunk_size = r->r_u32();
		if (!process_chunk(r, chunk_id, chunk_size)){
			msg("skip unhandled chunk [%04x] size [%d]\n", chunk_id, chunk_size);
			r->skip <uint8_t>(chunk_size);
		}
	}
	fs.r_close(r);
}