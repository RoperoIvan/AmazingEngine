#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"


enum  class Primitives
{
	CUBE,
	SPHERE,
	CONE,
	CYILINDER,
	PLANE,
	UKNOWN,
};

struct Geometry
{
	Geometry(float* ver, uint* ind,float* norm, uint num_vert, uint num_ind, uint num_norm, float* uv_coord);
	Geometry(Geometry* geo);
	Geometry(float* ver, uint* ind, float* normals, int num_vert, int num_ind, float r, float g, float b,float a); // Creation of primitives with par_shapes library
	Geometry();
	~Geometry();
	void Draw();
	void DrawPrimitives();
	void DebugDraw();
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
	uint id_coords = 0;
	uint num_coords = 0;
	float* uv_coord = nullptr;
	float r, g, b, a;
	int texture_id = 0;

};


#endif //_GEOMETRY_H
