#version 300 es
precision mediump float;

in vec2 position;
in vec2 texture0_coordinates;

out vec2 vert_texture0_coordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vert_texture0_coordinates = texture0_coordinates;
  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
}