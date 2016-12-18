#pragma once
#include <gl/opengl.h>
#include <string>
#include <string_view>

namespace js {

class image {
public:
  image() noexcept = default;

  image(const std::string& name);

  image(image&& other) noexcept = default;
  image& operator=(image&& other) noexcept = default;

  ~image();

  void open(const std::string& name);
  void close() noexcept;

  GLsizei cx() const noexcept {
    return cx_;
  }

  GLsizei cy() const noexcept {
    return cy_;
  }

  GLint format() const noexcept {
    return format_;
  }

  const char* data() const noexcept {
    return data_.data();
  }

  std::size_t size() const noexcept {
    return data_.size();
  }

  operator std::string_view() const noexcept {
    return data_;
  }

private:
  GLsizei cx_ = 0;
  GLsizei cy_ = 0;
  GLint format_ = 0;
#ifdef __EMSCRIPTEN__
  std::string_view data_;
#else
  std::string data_;
#endif
};

}  // namespace js