#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"
#include "par_shapes.h"

class Geometry;

class Transform :public Component
{
public:
	
	Transform(GameObject* parent);
	~Transform();

	void Enable();
	void Update();
	void Disable();

	void LoadTransformation(Geometry* p_mesh,int trans[3], int scle[3], float rd, float axs[3]);
	void UnLoadTransformation();

	int GetTextureId() { return 0; };
	void ShowProperties() {};
	std::string GetTexturePath() { return std::string(); };
	void ChangeScale(Geometry* mesh, float x, float y, float z);
	void ChangePosition(Geometry* mesh, float x, float y, float z);
	void Rotate(Geometry* mesh, float rd, float axs[3]);
	void DoRotation(Geometry* mesh, float  r_matrix[3][3]);
public:

	int translation[3] = { 1,1,1 };
	int scale[3] = { 1,1,1 };
	float rad = 0;
	float axis[3] = { 0,0,0 };
	Geometry* mesh = nullptr;

	float R[3][3] = 
	{ 1, 0, 0,
	0, 1, 0,
	0, 0, 1 };
};



#endif
