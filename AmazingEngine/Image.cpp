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

void Image::Disable()
{
	for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
	{
		if (*iter = this)
		{
			App->scene->textures.erase(iter);
			break;
		}
	}
}

void Image::Update()
{
}


void Image::Save(FILE * file)
{
	fputs("texture_id: ", file);
	fprintf(file, "%i", texture_id);
	fputs(";\n", file);
}

GLuint Image::LoadImages(const char* p_tex)
{

	ILuint img_id = GetID();

	//load from path
	ilLoadImage(p_tex);

	ILuint devilError1 = ilGetError();
	if (devilError1 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError1));
		return 0;
	}

	// If the image is flipped
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);



	ILuint devilError2 = ilGetError();
	if (devilError2 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError2));
		return 0;
	}

	//Send texture to GPU
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &img_id);
	glBindTexture(GL_TEXTURE_2D, img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
		0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

	ILuint devilError3 = ilGetError();
	if (devilError3 != IL_NO_ERROR)
	{
		LOG("Devil Error (ilInit: %s)", iluErrorString(devilError3));
		return 0;
	}

	tex_dimension[0] = ilGetInteger(IL_IMAGE_WIDTH);
	tex_dimension[1] = ilGetInteger(IL_IMAGE_HEIGHT);

	return img_id;
}

GLuint Image::GetID()
{
	//Gen image
	ILuint img_id = 0;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);
	return img_id;
}

bool Image::LoadMaterials(const aiScene* scene, std::string file_name, std::vector<std::pair<aiMaterial*, int>>& tmp_mat, int last_mat_ind)
{

	if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString text_path;
		tmp_mat[last_mat_ind].first->GetTexture(aiTextureType_DIFFUSE, 0, &text_path);
		std::string  tex = text_path.C_Str();
		std::string  p_geo = file_name;

		//We change the name of the fbx for the texture name, with this made we have the general path
		while (p_geo.back() != '\\')
		{
			p_geo.pop_back();
		}
		p_geo += tex;
		p_tex = p_geo;
		p_tex = std::experimental::filesystem::path(tex).stem().string().c_str();

		//Look for if the texture has been already loaded
		if (tmp_mat[last_mat_ind].second == -1)
		{
			texture_id = LoadImages(p_geo.c_str());
			tmp_id = texture_id;
			//Sending texture to our texture folder inside library folder
			App->mesh->ImportTextureToDDSFile(tex.c_str());
			tmp_mat[last_mat_ind].second = texture_id;
			if (texture_id == 0)
			{
				LOG("Warning: --------Scene missing textures");
			}
		}
		else
		{
			texture_id = tmp_mat[last_mat_ind].second;
			tmp_id = texture_id;
		}
		
		/*for (std::vector<Image*>::iterator iter = App->scene->textures.begin(); iter != App->scene->textures.end(); ++iter)
		{
			if (p_geo == (*iter)->p_tex)
			{
				return false;
				break;
			}
		}*/
		
	}
	else
		LOG("It hasn't been detected a material");
	return true;
}


int Image::GetTextureId()
{
	return texture_id;
}

std::string Image::GetTexturePath()
{
	return p_tex;
}

void Image::LoadCheckerTexture()
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

int Image::SetTextureId(int id)
{
	texture_id = id;
	return texture_id;
}