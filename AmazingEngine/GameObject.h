#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>
#include <vector>
#include "Component.h"
#include "Globals.h"
#include "MathGeoLib/include/Geometry/AABB.h"

class Geometry;
struct MouseHit;
struct less_than_key;

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	void Update();

	Component* CreateComponent(COMPONENT_TYPE type);
	virtual void GetHierarcy();
	void GetPropierties();
	void ShowPropertiesObject(GameObject* , uint&, uint&);
	void ShowNormalsVertices(const bool&);
	void ShowNormalsFaces(const bool&);
	void LookForRayCollision(GameObject* &near_one, LineSegment ray_segment, float& from_origin_dist, std::vector<MouseHit>& hit);
	void LookForMeshCollision(GameObject* &near_one, LineSegment ray_segment, float& from_origin_dist, std::vector<MouseHit>& hit);
	void AddComponent(Component* comp)
	{
		components.push_back(comp);
	}
	Component* GetComponentByType(COMPONENT_TYPE type);

public:
	std::string name;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	bool show_inspector_window = false;
	bool to_delete = false;
	bool is_enable = true;

	bool show_vertices_normals = false;
	bool show_face_normals = false;

	math::AABB bounding_box;
	bool show_bounding_box = false;
};

#endif