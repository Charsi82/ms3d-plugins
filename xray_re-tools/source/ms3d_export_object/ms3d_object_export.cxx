#include <msLib.h>
#include "ms3d_object_export.h"
#include "ms3d_mesh_builder.h"
#include "xr_object.h"
#include "xr_ini_ext.h"
#include "xr_ogf.h"

using namespace xray_re;
static xr_ini_ext* m_data = 0;

xr_ini_ext* get_data()
{
	if (m_data == 0) m_data = new xr_ini_ext;
	return m_data;
}

struct ms3d_surface_factory: public xr_surface_factory {
				ms3d_surface_factory(msModel* _model);
	virtual xr_surface*	create_surface(const xr_raw_surface& raw_surface) const;
private:
	msModel*		model;
};

inline ms3d_surface_factory::ms3d_surface_factory(msModel* _model): model(_model) {}

xr_surface* ms3d_surface_factory::create_surface(const xr_raw_surface& raw_surface) const
{
	xr_surface* surface = new xr_surface(msModel_GetBoneCount(model) > 0);
	if (raw_surface.two_sided())
		surface->set_two_sided();

	/*
		char buf[2*MS_MAX_PATH];
		char* texture = buf + MS_MAX_PATH;
		msMaterial_GetDiffuseTexture(mtl, texture, MS_MAX_PATH);
		if ((texture = std::strrchr(texture, '\\')) == 0)
		texture = buf + MS_MAX_PATH;
		else
		++texture;
		char* s = std::strrchr(texture, '.');
		if (s)
		*s = 0;
		if ((s = std::strchr(texture, '_'))) {
		size_t n = s - texture;
		std::memcpy(texture - n - 1, texture, n);
		*(--texture) = '\\';
		texture -= n;
		}*/
	msMaterial* mtl = msModel_GetMaterialAt(model, raw_surface.texture);
	char texture[MS_MAX_PATH];
	msMaterial_GetDiffuseTexture(mtl, texture, MS_MAX_PATH);
	char* texpos = strstr(texture, "textures\\");

	if (texpos)
		surface->texture() = texpos + 9;
	else
	{
		char* s = std::strchr(texture, '_');
		if (s) {
			memcpy(s + 1, texture, strlen(texture));
			*s = '\\';
		}
		surface->texture() = texture;
	}
	if (msModel_GetBoneCount(model))
	{
		// read from comment
		string sh = get_data()->r_line(mtl->szName, "shader");
		surface->eshader() =  (sh != "default") ? sh : "models\\model";//export shader
	}

	return surface;
}

int ms3d_object_export::do_export(const char* path)
{
	int num_meshes = msModel_GetMeshCount(m_model);
	if (num_meshes == 0)
		return -1;

	int num_mtls = msModel_GetMaterialCount(m_model);
	if (num_mtls == 0)
		return -1;

	//init data
	int nCommentLength = msModel_GetComment(m_model, NULL, 0);
	if (nCommentLength > 0)
	{
		char *pszComment = new char[nCommentLength + 1];
		msModel_GetComment(m_model, pszComment, nCommentLength);
		get_data()->parse(pszComment, nCommentLength);
		delete[] pszComment;
	}

	ms3d_surface_factory surface_factory(m_model);
	xr_object *object = new xr_object(&surface_factory);
	int num_bones = msModel_GetBoneCount(m_model);

	xr_bone_vec& xbones = object->bones();
	for (int i = 0; i != num_bones; ++i) {
		msBone* bone = msModel_GetBoneAt(m_model, i);

		xr_bone* xbone = new xr_bone;

		char name[MS_MAX_NAME];
		msBone_GetName(bone, name, sizeof(name));
		xbone->name() = name;
		xbone->vmap_name() = name;

		msBone_GetParentName(bone, name, sizeof(name));
		xbone->parent_name() = name;

		fvector3 temp;

		msBone_GetRotation(bone, temp.xyz);
		dvector3 rot2;
		dmatrix().set_euler_xyz(temp.x, temp.y, temp.z).get_xyz_i(rot2);
		xbone->bind_rotate().set(float(-rot2.x), float(-rot2.y), float(rot2.z));

		msBone_GetPosition(bone, temp.xyz);
		xbone->bind_offset().set(temp.x, temp.y, -temp.z);

		xbones.push_back(xbone);

		// export comment to game material
/*		int nCommentLength = msBone_GetComment(bone, NULL, 0);
		if (nCommentLength > 0)
		{
			char *pszComment = new char[nCommentLength + 1];
			msBone_GetComment(bone, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';
			xbone->gamemtl() = std::string(pszComment);
			delete[] pszComment;
		}*/
		string mtl = get_data()->r_line("bones", xbone->name());
		xbone->gamemtl() = (mtl != "default") ? mtl : "default_object"; //export bone material
	}
	// export motion refs
/*	msMesh* pMdl = msModel_GetMeshAt(m_model, 0);
	int nCommentLength = msMesh_GetComment(pMdl, NULL, 0);
	if (nCommentLength > 0)
	{
		char *pszComment = new char[nCommentLength + 1];
		msMesh_GetComment(pMdl, pszComment, nCommentLength);
		pszComment[nCommentLength] = '\0';
		std::string refs = "";
		refs.append(pszComment);
		if ((int)refs.find(" ") < 0) //mrefs
		{
			object->motion_refs().append(refs);
		}
		delete[] pszComment;
	}*/

		if (get_data()->section_exist("motions"))
		{
			//msg("export motions");
			string ogf_path = get_data()->r_line("src", "path");
			xr_ogf* ogf;
			if ((ogf_path != "default") && (ogf = xr_ogf::load_ogf(ogf_path))) {

				if (object->bones().size() == ogf->bones().size()){
					bool ok = true;
					auto it1 = object->bones().begin();
					auto it2 = ogf->bones().begin();
					for (; it1 != object->bones().end(); ++it1, ++it2)
					{
						if ((*it1)->name() != (*it2)->name()) {
							ok = false;						
							break;
						};
					}
					if (ok)	object->motions().swap(ogf->motions());
				}
				delete ogf;
			}
		}
		else
		{
			//msg("export mrefs");
			string tmp = get_data()->r_line("omfs", "list");
			if (tmp != "default") object->motion_refs() = tmp;				
		}
	if (num_bones > 0) {
		object->flags() = EOF_DYNAMIC;
		object->partitions().push_back(new xr_partition(xbones));
	}

	ms3d_mesh_builder* xmesh = new ms3d_mesh_builder;
	int status = xmesh->build(m_model, num_bones > 0);
	if (status == 0) {
		xmesh->commit(*object);
		object->denominate();

		// export userdata
		msMesh* pMesh0 = msModel_GetMeshAt(m_model, 0);
		nCommentLength = msMesh_GetComment(pMesh0, NULL, 0);
		if (nCommentLength)
		{
			char *pszComment = new char[nCommentLength + 1];
			msMesh_GetComment(pMesh0, pszComment, nCommentLength);
			pszComment[nCommentLength] = '\0';
			object->set_userdata(string(pszComment));
			delete[] pszComment;
		}

		//export bones parts
		object->export_partition();

		status = object->save_object(path) ? 0 : -1;		
	}
	delete object;

	return status;
}
