#ifndef _CAMERA_H
#define _CAMERA_H

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"

class Camera : public Component
{
public:
	Camera(GameObject* parent);
	~Camera();
	void Disable() override;
	void Update() override;
	std::string GetTexturePath() { return std::string(); };
	int GetTextureId() { return 0; };
	void Look(const float3 &Position);
	float* GetViewMatrix();
	void LoadCameraOptions();
private:

public:
	math::Frustum frustum;

private:
	float4x4 ViewMatrix, ViewMatrixInverse;
};


#endif

