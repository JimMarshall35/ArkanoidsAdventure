#include "JobSystem.h"
#include <thread> // std::thread::hardware_concurrency
#include "EngineLib.h"
#include "ObjectPool.h"
#include "FibreSafeQueue.h"
#include "WaitList.h"
#include "Counter.h"

namespace Job
{
#define KiB 1024
#define BIG_FIBRE_STACK_SIZE (KiB * 512)
#define SMALL_FIBRE_STACK_SIZE (KiB * 512)
#define NUM_SMALL_STACK_FIBERS 128
#define NUM_BIG_STACK_FIBERS 32
#define JOB_QUEUE_SIZE 1024
#define HIGH_PRIORITY_JOB_QUEUE_SIZE   JOB_QUEUE_SIZE
#define MEDIUM_PRIORITY_JOB_QUEUE_SIZE JOB_QUEUE_SIZE
#define LOW_PRIORITY_JOB_QUEUE_SIZE    JOB_QUEUE_SIZE



	enum class StackSize
	{
		Small,
		Big
	};

	struct JobDecl
	{
		void* pData = nullptr;
		void (*pFn)(LPVOID) = nullptr;
		Counter* pCounter = nullptr;
		Priority priority = Priority::Medium;
		StackSize stackSize = StackSize::Small;
	};

	using JobQueue = FibreSafeQueue<JobDecl>;

	
	JobQueue sHighPriorityJobQueue(HIGH_PRIORITY_JOB_QUEUE_SIZE);
	JobQueue sMediumPriorityJobQueue(MEDIUM_PRIORITY_JOB_QUEUE_SIZE);
	JobQueue sLowPriorityJobQueue(LOW_PRIORITY_JOB_QUEUE_SIZE);

	

	struct Fibre
	{
		void Create(int stackSize)
		{
			if(!m_pFiber)
			{
				m_pFiber = CreateFiber(stackSize, &MyFiberFunction, (LPVOID)this);
			}
			if (stackSize == SMALL_FIBRE_STACK_SIZE)
			{
				m_StackSize = StackSize::Small;
			}
			else if (stackSize == BIG_FIBRE_STACK_SIZE)
			{
				m_StackSize = StackSize::Big;
			}
			else
			{
				assert(false);
			}
		}

		inline void SetDecl(const JobDecl& d) { m_Decl = d; }
		inline void* GetFiber() const { return m_pFiber; }
		inline void SetParentFibre(void* pParentFibre) { m_pParentFibre = pParentFibre; }
		inline void* GetParentFibre() {	return m_pParentFibre; }
		inline bool GetFinished() { return m_bFinished; }
		size_t hHandle = 0;
	private:
		void* m_pParentFibre = nullptr;
		JobDecl m_Decl;
		bool m_bFinished = false;

		void* m_pFiber = nullptr;
		StackSize m_StackSize;
		static VOID WINAPI MyFiberFunction(LPVOID param)
		{
			Fibre* f = (Fibre*)param;
			f->m_bFinished = false;

			f->m_Decl.pFn(f->m_Decl.pData);
			if (f->m_Decl.pCounter)
			{
				f->m_Decl.pCounter->Decrement();
			}
			f->m_bFinished = true;
			SwitchToFiber(f->m_pParentFibre);
		}
	};

	static HANDLE* sExecutionThreads = nullptr;
	static DWORD* sExecutionThreadIDs = nullptr;
	static u32 sExeuctionThreadsSize = 0;


	EObjectPool<Fibre, NUM_SMALL_STACK_FIBERS> sSmallStackFiberPool;
	EObjectPool<Fibre, NUM_SMALL_STACK_FIBERS> sBigStackFiberPool;

	Spinlock sSmallFibreLock;
	Spinlock sBigFibreLock;

	Fibre* GetBigStackFibreFromPool(size_t& outHandle)
	{
		Fibre* p = nullptr;
		sBigFibreLock.Lock();
		sBigStackFiberPool.AllocateNew(outHandle, p);
		sBigFibreLock.Unlock();
		p->hHandle = outHandle;
		return  p;
	}

	Fibre* GetSmallStackFibreFromPool(size_t& outHandle)
	{
		Fibre* p = nullptr;
		sSmallFibreLock.Lock();
		sSmallStackFiberPool.AllocateNew(outHandle, p);
		sSmallFibreLock.Unlock();
		p->hHandle = outHandle;
		return  p;
	}

	auto CheckQueueEmpty(JobQueue& queue) -> bool
	{
		auto lock = queue.GetLock();
		return queue.Length() == 0;
	};

	struct ThreadsControlBlock
	{
		std::atomic<int> newTasks = false;
		
	};
	ThreadsControlBlock sTcb;

