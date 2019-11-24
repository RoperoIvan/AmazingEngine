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

	void Init(const float& x, const float& y, const float& z);
	void Init(float4x4 r);
	bool LoadTransformation();
	void RotateObjects(GameObject* object_to_rotate);

	int GetTextureId() { return 0; };
	std::string GetTexturePath() { return std::string(); };
	float3 GetPosition()
	{
		return position;
	}
	//bool HandleGizmos();
public:

	math::float3 position = math::float3::zero;
	math::float3 scale = math::float3::one;
	math::Quat rot = math::Quat::identity;
	math::float3 euler_angles = math::float3::zero;

	math::float4x4 rotation_matrix = math::float4x4::identity;
	math::float4x4 global_matrix = math::float4x4::identity;
private:
	bool transform_now = false;
};



#endif
