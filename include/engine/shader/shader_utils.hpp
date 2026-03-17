#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

inline std::string loadShaderFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline void checkShaderCompile(unsigned int shader, const std::string& type) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "ERROR: " << type << " shader compilation failed:\n"
                  << infoLog << std::endl;
    }
}

inline void checkProgramLink(unsigned int program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024]; 
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cerr << "ERROR: Shader program linking failed:\n"
                  << infoLog << std::endl;
    }
}
