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

	//GENERAL LOADING
	bool LoadFile(const char* file_name);
	bool LoadFBXFile(const char* file_name);
	
	//MOUSE PICKING
	bool IsCulling(Geometry* g);
	bool ContainsABB(const AABB &b_box);

	//MESH
	std::string LoadData(aiMesh* mesh);
	void LoadMeshFromFormat(const char* file_name, GameObject* g_object);

	//MATERIAL
	void LoadTemporalMaterials(const aiScene* scene);
	GLuint LoadImages(const char* p_tex, bool loading_scene = false);
	void LoadTextureFromFormat(const char*, GameObject* g_object);
	void LoadMaterials(const aiScene* scene, GameObject* g_object, const char* file_name, int last_mat_ind);
	void ImportTextureToDDSFile(const char* file_name) const;
	void ChangeTex(GameObject* object, const char* file_name, Image* tex = nullptr);
	GLuint GetID();
	std::string RandomName(aiMesh* mesh);

	//SCENE
	bool SaveCurrentScene(const char* s_name);
	uint SaveGameObjects(std::list<std::pair<char*, uint>> &buffer, GameObject* g_object);
	GameObject* LoadSceneFromFormat(const char* s_name);
	GameObject* LoadObjectFromFormat(char* &cursor);

	//DEBUG DRAWS
	void DrawBoundingBoxes();
	void DrawFrustums();
	void DrawRay();
	void AddFrustumBox(math::Frustum* c_frustum);

	void LookForMeshCollision(LineSegment ray_segment, std::vector<MouseHit>& hit);

private:
	float TriangleCenterAxis(const float &p1, const float &p2, const float &p3);

public:
	std::queue<math::AABB*> b_boxes; //we use a queue cause we want to push and pop the bounding boxes fast // We use it only for bb drawing
	std::queue<math::Frustum*> c_frustums; //same as bounding boxes, we'll use it only for drawing
	std::vector<std::pair<aiMaterial*, int>> tmp_material; //only used to look for repeated materials
	std::vector<std::pair <std::string, int>> tmp_textures; //
	std::vector<uint> tmp_parent_ids; //only used when loading scene from our format
	int rand_id = 0;
};



#endif //_MODULEMESH_H_