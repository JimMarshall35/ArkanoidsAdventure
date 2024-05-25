#pragma once
#include <glm.hpp>
#include <vector>
#include <functional>
#include "MouseButton.h"

class IMouseMoveAcceptor;
class IDragAcceptor;

typedef std::function<void(LONG& t, LONG& l, LONG& b, LONG& r)> WindowToClientFn;


class MouseEventDispatcher
{
public:
	void RegisterDragAcceptor(IDragAcceptor* acceptor);
	void RegisterMouseMoveAcceptor(IMouseMoveAcceptor* acceptor);

	void OnMiddleMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c);
	void OnMiddleMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c);

	void OnLeftMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c);
	void OnLeftMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c);

	void OnRightMouseDown(const glm::vec2& windowSpacePos, WindowToClientFn w2c);
	void OnRightMouseUp(const glm::vec2& windowSpacePos, WindowToClientFn w2c);

	void OnMouseMove(const glm::vec2& windowSpacePos, WindowToClientFn w2c);

protected:

	void ButtonDownInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c, MouseButton btn);
	void ButtonUpInternal(const glm::vec2& windowSpacePos, WindowToClientFn w2c);

protected:
	IDragAcceptor* m_pCurrentDrag = nullptr;
	std::vector<IDragAcceptor*> m_DragAcceptors;
	std::vector<IMouseMoveAcceptor*> m_MoveAcceptors;
};

