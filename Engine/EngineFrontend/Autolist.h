#pragma once

template<typename T>
class Autolist
{
public:
	Autolist(bool bAddToList = false)
	{
		if (bAddToList)
		{
			Head->Next = this;
			Prev = Head;
			ListLength++;
			bInList = true;
		}
	}

	~Autolist()
	{
		if (bInList)
		{
			Prev->Next = Next;
			Next->Prev = Prev;
			ListLength--;
		}
	}

private:
	bool bInList = false;
	Autolist* Prev = nullptr;
	Autolist* Next = nullptr;
	static Autolist<T> Head;
	static size_t ListLength = 0;
};

template<typename T>
Autolist<T> Autolist<T>::Head;

template<typename T>
size_t Autolist<T>::ListLength;