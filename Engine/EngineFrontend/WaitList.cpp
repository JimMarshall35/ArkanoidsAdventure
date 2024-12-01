#include "WaitList.h"
#include "Counter.h"

namespace Job
{
	WaitList gWaitList;

	void WaitList::AddWaitList(Fibre* fibre, Counter* counter)
	{
		m_Lock.Lock();
		m_List.push_back({ counter, fibre });
		m_Lock.Unlock();
	}

	static bool operator==(const WaitList::Element& e1, const WaitList::Element& e2)
	{
		return
			e1.pCounter == e2.pCounter &&
			e1.pFibre == e2.pFibre;
	}

	WaitList::Element WaitList::PopReadyEntry()
	{
		m_Lock.Lock();
		WaitList::Element eselected;
		bool bSuccess = false;
		for (const WaitList::Element& e : m_List)
		{
			if (e.pCounter->IsComplete())
			{
				bSuccess = true;
				eselected = e;
				break;
			}
		}
		if (bSuccess)
		{
			m_List.remove(eselected);
			m_Lock.Unlock();
			return eselected;
		}
		m_Lock.Unlock();
		
		return { nullptr, nullptr };
	}

}