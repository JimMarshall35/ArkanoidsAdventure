#pragma once
#include <atomic>
namespace Job
{

	struct Counter
	{
		Counter(int val, int awaitedVal, bool freeOnCompletion)
			:counter(val), bFreeOnCompletion(freeOnCompletion)
		{

		}
		void Decrement();


		bool IsComplete() const
		{
			return counter == awaitedVal;
		}

		bool ShouldFreeOnCompletion()
		{
			return bFreeOnCompletion;
		}
		bool bFreeOnCompletion;
		std::atomic<int> awaitedVal;

	private:
		std::atomic<int> counter;
	};
}