#ifndef _RESOURCEMATERIAL_H
#define _RESOURCEMATERIAL_H

#include "Resource.h"

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint id);
	~ResourceMaterial();
	void SetData(uint text_id, const char* t_name);

	uint GetTextureId() const
	{
		return texture_buffer_id;
	}

	std::string GetTextureName()const
	{
		return texture_name;
	}

public:
	int GetTextureId();
	std::string GetTexturePath();
	void LoadCheckerTexture();
	int SetTextureId(int id);

private:
	uint texture_buffer_id = 0;
	uint width = 0, height = 0;
	std::string texture_name;
	bool flip = false;
public:
	bool show_checker_tex = false;
	uint check_id = 0;
	int tmp_id = 0;
	int texture_id = 0;
	std::string p_tex;
};

#endif

