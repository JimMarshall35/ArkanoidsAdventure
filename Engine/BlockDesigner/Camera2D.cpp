#include "pch.h"
#include "Camera2D.h"
#include "Camera2D.h"
#include <ext/matrix_clip_space.hpp>
#include <gtx/transform.hpp>

Camera2D::Camera2D(u32 screenW, u32 screenH, std::string name)
    :_screenWidth(screenW), _screenHeight(screenH), _name(name)
{
}

Camera2D::Camera2D(u32 screenW, u32 screenH, std::string name, const glm::vec2& focusPosition)
    :_screenWidth(screenW),
    _screenHeight(screenH),
    _name(name),
    FocusPosition(focusPosition),
    Zoom(1.0)
{
}

Camera2D::Camera2D(u32 screenW, u32 screenH, std::string name, const glm::vec2& focusPosition, float zoom)
    :_screenWidth(screenW),
    _screenHeight(screenH),
    _name(name),
    FocusPosition(focusPosition),
    Zoom(zoom)
{
}

glm::mat4x4 Camera2D::GetProjectionMatrix(int windowWidth, int windowHeight) const
{
    float left = FocusPosition.x - ((float)windowWidth / 2.0f);
    float right = FocusPosition.x + ((float)windowWidth / 2.0f);
    float top = FocusPosition.y - ((float)windowHeight / 2.0f);
    float bottom = FocusPosition.y + ((float)windowHeight / 2.0f);
    auto proj_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    auto zoom_matrix = glm::scale(glm::vec3(Zoom));

    return zoom_matrix * proj_matrix;
}

glm::vec4 Camera2D::GetTLBR() const
{
    glm::vec4 tlbr;
    float halfWindowW = ((float)_screenWidth / 2.0f) / Zoom;
    float halfWindowH = ((float)_screenHeight / 2.0f) / Zoom;
    tlbr[0] = FocusPosition.y - halfWindowH;
    tlbr[1] = FocusPosition.x - halfWindowW;
    tlbr[2] = FocusPosition.y + halfWindowH;
    tlbr[3] = FocusPosition.x + halfWindowW;

    return tlbr;
}

glm::vec2 Camera2D::MouseScreenPosToWorld(float mousex, float mousey) const
{
    glm::vec2 norm = glm::vec2(mousex / (float)_screenWidth, mousey / (float)_screenHeight);
    glm::vec4 tlbr = GetTLBR();
    float view_w = tlbr[3] - tlbr[1];
    float view_h = tlbr[2] - tlbr[0];
    return glm::vec2(
        tlbr[1] + norm.x * view_w,
        tlbr[0] + norm.y * view_h
    );
}

glm::vec2 Camera2D::WorldToScreenPos(float x, float y)
{
    glm::vec4 tlbr = GetTLBR();
    float view_w = tlbr[3] - tlbr[1];
    float view_h = tlbr[2] - tlbr[0];

    float TopLeftSpaceX = x - tlbr[1];
    float TopLeftSpaceY = y - tlbr[0];

    float nX = TopLeftSpaceX / view_w;
    float nY = TopLeftSpaceY / view_h;

    return glm::vec2(nX * (float)_screenWidth, nY * (float)_screenHeight);
}

float Camera2D::WorldLengthToScreenLength(float worldL)
{
    glm::vec4 tlbr = GetTLBR();
    float view_w = tlbr[3] - tlbr[1];
    float view_h = tlbr[2] - tlbr[0];
    return (worldL / view_w) * (float)_screenWidth;
}

bool Camera2D::WorldPosInScreenRect(float x, float y)
{
    glm::vec4 tlbr = GetTLBR();
    //glm::vec2 screenPos = WorldToScreenPos(x, y);
    if (x >= tlbr[1] && x <= tlbr[3])
    {
        if (y >= tlbr[0] && y <= tlbr[2])
        {
            return true;
        }
    }
    return false;
}

bool Camera2D::WorldPosCircleInScreenRect(float x, float y, float radius)
{
    glm::vec4 tlbr = GetTLBR();
    //glm::vec2 screenPos = WorldToScreenPos(x, y);
    if (x >= (tlbr[1] - radius) && x <= (tlbr[3] + radius))
    {
        if (y >= (tlbr[0] - radius) && y <= (tlbr[2] + radius))
        {
            return true;
        }
    }
    return false;
}

void Camera2D::SetWindowWidthAndHeight(u32 newW, u32 newH)
{
    _screenWidth = newW;
    _screenHeight = newH;
}