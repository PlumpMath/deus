#include <js/log.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <windows.h>
#endif

namespace js {
namespace {

#ifdef __EMSCRIPTEN__

// Throws a "function signature mismatch" runtime error.
//inline void write_message(const char* message, bool error) {
//  emscripten_log(error ? EM_LOG_ERROR : EM_LOG_CONSOLE, message);
//}

extern "C" void write_message(const char* message, bool error);

#else

inline void write_message(const char* message, bool error) {
  const std::string_view src(message);
  std::wstring dst;
  dst.resize(MultiByteToWideChar(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), nullptr, 0) + 1);
  dst.resize(MultiByteToWideChar(CP_UTF8, 0, src.data(), static_cast<int>(src.size()), &dst[0], static_cast<int>(dst.size())));
  dst += '\n';
  if (error) {
    dst = L"error: " + dst;
  }
  OutputDebugString(dst.data());
}

#endif

}  // namespace

log::log(bool error) : std::stringbuf(), std::ostream(this), error_(error) {}

log::log(log&& other) : std::stringbuf(std::move(other)), std::ostream(this) {}

log& log::operator=(log&& other) {
  static_cast<std::stringbuf&>(*this) = std::move(other);
  return *this;
}

log::~log() {
  try {
    auto s = str();
    auto pos = s.find_last_not_of(" \t\n\v\f\r");
    if (pos != std::string::npos) {
      s.erase(pos + 1);
      write_message(s.data(), error_);
    }
  }
  catch (...) {
  }
}

}  // namespace js