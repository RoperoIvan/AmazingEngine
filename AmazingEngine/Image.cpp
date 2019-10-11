#include "Image.h"

#include "DevIL/include/IL/ilu.h"

#pragma comment(lib, "DevIL/lib/x86/Release/ILU.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/DevIL.lib")
#pragma comment(lib, "DevIL/lib/x86/Release/ILUT.lib")


Image::Image(const char* image_name)
{
	//init devil
	ilInit();
	//open image
	FILE* file = fopen(image_name, "rb");
	if (!file)
	{
		LOG("Image couldn't be opened\n");
	}
	else
	{
		//check if is a BMP file
		if (fread(header, 1, 54, file) != 54 || (header[0] != 'B' || header[1] != 'M'))
		{
			LOG("Not a correct BMP file\n");
		}
		//Load file
		else
		{
			data_pos = *(int*) & (header[0x0A]);
			image_size = *(int*) & (header[0x22]);
			width = *(int*) & (header[0x12]);
			height = *(int*) & (header[0x16]);

			if (image_size == 0)
				image_size = width * height * 3;
			if (data_pos == 0)
				data_pos = 54;
		}
		iluFlipImage();

		//copy image data and close the file
		data = new unsigned char[image_size];
		fread(data, 1, image_size, file);
		fclose(file);

		//create openGL texture
		LoadTexture();

	}
}

void Image::LoadTexture()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void Image::DrawTexture()
{

}


//compressed textures
ImageDDS::ImageDDS(const char* file_name)
{
	unsigned char header[124];

	FILE* fp;

	/* try to open the file */
	fp = fopen(file_name, "rb");

	if (fp == NULL)
	{
		LOG("Not a correct BMP file\n");
	}
	else
	{
		/* verify the type of file */

		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0)
			fclose(fp);


		/* get the surface desc */
		if (fread(&header, 124, 1, fp))
			LOG("Not a correct BMP file\n");

		height = *(unsigned int*) & (header[8]);
		width = *(unsigned int*) & (header[12]);
		linear_size = *(unsigned int*) & (header[16]);
		mip_map_count = *(unsigned int*) & (header[24]);
		fourCC = *(unsigned int*) & (header[80]);


		bufsize = mip_map_count > 1 ? linear_size * 2 : linear_size;
		buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
		fread(buffer, 1, bufsize, fp);
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		LoadTexture();

		fclose(fp);
	}
}

void ImageDDS::LoadTexture()
{
	
	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);


	uint block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	uint offset = 0;
	for (unsigned int level = 0; level < mip_map_count && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
	}
	free(buffer);
}

void ImageDDS::DrawTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}
