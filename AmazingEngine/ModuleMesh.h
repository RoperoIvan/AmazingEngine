#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include <gl/GL.h>

#include "Module.h"
#include <vector>

struct Mesh_data
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;
};


class ModuleMesh : public Module
{
public:
	ModuleMesh();
	~ModuleMesh();

	virtual bool Init();
	virtual update_status PostUpdate(float dt)override;
	virtual bool CleanUp();

	bool LoadFile(const char* file_name);

public:
	Mesh_data* data;
};



#endif //_MODULEMESH_H_