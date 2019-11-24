#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/include/Geometry/AABB.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_level, uint max_level, uint current_level, Octree* p = nullptr);
	~Octree();

	bool Clear();
	void Insert(GameObject*);
	bool Remove(GameObject*);

	template<typename TYPE>
	void CollectObjects(const TYPE& primitive, std::vector<MouseHit>&objects);
	bool Resize();
	void Subdivide();

	void Draw();

	void DeleteChilds();
private:
	std::vector<GameObject*> static_objects;
	std::vector<Octree*> childs;
	AABB aabb;
	uint max_objects;
	uint max_levels;
	uint current_level;
	bool is_divided = false;

	Octree* parent = nullptr;
};

template<typename TYPE>
void Octree::CollectObjects(const TYPE& primitive, std::vector<MouseHit>& objects)
{
	for (std::vector<GameObject*>::iterator iter = static_objects.begin(); iter != static_objects.end(); ++iter)
	{
		if ((*iter)->bounding_box->aabb.Intersects(primitive))
		{
			MouseHit h;
			h.object = *iter;
			objects.push_back(h);
		}
	}
	for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
	{
		if((*iter)->aabb.Intersects(primitive))
			(*iter)->CollectObjects(primitive, objects);
	}
}
#endif