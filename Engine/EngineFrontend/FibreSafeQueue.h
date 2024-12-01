#pragma once
#include "Spinlock.h"
namespace Job
{

	template<typename T>
	class FibreSafeQueue
	{
	public:
		FibreSafeQueue(size_t size)
			:m_nSize(size)
		{
			m_Data = new T[m_nSize];
		}
		~FibreSafeQueue()
		{
			delete[] m_Data;
		}
	public:

		struct ScopedLock
		{
			ScopedLock(Spinlock* lock)
				:m_pSpinlock(lock)
			{
				m_pSpinlock->Lock();
			}
			~ScopedLock()
			{
				m_pSpinlock->Unlock();
			}
		private:
			Spinlock* m_pSpinlock = nullptr;
		};


	private:
		void Decrement(int& i)
		{
			i--;
			if (i < 0)
			{
				i = m_nSize - 1;
			}
		}

	public:

		void Push(T d)
		{
			if (m_nCurrentSize >= m_nSize)
			{
				// queue full
				return;
			}
			m_Data[m_nTail] = d;
			Decrement(m_nTail);
			++m_nCurrentSize;
		}

		T Pop()
		{
			T r;
			if (m_nCurrentSize <= 0)
			{
				// queue empty
				return r;
			}

			r = m_Data[m_nHead];
			Decrement(m_nHead);
			--m_nCurrentSize;
			return r;
		}

		int Length()
		{
			return m_nCurrentSize;
		}

		ScopedLock GetLock()
		{
			return ScopedLock(&m_Lock);
		}

		inline void Lock() { m_Lock.Lock(); }
		inline void UnLock() { m_Lock.Unlock(); }

	private:
		size_t m_nSize = 0;
		T* m_Data;
		int m_nHead = 0;
		int m_nTail = 0;
		int m_nCurrentSize = 0;
		Spinlock m_Lock;
	};

}