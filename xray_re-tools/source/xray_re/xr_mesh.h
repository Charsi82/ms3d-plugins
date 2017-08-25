#ifndef __GNUC__
#pragma once
#endif
#ifndef __XR_MESH_H__
#define __XR_MESH_H__

#include <string>
#include <vector>
#include "xr_vector3.h"
#include "xr_vector2.h"
#include "xr_fixed_vector.h"
#include "xr_aabb.h"

namespace xray_re {

// editable mesh flags
enum {
	EMF_VALID	= 0x01,	// no rendering and collision (wireframe works though)
	EMF_3DSMAX	= 0x04,	// generated by 3ds Max export plugin (different sgroups)
};

const uint32_t EMESH_NO_SG = 0xffffffff;

class xr_reader;
class xr_writer;
class xr_object;
class xr_surface;

class xr_vmap {
public:
			xr_vmap();
			xr_vmap(const char* name);
			xr_vmap(const std::string& name);
	virtual		~xr_vmap();
	virtual void	load_data(xr_reader& r) = 0;
	virtual void	save_data(xr_writer& w) const = 0;
	virtual void	reserve(size_t _size);
	uint32_t	add_vertex(uint32_t vert_idx);

	static xr_vmap*	create(const char* name, unsigned type, unsigned dimension, bool discontiguous);

	struct f_read_0 { void operator()(xr_vmap*& vmap, xr_reader& r); };
	struct f_read_1 { void operator()(xr_vmap*& vmap, xr_reader& r); };
	struct f_read_2 { void operator()(xr_vmap*& vmap, xr_reader& r); };
	struct f_write { void operator()(const xr_vmap* vmap, xr_writer& w) const; };

	enum {
		VMT_UV,
		VMT_WEIGHT,
	};


	virtual unsigned		type() const = 0;
	size_t				size() const;
	const std::string&		name() const;
	const std::vector<uint32_t>&	vertices() const;

protected:
	std::string		m_name;
	std::vector<uint32_t>	m_vertices;
};

class xr_uv_vmap: public xr_vmap {
public:
				xr_uv_vmap(const char* name);
				xr_uv_vmap(const std::string& name);
	virtual void		load_data(xr_reader& r);
	virtual void		save_data(xr_writer& w) const;
	virtual void		reserve(size_t _size);
	uint32_t		add_uv(const fvector2& uv, uint32_t vert_idx);

	virtual unsigned		type() const;
	const std::vector<fvector2>&	uvs() const;

protected:
	std::vector<fvector2>	m_uvs;
};

TYPEDEF_STD_VECTOR_PTR(xr_vmap)

class xr_face_uv_vmap: public xr_uv_vmap {
public:
				xr_face_uv_vmap(const char* name);
				xr_face_uv_vmap(const std::string& name);
	virtual void		load_data(xr_reader& r);
	virtual void		save_data(xr_writer& w) const;
	virtual void		reserve(size_t _size);
	uint32_t		add_uv(const fvector2& uv, uint32_t vert_idx, uint32_t face_idx);

	const std::vector<uint32_t>&	faces() const;

protected:
	std::vector<uint32_t>	m_faces;
};

class xr_weight_vmap: public xr_vmap {
public:
				xr_weight_vmap(const char* name);
				xr_weight_vmap(const std::string& name);
	virtual void		load_data(xr_reader& r);
	virtual void		save_data(xr_writer& w) const;
	virtual void		reserve(size_t _size);
	uint32_t		add_weight(float weight, uint32_t vert_idx);

	virtual unsigned		type() const;
	const std::vector<float>&	weights() const;

protected:
	std::vector<float>	m_weights;
};

TYPEDEF_STD_VECTOR_PTR(xr_weight_vmap)

struct xr_surfmap {
				xr_surfmap();
				xr_surfmap(xr_surface* _surface);
	xr_surface*		surface;
	std::vector<uint32_t>	faces;
};

TYPEDEF_STD_VECTOR_PTR(xr_surfmap)

struct lw_options {
	uint32_t	unk1;
	uint32_t	unk2;
};

struct lw_face {
			lw_face();
			lw_face(uint32_t _v0, uint32_t _v1, uint32_t _v2);
	union {
		struct {
			uint32_t	v[3];
			uint32_t	ref[3];
		};
		struct {
			uint32_t	v0, v1, v2;
			uint32_t	ref0, ref1, ref2;
		};
	};
};

TYPEDEF_STD_VECTOR(lw_face)

struct lw_vmref_entry {
			lw_vmref_entry();
			lw_vmref_entry(uint32_t _vmap, uint32_t _offset);
	bool		operator==(const lw_vmref_entry& right) const;
	bool		operator<(const lw_vmref_entry& right) const;

	uint32_t	vmap;
	uint32_t	offset;
};

typedef _svector<lw_vmref_entry, 5> lw_vmref;

TYPEDEF_STD_VECTOR(lw_vmref)

class xr_mesh {
public:
			xr_mesh();
	virtual		~xr_mesh();
	void		load(xr_reader& r, xr_object& object);
	void		save(xr_writer& w) const;

