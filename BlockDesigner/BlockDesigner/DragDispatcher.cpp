#include "pch.h"
#include "DragDispatcher.h"

void DragDispatcher::RegisterAcceptor(IDragAcceptor* acceptor)
{
	if (std::find(m_Acceptors.begin(), m_Acceptors.end(), acceptor) != m_Acceptors.end())
	{
		ASSERT(false);
	}
	else
	{
		return m_Acceptors.push_back(acceptor);
	}
}

void DragDispatcher::OnMiddleMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Middle);
}

void DragDispatcher::OnMiddleMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}

void DragDispatcher::OnLeftMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Left);
}

void DragDispatcher::OnLeftMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}

void DragDispatcher::OnRightMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Right);
}

void DragDispatcher::OnRightMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}

void DragDispatcher::OnMouseMove(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	if (m_pCurrentDrag)
	{
		CRect rect;
		m_pCurrentDrag->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);

		bool bInRect = false;
		if (windowSpacePos.x >= rect.left && windowSpacePos.x <= rect.right)
		{
			if (windowSpacePos.y >= rect.top && windowSpacePos.y <= rect.bottom)
			{
				bInRect = true;
			}
		}
		if (!bInRect)
		{
			m_pCurrentDrag->StopDrag(windowSpacePos);
			m_pCurrentDrag = nullptr;
			return;
		}
		m_pCurrentDrag->UpdateDrag(windowSpacePos);

	}
}

void DragDispatcher::ButtonDownInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c, MouseButton btn)
{
	for (IDragAcceptor* a : m_Acceptors)
	{
		CRect rect;
		//::GetWindowRect(m_ctlPrismBaseDrawer.GetSafeHwnd(), &rect);
		a->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);
		if (windowSpacePos.x >= rect.left && windowSpacePos.x <= rect.right)
		{
			if (windowSpacePos.y >= rect.top && windowSpacePos.y <= rect.bottom)
			{

				a->StartDrag(windowSpacePos, btn);
				ASSERT(!m_pCurrentDrag);
				m_pCurrentDrag = a;
				return;
			}
		}
	}
}

void DragDispatcher::ButtonUpInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	for (IDragAcceptor* a : m_Acceptors)
	{
		CRect rect;
		//::GetWindowRect(m_ctlPrismBaseDrawer.GetSafeHwnd(), &rect);
		a->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);
		if (windowSpacePos.x >= rect.left && windowSpacePos.x <= rect.right)
		{
			if (windowSpacePos.y >= rect.top && windowSpacePos.y <= rect.bottom)
			{
				a->StopDrag(windowSpacePos);
				//ASSERT(m_pCurrentDrag);
				m_pCurrentDrag = nullptr;
				return;
			}
		}
	}
}
