#pragma once
#include "ComponentPool.h"
#include <map>
#include <memory>

class ComponentManager
{
public:
#define CreatePool(componentName, poolSize) AddComponentPool<componentName, poolSize>(gID_##componentName)
	template<typename ComponentDataT, size_t poolSize>
	void AddComponentPool(ComponentPoolID id)
	{
		size_t newAllocationSize = m_componentsAllocationSize + (poolSize * sizeof(ComponentDataT));
		std::unique_ptr<char[]> newAllocation = std::make_unique<char[]>(newAllocationSize);
		memcpy(newAllocation.get(), m_pPoolData.get(), m_componentsAllocationSize);

		auto newPool = std::make_unique<ComponentPool<ComponentDataT>>(&newAllocation[m_componentsAllocationSize], poolSize, id);
		m_pools[id] = std::move(newPool);


		m_componentsAllocationSize = newAllocationSize;

		if (m_pPoolData)
		{
			m_pPoolData.reset();
		}
		m_pPoolData = std::move(newAllocation);
	}

#define TryGetPool(componentName, outPtr) TryGetPoolInternal<componentName>(outPtr, ComponentID(componentName))
	template<typename ComponentT>
	bool TryGetPoolInternal(ComponentPool<ComponentT>*& outPtr, ComponentPoolID id)
	{
		bool b = TryGetPoolBase((ComponentPoolBase*&)outPtr, id);
		return b;
	}

	bool TryGetPoolBase(ComponentPoolBase*& outPtr, ComponentPoolID id)
	{
		if (m_pools.find(id) != m_pools.end())
		{
			if (outPtr = (ComponentPoolBase*)m_pools[id].get())
			{
				return  true;
			}
		}
		return false;
	}


	void DeletePool()
	{
		m_pPoolData.reset();
		m_pools.clear();
	}



private:
	std::unique_ptr<char[]> m_pPoolData;
	size_t m_componentsAllocationSize = 0;
	std::map<ComponentPoolID, std::unique_ptr<ComponentPoolBase>> m_pools;

};