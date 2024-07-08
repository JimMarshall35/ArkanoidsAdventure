#pragma once
#include <vector>
#include <functional>
#include <memory>
#include "ECSDefs.h"
#include "ComponentPool.h"
#include "HandlePool.h"
#include "ComponentManager.h"

class ECSScene
{
public:
	typedef std::function<void(Entity, float)> TickSystem;

	void System(TickSystem values, const EntityComponents& components);

	template<typename ComponentDataT, size_t poolSize>
	void AddComponentPool(ComponentPoolID ID)
	{
		m_componentManager.AddComponentPool<ComponentDataT, poolSize>(ID);
	}

	void Tick(float deltaT);
	

	ECSScene(size_t maxNumComponents, size_t maxNumEntities);
	

	template<typename ComponentT>
	bool TryGetPoolInternal(ComponentPool<ComponentT>*& outPtr, ComponentPoolID id)
	{
		return m_componentManager.TryGetPoolInternal(outPtr, id);
	}

	template<typename ComponentT>
	ComponentT* GetComponent(Entity e, ComponentPoolID id)
	{
		ComponentPool<ComponentT>* pool = nullptr;
		m_componentManager.TryGetPoolInternal(pool, id);
		ComponentT* rVal = nullptr;
		assert(pool);
		assert(pool->TryGetComponentData(e, rVal));
		return rVal;
	}

#define AddComponent(entity, componentName) AddComponentInternal<componentName>(entity, ComponentID(componentName))
	template<typename ComponentT>
	void AddComponentInternal(Entity e, ComponentPoolID id)
	{
		ComponentPool<ComponentT>* pool = nullptr;
		if (m_componentManager.TryGetPoolInternal(pool, id))
		{
			if (pool->ContainsEntity(e))
			{
				// log or something here
			}
			else
			{
				pool->AddToPool(e);
				m_pEntityComponents[e][id] = true;
				m_pEntityNumberOfComponents[e]++;
			}
		}
		else
		{
			// log or something here
		}
	}

	Entity AddEntity();

	void RemoveEntity(Entity e);

private:
	size_t m_liveEntities = 0;

	size_t m_highestLiveEntity = 0;

	size_t m_maxNumEntities = 0;

	std::unique_ptr<EntityComponents[]> m_pEntityComponents;

	std::unique_ptr<unsigned short[]> m_pEntityNumberOfComponents;

	HandlePool<Entity> m_entityIDdManager;

	ComponentManager m_componentManager;

	std::vector<std::function<void(float)>> m_systems;
};
