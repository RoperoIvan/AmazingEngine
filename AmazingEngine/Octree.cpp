#include "Octree.h"
#include "MathGeoLib\include\Math\float3.h"
#include "Application.h"
#include "ModuleMesh.h"
#include "ModuleScene.h"
#include "Transform.h"
Octree::Octree(AABB limits, uint max_objects_in_level, uint max_level, uint current_level, Octree* p)
	:aabb(limits), max_objects(max_objects_in_level), max_levels(max_level), current_level(current_level), parent(p)
{
}

Octree::~Octree()
{
	Clear();
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

	if (!is_divided)
	{
		static_objects.push_back(object);
		if (max_objects <= static_objects.size() && current_level <= max_levels)
		{
			Subdivide();
			is_divided = true;
		}
	}
	else
	{
		for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
		{
			
			if ((*iter)->aabb.Contains(object->bounding_box->aabb))
			{
				(*iter)->Insert(object);
			}
		}
	}
}

bool Octree::Remove(GameObject* object)
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
		{
			if ((*iter)->Remove(object))
			{
				DeleteChilds();
				if (Resize())
				{
					return true;
				}
				break;
			}
		}	
	}
	else
	{
		if (Resize())
		{
			return true;
		}
	}
	return false;
}



bool Octree::Resize()
{
	if (is_divided)
		return false;

	if (parent != nullptr)
	{
		float num_objects = static_objects.size();

		for (std::vector<Octree*>::iterator iter = parent->childs.begin(); iter != parent->childs.end(); ++iter)
		{
			num_objects += parent->static_objects.size();
			for (std::vector<GameObject*>::iterator it = (*iter)->static_objects.begin(); it != (*iter)->static_objects.end(); ++it)
			{
				if (*iter != this)
					static_objects.push_back(*it);

			}
		}
		if (num_objects <= max_objects)
		{
			for (std::vector<GameObject*>::iterator it = static_objects.begin(); it != static_objects.end(); ++it)
			{
				parent->static_objects.push_back(*it);
			}
			return true;
		}
	}
	return false;
}

