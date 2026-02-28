#pragma once // evita che questo header venga incluso più volte nello stesso file

#include <glad/glad.h>
#include <string>

// ---------------------------------------------------------------
// Classe Shader
// Incapsula la compilazione e l'uso di un shader program OpenGL.
// In C++ una "classe" raggruppa dati e funzioni correlate insieme —
// è il concetto base della programmazione orientata agli oggetti.
// ---------------------------------------------------------------
class Shader {
public:
    // L'ID del shader program sulla GPU
    // "unsigned int" perché OpenGL usa ID numerici positivi per le sue risorse
    unsigned int ID;

    // Costruttore: prende il codice sorgente di vertex e fragment shader
    // e li compila. In C++ il costruttore ha lo stesso nome della classe.
    Shader(const char* vertexSource, const char* fragmentSource);

    // Distruttore: chiamato automaticamente quando l'oggetto viene distrutto.
    // Il ~ davanti al nome indica che è un distruttore.
    // Qui liberiamo le risorse GPU — questo è il concetto RAII di C++:
    // "Resource Acquisition Is Initialization", le risorse si liberano
    // automaticamente quando l'oggetto esce dallo scope.
    ~Shader();

    // Attiva questo shader program per il rendering
    void use() const;

    // Metodi helper per passare valori agli uniform degli shader.
    // "const" alla fine significa che il metodo non modifica lo stato dell'oggetto.
    void setMat4(const std::string& name, const float* value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    // Funzione interna per compilare un singolo shader.
    // "private" significa che è usabile solo dentro questa classe,
    // non dall'esterno — nasconde i dettagli implementativi.
    unsigned int compileShader(unsigned int type, const char* source);
};