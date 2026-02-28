#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ---------------------------------------------------------------
// Classe Camera FPS
// Gestisce posizione, orientamento e movimento della camera.
// Separare la camera in una classe propria ci permette in futuro
// di avere più camere, switchare tra esse, serializzarle, ecc.
// ---------------------------------------------------------------
class Camera {
public:
    // ---------------------------------------------------------------
    // Dati della camera
    // ---------------------------------------------------------------
    glm::vec3 position;  // posizione nel mondo
    glm::vec3 front;     // direzione in cui guarda
    glm::vec3 up;        // vettore "su" della camera
    glm::vec3 right;     // vettore "destra" della camera (calcolato automaticamente)

    // Angoli di Eulero che descrivono l'orientamento
    float yaw;   // rotazione orizzontale (sinistra/destra)
    float pitch; // rotazione verticale (su/giù)

    // Parametri di movimento e sensibilità
    float movementSpeed;
    float mouseSensitivity;
    float fov; // field of view in gradi

    // ---------------------------------------------------------------
    // Costruttore con valori di default ragionevoli.
    // In C++ puoi specificare valori di default direttamente
    // nella firma del costruttore — se non passi quel parametro
    // viene usato il default.
    // ---------------------------------------------------------------
    Camera(
        glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        float startYaw          = -90.0f,
        float startPitch        = 0.0f
    );

    // Restituisce la view matrix calcolata dalla posizione e orientamento
    // "const" perché non modifica lo stato della camera
    glm::mat4 getViewMatrix() const;

    // Chiamata ogni frame per gestire input tastiera WASD
    // deltaTime serve per muoversi alla stessa velocità a qualsiasi FPS
    void processKeyboard(int direction, float deltaTime);

    // Chiamata quando il mouse si muove
    // xoffset e yoffset sono quanto si è mosso dall'ultimo frame
    void processMouseMovement(float xoffset, float yoffset);

    // Chiamata quando si scrolla la rotella del mouse (zoom)
    void processMouseScroll(float yoffset);

// ---------------------------------------------------------------
// "private" — questi metodi e dati sono interni alla classe,
// non accessibili dall'esterno. updateVectors() ricalcola i vettori
// front/right/up ogni volta che yaw o pitch cambiano.
// ---------------------------------------------------------------
private:
    void updateVectors();
};

// ---------------------------------------------------------------
// Enum per le direzioni di movimento.
// Un "enum" è un insieme di costanti con nome — molto più leggibile
// di usare numeri magici tipo 0, 1, 2, 3.
// ---------------------------------------------------------------
enum CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};