//
// Created by matija on 11/9/26.
//

#include "Shader.hpp"
#include <glad/glad.h>


#include <fstream>
#include <iostream>
#include <sstream>

namespace {
std::string readFile(std::string filePath) {
    std::ifstream fileStream(filePath);
    std::string line;
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}
}// namespace


Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath) {

    std::string vertexShaderSource = readFile(vertexShaderPath);
    const char *vertexShaderSourcePointer = vertexShaderSource.c_str();
    unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourcePointer, nullptr);
    glCompileShader(vertexShader);

    int success = 0;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "Failed to compile vertex shader\n"
                  << infoLog << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::string fragmentShaderSource = readFile(fragmentShaderPath);
    const char *fragmentShaderSourcePointer = fragmentShaderSource.c_str();
    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePointer, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "Failed to compile fragment shader\n"
                  << infoLog << "\n";
        std::exit(EXIT_FAILURE);
    }

    unsigned shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Failed to link shader program\n"
                  << infoLog << "\n";
        std::exit(EXIT_FAILURE);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    myId = shaderProgram;
}
void Shader::use() {
    glUseProgram(myId);
}

void Shader::setVec2(const std::string &name, float x, float y) {
    glUniform2f(glGetUniformLocation(myId, name.c_str()), x, y);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(myId, name.c_str()), x, y, z);
}
void Shader::setInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(myId, name.c_str()), value);
}
void Shader::deleteProgram() {
    glDeleteProgram(myId);
}