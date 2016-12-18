#include <js/image.h>
#include <js/log.h>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <stdlib.h>
#else
#include <js/file.h>
#include <algorithm>
#include <vector>
#include <png.h>
#include <turbojpeg.h>
#endif

namespace js {
namespace {

#ifndef __EMSCRIPTEN__

class jpeg {
public:
  static constexpr std::size_t signature_size = 8;

  jpeg() {
    handle_ = tjInitDecompress();
    if (!handle_) {
      throw std::runtime_error("Could not initialize libjpeg.");
    }
  }

  ~jpeg() {
    tjDestroy(handle_);
  }

  std::string read(const js::file& file, GLsizei& cx, GLsizei& cy, GLint& format) {
    const auto data = reinterpret_cast<unsigned char*>(const_cast<char*>(file.data()));
    const auto size = static_cast<unsigned long>(file.size());
    int subsampling = 0;
    if (tjDecompressHeader2(handle_, data, size, &cx, &cy, &subsampling) != 0) {
      throw std::runtime_error("Could not read image header: " + file.name());
    }
    std::string buffer;
    buffer.resize(cx * cy * 3);
    if (tjDecompress2(handle_, data, size, reinterpret_cast<unsigned char*>(&buffer[0]), cx, 0, cy, TJPF_RGB, TJFLAG_FASTDCT) != 0) {
      throw std::runtime_error("Could not read image data: " + file.name());
    }
    format = GL_RGB;
    return buffer;
  }

  static bool check(const js::file& file) noexcept {
    int cx = 0;
    int cy = 0;
    int subsampling = 0;
    if (const auto handle = tjInitDecompress()) {
      const auto data = reinterpret_cast<unsigned char*>(const_cast<char*>(file.data()));
      const auto size = static_cast<unsigned long>(file.size());
      const auto success = tjDecompressHeader2(handle, data, size, &cx, &cy, &subsampling) == 0;
      tjDestroy(handle);
      return success;
    }
    return false;
  }

private:
  tjhandle handle_ = nullptr;
};

class png {
public:
  static constexpr png_size_t signature_size = 8;

  png() {
    png_ = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, [](png_structp ptr, png_const_charp msg) {
      throw std::runtime_error(msg ? msg : "png error");
    }, [](png_structp ptr, png_const_charp msg) {
      if (msg) {
        js::log() << msg;
      }
    });
    if (!png_) {
      throw std::runtime_error("Could not initialize libpng.");
    }
    info_ = png_create_info_struct(png_);
    if (!info_) {
      png_destroy_read_struct(&png_, nullptr, nullptr);
      throw std::runtime_error("Could not initialize libpng info.");
    }
  }

  ~png() {
    png_destroy_read_struct(&png_, &info_, nullptr);
  }

  std::string read(const js::file& file, GLsizei& cx, GLsizei& cy, GLint& format) {
    // Read file and verify signature.
    const auto data = reinterpret_cast<png_const_bytep>(file.data());
    const auto size = static_cast<png_size_t>(file.size());
    constexpr png_size_t signature_size = 8;
    if (size < signature_size || png_sig_cmp(data, 0, signature_size)) {
      throw std::runtime_error("Invalid PNG signature image: " + file.name());
    }

    // Set read callback function.
    struct stream {
      std::string name;
      png_const_bytep beg;
      png_const_bytep end;
      png_const_bytep pos;
    } s = { file.name(), data, data + size, data + signature_size };
    png_set_read_fn(png_, &s, [](png_structp ptr, png_bytep data, png_size_t size) {
      auto& self = *reinterpret_cast<stream*>(png_get_io_ptr(ptr));
      const auto beg = self.pos;
      const auto end = beg + size;
      if (end > self.end) {
        throw std::runtime_error("Could not read image data: " + self.name);
      }
      std::copy(beg, end, data);
      self.pos += size;
    });

    // Read image info.
    png_set_sig_bytes(png_, signature_size);
    png_read_info(png_, info_);
    const auto buffer_cx = png_get_image_width(png_, info_);
    const auto buffer_cy = png_get_image_height(png_, info_);
    cx = static_cast<GLsizei>(buffer_cx);
    cy = static_cast<GLsizei>(buffer_cy);

    // Convert channel size to one byte.
    if (png_get_bit_depth(png_, info_) < 8) {
      png_set_packing(png_);
    }

    // Convert transparency to alpha.
    if (png_get_valid(png_, info_, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_);
    }

    // Get format.
    switch (png_get_color_type(png_, info_)) {
    case PNG_COLOR_TYPE_GRAY:
      format = GL_RGB;
      png_set_gray_to_rgb(png_);
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      format = GL_RGBA;
      png_set_gray_to_rgb(png_);
      break;
    case PNG_COLOR_TYPE_PALETTE:
      format = GL_RGB;
      png_set_expand(png_);
      break;
    case PNG_COLOR_TYPE_RGB:
      format = GL_RGB;
      break;
    case PNG_COLOR_TYPE_RGBA:
      format = GL_RGBA;
      break;
    default:
      throw std::runtime_error("Unsupported color type in image: " + file.name());
    }

    // Disable interlacing.
    png_set_interlace_handling(png_);

    // Update image info.
    png_read_update_info(png_, info_);

    // Create image buffer.
    const auto bpp = png_get_rowbytes(png_, info_) / buffer_cx;
    std::string buffer;
    buffer.resize(buffer_cx * buffer_cy * bpp);

    // Read image.
    std::vector<png_bytep> rows;
    rows.resize(buffer_cy);
    for (png_uint_32 i = 0; i < buffer_cy; i++) {
      rows[i] = reinterpret_cast<png_bytep>(&buffer[i * buffer_cx * bpp]);
    }
    png_read_image(png_, &rows[0]);
    png_read_end(png_, nullptr);
    return buffer;
  }

  static bool check(const js::file& file) noexcept {
    if (file.size() > signature_size) {
      return png_sig_cmp(reinterpret_cast<png_const_bytep>(file.data()), 0, signature_size) == 0;
    }
    return false;
  }

private:
  png_structp png_ = nullptr;
  png_infop info_ = nullptr;
};

#endif

}  // namespace

image::image(const std::string& name) {
#ifdef __EMSCRIPTEN__
  const auto data = emscripten_get_preloaded_image_data(name.data(), &cx_, &cy_);
  if (!data) {
    throw std::runtime_error("Could not load image: " + name);
  }
  format_ = GL_RGBA;
  data_ = { data, static_cast<std::size_t>(cx_) * static_cast<std::size_t>(cy_) * 4 };
#else
  const auto file = js::file(name);
  if (png::check(file)) {
    png png;
    data_ = png.read(file, cx_, cy_, format_);
    return;
  }
  if (jpeg::check(file)) {
    jpeg jpeg;
    data_ = jpeg.read(file, cx_, cy_, format_);
    return;
  }
  throw std::runtime_error("Could not load image: " + name);
#endif
}

image::~image() {
#ifdef __EMSCRIPTEN__
  if (data_.data()) {
    free(const_cast<char*>(data_.data()));
  }
#endif
}

}  // namespace js