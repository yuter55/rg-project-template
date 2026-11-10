//
// Created by matija on 11/9/26.
//

#include "Shader.hpp"


#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace {
std::string readFile(const std::string &filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        std::cerr << "Cannot open file: " << filePath << "\n";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

unsigned int compileShader(GLenum type, const std::string &source, const char *name) {
    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Failed to compile " << name << " shader\n"
                  << infoLog << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

std::filesystem::file_time_type lastWrite(const std::string &path) {
    std::error_code ec;
    auto t = std::filesystem::last_write_time(path, ec);
    return ec ? std::filesystem::file_time_type::min() : t;
}
}// namespace


unsigned int Shader::build(const std::string &vertexPath, const std::string &fragmentPath) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, readFile(vertexPath), "vertex");
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, readFile(fragmentPath), "fragment");

    if (vs == 0 || fs == 0) {
        if (vs != 0) glDeleteShader(vs);
        if (fs != 0) glDeleteShader(fs);
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "Failed to link shader program\n"
                  << infoLog << "\n";
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath) {
    vertexPath = vertexShaderPath;
    fragmentPath = fragmentShaderPath;
    vertexTime = lastWrite(vertexPath);
    fragmentTime = lastWrite(fragmentPath);
    myId = build(vertexPath, fragmentPath);
}
void Shader::use() {
    glUseProgram(myId);
}
bool Shader::reloadIfChanged() {
    auto newVertexTime = lastWrite(vertexPath);
    auto newFragmentTime = lastWrite(fragmentPath);

    if (newVertexTime == vertexTime && newFragmentTime == fragmentTime) {
        return false;
    }

    // pamtimo novo vreme i kad kompajliranje ne uspe, da ne ispisujemo grešku svaki frame
    vertexTime = newVertexTime;
    fragmentTime = newFragmentTime;

    unsigned int newProgram = build(vertexPath, fragmentPath);
    if (newProgram == 0) {
        std::cout << "Shader reload failed, keeping previous version\n";
        return false;
    }

    glDeleteProgram(myId);
    myId = newProgram;
    std::cout << "Shader reloaded\n";
    return true;
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
void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) {
    glUniformMatrix4fv(glGetUniformLocation(myId, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::deleteProgram() {
    glDeleteProgram(myId);
}