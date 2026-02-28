#include "shader.h"
#include <iostream>

// ---------------------------------------------------------------
// In C++ il :: è l'operatore di "scope resolution".
// Shader::Shader significa "il costruttore della classe Shader".
// Ogni metodo definito fuori dalla classe deve avere questo prefisso.
// ---------------------------------------------------------------

Shader::Shader(const char* vertexSource, const char* fragmentSource) {
    // Compila i due shader separatamente
    unsigned int vertex   = compileShader(GL_VERTEX_SHADER,   vertexSource);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // Crea il program e collega i due shader
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // Controlla errori di linking
    int success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cerr << "Errore linking shader program: " << infoLog << "\n";
    }

    // Gli shader singoli non servono più dopo il linking — li eliminiamo
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    // RAII: quando l'oggetto Shader viene distrutto, libera la risorsa GPU
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& name, const float* value) const {
    // glGetUniformLocation trova la "posizione" della variabile uniform nello shader
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

unsigned int Shader::compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Errore compilazione shader: " << infoLog << "\n";
    }
    return shader;
}