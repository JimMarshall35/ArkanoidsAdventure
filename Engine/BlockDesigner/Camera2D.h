#pragma once
#include "glm.hpp"
#include <string> 
#include "BasicTypedefs.h"

class Camera2D
{
public:
	Camera2D(u32 screenW, u32 screenH, std::string name);
	glm::vec2 FocusPosition;
	float Zoom;
public:
	Camera2D(u32 screenW, u32 screenH, std::string name, const glm::vec2& focusPosition);
	Camera2D(u32 screenW, u32 screenH, std::string name, const glm::vec2& focusPosition, float zoom);

	glm::mat4x4 GetProjectionMatrix(int windowWidth, int windowHeight) const;
	glm::vec4 GetTLBR() const;
	glm::vec2 MouseScreenPosToWorld(float mousex, float mousey) const;
	glm::vec2 WorldToScreenPos(float x, float y);
	float WorldLengthToScreenLength(float worldL);
	bool WorldPosInScreenRect(float x, float y);
	bool WorldPosCircleInScreenRect(float x, float y, float radius);

	virtual void SetWindowWidthAndHeight(u32 newW, u32 newH);
	std::string Name() { return _name; }
protected:
	u32 _screenWidth;
	u32 _screenHeight;
	std::string _name;
};
