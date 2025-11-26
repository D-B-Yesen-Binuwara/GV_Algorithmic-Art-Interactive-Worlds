#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    float yaw = -90.0f, pitch = -20.0f;
    float distance = 25.0f;
    glm::vec3 target = glm::vec3(0.0f);
    bool autoRotate = false;
    float rotationSpeed = 15.0f; // degrees per second
    Camera() {}
    glm::vec3 position() const;
    glm::mat4 viewMatrix() const;
    void processMouseDelta(float dx, float dy);
    void processScroll(float dy);
    void update(float deltaTime);
    void toggleAutoRotate();
};

#endif // CAMERA_H
