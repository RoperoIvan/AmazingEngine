#ifndef _RESOURCEMESH_H
#define _RESOURCEMESH_H

#include "Resource.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "par_shapes.h"
#include "GameObject.h"
#include "Transform.h"
#include "Image.h"
#include "Geometry.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint id);
	~ResourceMesh();
	void SetData(float* ver, uint* ind, uint num_vert, uint num_ind, float* normals, float* texture_coords, uint num_coords, uint num_face_normals, uint id_coords);
	void Disable();
	void ActualitzateBuffer();
	void DrawMesh();
	void DebugDraw(bool vert = true, bool face = true);
	void LoadBuffers();
	void CalculateParentBoundingBox(GameObject* object);
	void CreatePrimitive(par_shapes_mesh* p_mesh, Geometry* geo,float col0 = 255, float col1 = 255, float col2 = 255, float col3 = 255);

public:
	uint id_vertices = 0;  
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_indices = 0;  
	uint num_indices = 0;
	uint* indices = nullptr;

	int par_num_indices = 0;

	uint id_normals = 2;
	uint num_normals = 0;
	float* normals = nullptr;
	uint num_face_normals = 0;
	float* face_normals = nullptr;

	int mesh_id;

	uint id_coords = 0;
	uint num_coords = 0;
	float* uv_coord = nullptr;

	bool is_primitive = false;

	float r, g, b, a;

	Image* texture = nullptr;
	Transform* transform = nullptr;
};

#endif

