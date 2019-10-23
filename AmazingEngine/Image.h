#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"
#include "Globals.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <string>

struct aiScene;
struct aiMesh;

class Image : public Component
{
public:
	Image(GameObject* parent);
	~Image();

	void Update() override;


	GLuint LoadImages(const char* p_tex);
	void LoadCoords(aiMesh* scene);
	void LoadMaterials(const aiScene* scene, std::string file_name);
	void LoadBuffers();
	int GetTextureId();
	void ShowProperties() {};
	std::string GetTexturePath();
public:
	uint id_coords = 0;
	uint num_coords = 0;
	float* uv_coord = nullptr;
	int texture_id = 0;
	std::string p_tex;
};

#endif
