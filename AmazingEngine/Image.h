#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"
#include "Globals.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <string>
#include <vector>
#include "par_shapes.h"
#include "ResourceMaterial.h"

struct aiScene;
struct aiMesh;
struct aiMaterial;

class ResourceMaterial;

class Image : public Component
{
public:
	Image(GameObject* parent);
	~Image();
	
	//void Disable() override;
	void Update() override;
	std::string GetTexturePath() { return std::string(); };
	int GetTextureId() { return 0; };
	//int GetTextureId();
	//std::string GetTexturePath();
	//void LoadCheckerTexture();
	//int SetTextureId(int id);
	void SetTexture(ResourceMaterial* t)
	{
		if (r_texture)
			r_texture->DecreaseInstancies();
		r_texture = t;
		if (r_texture)
			r_texture->IncreaseInstancies();
	}
public:
	/*bool show_checker_tex = false;
	uint check_id = 0;
	int tmp_id = 0;
	int texture_id = 0;
	std::string p_tex;*/
	ResourceMaterial* r_texture = nullptr;
	//std::string name;
};

#endif
