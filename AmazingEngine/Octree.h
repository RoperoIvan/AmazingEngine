#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/include/Geometry/AABB.h"
#include "GameObject.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_level, uint max_level, uint current_level, Octree* p = nullptr);
	~Octree();

	bool Clear();
	void Insert(GameObject*);
	bool Remove(GameObject*);
	void Intersect(std::vector<GameObject*>&);

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

#endif