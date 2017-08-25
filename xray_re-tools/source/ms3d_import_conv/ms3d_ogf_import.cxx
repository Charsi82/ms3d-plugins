#include <msLib.h>
#include "ms3d_ogf_import.h"
#include "ms3d_xray_utils.h"
#include "xr_string_utils.h"
#include <windows.h>

using namespace xray_re;

int ms3d_ogf_import::import(const xr_bone* xbone, const xr_bone_vec& xbones, int* bone_refs)
{
	xr_bone_vec_cit it = std::find(xbones.begin(), xbones.end(), xbone);
	xr_assert(it != xbones.end());

	int bone_idx = msModel_AddBone(m_model);
	if (bone_idx < 0)
		return -1;
	bone_refs[it - xbones.begin()] = bone_idx;

	msBone* bone = msModel_GetBoneAt(m_model, bone_idx);
	msBone_SetName(bone, xbone->name().c_str());
	if (!xbone->parent_name().empty())
		msBone_SetParentName(bone, xbone->parent_name().c_str());

	fvector3 pos = xbone->bind_offset();
	pos.z = -pos.z;
	msBone_SetPosition(bone, pos.xyz);

	const fvector3& rot = xbone->bind_rotate();
	dvector3 rot2;
	dmatrix().set_xyz_i(-rot.x, -rot.y, rot.z).get_euler_xyz(rot2);
	msBone_SetRotation(bone, fvector3().set(rot2).xyz);

	int status = 0;
	for (xr_bone_vec_cit it = xbone->children().begin(),
			end = xbone->children().end(); it != end; ++it) {
		if ((status = import(*it, xbones, bone_refs)) < 0)
			break;
	}
	return status;
}

