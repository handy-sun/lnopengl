#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "glad.h"
#include "glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class FirstPersonCamera
{
public:
    enum MoveKey
    {
        FORWARD = GLFW_KEY_W,
        BACKWARD = GLFW_KEY_S,
        LEFT = GLFW_KEY_A,
        RIGHT = GLFW_KEY_D
    };

    FirstPersonCamera(float posX, float posY, float posZ, GLFWwindow *window = 0)
        : m_yaw(-90.0f)
        , m_pitch(0.0f)
        , m_moveSpeed(2.5f)
        , m_mouseSensitivity(0.1f)
        , m_fovZoom(45.0f)
        , m_deltaTime(0.0f)
        , m_lastFrame(0.0f)
        , m_rotation(0.0f)
        , m_pressedkey(-1)
        , m_keyMods(-1)
        , m_mouseButton(-1)
        , m_isFirstMouse(true)
        , m_window(window)
        , m_position(glm::vec3(posX, posY, posZ))
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_up(glm::vec3(0.0f, 1.0f, 0.0f))
        , m_worldUp(m_up)
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    float rotation() const { return m_rotation; }
    float fovZoom() const { return m_fovZoom; }
    int mouseButton() const { return m_mouseButton; }
    const float *cameraPos() const { return &m_position[0]; }
    const float *cameraFront() const { return &m_front[0]; }
    void setWindowSize(GLFWwindow *win, int width, int height)
    {
        if (!m_window)
        {
            m_window = win;
            m_windowWidth = width;
            m_windowHeight = height;
            m_lastX = width / 2;
            m_lastY = height / 2;
        }
    }

    // processes input received from any keyboard-like input system.
    void ProcessWindowEvent(double currentFrame)
    {
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;
        float velocity = m_moveSpeed * m_deltaTime;

        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);

        if (glfwGetKey(m_window, FORWARD) == GLFW_PRESS)
            m_position += m_front * velocity;
        if (glfwGetKey(m_window, BACKWARD) == GLFW_PRESS)
            m_position -= m_front * velocity;
        if (glfwGetKey(m_window, LEFT) == GLFW_PRESS)
            m_position -= m_right * velocity;
        if (glfwGetKey(m_window, RIGHT) == GLFW_PRESS)
            m_position += m_right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void cursorPosCallback(GLFWwindow *, double xpos, double ypos)
    {
        if (xpos > m_windowWidth || ypos > m_windowHeight)
        {
            m_isFirstMouse = true;
            m_rotation = 0.0f;
            return;
        }

        if (m_isFirstMouse)
        {
            m_lastX = xpos;
            m_lastY = ypos;
            m_isFirstMouse = false;
        }
        double xOffset = xpos - m_lastX;
        double yOffset = m_lastY - ypos;
        m_lastX = xpos;
        m_lastY = ypos;

        if (m_pressedkey == GLFW_KEY_LEFT_CONTROL && m_keyMods & GLFW_MOD_CONTROL)
        {
            m_yaw += xOffset * m_mouseSensitivity;
            m_pitch += yOffset * m_mouseSensitivity;

            if (m_pitch > 89.0f)
                m_pitch = 89.0f;
            else if (m_pitch < -89.0f)
                m_pitch = -89.0f;

            updateCameraVectors();
        }
        else
        {
            if (m_mouseButton >= 0)
            {
                if (m_mouseButton == GLFW_MOUSE_BUTTON_LEFT)
                    m_rotation = 0.2f * xOffset;
                else if (m_mouseButton == GLFW_MOUSE_BUTTON_RIGHT)
                    m_rotation = 0.2f * yOffset;
            }
            else
            {
                m_rotation = 0.0f;
            }
        }
    }

    void mouseButtonCallback(GLFWwindow *, int button, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            m_mouseButton = button;
        }
        else
        {
            m_mouseButton = -1;
        }
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void scrollCallback(GLFWwindow *, double xOffset, double yOffset)
    {
        (void)xOffset;

        m_fovZoom -= (float)yOffset;
        if (m_fovZoom < 1.0f)
        {
            m_fovZoom = 1.0f;
        }
        else if (m_fovZoom > 45.0f)
        {
            m_fovZoom = 45.0f;
        }
    }

    void keyCallback(GLFWwindow *, int key, int scancode, int action, int mods)
    {
        m_pressedkey = key;
        m_keyMods = mods;
    }

//    typedef void (FirstPersonCamera::*pkeyCallback)(GLFWwindow *, int, int, int, int);
//    pkeyCallback kab = &FirstPersonCamera::keyCallback;
private:
    float m_yaw;
    float m_pitch;
    float m_moveSpeed;
    float m_mouseSensitivity;
    float m_fovZoom;
    float m_deltaTime; // 当前帧与上一帧的时间差
    float m_lastFrame; // 上一帧的时间
    float m_rotation;

    double m_lastX;
    double m_lastY;
    int m_windowWidth;
    int m_windowHeight;
    int m_pressedkey;
    int m_keyMods;
    int m_mouseButton;
    bool m_isFirstMouse;

    GLFWwindow *m_window;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    const glm::vec3 m_worldUp;

    // calculates the fvector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 _fr;
        _fr.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        _fr.y = sin(glm::radians(m_pitch));
        _fr.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(_fr);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};

#endif // FIRSTPERSONCAMERA_H
