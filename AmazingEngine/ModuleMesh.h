#ifndef _MODULEMESH_H_
#define _MODULEMESH_H_

#include "Globals.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include "Geometry.h"
#include "Camera.h"
#include "Module.h"
#include <queue>
#include "MathGeoLib/include/Geometry/AABB.h"

class ModuleMesh : public Module
{
public:
	ModuleMesh(Application* app, bool start_enabled = true);
	~ModuleMesh();

	bool Init();
	update_status PostUpdate(float dt)override;
	bool CleanUp();
	bool LoadFile(const char* file_name);
	bool LoadFBXFile(const char* file_name);
	bool LoadTextureFile(const char* file_name);
	bool IsCulling(Geometry* g);
	void ChangeTex(GameObject* object, const char* file_name, Image* tex = nullptr);
	void DrawBoundingBoxes();
	void DrawFrustums();
	void AddFrustumBox(math::Frustum* c_frustum);
private:
	float TriangleCenterAxis(const float &p1, const float &p2, const float &p3);

public:
	std::queue<math::AABB*> b_boxes; //we use a queue cause we want to push and pop the bounding boxes fast // We use it only for bb drawing
	std::queue<math::Frustum*> c_frustums; //same as bounding boxes, we'll use it only for drawing
	Camera* current_camera = nullptr;
};



#endif //_MODULEMESH_H_