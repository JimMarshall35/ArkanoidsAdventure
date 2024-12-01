#pragma once

#include <Windows.h>
#include <atomic>

namespace Job
{
	struct Counter;
	struct JobDecl;
	struct Fibre;

	enum class Priority
	{
		High,
		Medium,
		Low
	};
#define DEFAULT_PRIORITIES_MASK (1<<(int)Priority::Medium)

	void Init();
	void RunJobs(JobDecl* jobs, size_t numJobs, Counter*& outCounter, const int prioritiesMask = DEFAULT_PRIORITIES_MASK);
	void RunJobs(JobDecl* jobs, size_t numJobs, const int prioritiesMask = DEFAULT_PRIORITIES_MASK);

	void WaitForCounterAndFree(Counter* c, int val2WaitFor, Fibre* callingFibre);

}