#pragma once
#include <string>
#include <string_view>

namespace js {

class file {
public:
  file() noexcept = default;

  file(const std::string& name);

  file(file&& other) noexcept = default;
  file& operator=(file&& other) noexcept = default;

  ~file();

  void open(const std::string& name);
  void close() noexcept;

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
  std::string_view data_;
};

}  // namespace js