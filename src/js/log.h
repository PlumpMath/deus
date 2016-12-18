#pragma once
#include <ostream>
#include <sstream>
#include <string_view>

namespace js {

class log : public std::stringbuf, public std::ostream {
public:
  log(bool error = false);
  log(log&& other);
  log& operator=(log&& other);
  ~log();

  template <typename T>
  log& operator<<(const T& v) {
    static_cast<std::ostream&>(*this) << v;
    return *this;
  }

  log& operator<<(const std::string_view& s) {
    static_cast<std::ostream&>(*this).write(s.data(), s.size());
    return *this;
  }

private:
  bool error_ = false;
};

inline log error() {
  return log(true);
}

}  // namespace js