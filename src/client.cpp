#include <client.h>
#include <js/file.h>
#include <js/image.h>
#include <js/log.h>
#include <algorithm>
#include <vector>

client::client(GLsizei cx, GLsizei cy) {
  // Create scene.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

  // Create scene objects.
  vao_ = gl::arrays(1);
  vbo_ = gl::buffers(2);
  textures_ = gl::textures(2);
  program_ = gl::program(js::file("shader/text.vert"), js::file("shader/text.frag"));
  const auto position = program_.attribute("position");
  const auto texture_coordinates = program_.attribute("texture_coordinates");
  const auto texture0 = program_.uniform("texture0");
  const auto texture1 = program_.uniform("texture1");
  visibility_ = program_.uniform("visibility");

  // Create vertices VBO.
  GLfloat vertices[] = {
  // Position   Coordinates
   -1.0f,  1.0f, 0.0f, 0.0f, // top-left
    1.0f,  1.0f, 1.0f, 0.0f, // top-right
    1.0f, -1.0f, 1.0f, 1.0f, // bottom-right
   -1.0f, -1.0f, 0.0f, 1.0f  // bottom-left
  };
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create elements VBO.
  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
  };
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Add enabled attributes (including their layout information and currently boud buffers) to the VAO.
  glBindVertexArray(vao_[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_[1]);
  glEnableVertexAttribArray(position);
  glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(texture_coordinates);
  glVertexAttribPointer(texture_coordinates, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);  // Must be called before rebinding the buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Tell the sampler2D uniforms which texture units they refer to.
  glUseProgram(program_);
  glUniform1i(texture0, 0);  // GL_TEXTURE0
  glUniform1i(texture1, 1);  // GL_TEXTURE1
    glUseProgram(0);

  // Initialize textures using the GL_TEXTURE0 texture unit.
  glActiveTexture(GL_TEXTURE0);

  // Initialize texture 1.
  const auto image1 = js::image("image1.png");
  glBindTexture(GL_TEXTURE_2D, textures_[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, image1.format(), image1.cx(), image1.cy(), 0, image1.format(), GL_UNSIGNED_BYTE, image1.data());
    glBindTexture(GL_TEXTURE_2D, 0);

  // Initialize texture 2.
  const auto image2 = js::image("image2.jpg");
  glBindTexture(GL_TEXTURE_2D, textures_[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, image2.format(), image2.cx(), image2.cy(), 0, image2.format(), GL_UNSIGNED_BYTE, image2.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void client::resize(GLsizei cx, GLsizei cy) {
  const auto cw = std::min(cx, cy);
  glViewport((cx - cw) / 2, (cy - cw) / 2, cw, cw);
}

void client::render() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Select proogram and use attribute information stored in the VAO.
  glUseProgram(program_);
  glBindVertexArray(vao_[0]);

  // Bind texture to GL_TEXTURE0.
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures_[0]);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures_[1]);

  // Draw elements. 
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool client::on_key(std::string_view s) {
  //js::log() << "key: '" << s << "'";
  return false;
}

bool client::on_key(js::key key, bool down, bool repeat) {
  //js::log() << "key: " << key << " (" << (down ? "down" : "up") << (repeat ? ", repeat)" : "") << ')';
  return false;
}

bool client::on_button(int button, double x, double y, bool down) {
  //js::log() << "button: " << button << ' ' << x << ' ' << y << " (" << (down ? "down" : "up") << ')';
  return false;
}

bool client::on_scroll(double cx, double cy) {
  //js::log() << "scroll: " << cx << ' ' << cy;
  if (cy > 0.0) {
    visibility_value_ += 0.05f;
  } else if (cy < 0.0) {
    visibility_value_ -= 0.05f;
  }
  glUniform1f(visibility_, visibility_value_);
  return false;
}

bool client::on_mouse(double x, double y) {
  //js::log() << "mouse: " << x << ' ' << y;
  return false;
}

bool client::on_mouse(bool enter) {
  //js::log() << "cursor: " << (enter ? "enter" : "leave");
  return false;
}