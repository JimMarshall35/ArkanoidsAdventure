#pragma once
#include "ComponentPoolBase.h"
#include <map>

template<typename ComponentDataT>
class ComponentPool : public ComponentPoolBase
{
public:
	ComponentPool(void* data, size_t poolSize, ComponentPoolID id)
		:m_pData((ComponentDataT*)data),
		m_poolSize(poolSize),
		m_highestDataIndex(0),
		m_hID(id)
	{

	}
	ComponentPool(ComponentDataT* data, size_t poolSize, ComponentPoolID id)
		:m_pData(data),
		m_poolSize(poolSize),
		m_highestDataIndex(0),
		m_hID(id)
	{
	}

	ComponentDataT* AddToPoolInternal(Entity entity)
	{
		if (PoolFull())
		{
			return nullptr;
		}

		m_entityToIndex[entity] = m_highestDataIndex;
		m_indexToEntity[m_highestDataIndex] = entity;
		return &m_pData[m_highestDataIndex++];
	}

	virtual void AddToPool(Entity entity) override
	{
		AddToPoolInternal(entity);
	}

	virtual void RemoveFromPool(Entity entity) override
	{
		size_t index = m_entityToIndex[entity];
		m_entityToIndex[entity] = -1;
		Entity endEntity = m_indexToEntity[m_highestDataIndex - 1];
		m_entityToIndex[endEntity] = index;

		m_pData[index] = m_pData[--m_highestDataIndex];
	}

	virtual bool ContainsEntity(Entity entity) override
	{
		if (m_entityToIndex.find(entity) != m_entityToIndex.end())
		{
			if (m_entityToIndex[entity] != -1)
			{
				return true;
			}
		}
		return false;
	}

	bool TryGetComponentData(Entity entity, ComponentDataT*& componentData)
	{
		componentData = nullptr;
		if (m_entityToIndex.find(entity) != m_entityToIndex.end())
		{
			if (m_entityToIndex[entity] != -1)
			{
				componentData = &m_pData[m_entityToIndex[entity]];
				return true;
			}
		}
		return false;
	}

	bool PoolFull()
	{
		return m_highestDataIndex < m_poolSize - 1;
	}

private:
	ComponentDataT* m_pData;
	std::map<Entity, int> m_entityToIndex;
	std::map<int, Entity> m_indexToEntity;


	size_t m_poolSize;
	size_t m_highestDataIndex;
	ComponentPoolID m_hID;
};