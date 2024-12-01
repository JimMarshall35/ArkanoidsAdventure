#include "Counter.h"
#include "WaitList.h"
namespace Job
{

	void Counter::Decrement()
	{
		--counter;
		if (counter == awaitedVal)
		{
			gWaitList.IncrementNumReady();
		}
	}

}