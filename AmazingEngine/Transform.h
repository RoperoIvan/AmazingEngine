#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"
#include "par_shapes.h"

class Transform :public Component
{
public:
	
	Transform(GameObject* parent);
	~Transform();

	void Enable();
	void Update();
	void Disable();

	void LoadTransformation(par_shapes_mesh* p_mesh,int trans[3], int scle[3], float rd, float axs[3]);
	void UnLoadTransformation();

	void ShowProperties() {};
public:

	int translation[3] = { 1,1,1 };
	int scale[3] = { 1,1,1 };
	float rad = 0;
	float axis[3] = { 0,0,0 };
	par_shapes_mesh* mesh = nullptr;
};



#endif
