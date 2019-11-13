#ifndef _CAMERA_H
#define _CAMERA_H

#include "Component.h"
#include "MathGeoLib/include/Geometry/Frustum.h"

class Camera : public Component
{
public:
	Camera(GameObject* parent, float z_near_distance, float z_far_distance);
	~Camera();
	void Disable() override;
	void Update() override;
	std::string GetTexturePath() { return std::string(); };
	int GetTextureId() { return 0; };
public:
	math::Frustum frustum;
	float window_aspect_ratio = 0.f;
};


#endif

