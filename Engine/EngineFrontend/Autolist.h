#pragma once

template<typename T>
class Autolist
{
public:
	Autolist(bool bAddToList = false)
	{
		if (bAddToList)
		{
			if (Head.Next)
			{
				Head.Next->Prev = this;
				Next = Head.Next;
				Head.Next = this;
			}
			else
			{
				Head.Next = this;
				Prev = Head.Next;
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
			ListLength--;
		}
	}
	static Autolist* GetHead() { return &Head; }
	Autolist* GetPrev() { return Prev; }
	Autolist* GetNext() { return Next; }
private:
	bool bInList = false;
	Autolist* Prev = nullptr;
	Autolist* Next = nullptr;
	static Autolist<T> Head;
	static size_t ListLength;
};

template<typename T>
Autolist<T> Autolist<T>::Head;

template<typename T>
size_t Autolist<T>::ListLength = 0;