int ms3d_ogf_import::import(const xr_ogf *ogf)
{
	int status = 0;
	bool need_rotate = false;

	int* bone_refs = 0;
	if (ogf->skeletal()) {
		const xr_bone_vec& xbones = ogf->bones();
		m_toCS = (xbones.size() == 45) && (MessageBox(NULL, "Грузим модель НПС из ТЧ. Конвертировать в ЧН/ЗП?", "X-Ray NPC Converter", MB_YESNO) == IDYES);
		if (m_toCS){

			int ind = msModel_AddBone(m_model);
			msBone* bone = msModel_GetBoneAt(m_model, ind);
			msBone_SetName(bone, "root_stalker");

			msVec3 b_pos = { 0, 0, 0 };
			msBone_SetPosition(bone, b_pos);
			b_pos[1] = -3.141593f;
			msBone_SetRotation(bone, b_pos);

			ind = msModel_AddBone(m_model);
			bone = msModel_GetBoneAt(m_model, ind);
			msBone_SetName(bone, "bip01");
			msBone_SetParentName(bone, "root_stalker");
			//b_pos = { 0, 0.2f, 0 };
			msVec3 pos1 = { 0.000000, 0.986000f, 0.000000 }; // soc - 0.984, cs\cop - 0.987
			msBone_SetPosition(bone, pos1);
			need_rotate = true;
		};

		if ((xbones.size() == 47) &&
			(MessageBox(NULL, "Грузим модель НПС из ЧН/ЗП. Конвертировать в ТЧ?", "X-Ray NPC Converter", MB_YESNO) == IDYES)) {

			// импортировать без первых двух костей
			xr_bone_vec_cit it = xbones.begin(), end = xbones.end();
			it++;
			fvector3 pos = (*it)->bind_offset();
			it++;

			fvector3 rot = { 0, 1.570798f + 1.570798f, 1.570798f };
			(*it)->bind_rotate(rot);
			(*it)->bind_offset(pos);
			(*it)->parent_name("");

			status = import(*it, xbones, bone_refs = new int[xbones.size() - 2]);
			need_rotate = true;
		}
		else
		for (xr_bone_vec_cit it = xbones.begin(), end = xbones.end(); it != end; ++it) {
			if ((*it)->is_root()) {
				status = import(*it, xbones, bone_refs = new int[xbones.size()]);
				break;
			}
		}
	}
	if (status == 0) {
		if (ogf->hierarchical()) {
			for (xr_ogf_vec_cit it = ogf->children().begin(),
				end = ogf->children().end(); it != end; ++it) {
				if ((status = import(*it, bone_refs)) < 0)
					break;
			}
		}
		else {
			status = import(ogf, bone_refs);
		}
	}

	if (m_toCS){
		int pelvis_index = msModel_FindBoneByName(m_model, "bip01_pelvis");
		//if (pelvis_index < 0) { msg("can't find bone with name 'bip01_pelvis'"); return 1; }
		msBone* pelvis_bone = msModel_GetBoneAt(m_model, pelvis_index);

		msVec3 pelvis_pos;
		msBone_GetPosition(pelvis_bone, pelvis_pos);
		msBone_SetParentName(pelvis_bone, "bip01");
		msVec3 b_pos = { 0, 0, 0 };
		msBone_SetPosition(pelvis_bone, b_pos);

		msBone* bone_bip01 = msModel_GetBoneAt(m_model, 1);
		msBone_SetPosition(bone_bip01, pelvis_pos);
	}

	if (need_rotate){
		msVec3 pos = { 0, 0, 0 };
		for (int mesh_ind = 0; mesh_ind < msModel_GetMeshCount(m_model); mesh_ind++){
			msMesh* pMesh = msModel_GetMeshAt(m_model, mesh_ind);
			for (int ind = 0; ind < msMesh_GetVertexCount(pMesh); ind++){
				msVertex* _vt = msMesh_GetVertexAt(pMesh, ind);
				msVertex_GetVertex(_vt, pos);
				pos[0] = -pos[0];
				pos[2] = -pos[2];
				msVertex_SetVertex(_vt, pos);
			};
		}
	};

	// bones data
	const xr_bone_vec& sf = ogf->bones();
	for (auto it = sf.begin(), end = sf.end(); it != end; ++it) {
		std::string bname = (*it)->name();
		int ind = msModel_FindBoneByName(m_model, bname.c_str());
		msBone* bone = msModel_GetBoneAt(m_model, ind);
		m_data->add_item("bones", bname, (*it)->gamemtl());
	}

	if (need_rotate)
	{
		if (m_toCS)
		{
			m_data->add_item("omfs", "list", "actors\\stalker_animation,actors\\stalker_scenario_animation,actors\\stalker_scripts_animation,actors\\stalker_smart_cover_animation");
		}
		else
		{
			m_data->add_item("omfs", "list", "actors\\stalker_animation");
		}
	}
	else
	{
		std::string comment = ogf->motion_refs();
		if (!comment.size())
		{
			auto m_vec = ogf->motions();
			for (auto it = m_vec.begin(), end = m_vec.end(); it != end; ++it)
				m_data->add_item("motions", (*it)->name());
		}
		else
			m_data->add_item("omfs", "list", comment.c_str());
	}

	delete[] bone_refs;

	if (need_rotate) //shader correction
	{
		string old_shader = m_toCS ? "models\\model" : "models\\model_pn";
		string new_shader = m_toCS ? "models\\model_pn" : "models\\model";
		uint8_t mtl_idx = 0;
		char name[MS_MAX_NAME];
		xr_snprintf(name, sizeof(name), "material%02d", mtl_idx++);
		while (m_data->section_exist(name))
		{
			if (m_data->r_line(name, "shader") == old_shader)
				m_data->set_item_value(name, "shader", new_shader);
			xr_snprintf(name, sizeof(name), "material%02d", mtl_idx++);
		}
	}

	// set model info to model comment
	msModel_SetComment(m_model, m_data->tostring().c_str());

	// set userdata into 0 mesh comment
	msMesh* pMdl = msModel_GetMeshAt(m_model, 0);
	if (ogf->userdata().size()) msMesh_SetComment(pMdl, ogf->userdata().c_str());		

	return status;
}

struct xinfl_pred { bool operator()(const fbone_weight& l, const fbone_weight& r) {
	return l.weight > r.weight;
}};

