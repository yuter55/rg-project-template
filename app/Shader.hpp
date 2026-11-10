//
// Created by matija on 11/9/26.
//

#ifndef PROJECT_SHADER_H
#define PROJECT_SHADER_H

#include <filesystem>
#include <glm/glm.hpp>
#include <string>


class Shader {
    unsigned int myId;

    std::string vertexPath;
    std::string fragmentPath;
    std::filesystem::file_time_type vertexTime;
    std::filesystem::file_time_type fragmentTime;

    static unsigned int build(const std::string &vertexPath, const std::string &fragmentPath);

public:
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath);

    void use();

    bool reloadIfChanged();

    void setVec2(const std::string &name, float x, float y);

    void setVec3(const std::string &name, float x, float y, float z);

    void setInt(const std::string &name, int value);

    void setMat4(const std::string &name, const glm::mat4 &matrix);

    void deleteProgram();
};


#endif//PROJECT_SHADER_H
