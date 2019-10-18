#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "Image.h"
#include "par_shapes.h"
#include "Transform.h"

enum  class Primitives
{
	CUBE,
	SPHERE,
	CONE,
	CYILINDER,
	PLANE,
	UKNOWN,
};

class Geometry: public Component
{
public:

	Geometry(GameObject* parent = nullptr);
	~Geometry();

	void CreatePrimitive(par_shapes_mesh* p_mesh, float col0 = 255, float col1 = 255, float col2 = 255, float col3 = 255);
	void DrawPrimitives();
	void DebugDraw();

	void Update() override;

	void LoadData(aiMesh* mesh);

	void ShowProperties();
private:

	void LoadBuffers();
public:
	uint id_vertices = 0;  
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint id_indices = 0;  
	uint num_indices = 0;
	uint* indices = nullptr;
	uint id_normals = 2;
	uint num_normals = 0;
	float* normals = nullptr;
	int par_num_indices = 0;

	float r, g, b, a;

	Image* texture = nullptr;

	bool is_primitive = false;
	par_shapes_mesh* primitive_mesh = nullptr;

public:
	Transform* transform = nullptr;
};


#endif //_GEOMETRY_H
