#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "json.hpp"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "Camera.h"

class GameObject;

struct MouseHit
{
	float distance = 0.f;
	GameObject* object = nullptr;
};

struct less_than_key
{
	inline bool operator() (const MouseHit& struct1, const MouseHit& struct2)
	{
		return (struct1.distance < struct2.distance);
	}
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	bool Save(nlohmann::json &j) const override;
	bool Load(nlohmann::json &j) override;
	bool LittleThan(float i, float j) { return i < j; }
	void CameraControls(float dt);
	void GoAroundGeometry(GameObject* obj);
	void CatchMousePicking();
public:
	Camera* my_camera = nullptr;
	float3 Reference = float3::zero;
	float camera_speed = 0.1f;
	bool write = false;
	LineSegment ray_picking;
private:
	Frustum* c_frustum = nullptr;
	mat4x4 ViewMatrix, ViewMatrixInverse;
};