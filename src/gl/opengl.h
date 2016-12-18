#pragma once
#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>
#else
#include <GL/glew.h>
#endif
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace gl {

enum class errc : GLenum {
  no_error = GL_NO_ERROR,
  invalid_enum = GL_INVALID_ENUM,
  invalid_value = GL_INVALID_VALUE,
  invalid_operation = GL_INVALID_OPERATION,
  invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
  out_of_memory = GL_OUT_OF_MEMORY
};

class error_category_impl : public std::error_category {
public:
  const char* name() const noexcept override {
    return "OpenGL Error";
  }

  std::string message(int code) const override {
    switch (static_cast<errc>(code)) {
    case errc::no_error: return "No error has been recorded.";
    case errc::invalid_enum: return "An unacceptable value was specified for an enumerated argument.";
    case errc::invalid_value: return "A numeric argument is out of range.";
    case errc::invalid_operation: return "The specified operation is not allowed in the current state.";
    case errc::invalid_framebuffer_operation: return "The framebuffer object is not complete.";
    case errc::out_of_memory: return "There is not enough memory left to execute the command.";
    }
    return "Unknown error code: " + std::to_string(code);
  }
};

inline const std::error_category& error_category() noexcept {
  static error_category_impl error_category;
  return error_category;
}

inline std::error_code make_error(errc code) noexcept {
  return std::error_code(static_cast<int>(code), error_category());
}

inline std::error_code make_error(GLenum code) noexcept {
  return std::error_code(static_cast<int>(code), error_category());
}

inline std::error_code get_error() noexcept {
  return make_error(glGetError());
}

class exception : public std::domain_error {
public:
  exception(const std::string& what) : std::domain_error(what) {}
  exception(const std::string& what, errc code) : exception(what, make_error(code)) {}
  exception(const std::string& what, GLenum code) : exception(what, make_error(code)) {}
  exception(const std::string& what, std::error_code ec) : exception(what + '\n' + ec.message()) {}
};

template <typename T, typename I>
class resource {
public:
  resource() noexcept = default;
  resource(T handle) noexcept : handle_(handle) {}

  resource(resource&& other) noexcept : handle_(std::exchange(other.handle_, 0)) {}

  resource& operator=(resource&& other) noexcept {
    if (handle_) {
      I::release(handle_);
    }
    handle_ = std::exchange(other.handle_, 0);
    return *this;
  }

  ~resource() {
    if (handle_) {
      I::release(handle_);
    }
  }

  void reset(GLuint handle) noexcept {
    if (handle_) {
      I::release(handle_);
    }
    handle_ = handle;
  }

  GLuint release() noexcept {
    return std::exchange(handle_, 0);
  }

  operator T() const noexcept {
    return handle_;
  }

  friend bool operator==(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ == rhs.handle_;
  }

  friend bool operator!=(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ != rhs.handle_;
  }

  friend bool operator<(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ < rhs.handle_;
  }

  friend bool operator>(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ > rhs.handle_;
  }

  friend bool operator<=(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ <= rhs.handle_;
  }

  friend bool operator>=(const resource& lhs, const resource& rhs) noexcept {
    return lhs.handle_ >= rhs.handle_;
  }

private:
  T handle_ = 0;
};

}  // namespace gl

namespace std {

template<>
struct is_error_condition_enum<gl::errc> : true_type {};

inline error_code make_error_code(gl::errc code) noexcept {
  return error_code(static_cast<int>(code), gl::error_category());
}

inline error_condition make_error_condition(gl::errc code) noexcept {
  return error_condition(static_cast<int>(code), gl::error_category());
}

}  // namespace std