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

private:
	uint texture_buffer_id = 0;
	uint width = 0, height = 0;
	std::string texture_name;
	bool flip = false;
};

#endif

