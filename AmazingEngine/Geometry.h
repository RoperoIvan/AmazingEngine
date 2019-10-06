#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

struct Geometry
{
	Geometry(float* ver, uint* ind, uint num_vert, uint num_ind);
	~Geometry();
	void Draw();

	uint id_vertices = 0;  
	uint num_vertices = 0;
	float* vertices = nullptr;
	uint id_indices = 0;  
	uint num_indices = 0;
	uint* indices = nullptr;
};













#endif //_GEOMETRY_H
