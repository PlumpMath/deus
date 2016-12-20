#pragma once
#include <gl/opengl.h>
#include <gl/arrays.h>
#include <gl/buffers.h>
#include <gl/program.h>
#include <gl/textures.h>
#include <js/input.h>
#include <chrono>

class client {
public:
  using clock = std::chrono::steady_clock;

  client();
  void resize(GLsizei cx, GLsizei cy, double pixel_ratio);
  void render();
  
  bool on_key(std::string_view s);
  bool on_key(js::key key, bool down, bool repeat);
  bool on_button(int button, double x, double y, bool down);
  bool on_scroll(double cx, double cy);
  bool on_mouse(double x, double y);
  bool on_mouse(bool enter);

private:
  gl::arrays vao_;
  gl::buffers vbo_;
  gl::program program_;
  gl::textures textures_;
  clock::time_point start_ = clock::now();
  GLfloat cx_ = 0.0f;
  GLfloat cy_ = 0.0f;
};