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

file::file(const std::string& name) {
  open(name);
}

file::~file() {
  close();
}

void file::open(const std::string& name) {
  close();
#ifdef __EMSCRIPTEN__
  const auto fd = ::open(name.data(), O_RDONLY);
  if (!fd) {
    throw std::runtime_error("Could not open resource: " + name);
  }
  struct stat s = {};
  if (::fstat(fd, &s) < 0) {
    ::close(fd);
    throw std::runtime_error("Could not stat resource: " + name);
  }
  if (const auto size = static_cast<std::size_t>(s.st_size)) {
    const auto data = reinterpret_cast<const char*>(::mmap(nullptr, size, PROT_READ, 0, fd, 0));
    if (!data) {
      ::close(fd);
      throw std::runtime_error("Could not mmap resource: " + name);
    }
    data_ = { data, size };
  }
  ::close(fd);
#else
  std::wstring str;
  str.resize(MultiByteToWideChar(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), nullptr, 0) + 1);
  str.resize(MultiByteToWideChar(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), &str[0], static_cast<int>(str.size())));
  auto module = GetModuleHandle(nullptr);
  auto resource = FindResource(module, str.data(), RT_RCDATA);
  if (!resource) {
    throw std::runtime_error("Could not find resource: " + name);
  }
  if (const auto size = SizeofResource(module, resource)) {
    const auto memory = LoadResource(module, resource);
    if (!memory) {
      throw std::runtime_error("Could not load resource: " + name);
    }
    const auto data = reinterpret_cast<const char*>(LockResource(memory));
    if (!data) {
      throw std::runtime_error("Could not lock resource: " + name);
    }
    data_ = { data, size };
  }
#endif
}

void file::close() noexcept {
#ifdef __EMSCRIPTEN__
  if (data_.data()) {
    ::munmap(const_cast<char*>(data_.data()), data_.size());
    data_ = {};
  }
#endif
}

}  // namespace js