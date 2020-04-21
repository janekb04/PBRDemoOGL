#pragma once

#include "OpenGLWindow.h"
#include "Utility.h"

class OrbitCamera
{
private:
    float lookSpeedH = 400;
    float lookSpeedV = 400;
    float zoomSpeed = 0.5;
    float dragSpeed = 70000;
    float exponent = 2;

    float yaw = 0;
    float pitch = 0;

    Transform& cam;
public:
    OrbitCamera(Transform& cam) :
        cam{ cam }
    {
    }
public:
    void update(const OpenGLWindow& wnd, double delta_time)
    {
        auto [x, y] = wnd.cursor_delta();
        auto [w, h] = wnd.get_framebuffer_size();
        x = sgn(x) * pow(x / h, exponent);
        y = sgn(y) * pow(y / h, exponent);

        if (wnd.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_RIGHT))
        {
            yaw += lookSpeedH * -x;
            pitch -= lookSpeedV * y;

            cam.set_orientation({ pitch, yaw, 0 });
        }

        if (wnd.is_mouse_button_pressed(GLFW_MOUSE_BUTTON_MIDDLE))
        {
            cam.translate_local(glm::vec3{ -x, y, 0 } *(float)delta_time * dragSpeed);
        }

        auto scroll = wnd.get_scroll();
        scroll = sgn(scroll) * pow(scroll, exponent);

        cam.translate_local(glm::vec3{ 0, 0, -1 } *(float)scroll * zoomSpeed);
    }
};