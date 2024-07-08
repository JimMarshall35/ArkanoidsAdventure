#include <memory>
#include <cassert>

template<typename IDT>
class HandlePool
{
public:
	HandlePool(size_t maxIDs)
		:m_pFreeIds(std::make_unique<IDT[]>(maxIDs)),
		m_pFreeIdMax(m_pFreeIds.get() + maxIDs),
		m_pFreeIdTop(m_pFreeIds.get())
	{
		for (int i = 1; i < maxIDs - 1; i++)
		{
			PushFree(i);
		}
	}

	void PushFree(IDT val)
	{
		assert(m_pFreeIdTop != m_pFreeIdMax);
		*m_pFreeIdTop++ = val;
	}

	IDT PopFree()
	{
		assert(m_pFreeIdTop > m_pFreeIds.get());
		return *m_pFreeIdTop--;
	}

private:
	std::unique_ptr<IDT[]> m_pFreeIds;
	IDT* m_pFreeIdMax;
	IDT* m_pFreeIdTop;
};