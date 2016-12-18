#pragma once
#include <gl/opengl.h>
#include <gl/shader.h>
#include <functional>
#include <string_view>

namespace gl {

class program {
public:
  program() noexcept = default;

  explicit program(const shader& vert, const shader& frag) {
    handle_.reset(glCreateProgram());
    if (auto ec = get_error()) {
      throw exception("Could not create program.", ec);
    }

    glAttachShader(handle_, vert);
    if (auto ec = get_error()) {
      throw exception("Could not attach vertex shader.", ec);
    }

    glAttachShader(handle_, frag);
    if (auto ec = get_error()) {
      throw exception("Could not attach fragment shader.", ec);
    }

    glLinkProgram(handle_);
    if (auto ec = get_error()) {
      throw exception("Could not link program.", ec);
    }

    glDetachShader(handle_, frag);
    if (auto ec = get_error()) {
      throw exception("Could not detach fragment shader.", ec);
    }

    glDetachShader(handle_, vert);
    if (auto ec = get_error()) {
      throw exception("Could not detach vertex shader.", ec);
    }

    GLint success = GL_FALSE;
    glGetProgramiv(handle_, GL_LINK_STATUS, &success);
    if (auto ec = get_error()) {
      throw exception("Could not get program link status.", ec);
    }

    if (!success) {
      std::string info;
      GLsizei size = 0;
      glGetProgramiv(handle_, GL_INFO_LOG_LENGTH, &size);
      if (!get_error()) {
        info.resize(size);
        glGetProgramInfoLog(handle_, size, &size, &info[0]);
        if (!get_error()) {
          info.resize(size);
        } else {
          info.clear();
        }
      }
      throw exception("Shader compilation failed.\n" + info + "\n\n" + vert.source() + "\n\n" + frag.source());
    }
  }

  explicit program(std::string_view vert, std::string_view frag) :
    program(gl::shader(vert, GL_VERTEX_SHADER), gl::shader(frag, GL_FRAGMENT_SHADER)) {}

  GLint attribute(const char* name) const noexcept {
    return glGetAttribLocation(handle_, name);
  }

  GLint uniform(const char* name) const noexcept {
    return glGetUniformLocation(handle_, name);
  }

  operator GLuint() const noexcept {
    return handle_;
  }

  static void release(GLuint handle) noexcept {
    glDeleteProgram(handle);
  }

private:
  resource<GLuint, program> handle_;
};

}  // namespace gl