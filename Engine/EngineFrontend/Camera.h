#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
class Camera
{
public:
    Camera() = default;

    Camera(glm::vec3 eye, glm::vec3 lookat, glm::vec3 upVector)
        : m_eye(std::move(eye))
        , m_lookAt(std::move(lookat))
        , m_upVector(std::move(upVector))
    {
        UpdateViewMatrix();
    }

    glm::mat4x4 GetViewMatrix() const { return m_viewMatrix; }
    glm::vec3 GetEye() const { return m_eye; }
    glm::vec3 GetUpVector() const { return m_upVector; }
    glm::vec3 GetLookAt() const { return m_lookAt; }
    glm::vec3 GetFront() const { return glm::normalize(m_lookAt - m_eye); }

    void SetLookAt(const glm::vec3& pt) { m_lookAt = pt; UpdateViewMatrix(); }

    // Camera forward is -z
    glm::vec3 GetViewDir() const { return -glm::transpose(m_viewMatrix)[2]; }
    glm::vec3 GetRightVector() const { return glm::transpose(m_viewMatrix)[0]; }

    void SetCameraView(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up)
    {
        m_eye = std::move(eye);
        m_lookAt = std::move(lookat);
        m_upVector = std::move(up);
        UpdateViewMatrix();
    }

    void UpdateViewMatrix()
    {
        // Generate view matrix using the eye, lookAt and up vector
        m_viewMatrix = glm::lookAt(m_eye, m_lookAt, m_upVector);
    }

private:
    glm::mat4x4 m_viewMatrix;
    glm::vec3 m_eye; // Camera position in 3D
    glm::vec3 m_lookAt; // Point that the camera is looking at
    glm::vec3 m_upVector; // Orientation of the camera
};