void Octree::Subdivide()
{	
	//1 division left-down-front
	float3 aabb_points[8];
	aabb_points[0] = aabb.CornerPoint(0);
	aabb_points[1] = (aabb.CornerPoint(0) + aabb.CornerPoint(1))/2;
	aabb_points[2] = (aabb.CornerPoint(0) + aabb.CornerPoint(4)) / 2;
	aabb_points[3] = (aabb.CornerPoint(0) + aabb.CornerPoint(2)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(0);//-X
	aabb_points[5] = aabb.FaceCenterPoint(2);//-Y
	aabb_points[6] = aabb.FaceCenterPoint(4);//-Z
	aabb_points[7] = aabb.CenterPoint();
	math::AABB new_aabb;
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);
	
	Octree* node0 = new Octree(new_aabb,max_objects,max_levels,current_level + 1,this);
	childs.push_back(node0);

	//2 division left-up-front
	aabb_points[0] = aabb.CornerPoint(2);
	aabb_points[1] = (aabb.CornerPoint(2) + aabb.CornerPoint(3)) / 2;
	aabb_points[2] = (aabb.CornerPoint(2) + aabb.CornerPoint(0)) / 2;
	aabb_points[3] = (aabb.CornerPoint(2) + aabb.CornerPoint(6)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(0);//-X
	aabb_points[5] = aabb.FaceCenterPoint(3);//+Y
	aabb_points[6] = aabb.FaceCenterPoint(4);//-Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node1 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node1);

	//3 division right-down-front
	aabb_points[0] = aabb.CornerPoint(4);
	aabb_points[1] = (aabb.CornerPoint(4) + aabb.CornerPoint(5)) / 2;
	aabb_points[2] = (aabb.CornerPoint(4) + aabb.CornerPoint(0)) / 2;
	aabb_points[3] = (aabb.CornerPoint(4) + aabb.CornerPoint(6)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(1);//+X
	aabb_points[5] = aabb.FaceCenterPoint(2);//-Y
	aabb_points[6] = aabb.FaceCenterPoint(4);//-Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node2 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node2);

	//4 division right-down-front
	aabb_points[0] = aabb.CornerPoint(6);
	aabb_points[1] = (aabb.CornerPoint(6) + aabb.CornerPoint(7)) / 2;
	aabb_points[2] = (aabb.CornerPoint(6) + aabb.CornerPoint(4)) / 2;
	aabb_points[3] = (aabb.CornerPoint(6) + aabb.CornerPoint(2)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(1);//+X
	aabb_points[5] = aabb.FaceCenterPoint(3);//+Y
	aabb_points[6] = aabb.FaceCenterPoint(4);//-Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node3 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node3);

	//5 division left-down-back
	aabb_points[0] = aabb.CornerPoint(1);
	aabb_points[1] = (aabb.CornerPoint(1) + aabb.CornerPoint(0)) / 2;
	aabb_points[2] = (aabb.CornerPoint(1) + aabb.CornerPoint(3)) / 2;
	aabb_points[3] = (aabb.CornerPoint(1) + aabb.CornerPoint(5)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(0);//-X
	aabb_points[5] = aabb.FaceCenterPoint(2);//-Y
	aabb_points[6] = aabb.FaceCenterPoint(5);//+Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node4 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node4);

	//6 division left-up-back
	aabb_points[0] = aabb.CornerPoint(3);
	aabb_points[1] = (aabb.CornerPoint(3) + aabb.CornerPoint(1)) / 2;
	aabb_points[2] = (aabb.CornerPoint(3) + aabb.CornerPoint(2)) / 2;
	aabb_points[3] = (aabb.CornerPoint(3) + aabb.CornerPoint(7)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(0);//-X
	aabb_points[5] = aabb.FaceCenterPoint(3);//+Y
	aabb_points[6] = aabb.FaceCenterPoint(5);//+Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node5 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node5);

	//7 division right-down-back
	aabb_points[0] = aabb.CornerPoint(5);
	aabb_points[1] = (aabb.CornerPoint(5) + aabb.CornerPoint(1)) / 2;
	aabb_points[2] = (aabb.CornerPoint(5) + aabb.CornerPoint(4)) / 2;
	aabb_points[3] = (aabb.CornerPoint(5) + aabb.CornerPoint(7)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(1);//+X
	aabb_points[5] = aabb.FaceCenterPoint(2);//-Y
	aabb_points[6] = aabb.FaceCenterPoint(5);//+Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node6 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node6);

	//8 division right-up-back
	aabb_points[0] = aabb.CornerPoint(7);
	aabb_points[1] = (aabb.CornerPoint(7) + aabb.CornerPoint(3)) / 2;
	aabb_points[2] = (aabb.CornerPoint(7) + aabb.CornerPoint(5)) / 2;
	aabb_points[3] = (aabb.CornerPoint(7) + aabb.CornerPoint(6)) / 2;
	aabb_points[4] = aabb.FaceCenterPoint(1);//+X
	aabb_points[5] = aabb.FaceCenterPoint(3);//+Y
	aabb_points[6] = aabb.FaceCenterPoint(5);//+Z
	aabb_points[7] = aabb.CenterPoint();
	new_aabb.SetNegativeInfinity();
	new_aabb.Enclose(&aabb_points[0], 8);

	Octree* node7 = new Octree(new_aabb, max_objects, max_levels, current_level + 1, this);
	childs.push_back(node7);

	std::vector<GameObject*>::iterator iter = static_objects.begin();
	while (iter != static_objects.end())
	{
		if (*iter != nullptr)
		{
			bool inserted = false;
			for (std::vector<Octree*>::iterator it = childs.begin(); it != childs.end(); ++it)
			{
				if ((*it)->aabb.Contains((*iter)->bounding_box->aabb))
				{
					(*it)->Insert((*iter));
					iter = static_objects.erase(iter);
					inserted = true;
					break;
				}
			}
			if (!inserted)
				++iter;
		}
	}
}

void Octree::Draw()
{
	
		for (int i = 0; i < 4; i++)
		{
			glVertex3f(aabb.CornerPoint(i + 4).x, aabb.CornerPoint(i + 4).y, aabb.CornerPoint(i + 4).z);
			glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
		}
		//Create the vertices that define the cubes faces that form the mesh box
		for (int i = 0; i <= 4; i += 4)
		{
			glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
			glVertex3f(aabb.CornerPoint(i + 1).x, aabb.CornerPoint(i + 1).y, aabb.CornerPoint(i + 1).z);

			glVertex3f(aabb.CornerPoint(i + 2).x, aabb.CornerPoint(i + 2).y, aabb.CornerPoint(i + 2).z);
			glVertex3f(aabb.CornerPoint(i + 3).x, aabb.CornerPoint(i + 3).y, aabb.CornerPoint(i + 3).z);

			glVertex3f(aabb.CornerPoint(i).x, aabb.CornerPoint(i).y, aabb.CornerPoint(i).z);
			glVertex3f(aabb.CornerPoint(i + 2).x, aabb.CornerPoint(i + 2).y, aabb.CornerPoint(i + 2).z);

			glVertex3f(aabb.CornerPoint(i + 1).x, aabb.CornerPoint(i + 1).y, aabb.CornerPoint(i + 1).z);
			glVertex3f(aabb.CornerPoint(i + 3).x, aabb.CornerPoint(i + 3).y, aabb.CornerPoint(i + 3).z);
		}

	if(is_divided)
		for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
			(*iter)->Draw();
}

void Octree::DeleteChilds()
{
	for (std::vector<Octree*>::iterator iter = childs.begin(); iter != childs.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;		
	}
	is_divided = false;
	childs.clear();
}
