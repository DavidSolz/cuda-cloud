#pragma once

#include <cstddef>
#include <glad/glad.h>
#include <resource.hpp>
#include <functional>

enum class TextureSlot {
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

class Texture2D;

struct Texture2DDescriptor {
  size_t width;
  size_t height;
  GLenum internalFormat;
  GLenum format;
  GLenum type;

  bool operator==(const Texture2DDescriptor &other) const {
    return width == other.width && height == other.height &&
           internalFormat == other.internalFormat && format == other.format &&
           type == other.type;
  }
};

template<>
struct std::hash<Texture2DDescriptor> {
    size_t operator()(const Texture2DDescriptor& d) const noexcept {
        size_t h = 0;

        auto combine = [](size_t& seed, size_t v) {
            seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        };

        combine(h, std::hash<size_t>{}(d.width));
        combine(h, std::hash<size_t>{}(d.height));
        combine(h, std::hash<GLenum>{}(d.internalFormat));
        combine(h, std::hash<GLenum>{}(d.format));
        combine(h, std::hash<GLenum>{}(d.type));

        return h;
    }
};

template <> struct Allocator<Texture2D> {
  static GLuint create(const Texture2DDescriptor &descriptor) {
    GLuint nativeHandle;
    glGenTextures(1, &nativeHandle);
    glBindTexture(GL_TEXTURE_2D, nativeHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, descriptor.internalFormat, descriptor.width,
                 descriptor.height, 0, descriptor.format, descriptor.type,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return nativeHandle;
  }

  static void dispose(GLuint nativeHandle) {
    if(nativeHandle != 0) {
      glDeleteTextures(1, &nativeHandle);
    }

  }
};

template <> struct ResourceTrait<Texture2D> {
  using NativeHandle = GLuint;
  using Descriptor = Texture2DDescriptor;
  using Allocator = Allocator<Texture2D>;

  static bool isValid(const NativeHandle & nativeHandle) {
    return nativeHandle != 0;
  }
};

class Texture2D : public Resource {
private:
  GLuint _id;

  Texture2DDescriptor _descriptor;

  bool _owned;

public:
  Texture2D(const Texture2D&) = delete;
  Texture2D& operator=(const Texture2D&) = delete;

  Texture2D(Texture2D&& other) noexcept;
  Texture2D& operator=(Texture2D&& other) noexcept;

  explicit Texture2D(const Texture2DDescriptor descriptor);

  Texture2D(const Texture2DDescriptor descriptor, GLuint nativeHandle);

  void bind(TextureSlot slot = TextureSlot::SLOT_0) const;

  GLuint getNativeHandle() const;

  Texture2DDescriptor getDescriptor() const;

  ~Texture2D();
};




