//
// Created by matija on 11/12/26.
//

#ifndef PROJECT_CAMERA_H
#define PROJECT_CAMERA_H


#include <glm/glm.hpp>

enum class CameraMove { Forward,
                        Backward,
                        Left,
                        Right,
                        Up,
                        Down };

class Camera {
public:
    glm::vec3 position;

    Camera(glm::vec3 position = glm::vec3(3.0f, 2.0f, 4.0f),
           float yaw = -126.87f,
           float pitch = -21.8f);

    glm::mat4 viewMatrix() const;


    void move(CameraMove direction, float deltaTime);


    void rotate(float xoffset, float yoffset);

    void toggleWalkMode();
    bool isWalking() const { return walkMode; }


    void constrainToFloor();

    glm::vec3 getFront() const { return front; }

private:
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;

    float speed = 2.5f;
    float sensitivity = 0.1f;

    bool walkMode = false;
    float eyeHeight = 0.6f;
    float floorLimit = 2.0f;

    void updateVectors();
};


#endif//PROJECT_CAMERA_H
