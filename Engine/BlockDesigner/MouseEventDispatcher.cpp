#include "pch.h"
#include "MouseEventDispatcher.h"
#include "IDragAcceptor.h"
#include "IMouseMoveAcceptor.h"
#include "IMouseDownAcceptor.h"
#include "MiscFunctions.h"

MouseEventDispatcher::MouseEventDispatcher(bool bDragAcceptorAndMouseDownMutuallyExclusive)
	:m_bDragAcceptorAndMouseDownMutuallyExclusive(bDragAcceptorAndMouseDownMutuallyExclusive)
{
}

void MouseEventDispatcher::RegisterDragAcceptor(IDragAcceptor* acceptor, MouseButton btn)
{
	if (m_bDragAcceptorAndMouseDownMutuallyExclusive)
	{
		std::vector<IMouseDownAcceptor*>& acceptorList = m_MouseDownAcceptors[(int)btn];
		if (std::find(acceptorList.begin(), acceptorList.end(), (IMouseDownAcceptor*)acceptor) != acceptorList.end())
		{
			ASSERT(false);
			return;
		}
	}
	std::vector<IDragAcceptor*>& acceptorList = m_DragAcceptors[(int)btn];
	if (std::find(acceptorList.begin(), acceptorList.end(), acceptor) != acceptorList.end())
	{
		ASSERT(false);
	}
	else
	{
		return acceptorList.push_back(acceptor);
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

void MouseEventDispatcher::RegisterMouseDownAcceptor(IMouseDownAcceptor* acceptor, MouseButton btn)
{
	if (m_bDragAcceptorAndMouseDownMutuallyExclusive)
	{
		std::vector<IDragAcceptor*>& acceptorList = m_DragAcceptors[(int)btn];
		if (std::find(acceptorList.begin(), acceptorList.end(), (IDragAcceptor*)acceptor) != acceptorList.end())
		{
			ASSERT(false);
			return;
		}
	}
	std::vector<IMouseDownAcceptor*>& acceptorList = m_MouseDownAcceptors[(int)btn];
	if (std::find(acceptorList.begin(), acceptorList.end(), acceptor) != acceptorList.end())
	{
		ASSERT(false);
	}
	else
	{
		return acceptorList.push_back(acceptor);
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

void MouseEventDispatcher::OnMouseMove(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	if (m_pCurrentDrag)
	{
		CRect rect;
		m_pCurrentDrag->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);

		glm::vec2 topLeft = { rect.left, rect.top };
		glm::vec2 acceptorPos = windowSpacePos - topLeft;

		if (!RectTest(rect, windowSpacePos))
		{
			m_pCurrentDrag->StopDrag(acceptorPos);
			m_pCurrentDrag = nullptr;
		}
		else
		{

			m_pCurrentDrag->UpdateDrag(acceptorPos);
		}
		
	}
	else
	{
		for (IMouseMoveAcceptor* acceptor : m_MoveAcceptors)
		{
			CRect rect;
			acceptor->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
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
	const std::vector<IDragAcceptor*>& dragAcceptorList = m_DragAcceptors[(int)btn];

	for (IDragAcceptor* a : dragAcceptorList)
	{
		CRect rect;
		a->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);
		if (RectTest(rect, windowSpacePos))
		{
			glm::vec2 topLeft = { rect.left, rect.top };
			glm::vec2 acceptorPos = windowSpacePos - topLeft;
			a->StartDrag(acceptorPos, btn);
			ASSERT(!m_pCurrentDrag);
			m_pCurrentDrag = a;
			return;
		}
	}

	const std::vector<IMouseDownAcceptor*>& mouseDownAcceptorList = m_MouseDownAcceptors[(int)btn];
	for (IMouseDownAcceptor* a : mouseDownAcceptorList)
	{
		CRect rect;
		a->GetWindowRect(rect.top, rect.left, rect.bottom, rect.right);
		w2c(rect.top, rect.left, rect.bottom, rect.right);
		if (RectTest(rect, windowSpacePos))
		{
			glm::vec2 topLeft = { rect.left, rect.top };
			glm::vec2 acceptorPos = windowSpacePos - topLeft;
			a->MouseDown(acceptorPos, btn);
			return;
		}
	}
}

void MouseEventDispatcher::ButtonUpInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c)
{
	if (m_pCurrentDrag)
	{
		m_pCurrentDrag->StopDrag(windowSpacePos);
		m_pCurrentDrag = nullptr;
	}
}
