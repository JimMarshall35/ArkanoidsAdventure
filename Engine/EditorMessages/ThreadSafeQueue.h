#pragma once
#include <array>
#include <mutex>
#include "pch.h"
#include <cassert>

typedef int(*errorMsgFn)(const char* const, ...);

template<size_t size, typename T>
class TSQueueFixedSizeStackAllocatedQueue
{
public:
	TSQueueFixedSizeStackAllocatedQueue(const char* name, errorMsgFn errorMsg = nullptr)
		:m_strName(name),
		m_ErrorFn(errorMsg ? errorMsg : &printf)
	{}
	bool Push(const T& val);
	bool Pop(T& out);
	bool Empty()
	{
		std::lock_guard<std::mutex> lg(m_Mutex); 
		return Size() == 0;
	}
	size_t Size() { return m_nSize; }
private:
	std::array<T, size> m_Data;
	size_t m_nSize = 0;
	size_t m_nFront = 0;
	std::mutex m_Mutex;
	const char* m_strName;
	errorMsgFn m_ErrorFn;
};

template<size_t size, typename T>
inline bool TSQueueFixedSizeStackAllocatedQueue<size, T>::Push(const T& val)
{
	std::lock_guard<std::mutex> lg(m_Mutex);

	if (m_nSize + 1 > size)
	{
		assert(m_ErrorFn);
		m_ErrorFn("queue '%s' is full!", m_strName);
		return true;
	}
	m_Data[(m_nFront + m_nSize) % size] = val;
	++m_nSize;
	return false;
}

template<size_t size, typename T>
inline bool TSQueueFixedSizeStackAllocatedQueue<size, T>::Pop(T& out)
{
	std::lock_guard<std::mutex> lg(m_Mutex);
	/*if (m_nQueueSize == 0)
	{
		assert(m_ErrorFn);
		m_ErrorFn("can't pop: queue '%s' is empty!", m_strName);
		return true;
	}
	out = m_Data[--m_nQueueSize];*/
	out = m_Data[m_nFront];
	if (m_nSize < 1)
	{
		assert(m_ErrorFn);
		m_ErrorFn("can't pop: queue '%s' is empty!", m_strName);
		return true;

	}
	++m_nFront;
	--m_nSize;
	if (m_nFront >= size)
	{
		m_nFront = 0;
	}
	return false;
}
