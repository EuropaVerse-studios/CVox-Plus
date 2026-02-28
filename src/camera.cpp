#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <algorithm> // per std::clamp

Camera::Camera(glm::vec3 startPosition, float startYaw, float startPitch)
    // ---------------------------------------------------------------
    // Questa è la "initializer list" di C++: inizializza i membri
    // direttamente invece di assegnarli nel corpo del costruttore.
    // È più efficiente e idiomatica in C++.
    // ---------------------------------------------------------------
    : position(startPosition)
    , front(glm::vec3(0.0f, 0.0f, -1.0f))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , yaw(startYaw)
    , pitch(startPitch)
    , movementSpeed(5.0f)
    , mouseSensitivity(0.1f)
    , fov(45.0f)
{
    // Calcola subito i vettori corretti in base a yaw e pitch iniziali
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    // lookAt costruisce la view matrix da posizione, punto guardato, e vettore su
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;

    if (direction == FORWARD)  position += front * velocity;
    if (direction == BACKWARD) position -= front * velocity;
    if (direction == LEFT)     position -= right * velocity;
    if (direction == RIGHT)    position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // std::clamp è come min/max combinati: mantiene pitch tra -89 e +89
    // per evitare che la camera si ribalta
    pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera::processMouseScroll(float yoffset) {
    // Lo scroll cambia il field of view — effetto zoom
    fov = std::clamp(fov - yoffset, 1.0f, 90.0f);
}

void Camera::updateVectors() {
    // Trigonometria che converte yaw e pitch in un vettore direzione 3D.
    // Questo è il cuore matematico della camera FPS.
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // Il vettore "destra" è perpendicolare a front e al vettore su del mondo.
    // Il cross product di due vettori dà un vettore perpendicolare ad entrambi.
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Il vettore "su" della camera è perpendicolare a right e front.
    // Serve per gestire correttamente l'inclinazione della testa.
    up = glm::normalize(glm::cross(right, front));
}