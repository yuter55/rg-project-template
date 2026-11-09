//
// Created by matija on 11/9/26.
//

#ifndef PROJECT_SHADER_H
#define PROJECT_SHADER_H

#include <string>


class Shader {
    unsigned int myId;

public:
    Shader(std::string vertexShaderPath, std::string fragmentShaderPath);

    void use();

    void setVec2(const std::string &name, float x, float y);

    void setVec3(const std::string &name, float x, float y, float z);

    void deleteProgram();
};


#endif//PROJECT_SHADER_H
