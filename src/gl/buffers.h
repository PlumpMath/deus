#pragma once
#include <gl/opengl.h>
#include <memory>

namespace gl {

class buffers {
public:
  buffers() noexcept = default;

  explicit buffers(std::size_t size) : handles_(std::make_unique<GLuint[]>(size)), size_(size) {
    glGenBuffers(static_cast<GLsizei>(size_), handles_.get());
    if (auto ec = get_error()) {
      throw exception("Could not generate buffer object names.", ec);
    }
  }

  buffers(buffers&& other) noexcept : size_(std::exchange(other.size_, 0)), handles_(std::move(other.handles_)) {}

  buffers& operator=(buffers&& other) noexcept {
    if (handles_) {
      glDeleteBuffers(static_cast<GLsizei>(size_), handles_.get());
    }
    size_ = std::exchange(other.size_, 0);
    handles_ = std::move(other.handles_);
    return *this;
  }

  ~buffers() {
    if (handles_) {
      glDeleteBuffers(static_cast<GLsizei>(size_), handles_.get());
    }
  }

  GLuint at(std::size_t index) const {
    if (index >= size_) {
      throw exception("Buffer index out of range.");
    }
    return handles_[index];
  }

  GLuint operator[](std::size_t index) const noexcept {
    return handles_[index];
  }

private:
  std::unique_ptr<GLuint[]> handles_;
  std::size_t size_ = 0;
};

}  // namespace gl