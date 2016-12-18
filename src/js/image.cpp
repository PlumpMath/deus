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
#endif

namespace js {
namespace {

#ifndef __EMSCRIPTEN__

class png {
public:
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

  std::string read(const std::string& name, GLsizei& cx, GLsizei& cy, GLint& format) {
    // Read file and verify signature.
    const auto file = js::file(name);
    const auto data = reinterpret_cast<png_const_bytep>(file.data());
    const auto size = static_cast<png_size_t>(file.size());
    constexpr png_size_t signature_size = 8;
    if (size < signature_size || png_sig_cmp(data, 0, signature_size)) {
      throw std::runtime_error("Invalid PNG signature image: " + name);
    }

    // Set read callback function.
    struct stream {
      std::string name;
      png_const_bytep beg;
      png_const_bytep end;
      png_const_bytep pos;
    } s = { name, data, data + size, data + signature_size };
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
      throw std::runtime_error("Unsupported color type in image: " + name);
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

private:
  png_structp png_ = nullptr;
  png_infop info_ = nullptr;
};

#endif

}  // namespace

image::image(const std::string& name) {
  open(name);
}

image::~image() {
  close();
}

void image::open(const std::string& name) {
  close();
#ifdef __EMSCRIPTEN__
  int cx = 0;
  int cy = 0;
  const auto data = emscripten_get_preloaded_image_data(name.data(), &cx, &cy);
  if (!data) {
    throw std::runtime_error("Could not load image: " + std::string(name));
  }
  if (cx && cy) {
    cx_ = cx;
    cy_ = cy;
    format_ = GL_RGBA;
    data_ = { data, static_cast<std::size_t>(cx) * static_cast<std::size_t>(cy) * 4 };
  }
#else
  png png;
  data_ = png.read(name, cx_, cy_, format_);
#endif
}

void image::close() noexcept {
#ifdef __EMSCRIPTEN__
  if (data_.data()) {
    free(const_cast<char*>(data_.data()));
  }
  data_ = {};
#else
  data_.clear();
#endif
  format_ = 0;
  cy_ = 0;
  cx_ = 0;
}

}  // namespace js