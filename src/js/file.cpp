#include <js/file.h>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

namespace js {

file::file(std::string name) : name_(std::move(name)) {
#ifdef __EMSCRIPTEN__
  const auto fd = ::open(name_.data(), O_RDONLY);
  if (!fd) {
    throw std::runtime_error("Could not open resource: " + name_);
  }
  struct stat s = {};
  if (::fstat(fd, &s) < 0) {
    ::close(fd);
    throw std::runtime_error("Could not stat resource: " + name_);
  }
  if (const auto size = static_cast<std::size_t>(s.st_size)) {
    const auto data = reinterpret_cast<const char*>(::mmap(nullptr, size, PROT_READ, 0, fd, 0));
    if (!data) {
      ::close(fd);
      throw std::runtime_error("Could not mmap resource: " + name_);
    }
    data_ = { data, size };
  }
  ::close(fd);
#else
  std::wstring str;
  str.resize(MultiByteToWideChar(CP_UTF8, 0, name_.data(), static_cast<int>(name_.size()), nullptr, 0) + 1);
  str.resize(MultiByteToWideChar(CP_UTF8, 0, name_.data(), static_cast<int>(name_.size()), &str[0], static_cast<int>(str.size())));
  auto module = GetModuleHandle(nullptr);
  auto resource = FindResource(module, str.data(), RT_RCDATA);
  if (!resource) {
    throw std::runtime_error("Could not find resource: " + name_);
  }
  if (const auto size = SizeofResource(module, resource)) {
    const auto memory = LoadResource(module, resource);
    if (!memory) {
      throw std::runtime_error("Could not load resource: " + name_);
    }
    const auto data = reinterpret_cast<const char*>(LockResource(memory));
    if (!data) {
      throw std::runtime_error("Could not lock resource: " + name_);
    }
    data_ = { data, size };
  }
#endif
}

file::~file() {
#ifdef __EMSCRIPTEN__
  if (data_.data()) {
    ::munmap(const_cast<char*>(data_.data()), data_.size());
    data_ = {};
  }
#endif
}

}  // namespace js