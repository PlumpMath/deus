# OpenGL

* <https://open.gl>

## Buffers
```cpp
client::client(GLsizei cx, GLsizei cy) {
  // Create scene.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
  glViewport(0, 0, cx, cy);

  // Create scene objects.
  vao_ = gl::arrays(1);
  vbo_ = gl::buffers(2);
  textures_ = gl::textures(2);
  program_ = gl::program(js::file("shader/text.vert"), js::file("shader/text.frag"));
  const auto position = program_.attribute("position");
  const auto texture_coordinates = program_.attribute("texture_coordinates");
  const auto texture_sampler = program_.uniform("texture_sampler");

  // Create vertices VBO.
  GLfloat vertices[] = {
  // Position   Coordinates
   -1.0f,  1.0f, 0.0f, 0.0f, // Top-left
    1.0f,  1.0f, 1.0f, 0.0f, // Top-right
    1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
   -1.0f, -1.0f, 0.0f, 1.0f  // Bottom-left
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

  // Tell the sampler2D uniform that it represents the texture bound to GL_TEXTURE0 while the object is drawn.
  glUseProgram(program_);
  glUniform1i(texture_sampler, 0);
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
  glViewport(0, 0, cx, cy);
}

void client::render() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Select proogram and use attribute information stored in the VAO.
  glUseProgram(program_);
  glBindVertexArray(vao_[0]);

  // Bind texture to GL_TEXTURE0.
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures_[1]);

  // Draw elements.
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
```