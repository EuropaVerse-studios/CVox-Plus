#pragma once

// ---------------------------------------------------------------
// DebugUI
// Gestisce il pannello di debug stile F3 di Minecraft.
// Usa Dear ImGui, una libreria immediata per UI di debug —
// "immediata" significa che ridisegni tutta la UI ogni frame
// invece di mantenere uno stato, molto più semplice per debug tools.
// ---------------------------------------------------------------

// Forward declaration: diciamo al compilatore che GLFWwindow esiste
// senza includere tutto GLFW qui — riduce i tempi di compilazione
struct GLFWwindow;

class DebugUI {
public:
    // Inizializza ImGui e lo collega alla finestra GLFW e al contesto OpenGL
    void init(GLFWwindow* window);

    // Libera le risorse ImGui — chiamato alla chiusura
    void shutdown();

    // Chiamato ogni frame prima di disegnare la UI:
    // prepara un nuovo "frame" ImGui
    void beginFrame();

    // Chiamato ogni frame dopo beginFrame():
    // disegna il pannello debug se è visibile e renderizza tutto
    void endFrame(
        float deltaTime,        // tempo dell'ultimo frame in secondi
        float cameraX,          // posizione camera X
        float cameraY,          // posizione camera Y
        float cameraZ,          // posizione camera Z
        int   totalCubes        // numero di cubi nel mondo
    );

    // Alterna la visibilità del pannello con F3
    void toggleVisible();

    // Restituisce true se il pannello è aperto
    bool isVisible() const { return visible; }

private:
    bool visible = false; // il pannello parte nascosto, F3 lo mostra

    // Dati storici per il grafico degli FPS
    // std::array è un array a dimensione fissa — più efficiente di vector
    // quando la dimensione è nota a compile time
    static constexpr int FPS_HISTORY_SIZE = 100;
    float fpsHistory[FPS_HISTORY_SIZE] = {};
    int   fpsHistoryIndex = 0;
};