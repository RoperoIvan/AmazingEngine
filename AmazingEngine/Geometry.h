#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"
#include "Image.h"
#include "par_shapes.h"
#include "Transform.h"
#include "Resource.h"
#include "ResourceMesh.h"

class ResourceMesh;

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

	void DebugDraw();

	void Update() override;
	std::string GetTexturePath() { return std::string(); };
	int GetTextureId() { return 0; };
	void SetMesh(ResourceMesh* m);

public:

	std::string name;
	ResourceMesh* r_mesh = nullptr;

};


#endif //_GEOMETRY_H
