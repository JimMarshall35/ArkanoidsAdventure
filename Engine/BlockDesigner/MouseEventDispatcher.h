#pragma once
#include <glm.hpp>
#include <vector>
#include <functional>
#include "MouseButton.h"

class IMouseMoveAcceptor;
class IDragAcceptor;
class IMouseDownAcceptor;

typedef std::function<void(LONG& t, LONG& l, LONG& b, LONG& r)> WindowToClientFn;


class MouseEventDispatcher
{
public:
	/*
		runtime check that an acceptor object hasn't registered the same button for both drag and mouse down.
		If it is, just don't add the acceptor and fail an assert - default to being turned on in debug
	*/
#ifdef DEBUG
	MouseEventDispatcher(bool bDragAcceptorAndMouseDownMutuallyExclusive = true);
#else
	MouseEventDispatcher(bool bDragAcceptorAndMouseDownMutuallyExclusive = false);
#endif

	void RegisterDragAcceptor(IDragAcceptor* acceptor, MouseButton btn);
	void RegisterMouseDownAcceptor(IMouseDownAcceptor* acceptor, MouseButton btn);
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
	std::vector<IDragAcceptor*> m_DragAcceptors[(int)MouseButton::NumMouseButtons];
	std::vector<IMouseDownAcceptor*> m_MouseDownAcceptors[(int)MouseButton::NumMouseButtons];
	std::vector<IMouseMoveAcceptor*> m_MoveAcceptors;
	bool m_bDragAcceptorAndMouseDownMutuallyExclusive;
};

