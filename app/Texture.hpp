//
// Created by matija on 11/9/26.
//

#ifndef PROJECT_TEXTURE_H
#define PROJECT_TEXTURE_H
#include <string>


class Texture {
    unsigned int myId = 0;

public:
    explicit Texture(const std::string &path);

    void bindTexture(unsigned int unit) const;

    void setFilter(int filter);

    void deleteTexture();
};


#endif//PROJECT_TEXTURE_H
