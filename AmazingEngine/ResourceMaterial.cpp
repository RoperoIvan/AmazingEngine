#include "ResourceMaterial.h"
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "SDL/include/SDL_opengl.h"


ResourceMaterial::ResourceMaterial(uint id) : Resource(id, TYPE::MATERIAL)
{
}


ResourceMaterial::~ResourceMaterial()
{
	glDeleteTextures(1, &texture_buffer_id);
}

void ResourceMaterial::SetData(uint text_id, const char * t_name)
{
	texture_buffer_id = text_id;
	texture_name = t_name;
}
