#pragma once

#include <glad/glad.h>

#include <cstddef>

enum class TextureSlot
{
    SLOT_0 = GL_TEXTURE0,
    SLOT_1 = GL_TEXTURE1,
    SLOT_2 = GL_TEXTURE2,
    SLOT_3 = GL_TEXTURE3,
    SLOT_4 = GL_TEXTURE4,
    SLOT_5 = GL_TEXTURE5,
    SLOT_6 = GL_TEXTURE6,
    SLOT_7 = GL_TEXTURE7,
    SLOT_8 = GL_TEXTURE8,
    SLOT_9 = GL_TEXTURE9,
    SLOT_10 = GL_TEXTURE10,
    SLOT_11 = GL_TEXTURE11,
    SLOT_12 = GL_TEXTURE12,
    SLOT_13 = GL_TEXTURE13,
    SLOT_14 = GL_TEXTURE14,
    SLOT_15 = GL_TEXTURE15
};

class Texture2D {
private:

    GLuint _id;

    size_t _width;
    size_t _height;
public:
    
    Texture2D(size_t width, size_t height, GLenum internalFormat, GLenum format, GLenum type, const void* data = nullptr);

    void bind(TextureSlot slot = TextureSlot::SLOT_0) const;

    ~Texture2D();
};