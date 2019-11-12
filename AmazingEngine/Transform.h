#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"
#include "par_shapes.h"
#include "MathGeoLib\include\Math\Quat.h"
#include "MathGeoLib\include\Math\float3.h"
#include "MathGeoLib\include\Math\float4x4.h"

class Geometry;

class Transform :public Component
{
public:
	
	Transform(GameObject* parent);
	~Transform();

	void Enable();
	void Update();
	void Disable();

	void Init(const int& x, const int& y, const int& z);
	bool LoadTransformation();
	void RotateObjects(GameObject* object_to_rotate);
	void UnLoadTransformation();

	int GetTextureId() { return 0; };
	std::string GetTexturePath() { return std::string(); };
	void DoRotation(Geometry* mesh, float4x4 r_matrix);
public:

	math::float3 position = math::float3::zero;
	math::float3 scale = math::float3::one;
	math::Quat rot = math::Quat::identity;
	math::float3 euler_angles = math::float3::zero;

	math::float4x4 rotation_matrix = math::float4x4::identity;
};



#endif
