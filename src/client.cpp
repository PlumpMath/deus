#include <client.h>
#include <js/file.h>
#include <js/image.h>
#include <js/log.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <vector>

client::client() {
  // Create scene.
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

  // Determine the maximum possible anisotropy.
  GLfloat anisotropy = 0.0f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
  anisotropy = std::min(anisotropy, 16.0f);

  // Create scene objects.
  vao_ = gl::arrays(1);
  vbo_ = gl::buffers(2);
  textures_ = gl::textures(1);
  program_ = gl::program(js::file("shader/text.vert"), js::file("shader/text.frag"));

  // Get program attribute and uniform locations.
  glUseProgram(program_);
  const auto position = program_.attribute("position");
  const auto texture0_coordinates = program_.attribute("texture0_coordinates");
  const auto texture0 = program_.uniform("texture0");
    glUseProgram(0);

  // Create vertices VBO.
  GLfloat vertices[] = {
  // Position   Coordinates
   -0.5f,  0.5f, 0.0f, 0.0f, // top-left
    0.5f,  0.5f, 1.0f, 0.0f, // top-right
    0.5f, -0.5f, 1.0f, 1.0f, // bottom-right
   -0.5f, -0.5f, 0.0f, 1.0f  // bottom-left
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
  glEnableVertexAttribArray(texture0_coordinates);
  glVertexAttribPointer(texture0_coordinates, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);  // Must be called before rebinding the buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Set the view matrix.
  glUseProgram(program_);
  glm::mat4 view;
  view = glm::lookAt(
    glm::vec3(0.0f, -1.2f, 0.6f),
    glm::vec3(0.0f, -0.1f, 0.0f),
    glm::vec3(0.0f,  1.0f, 0.0f)
  );
  glUniformMatrix4fv(program_.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
    glUseProgram(0);

  // Tell the sampler2D uniforms which texture units they refer to.
  glUseProgram(program_);
  glUniform1i(texture0, 0);  // GL_TEXTURE0
    glUseProgram(0);

  // Initialize textures using the GL_TEXTURE0 texture unit.
  glActiveTexture(GL_TEXTURE0);

  // Initialize texture 1.
  const auto image = js::image("test.png");
  glBindTexture(GL_TEXTURE_2D, textures_[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (anisotropy >= 1.0f) {
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, image.format(), image.cx(), image.cy(), 0, image.format(), GL_UNSIGNED_BYTE, image.data());
  glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void client::resize(GLsizei cx, GLsizei cy, double pixel_ratio) {
  glViewport(0, 0, cx, cy);
  cx_ = static_cast<GLfloat>(cx);
  cy_ = static_cast<GLfloat>(cy);

  glUseProgram(program_);
  glm::mat4 projection;
  projection = glm::perspective(glm::radians(45.0f), cx_ / cy_, 0.1f, 10.0f);
  glUniformMatrix4fv(program_.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);
}

void client::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Select proogram and use attribute information stored in the VAO.
  glUseProgram(program_);
  glBindVertexArray(vao_[0]);

  // Bind texture to GL_TEXTURE0.
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures_[0]);

  // Update the model translation matrix.
  glm::mat4 model;
  float time = std::chrono::duration_cast<std::chrono::duration<float>>(clock::now() - start_).count();
  model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  glUniformMatrix4fv(program_.uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

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