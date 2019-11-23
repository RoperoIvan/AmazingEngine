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

int ResourceMaterial::GetTextureId()
{
	return texture_id;
}

std::string ResourceMaterial::GetTexturePath()
{
	return p_tex;
}

void ResourceMaterial::LoadCheckerTexture()
{
	GLubyte checkImage[64][64][4];
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &check_id);
	glBindTexture(GL_TEXTURE_2D, check_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glBindTexture(GL_TEXTURE_2D, NULL);
}

int ResourceMaterial::SetTextureId(int id)
{
	texture_id = id;
	return texture_id;
}
