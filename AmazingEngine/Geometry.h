#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

struct Geometry
{
	Geometry(float* ver, uint* ind,float* norm, uint num_vert, uint num_ind, uint num_norm);
	Geometry(Geometry* geo);
	Geometry();
	~Geometry();
	void Draw();

	uint id_vertices = 0;  
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint id_indices = 0;  
	uint num_indices = 0;
	uint* indices = nullptr;
	uint id_normals = 2;
	uint num_normals = 0;
	float* normals = nullptr;
};


#endif //_GEOMETRY_H
