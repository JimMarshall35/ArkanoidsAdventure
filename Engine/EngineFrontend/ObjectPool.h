#pragma once
#include "EngineLib.h"

template<typename T, size_t maxArraySize>
class FixedSizeUnorderedArray
{
public:
	FixedSizeUnorderedArray()
		:m_pMem(new T[maxArraySize])
	{

	}
public:
	void Push(const T& val)
	{
		m_pMem[m_nNumItems++] = val;
	}
	bool Remove(const T& val)
	{
		int foundID = -1;
		for (int i = 0; i < m_nNumItems; i++)
		{
			if (m_pMem[i] == val)
			{
				foundID = i;
				break;
			}
		}
		if (foundID < 0)
		{
			return false;
		}
		m_pMem[foundID] = m_pMem[--m_nNumItems];
		return true;
	}
	size_t Size() { return m_nNumItems; }
	T& operator[](size_t i)
	{
		return m_pMem[i];
	}
private:
	EUPtr<T[]> m_pMem;
	size_t m_nNumItems = 0;
};

template<typename T, size_t maxPoolSize>
class EObjectPool
{
public:
	EObjectPool()
		:m_pMem(new T[maxPoolSize]),
		m_pFreeIndices(new size_t[maxPoolSize]),
		m_nFreeIndices(0)
	{
		for (int i = maxPoolSize - 1; i >= 0; i--)
		{
			m_pFreeIndices[m_nFreeIndices++] = i;
		}
	}
	bool AllocateNew(size_t& outIndex, T*& outNewObject)
	{
		if (m_nFreeIndices == 0)
		{
			return false;
		}
		outIndex = m_pFreeIndices[--m_nFreeIndices];
		m_OccupiedIndices.Push(outIndex);
		m_pMem[outIndex] = T();
		outNewObject = &m_pMem[outIndex];
		return true;
	}
	bool DeallocateObject(size_t index)
	{
		if (!m_OccupiedIndices.Remove(index))
		{
			return false;
		}
		m_pFreeIndices[m_nFreeIndices++] = index;
	}
	
	bool IsIndexAlive(size_t index2Test)
	{
		if (m_OccupiedIndices.Size() >= m_nFreeIndices)
		{
			for (int i = 0; i < m_nFreeIndices; i++)
			{
				if (m_pFreeIndices[i] == index2Test)
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			for (int i = 0; i < m_OccupiedIndices.Size(); i++)
			{
				if (m_OccupiedIndices[i] == index2Test)
				{
					return true;
				}
			}
			return false;
		}
	}

	size_t* GetFreeIndicesPtr() { return m_pFreeIndices.get(); }
	size_t GetNumFreeIndixes() { return m_nFreeIndices; }
	size_t GetCapcity() { return maxPoolSize; }
	T& operator[](size_t i)
	{
		return m_pMem[i];
	}
private:
	EUPtr<T[]> m_pMem;
	EUPtr<size_t[]> m_pFreeIndices;
	FixedSizeUnorderedArray<size_t, maxPoolSize> m_OccupiedIndices;
	size_t m_nFreeIndices = 0;
};