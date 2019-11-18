#include "Octree.h"
#include "MathGeoLib\include\Math\float3.h"


Octree::Octree(AABB limits, uint max_objects_in_level):aabb(limits), max_objects(max_objects_in_level)
{
}

Octree::~Octree()
{
	for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}
}

bool Octree::Clear()
{
	bool ret = true;
	for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
	{
		(*iter)->Clear();
		delete* iter;
		*iter = nullptr;
	}
	childs.clear();
	static_objects.clear();

	return ret;
}

void Octree::Insert(GameObject* object)
{
	if (!object->is_static)
		return;

	if (!is_divided)
	{
		if (max_objects < static_objects.size())
			Subdivide();
		else
			static_objects.push_back(object);
	}
	else
	{
		for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
		{
			if ((*iter)->aabb.Contains(object->bounding_box))
				(*iter)->Insert(object);
		}
	}
}

void Octree::Remove(GameObject* object)
{
	bool is_removed = false;
	for (std::vector<GameObject*>::iterator iter = static_objects.begin(); iter != static_objects.end(); ++iter)
	{
		if (object == *iter)
		{
			static_objects.erase(iter);
			is_removed = true;
			break;
		}
	}
	if (!is_removed)
	{
		for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
			(*iter)->Remove(object);
	}
}

void Octree::Intersect(GameObject*)
{
}

void Octree::Subdivide()
{	
	//1 division left-down-front
	std::vector<math::float3> aabb_points;
	aabb_points.push_back(aabb.CornerPoint(0));
	aabb_points.push_back((aabb.CornerPoint(0) + aabb.CornerPoint(1))/2);
	aabb_points.push_back((aabb.CornerPoint(0) + aabb.CornerPoint(4)) / 2);
	aabb_points.push_back((aabb.CornerPoint(0) + aabb.CornerPoint(2)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(0));//-X
	aabb_points.push_back(aabb.FaceCenterPoint(2));//-Y
	aabb_points.push_back(aabb.FaceCenterPoint(4));//-Z
	aabb_points.push_back(aabb.CenterPoint());
	math::AABB new_aabb;
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());
	
	Octree* node0 = new Octree(new_aabb,max_objects);
	childs.push_back(node0);

	//2 division left-up-front
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(2));
	aabb_points.push_back((aabb.CornerPoint(2) + aabb.CornerPoint(3)) / 2);
	aabb_points.push_back((aabb.CornerPoint(2) + aabb.CornerPoint(0)) / 2);
	aabb_points.push_back((aabb.CornerPoint(2) + aabb.CornerPoint(6)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(0));//-X
	aabb_points.push_back(aabb.FaceCenterPoint(3));//+Y
	aabb_points.push_back(aabb.FaceCenterPoint(4));//-Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node1 = new Octree(new_aabb, max_objects);
	childs.push_back(node1);

	//3 division right-down-front
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(4));
	aabb_points.push_back((aabb.CornerPoint(4) + aabb.CornerPoint(5)) / 2);
	aabb_points.push_back((aabb.CornerPoint(4) + aabb.CornerPoint(0)) / 2);
	aabb_points.push_back((aabb.CornerPoint(4) + aabb.CornerPoint(6)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(1));//+X
	aabb_points.push_back(aabb.FaceCenterPoint(2));//-Y
	aabb_points.push_back(aabb.FaceCenterPoint(4));//-Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node2 = new Octree(new_aabb, max_objects);
	childs.push_back(node2);

	//4 division right-down-front
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(6));
	aabb_points.push_back((aabb.CornerPoint(6) + aabb.CornerPoint(7)) / 2);
	aabb_points.push_back((aabb.CornerPoint(6) + aabb.CornerPoint(4)) / 2);
	aabb_points.push_back((aabb.CornerPoint(6) + aabb.CornerPoint(2)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(1));//+X
	aabb_points.push_back(aabb.FaceCenterPoint(3));//+Y
	aabb_points.push_back(aabb.FaceCenterPoint(4));//-Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node3 = new Octree(new_aabb, max_objects);
	childs.push_back(node3);

	//5 division left-down-back
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(1));
	aabb_points.push_back((aabb.CornerPoint(1) + aabb.CornerPoint(0)) / 2);
	aabb_points.push_back((aabb.CornerPoint(1) + aabb.CornerPoint(3)) / 2);
	aabb_points.push_back((aabb.CornerPoint(1) + aabb.CornerPoint(5)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(0));//-X
	aabb_points.push_back(aabb.FaceCenterPoint(2));//-Y
	aabb_points.push_back(aabb.FaceCenterPoint(5));//+Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node4 = new Octree(new_aabb, max_objects);
	childs.push_back(node4);

	//6 division left-up-back
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(3));
	aabb_points.push_back((aabb.CornerPoint(3) + aabb.CornerPoint(1)) / 2);
	aabb_points.push_back((aabb.CornerPoint(3) + aabb.CornerPoint(2)) / 2);
	aabb_points.push_back((aabb.CornerPoint(3) + aabb.CornerPoint(7)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(0));//-X
	aabb_points.push_back(aabb.FaceCenterPoint(3));//+Y
	aabb_points.push_back(aabb.FaceCenterPoint(5));//+Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node5 = new Octree(new_aabb, max_objects);
	childs.push_back(node5);

	//7 division right-down-back
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(5));
	aabb_points.push_back((aabb.CornerPoint(5) + aabb.CornerPoint(1)) / 2);
	aabb_points.push_back((aabb.CornerPoint(5) + aabb.CornerPoint(4)) / 2);
	aabb_points.push_back((aabb.CornerPoint(5) + aabb.CornerPoint(7)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(1));//+X
	aabb_points.push_back(aabb.FaceCenterPoint(2));//-Y
	aabb_points.push_back(aabb.FaceCenterPoint(5));//+Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node6 = new Octree(new_aabb, max_objects);
	childs.push_back(node6);

	//8 division right-up-back
	aabb_points.clear();
	aabb_points.push_back(aabb.CornerPoint(7));
	aabb_points.push_back((aabb.CornerPoint(7) + aabb.CornerPoint(3)) / 2);
	aabb_points.push_back((aabb.CornerPoint(7) + aabb.CornerPoint(5)) / 2);
	aabb_points.push_back((aabb.CornerPoint(7) + aabb.CornerPoint(6)) / 2);
	aabb_points.push_back(aabb.FaceCenterPoint(1));//+X
	aabb_points.push_back(aabb.FaceCenterPoint(3));//+Y
	aabb_points.push_back(aabb.FaceCenterPoint(5));//+Z
	aabb_points.push_back(aabb.CenterPoint());
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], (int)aabb_points.size());

	Octree* node7 = new Octree(new_aabb, max_objects);
	childs.push_back(node7);
}
