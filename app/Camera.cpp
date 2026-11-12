//
// Created by matija on 11/12/26.
//

#include "Camera.hpp"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : position(position)
    , front(0.0f, 0.0f, -1.0f)
    , up(0.0f, 1.0f, 0.0f)
    , yaw(yaw)
    , pitch(pitch) {
    updateVectors();
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::move(CameraMove direction, float deltaTime) {
    float velocity = speed * deltaTime;

    glm::vec3 forward = front;
    if (walkMode) {
        forward = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    }
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    switch (direction) {
        case CameraMove::Forward:
            position += velocity * forward;
            break;
        case CameraMove::Backward:
            position -= velocity * forward;
            break;
        case CameraMove::Right:
            position += velocity * right;
            break;
        case CameraMove::Left:
            position -= velocity * right;
            break;
        case CameraMove::Up:
            if (!walkMode) position += velocity * up;
            break;
        case CameraMove::Down:
            if (!walkMode) position -= velocity * up;
            break;
    }
}

void Camera::rotate(float xoffset, float yoffset) {
    yaw += xoffset * sensitivity;
    pitch = std::clamp(pitch + yoffset * sensitivity, -89.0f, 89.0f);
    updateVectors();
}

void Camera::toggleWalkMode() {
    walkMode = !walkMode;
}

void Camera::constrainToFloor() {
    if (!walkMode) return;
    position.y = eyeHeight;
    position.x = std::clamp(position.x, -floorLimit, floorLimit);
    position.z = std::clamp(position.z, -floorLimit, floorLimit);
}

void Camera::updateVectors() {
    glm::vec3 f;
    f.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    f.y = std::sin(glm::radians(pitch));
    f.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(f);
}