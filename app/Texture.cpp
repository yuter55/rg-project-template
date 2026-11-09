//
// Created by matija on 11/9/26.
//

#include "Texture.hpp"
#include <glad/glad.h>

#include <iostream>
#include <stb_image.h>


Texture::Texture(const std::string &path) {
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Failed to load texture" << path << "\n";
        return;
    }

    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    glGenTextures(1, &myId);
    glBindTexture(GL_TEXTURE_2D, myId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
void Texture::bindTexture(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, myId);
}
void Texture::setFilter(int filter) {
    glBindTexture(GL_TEXTURE_2D, myId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}
void Texture::deleteTexture() {
    glDeleteTextures(1, &myId);
}
