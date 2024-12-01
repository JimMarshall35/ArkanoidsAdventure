#pragma once
#include <atomic>
namespace Job
{

	class Spinlock
	{
	public:
		void Lock()
		{
			while (m_bLocked)
			{

			}
			m_bLocked = true;
		}
		void Unlock()
		{
			m_bLocked = false;
		}
	private:
		std::atomic<bool> m_bLocked = false;
	};

}