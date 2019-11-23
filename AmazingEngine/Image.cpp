#include "Image.h"
#include <vector>
#include "Application.h"
#include "ModuleScene.h"
#include <experimental\filesystem>
#include "ImGui/imgui.h"
#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment(lib, "DevIL/libx86/ILU.lib")
#pragma comment(lib, "DevIL/libx86/DevIL.lib")
#pragma comment(lib, "DevIL/libx86/ILUT.lib")

Image::Image(GameObject* parent) : Component(parent, COMPONENT_TYPE::COMPONENT_MATERIAL)
{
}

Image::~Image()
{

}
//
//void Image::Disable()
//{
//	for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
//	{
//		if (*iter = this)
//		{
//			App->scene->textures.erase(iter);
//			break;
//		}
//	}
//}

void Image::Update()
{
}

//int Image::GetTextureId()
//{
//	return texture_id;
//}
//
//std::string Image::GetTexturePath()
//{
//	return p_tex;
//}
//
//void Image::LoadCheckerTexture()
//{
//	GLubyte checkImage[64][64][4];
//	for (int i = 0; i < 64; i++) {
//		for (int j = 0; j < 64; j++) {
//			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
//			checkImage[i][j][0] = (GLubyte)c;
//			checkImage[i][j][1] = (GLubyte)c;
//			checkImage[i][j][2] = (GLubyte)c;
//			checkImage[i][j][3] = (GLubyte)255;
//		}
//	}
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glGenTextures(1, &check_id);
//	glBindTexture(GL_TEXTURE_2D, check_id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
//	glBindTexture(GL_TEXTURE_2D, NULL);
//}
//
//int Image::SetTextureId(int id)
//{
//	texture_id = id;
//	return texture_id;
//}