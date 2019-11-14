#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "json.hpp"
#include <vector>
#include "MathGeoLib/include/MathGeoLib.h"
#include "Camera.h"

class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Save(nlohmann::json &j) const override;
	bool Load(nlohmann::json &j) override;

	/*void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);*/
	void Move(const float3 &Movement);
	void CameraControls(float dt);
	float* GetViewMatrix();
	void GoAroundGeometry(GameObject* obj);
	std::vector<float3> LoadAABBVertex(GameObject* obj, std::vector<float3> vertices);
private:

	//void CalculateViewMatrix();

public:
	Camera* my_camera = nullptr;
	/*vec3 X, Y, Z, Position, Reference;*/
	float3 Reference = float3::zero;
	float speed = 0.01;
	bool write = false;
private:
	Frustum* c_frustum = nullptr;
	mat4x4 ViewMatrix, ViewMatrixInverse;
};