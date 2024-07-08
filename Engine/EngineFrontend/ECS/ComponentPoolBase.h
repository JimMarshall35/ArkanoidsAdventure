#pragma once
#include "ECSDefs.h"
class ComponentPoolBase
{
public:
	virtual void AddToPool(Entity entity) = 0;
	virtual void RemoveFromPool(Entity entity) = 0;
	virtual bool ContainsEntity(Entity entity) = 0;
};
