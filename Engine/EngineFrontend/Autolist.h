#pragma once

template<typename T>
class Autolist
{
public:
	Autolist(bool bAddToList = false)
	{
		if (bAddToList)
		{
			if (Head)
			{
				Next = Head;
				Head->Prev = this;
				Head = this;
			}
			else
			{
				Head = this;
			}
			ListLength++;
			bInList = true;
		}
	}

	~Autolist()
	{
		if (bInList)
		{
			if (Prev)
			{
				Prev->Next = Next;
			}
			if (Next)
			{
				Next->Prev = Prev;
			}
			if (Head == this)
			{
				Head = Next;
			}
			ListLength--;
		}
	}
	static Autolist* GetHead() { return Head; }
	static size_t GetCount() { return ListLength; }
 	Autolist* GetPrev() { return Prev; }
	Autolist* GetNext() { return Next; }
private:
	bool bInList = false;
	Autolist* Prev = nullptr;
	Autolist* Next = nullptr;
	static Autolist<T>* Head;
	static size_t ListLength;
};

template<typename T>
Autolist<T>* Autolist<T>::Head = nullptr;

template<typename T>
size_t Autolist<T>::ListLength = 0;