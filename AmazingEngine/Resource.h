#ifndef _RESOURCE_H
#define _RESOURCE_H

#include "Globals.h"
#include <string>


class Resource
{
public:

	enum TYPE
	{
		MATERIAL,
		MESH,
		NONE = 0,
	};

	Resource(uint uid, TYPE type);
	virtual ~Resource();

	TYPE GetType() const
	{
		return type;
	}

	uint GetID() const
	{
		return UID;
	}

	bool HasInstancies() const
	{
		return instancies > 0;
	}

	void IncreaseInstancies()
	{
		instancies++;
	}

	void DecreaseInstancies()
	{
		if (instancies>0)
			instancies--;
	}

protected:
	TYPE type = TYPE::NONE;
	uint UID = 0;
	std::string file;
	std::string library_file;
	uint instancies = 0;
};
#endif 