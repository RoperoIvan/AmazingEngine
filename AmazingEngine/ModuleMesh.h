#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include "Geometry.h"
#include "Module.h"
#include <vector>

class ModuleMesh : public Module
{
public:
	ModuleMesh(Application* app, bool start_enabled = true);
	~ModuleMesh();

	bool Init();
	update_status PostUpdate(float dt)override;
	bool CleanUp();
	bool LoadFile(const char* file_name);
	GLuint LoadTexture(const char* p_tex);

private:
	float TriangleCenterAxis(const float &p1, const float &p2, const float &p3);
public:
	std::vector<Geometry*> geometry;
};



#endif //_MODULEMESH_H_