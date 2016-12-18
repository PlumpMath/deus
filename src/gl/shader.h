#pragma once
#include <gl/opengl.h>
#include <string_view>

namespace gl {

class shader {
public:
  shader() noexcept = default;

  explicit shader(std::string_view src, GLenum type) {
    handle_.reset(glCreateShader(type));
    if (auto ec = get_error()) {
      throw exception("Could not create shader object.", ec);
    }

    auto data = src.data();
    auto size = static_cast<GLint>(src.size());
    glShaderSource(handle_, 1, &data, &size);
    if (auto ec = get_error()) {
      throw exception("Could not set shader source.", ec);
    }

    glCompileShader(handle_);
    if (auto ec = get_error()) {
      throw exception("Could not compile the shader.", ec);
    }

    GLint success = GL_FALSE;
    glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);
    if (auto ec = get_error()) {
      throw exception("Could not get shader compile status.", ec);
    }

    if (!success) {
      std::string info;
      GLsizei size = 0;
      glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &size);
      if (!get_error()) {
        info.resize(size);
        glGetShaderInfoLog(handle_, size, &size, &info[0]);
        if (!get_error()) {
          info.resize(size);
        } else {
          info.clear();
        }
      }
      throw exception("Shader compilation failed.\n" + info + "\n" + std::string(src));
    }
  }

  std::string source() const {
    if (!handle_ || !glIsShader(handle_)) {
      return{};
    }
    GLsizei size = 0;
    glGetShaderiv(handle_, GL_SHADER_SOURCE_LENGTH, &size);
    if (size) {
      return{};
    }
    std::string src;
    src.resize(size);
    glGetShaderSource(handle_, size, &size, &src[0]);
    src.resize(size);
    return src;
  }

  operator GLuint() const noexcept {
    return handle_;
  }

  static void release(GLuint handle) noexcept {
    glDeleteShader(handle);
  }

private:
  resource<GLuint, shader> handle_;
};

}  // namespace gl