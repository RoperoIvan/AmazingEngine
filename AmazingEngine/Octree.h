#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "MathGeoLib/include/Geometry/AABB.h"
#include "GameObject.h"

class Octree
{
public:
	Octree(AABB limits, uint max_objects_in_level);
	~Octree();

	bool Clear();
	void Insert(GameObject*);
	void Remove(GameObject*);
	void Intersect(GameObject*);

	void Subdivide();

	void Draw();

private:
	std::vector<GameObject*> static_objects;
	std::vector<Octree*> childs;
	AABB aabb;
	uint max_objects;
	
	bool is_divided = false;
};

#endif