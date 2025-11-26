#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

glm::vec3 Camera::position() const {
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return target - dir * distance;
}
glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position(), target, glm::vec3(0,1,0));
}
void Camera::processMouseDelta(float dx, float dy) {
    yaw += dx * 0.1f;
    pitch += dy * 0.1f;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}
void Camera::processScroll(float dy) {
    distance -= dy;
    if (distance < 5.0f) distance = 5.0f;
    if (distance > 150.0f) distance = 150.0f;
}

void Camera::update(float deltaTime) {
    if (autoRotate) {
        yaw += rotationSpeed * deltaTime;
        if (yaw >= 360.0f) yaw -= 360.0f;
    }
}

void Camera::toggleAutoRotate() {
    autoRotate = !autoRotate;
}
