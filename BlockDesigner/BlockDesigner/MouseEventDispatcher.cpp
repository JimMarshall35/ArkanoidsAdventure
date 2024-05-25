#include "pch.h"
#include "MouseEventDispatcher.h"
#include "IDragAcceptor.h"
#include "IMouseMoveAcceptor.h"

void MouseEventDispatcher::RegisterDragAcceptor(IDragAcceptor* acceptor)
{
	if (std::find(m_DragAcceptors.begin(), m_DragAcceptors.end(), acceptor) != m_DragAcceptors.end())
	{
		ASSERT(false);
	}
	else
	{
		return m_DragAcceptors.push_back(acceptor);
	}
}

void MouseEventDispatcher::RegisterMouseMoveAcceptor(IMouseMoveAcceptor* acceptor)
{
	if (std::find(m_MoveAcceptors.begin(), m_MoveAcceptors.end(), acceptor) != m_MoveAcceptors.end())
	{
		ASSERT(false);
	}
	else
	{
		return m_MoveAcceptors.push_back(acceptor);
	}
}

void MouseEventDispatcher::OnMiddleMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Middle);
}

void MouseEventDispatcher::OnMiddleMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}

void MouseEventDispatcher::OnLeftMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Left);
}

void MouseEventDispatcher::OnLeftMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}

void MouseEventDispatcher::OnRightMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonDownInternal(windowSpacePos, w2c, MouseButton::Right);
}

void MouseEventDispatcher::OnRightMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	ButtonUpInternal(windowSpacePos, w2c);
}


static bool RectTest(const CRect& rect, const glm::vec2& pos)
{
	if (pos.x >= rect.left && pos.x <= rect.right)
	{
		if (pos.y >= rect.top && pos.y <= rect.bottom)
		{
			return true;
		}
	}
	return false;
}

void MouseEventDispatcher::OnMouseMove(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	if (m_pCurrentDrag)
	{
		CRect rect;
		m_pCurrentDrag->GetWindowRectForDrag(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);

		if (!RectTest(rect, windowSpacePos))
		{
			m_pCurrentDrag->StopDrag(windowSpacePos);
			m_pCurrentDrag = nullptr;
		}
		else
		{
			m_pCurrentDrag->UpdateDrag(windowSpacePos);
		}
		
	}
	else
	{
		for (IMouseMoveAcceptor* acceptor : m_MoveAcceptors)
		{
			CRect rect;
			acceptor->GetWindowRectForMove(rect.top, rect.left, rect.bottom, rect.right);
			w2c(rect.top, rect.left, rect.bottom, rect.right);
			if (RectTest(rect, windowSpacePos))
			{
				glm::vec2 topLeft = { rect.left, rect.top };
				glm::vec2 acceptorPos = windowSpacePos - topLeft;
				acceptor->UpdateMousePos(acceptorPos);
			}
		}
	}
}

void MouseEventDispatcher::ButtonDownInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c, MouseButton btn)
{
	for (IDragAcceptor* a : m_DragAcceptors)
	{
		CRect rect;
		//::GetWindowRect(m_ctlPrismBaseDrawer.GetSafeHwnd(), &rect);
		a->GetWindowRectForDrag(rect.top, rect.left, rect.bottom, rect.right);
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

void MouseEventDispatcher::ButtonUpInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	for (IDragAcceptor* a : m_DragAcceptors)
	{
		CRect rect;
		//::GetWindowRect(m_ctlPrismBaseDrawer.GetSafeHwnd(), &rect);
		a->GetWindowRectForDrag(rect.top, rect.left, rect.bottom, rect.right);
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
