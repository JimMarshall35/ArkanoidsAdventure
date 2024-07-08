#include "ECS/ECSScene.h"

void ECSScene::System(TickSystem values, const EntityComponents& components)
{
	std::function<void(float)> sys = [&](float deltaT)
	{
		int componentsCount = components.count();
		//auto tuple = std::tuple<ArgsT*...>();
		for (int i = 0; i <= m_highestLiveEntity; i++)
		{
			EntityComponents c = m_pEntityComponents[i];
			c &= components;
			if (c.count() >= componentsCount)
			{
				values(i, deltaT);
			}
		}
	};
	m_systems.push_back(sys);
}

void ECSScene::Tick(float deltaT)
{
	for (const std::function<void(float)>& func : m_systems)
	{
		func(deltaT);
	}
}

ECSScene::ECSScene(size_t maxNumComponents, size_t maxNumEntities)
	:m_entityIDdManager(maxNumEntities),
	m_maxNumEntities(maxNumEntities),
	m_pEntityComponents(std::make_unique<EntityComponents[]>(maxNumEntities)),
	m_pEntityNumberOfComponents(std::make_unique<unsigned short[]>(maxNumEntities))
{
	for (int i = 0; i < maxNumEntities; i++)
	{
		m_pEntityComponents[i].reset();
	}
	memset(m_pEntityNumberOfComponents.get(), 0, maxNumEntities * sizeof(unsigned short));
}

Entity ECSScene::AddEntity()
{

	if (m_liveEntities + 1 > m_maxNumEntities)
	{
		// log here
		return 0;
	}

	++m_liveEntities;
	Entity e = m_entityIDdManager.PopFree();

	if (e > m_highestLiveEntity)
	{
		m_highestLiveEntity = e;
	}

	m_pEntityComponents[e].reset();
	m_pEntityNumberOfComponents[e] = 0;
	return e;
}

void ECSScene::RemoveEntity(Entity e)
{
	unsigned short numComponentsThisEntity = m_pEntityNumberOfComponents[e];
	const std::bitset<MaxTypesOfComponent>& components = m_pEntityComponents[e];
	int componentsEncountered = 0;
	for (int i = 1; i < MaxTypesOfComponent; i++)
	{
		if (components[i])
		{
			ComponentPoolBase* base = nullptr;
			if (m_componentManager.TryGetPoolBase(base, i))
			{
				base->RemoveFromPool(e);
				componentsEncountered++;
				if (componentsEncountered > numComponentsThisEntity)
				{
					break;
				}
			}
		}
	}
	if (e == m_highestLiveEntity)

		--m_highestLiveEntity;
	}
	m_pEntityComponents[e].reset();
	m_pEntityNumberOfComponents[e] = 0;
	m_entityIDdManager.PushFree(e);
}
