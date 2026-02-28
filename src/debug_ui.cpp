#include "debug_ui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

void DebugUI::init(GLFWwindow* window) {
    // Crea il contesto ImGui — deve essere fatto una sola volta
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Stile visivo: sceliamo "dark" per sembrare una vera debug UI
    ImGui::StyleColorsDark();

    // Collega ImGui al backend GLFW e OpenGL3
    // Questi "backend" traducono gli eventi GLFW in eventi ImGui
    // e disegnano la UI usando OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void DebugUI::shutdown() {
    // Pulizia nell'ordine inverso rispetto all'inizializzazione
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::beginFrame() {
    // Ogni frame dobbiamo dire a ImGui che inizia un nuovo frame
    // L'ordine è importante: prima OpenGL3, poi GLFW, poi ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugUI::endFrame(float deltaTime, float cameraX, float cameraY, float cameraZ, int totalCubes) {
    // Disegna il pannello solo se è visibile
    if (visible) {
        // Calcola FPS dall'inverso del deltaTime
        float fps = (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f;

        // Aggiorna lo storico FPS per il grafico
        // Usiamo un "ring buffer": sovrascriviamo i valori più vecchi
        fpsHistory[fpsHistoryIndex] = fps;
        fpsHistoryIndex = (fpsHistoryIndex + 1) % FPS_HISTORY_SIZE;

        // ---------------------------------------------------------------
        // Posiziona la finestra in alto a sinistra, come il debug di Minecraft
        // ImGui::SetNextWindow* deve essere chiamato PRIMA di Begin()
        // ---------------------------------------------------------------
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_Always); // 0 = altezza automatica
        ImGui::SetNextWindowBgAlpha(0.75f); // semi-trasparente come Minecraft

        // Crea la finestra debug — ImGuiWindowFlags rimuove decorazioni inutili
        ImGui::Begin("Debug Info [F3]",
            nullptr,
            ImGuiWindowFlags_NoResize        |
            ImGuiWindowFlags_NoMove          |
            ImGuiWindowFlags_NoCollapse      |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        // --- Sezione Performance ---
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "[ Performance ]");
        ImGui::Text("FPS:        %.1f", fps);
        ImGui::Text("Frame time: %.3f ms", deltaTime * 1000.0f);

        // Grafico storico FPS — molto utile per vedere spike di lag
        ImGui::PlotLines(
            "##fps_graph",          // label vuota (## nasconde il testo)
            fpsHistory,
            FPS_HISTORY_SIZE,
            fpsHistoryIndex,        // offset: parte dal valore più vecchio
            "FPS History",
            0.0f, 200.0f,           // range Y del grafico
            ImVec2(0.0f, 50.0f)     // dimensione del grafico
        );

        ImGui::Separator(); // linea divisoria orizzontale

        // --- Sezione Camera/Posizione ---
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "[ Camera ]");
        ImGui::Text("X: %.2f", cameraX);
        ImGui::Text("Y: %.2f", cameraY);
        ImGui::Text("Z: %.2f", cameraZ);

        ImGui::Separator();

        // --- Sezione Mondo ---
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ World ]");
        ImGui::Text("Cubes: %d", totalCubes);

        ImGui::Separator();

        // --- Info controlli ---
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "[ Controls ]");
        ImGui::Text("WASD    - Move");
        ImGui::Text("Mouse   - Look");
        ImGui::Text("Scroll  - Zoom");
        ImGui::Text("F3      - Toggle debug");
        ImGui::Text("ESC     - Quit");

        ImGui::End();
    }

    // Renderizza tutto quello che ImGui ha accumulato questo frame
    // Questo deve essere chiamato SEMPRE, anche se visible è false,
    // per mantenere lo stato interno di ImGui consistente
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::toggleVisible() {
    visible = !visible;
}