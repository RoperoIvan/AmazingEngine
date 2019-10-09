#ifndef _IMAGE_H
#define _IMAGE_H

#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Globals.h"

struct Image
{
	Image(const char* image_name);

	void LoadTexture();

	void DrawTexture();

	GLuint texture_id = 0;
	unsigned char header[54];
	uint data_pos;
	uint width, height;
	uint image_size;
	unsigned char* data;
};

struct ImageDDS
{
	ImageDDS(const char* image_name);

	void LoadTexture();

	void DrawTexture();

	GLuint texture_id = 0;
	char filecode[4];
	uint height;
	uint width;
	uint linear_size;
	uint mip_map_count;
	uint fourCC;

	uint bufsize;
	unsigned char* buffer;

	uint format;
};


#endif //_GEOMETRY_H


