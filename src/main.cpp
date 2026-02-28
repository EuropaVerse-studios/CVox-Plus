#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "shader.h"
#include "camera.h"
#include "debug_ui.h"
#include <imgui.h>

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    void main() {
        FragColor = vec4(0.4, 0.7, 0.3, 1.0);
    }
)";

const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;

Camera  camera(glm::vec3(0.0f, 1.0f, 5.0f));
DebugUI debugUI;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool  firstMouse = true;
float lastX = SCREEN_WIDTH  / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;

// ---------------------------------------------------------------
// Teniamo traccia se F3 era premuto al frame precedente per evitare
// che il toggle si attivi più volte tenendo premuto il tasto.
// Questo pattern si chiama "edge detection" dell'input.
// ---------------------------------------------------------------
bool f3WasPressed = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // Se ImGui vuole catturare il mouse (es. ci stiamo sopra con il cursore)
    // non passiamo l'input alla camera
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll((float)yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Edge detection per F3: toglia solo quando il tasto viene premuto,
    // non continuamente mentre è tenuto giù
    bool f3IsPressed = glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS;
    if (f3IsPressed && !f3WasPressed)
        debugUI.toggleVisible();
    f3WasPressed = f3IsPressed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD,  deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT,     deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT,    deltaTime);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel Engine", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_DEPTH_TEST);

    // Inizializza il debug UI dopo aver creato il contesto OpenGL
    debugUI.init(window);

    Shader shader(vertexShaderSource, fragmentShaderSource);

    float vertices[] = {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f
    };
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,
        5, 4, 7,  7, 6, 5,
        4, 0, 3,  3, 7, 4,
        1, 5, 6,  6, 2, 1,
        3, 2, 6,  6, 7, 3,
        4, 5, 1,  1, 0, 4
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    std::vector<glm::vec3> cubePositions;
    for (int x = -2; x <= 2; x++)
        for (int z = -2; z <= 2; z++)
            cubePositions.push_back(glm::vec3((float)x, 0.0f, (float)z));

    // --- Game loop ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui: inizia il frame PRIMA di disegnare qualsiasi cosa
        debugUI.beginFrame();

        shader.use();

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.fov),
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
            0.1f, 100.0f
        );
        shader.setMat4("view",       glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        glBindVertexArray(VAO);
        for (const auto& pos : cubePositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            shader.setMat4("model", glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        // ImGui: chiudi il frame DOPO aver disegnato tutto il resto
        // passiamo i dati da mostrare nel pannello
        debugUI.endFrame(
            deltaTime,
            camera.position.x,
            camera.position.y,
            camera.position.z,
            (int)cubePositions.size()
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup nell'ordine inverso rispetto all'inizializzazione
    debugUI.shutdown();
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}