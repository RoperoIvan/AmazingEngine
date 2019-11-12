#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"
#include "Globals.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <string>
#include "par_shapes.h"

struct aiScene;
struct aiMesh;

class Image : public Component
{
public:
	Image(GameObject* parent);
	~Image();
	
	void Disable() override;
	void Update() override;


	GLuint LoadImages(const char* p_tex);
	GLuint GetID();

	bool LoadMaterials(const aiScene* scene, std::string file_name);
	int GetTextureId();
	std::string GetTexturePath();
	void LoadCheckerTexture();
	int SetTextureId(int id);
public:
	bool show_checker_tex = false;
	uint check_id = 0;
	int tmp_id = 0;
	int texture_id = 0;
	std::string p_tex;
};

#endif
