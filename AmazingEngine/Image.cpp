#include "Image.h"
#include <vector>
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
	LoadCheckerTexture();
}

Image::~Image()
{

}

void Image::Disable()
{
	delete[] uv_coord;
	uv_coord = nullptr;
}

void Image::Update()
{
}


GLuint Image::LoadImages(const char* p_tex)
{
	//Gen image
	ILuint img_id = 0;
	ilGenImages(1, &img_id);
	ilBindImage(img_id);

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

void Image::LoadCoords(aiMesh* scene)
{
	if (scene->HasTextureCoords(0))
	{
		num_coords = scene->mNumVertices * 2;
		uv_coord = new float[num_coords];
		for (uint i = 0; i < scene->GetNumUVChannels(); ++i)
		{
			for (uint k = 0; k < scene->mNumVertices; ++k) {
				uv_coord[k * 2] = scene->mTextureCoords[i][k].x;
				uv_coord[k * 2 + 1] = scene->mTextureCoords[i][k].y;
				/*LOG("Texture coords: %f", texture_coords[k]);*/
			}
		}
	}
}

void Image::LoadCoords(par_shapes_mesh* p_mesh)
{
	num_coords = p_mesh->npoints * 2 / 3;
	uv_coord = p_mesh->tcoords;
	LoadBuffers();
}

void Image::LoadMaterials(const aiScene* scene, std::string file_name)
{
	if (scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString text_path;
		scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &text_path);
		std::string  tex = text_path.C_Str();
		std::string  p_geo = file_name;

		//We change the name of the fbx for the texture name, with this made we have the general path
		while (p_geo.back() != '\\')
		{
			p_geo.pop_back();
		}
		p_geo += tex;
		p_tex = p_geo;
		texture_id = LoadImages(p_geo.c_str());

		LoadBuffers();
	}
	else
		LOG("It hasn't been detected a material");
}

void Image::LoadBuffers()
{
	glGenBuffers(1, (uint*) & (id_coords));
	glBindBuffer(GL_ARRAY_BUFFER, id_coords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_coords, uv_coord, GL_STATIC_DRAW);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		LOG("Error Storing textures! %s\n", gluErrorString(error));
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
}

int Image::GetTextureId()
{
	return texture_id;
}

void Image::SetTextureId()
{
	texture_id = check_id;
}

std::string Image::GetTexturePath()
{
	return p_tex;
}

