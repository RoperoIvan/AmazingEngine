#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <string>
#include <vector>
#include "Component.h"
#include "Globals.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"


class Geometry;
struct MouseHit;
struct less_than_key;

struct BoundingBox
{
	AABB aabb;
	OBB obb;
};

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	void Update();

	void Draw();

	Component* CreateComponent(COMPONENT_TYPE type);
	virtual void GetHierarcy();
	void GetPropierties();
	void ShowPropertiesObject(GameObject* , uint&, uint&);
	void ShowNormalsVertices(const bool&);
	void ShowNormalsFaces(const bool&);

	void LookForMeshCollision(LineSegment ray_segment, MouseHit& hit);
	void AddComponent(Component* comp) { components.push_back(comp); }
	Component* GetComponentByType(COMPONENT_TYPE type);

	GameObject* FindChildByID(uint other_uid) const;
	void SetParent(GameObject* new_parent);
	void TransformBoundingBox(math::float4x4 matrix);

public:
	std::string name;
	int ID;
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	bool show_inspector_window = false;
	bool to_delete = false;
	bool is_enable = true;

	bool show_vertices_normals = false;
	bool show_face_normals = false;

	BoundingBox* bounding_box = nullptr;
	bool show_bounding_box = false;

	bool is_static = true;
};

#endif