#include <texture2d.hpp>


Texture2D::Texture2D(Texture2D &&other) noexcept
    : _id(other._id), _descriptor(other._descriptor), _owned(other._owned) {
  other._id = 0;
  other._owned = false;
}

Texture2D &Texture2D::operator=(Texture2D &&other) noexcept {
  if (this != &other) {
    if (_owned) Allocator<Texture2D>::dispose(_id);
    _id = other._id;
    _descriptor = other._descriptor;
    _owned = other._owned;
    other._id = 0;
    other._owned = false;
  }
  return *this;
}

Texture2D::Texture2D(const Texture2DDescriptor descriptor)
    : _descriptor(descriptor), _owned(true) {
  _id = Allocator<Texture2D>::create(descriptor);
}

Texture2D::Texture2D(const Texture2DDescriptor descriptor, GLuint nativeHandle)
    : _descriptor(descriptor), _id(nativeHandle), _owned(false) {}

void Texture2D::bind(TextureSlot slot) const {
  glActiveTexture(static_cast<GLenum>(slot));
  glBindTexture(GL_TEXTURE_2D, _id);
}

GLuint Texture2D::getNativeHandle() const { return _id; }

Texture2DDescriptor Texture2D::getDescriptor() const { return _descriptor; }

Texture2D::~Texture2D() { 
  if (_owned) {
    Allocator<Texture2D>::dispose(_id);
  }
}
