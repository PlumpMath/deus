#pragma once
#include <gl/opengl.h>
#include <gl/arrays.h>
#include <gl/buffers.h>
#include <gl/program.h>
#include <gl/textures.h>
#include <js/input.h>

class client {
public:
  gl::arrays vao_;
  gl::buffers vbo_;
  gl::program program_;
  gl::textures textures_;
  GLint visibility_ = 0;
  float visibility_value_ = 0.0f;

  client(GLsizei cx, GLsizei cy);
  void resize(GLsizei cx, GLsizei cy);
  void render();
  
  bool on_key(std::string_view s);
  bool on_key(js::key key, bool down, bool repeat);
  bool on_button(int button, double x, double y, bool down);
  bool on_scroll(double cx, double cy);
  bool on_mouse(double x, double y);
  bool on_mouse(bool enter);
};