	std::string&			name();
	const std::string&		name() const;
	uint8_t				flags() const;
	uint8_t&			flags();
	fbox&				bbox();
	const fbox&			bbox() const;
	std::vector<fvector3>&		points();
	const std::vector<fvector3>&	points() const;
	lw_face_vec&			faces();
	const lw_face_vec&		faces() const;
	std::vector<uint32_t>&		sgroups();
	const std::vector<uint32_t>&	sgroups() const;
	lw_vmref_vec&			vmrefs();
	const lw_vmref_vec&		vmrefs() const;
	xr_surfmap_vec&			surfmaps();
	const xr_surfmap_vec&		surfmaps() const;
	xr_vmap_vec&			vmaps();
	const xr_vmap_vec&		vmaps() const;

	void				calculate_bbox();

protected:
	std::string		m_name;		// EMESH_CHUNK_MESHNAME
	uint8_t			m_flags;	// EMESH_CHUNK_FLAGS
	lw_options		m_options;	// EMESH_CHUNK_OPTIONS
	fbox			m_bbox;		// EMESH_CHUNK_BBOX
	std::vector<fvector3>	m_points;	// EMESH_CHUNK_VERTS
	std::vector<uint8_t>	m_adj_counts;
	std::vector<uint32_t>	m_adj_refs;
	lw_face_vec		m_faces;	// EMESH_CHUNK_FACES
	std::vector<uint32_t>	m_sgroups;	// EMESH_CHUNK_SG
	lw_vmref_vec		m_vmrefs;	// EMESH_CHUNK_VMREFS
	xr_surfmap_vec		m_surfmaps;	// EMESH_CHUNK_SFACE
	xr_vmap_vec		m_vmaps;	// EMESH_CHUNK_VMAPS
};

TYPEDEF_STD_VECTOR_PTR(xr_mesh)

inline size_t xr_vmap::size() const { return m_vertices.size(); }
inline const std::string& xr_vmap::name() const { return m_name; }
inline const std::vector<uint32_t>& xr_vmap::vertices() const { return m_vertices; }
inline const std::vector<fvector2>& xr_uv_vmap::uvs() const { return m_uvs; }
inline const std::vector<uint32_t>& xr_face_uv_vmap::faces() const { return m_faces; }
inline const std::vector<float>& xr_weight_vmap::weights() const { return m_weights; }

inline lw_face::lw_face() {}
inline lw_face::lw_face(uint32_t _v0, uint32_t _v1, uint32_t _v2):
	v0(_v0), v1(_v1), v2(_v2), ref0(UINT32_MAX), ref1(UINT32_MAX), ref2(UINT32_MAX) {}

inline lw_vmref_entry::lw_vmref_entry() {}
inline lw_vmref_entry::lw_vmref_entry(uint32_t _vmap, uint32_t _offset):
	vmap(_vmap), offset(_offset) {}

inline bool lw_vmref_entry::operator==(const lw_vmref_entry& right) const
{
	return vmap == right.vmap && offset == right.offset;
}

inline bool lw_vmref_entry::operator<(const lw_vmref_entry& right) const
{
	return vmap < right.vmap || (vmap == right.vmap && offset < right.offset);
}

inline xr_surfmap::xr_surfmap(): surface(0) {}
inline xr_surfmap::xr_surfmap(xr_surface* _surface): surface(_surface) {}

inline std::string& xr_mesh::name() { return m_name; }
inline const std::string& xr_mesh::name() const { return m_name; }
inline uint8_t& xr_mesh::flags() { return m_flags; }
inline uint8_t xr_mesh::flags() const { return m_flags; }
inline fbox& xr_mesh::bbox() { return m_bbox; }
inline const fbox& xr_mesh::bbox() const { return m_bbox; }
inline std::vector<fvector3>& xr_mesh::points() { return m_points; }
inline const std::vector<fvector3>& xr_mesh::points() const { return m_points; }
inline lw_face_vec& xr_mesh::faces() { return m_faces; }
inline const lw_face_vec& xr_mesh::faces() const { return m_faces; }
inline std::vector<uint32_t>& xr_mesh::sgroups() { return m_sgroups; }
inline const std::vector<uint32_t>& xr_mesh::sgroups() const { return m_sgroups; }
inline lw_vmref_vec& xr_mesh::vmrefs() { return m_vmrefs; }
inline const lw_vmref_vec& xr_mesh::vmrefs() const { return m_vmrefs; }
inline xr_surfmap_vec& xr_mesh::surfmaps() { return m_surfmaps; }
inline const xr_surfmap_vec& xr_mesh::surfmaps() const { return m_surfmaps; }
inline xr_vmap_vec& xr_mesh::vmaps() { return m_vmaps; }
inline const xr_vmap_vec& xr_mesh::vmaps() const { return m_vmaps; }

} // end of namespace xray_re

#endif
