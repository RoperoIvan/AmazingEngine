#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "Image.h";

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
	Geometry(float* ver, uint* ind,float* norm, uint num_vert, uint num_ind, uint num_norm, GameObject* parent = nullptr);
	Geometry(Geometry* geo, GameObject* parent);
	Geometry(float* ver, uint* ind, float* normals, int num_vert, int num_ind, float r, float g, float b,float a, GameObject* parent = nullptr); // Creation of primitives with par_shapes library
	Geometry(GameObject* parent);
	~Geometry();
	void Draw();
	void DrawPrimitives();
	void DebugDraw();

	void Enable() override;
	void Update() override;
	void Disable() override;

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

public:
	GameObject* parent;

};


#endif //_GEOMETRY_H
