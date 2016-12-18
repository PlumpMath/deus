#version 300 es
precision mediump float;

layout (location = 0) in vec2 position;
layout (location = 2) in vec2 texture_coordinates;

out vec2 vert_texture_coordinates;

void main() {
  vert_texture_coordinates = texture_coordinates;
  gl_Position = vec4(position, 0.0, 1.0);
}