	DWORD WINAPI MyThreadFunction(LPVOID lpParam)
	{
		LPVOID fiber = ConvertThreadToFiber(NULL);

		auto TryQueue = [fiber](JobQueue& queue) -> bool
		{
			JobDecl decl;
			bool onePopped = false;
			{
				auto lock = queue.GetLock();

				if (queue.Length())
				{
					decl = queue.Pop();
					onePopped = true;
					sTcb.newTasks--;
				}
			}

			if (onePopped)
			{
				Fibre* pNewFibre = nullptr;
				size_t handle = 0;
				switch (decl.stackSize)
				{
				case StackSize::Big:
					pNewFibre = GetBigStackFibreFromPool(handle);
					break;
				case StackSize::Small:
					pNewFibre = GetSmallStackFibreFromPool(handle);
					break;
				}
				pNewFibre->SetDecl(decl);
				pNewFibre->SetParentFibre(fiber);
				SwitchToFiber(pNewFibre->GetFiber());
				if (pNewFibre->GetFinished())
				{
					switch (decl.stackSize)
					{
					case StackSize::Big:
						sBigFibreLock.Lock();
						sBigStackFiberPool.DeallocateObject(pNewFibre->hHandle);
						sBigFibreLock.Unlock();
						break;
					case StackSize::Small:
						sSmallFibreLock.Lock();
						sSmallStackFiberPool.DeallocateObject(pNewFibre->hHandle);
						sSmallFibreLock.Unlock();
						break;
					}
				}
			}

			return onePopped;
		};
		auto Schedule = [&]()
		{
			Fibre* f = nullptr;
			size_t fIndex = 0xffff;
			if (TryQueue(sHighPriorityJobQueue))
			{
				return;
			}
			if (TryQueue(sMediumPriorityJobQueue))
			{
				return;
			}
			if (TryQueue(sLowPriorityJobQueue))
			{
				return;
			}
		};
		while (true)
		{
			if (gWaitList.NumReady())
			{
				WaitList::Element e = gWaitList.PopReadyEntry();
				if (e.pCounter && e.pCounter->ShouldFreeOnCompletion())
				{
					delete e.pCounter;
				}
				SwitchToFiber(e.pFibre->GetFiber());
				continue;
			}
			if (sTcb.newTasks)
			{
				Schedule();
			}
		}
	}

	void Init()
	{
		sExeuctionThreadsSize = std::thread::hardware_concurrency() - 1;
		sExecutionThreads = new HANDLE[sExeuctionThreadsSize];
		sExecutionThreadIDs = new DWORD[sExeuctionThreadsSize];
		


		for (int i = 0; i < NUM_SMALL_STACK_FIBERS; i++)
		{
			sSmallStackFiberPool[i].Create(KiB * 64);
		}
		for (int i = 0; i < NUM_BIG_STACK_FIBERS; i++)
		{
			sBigStackFiberPool[i].Create(KiB * 512);
		}

		int affinitMaskShift = 0;
		SetThreadAffinityMask(GetCurrentThread(), (1 << affinitMaskShift++));
		for (int i = 0; i < sExeuctionThreadsSize; i++)
		{
			sExecutionThreads[i] = CreateThread(
				NULL,                       // default security attributes
				0,                          // use default stack size  
				MyThreadFunction,           // thread function name
				&sTcb,                      // argument to thread function 
				0,                          // use default creation flags 
				&sExecutionThreadIDs[i]);   // returns the thread identifier 

			SetThreadAffinityMask(sExecutionThreads[i], (1 << affinitMaskShift++));
		}

		//MyThreadFunction(NULL);
	}

	static void RunJobs(JobDecl* jobs, size_t numJobs, Counter*& outCounter, const int prioritiesMask, bool bAddCounter)
	{
		if (bAddCounter)
		{
			outCounter = new Counter(numJobs, 0, false);
		}
		
		if (prioritiesMask & (1 << (int)Priority::High))
		{
			sHighPriorityJobQueue.Lock();
		}
		if (prioritiesMask & (1 << (int)Priority::Medium))
		{
			sMediumPriorityJobQueue.Lock();
		}
		if (prioritiesMask & (1 << (int)Priority::Low))
		{
			sLowPriorityJobQueue.Lock();
		}

		for (int i = 0; i < numJobs; i++)
		{
			JobDecl& j = jobs[i];
			if (bAddCounter)
			{
				j.pCounter = outCounter;
			}
			switch (j.priority)
			{
			case Priority::High:
				sHighPriorityJobQueue.Push(j);
				break;
			case Priority::Medium:
				sMediumPriorityJobQueue.Push(j);
				break;
			case Priority::Low:
				sLowPriorityJobQueue.Push(j);
				break;
			}
		}
		if (prioritiesMask & (1 << (int)Priority::High))
		{
			sHighPriorityJobQueue.UnLock();
		}
		if (prioritiesMask & (1 << (int)Priority::Medium))
		{
			sMediumPriorityJobQueue.UnLock();
		}
		if (prioritiesMask & (1 << (int)Priority::Low))
		{
			sLowPriorityJobQueue.UnLock();
		}
		sTcb.newTasks = numJobs;
		
	}

	void RunJobs(JobDecl* jobs, size_t numJobs, Counter*& outCounter, const int prioritiesMask/* = DEFAULT_PRIORITIES_MASK*/)
	{
		RunJobs(jobs, numJobs, outCounter, prioritiesMask, true);
	}

	void RunJobs(JobDecl* jobs, size_t numJobs, const int prioritiesMask)
	{
		Counter* c;
		RunJobs(jobs, numJobs, c, prioritiesMask, false);
	}

	void WaitForCounterAndFree(Counter* c, int val2WaitFor, Fibre* callingFibre)
	{
		c->bFreeOnCompletion = true;
		c->awaitedVal = val2WaitFor;
		gWaitList.AddWaitList(callingFibre, c);
	
		// go back to the parent fibre - the function the thread was initialised with just after converting itself to a fibre which
		// waits for new jobs from the queues or wait list
		SwitchToFiber(callingFibre->GetParentFibre());
	}
}