#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "json.hpp"
#include <vector>

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

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void GoAroundGeometry(std::vector<GameObject*>* vec);
private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;

	float camera_speed = 0.01;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};