int ms3d_ogf_import::import(const xr_ogf* ogf, const int* bone_refs)
{
	int mtl_idx = msModel_AddMaterial(m_model);
	msMaterial* mtl = msModel_GetMaterialAt(m_model, mtl_idx);
	if (mtl == 0)
		return -1;
	char name[MS_MAX_NAME];
	xr_snprintf(name, sizeof(name), "material%02d", mtl_idx);
	msMaterial_SetName(mtl, name);
	set_texture(mtl, ogf->texture());

	m_data->add_item(name, "texture", ogf->texture());
	m_data->add_item(name, "shader", ogf->shader());

	int mesh_idx = msModel_AddMesh(m_model);
	msMesh* mesh = msModel_GetMeshAt(m_model, mesh_idx);
	if (mesh == 0) return -1;

	xr_snprintf(name, sizeof(name), "mesh%02d", mesh_idx);
	msMesh_SetName(mesh, name);
	msMesh_SetMaterialIndex(mesh, mtl_idx);

	const xr_vbuf& vb = ogf->vb();
	size_t num_verts = vb.size();

	// FIXME: the refs arrays are certainly an overkill, simple offsets should be enough.
	int* vert_refs = new int[num_verts];
	for (size_t i = 0; i != num_verts; ++i) {
		int vert_idx = msMesh_AddVertex(mesh);
		msVertex* vert = msMesh_GetVertexAt(mesh, vert_idx);

		fvector3 p = vb.p(i);
		p.z = -p.z;
		msVertex_SetVertex(vert, p.xyz);

		if (vb.has_texcoords()) {
			fvector2 uv = vb.tc(i);
			msVertex_SetTexCoords(vert, uv.xy);
		}
		if (bone_refs && vb.has_influences()) {
			finfluence xinfl = vb.w(i);
			int num_xinfls = int(xinfl.size() & INT_MAX);
			if (num_xinfls == 1) {
				msVertex_SetBoneIndex(vert, bone_refs[xinfl[0].bone]);
			} else {
				// FIXME: some kind of compensation system is required to minimize quantization errors.
				xr_assert(num_xinfls > 0);
				std::sort(xinfl.begin(), xinfl.end(), xinfl_pred());
				msVertex_SetBoneIndex(vert, bone_refs[xinfl[0].bone]);
				msVertexEx* vert2 = msMesh_GetVertexExAt(mesh, vert_idx);
				for (int j = 1; j != num_xinfls; ++j)
					msVertexEx_SetBoneIndices(vert2, j - 1, bone_refs[xinfl[j].bone]);
//int sum = 0;
				for (int j = 0; j != num_xinfls && j != 3; ++j) {
					int weight = int(floorf(xinfl[j].weight*100.f + 0.501f));
//sum += weight;
					msVertexEx_SetBoneWeights(vert2, j, weight);
				}
//msg("vert%d: sum=%d, infls=%d", vert_idx, sum, num_xinfls);
			}
		}
		vert_refs[i] = vert_idx;
	}
	const xr_ibuf& ib = ogf->ib();
	for (size_t i = 0, num_inds = ib.size(); i != num_inds;) {
		uint_fast16_t v0 = ib[i++], v1 = ib[i++], v2 = ib[i++];

		word inds[3];
		inds[2] = word(vert_refs[v0] & UINT16_MAX);
		inds[1] = word(vert_refs[v1] & UINT16_MAX);
		inds[0] = word(vert_refs[v2] & UINT16_MAX);

		int tri_idx = msMesh_AddTriangle(mesh);
		msTriangle* tri = msMesh_GetTriangleAt(mesh, tri_idx);
		msTriangle_SetVertexIndices(tri, inds);

		msTriangleEx* tri2 = msMesh_GetTriangleExAt(mesh, tri_idx);
		if (vb.has_texcoords()) {
			fvector2 uv;
			msTriangleEx_SetTexCoord(tri2, 2, uv.set(vb.tc(v0)).xy);
			msTriangleEx_SetTexCoord(tri2, 1, uv.set(vb.tc(v1)).xy);
			msTriangleEx_SetTexCoord(tri2, 0, uv.set(vb.tc(v2)).xy);
		}
		if (vb.has_normals()) {
			fvector3 n;
			msTriangleEx_SetNormal(tri2, 2, n.set(vb.n(v0)).xyz);
			msTriangleEx_SetNormal(tri2, 1, n.set(vb.n(v1)).xyz);
			msTriangleEx_SetNormal(tri2, 0, n.set(vb.n(v2)).xyz);
		}
	}
	delete[] vert_refs;

	return 0;
}
