#pragma once
#include <atomic>
#include "Spinlock.h"
#include <list>

namespace Job
{
	struct Counter;
	struct Fibre;

	class WaitList
	{
	public:
		struct Element
		{
			Counter* pCounter = nullptr;
			Fibre* pFibre = nullptr;
		};
		
		void AddWaitList(Fibre* fibre, Counter* counter);

		const std::atomic<int>& NumReady()
		{
			return m_NumReady;
		}

		void IncrementNumReady()
		{
			m_NumReady++;
		}

		Element PopReadyEntry();

	private:
		Spinlock m_Lock;
		std::list<Element> m_List;
		std::atomic<int> m_NumReady = 0;
	};

	extern WaitList gWaitList;
}
