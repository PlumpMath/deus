#version 300 es
precision mediump float;

in vec2 position;
in vec2 texture_coordinates;

out vec2 vert_texture_coordinates;

void main() {
  vert_texture_coordinates = texture_coordinates;
  gl_Position = vec4(position, 0.0, 1.0);
}