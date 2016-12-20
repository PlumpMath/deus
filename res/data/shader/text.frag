#version 300 es
precision mediump float;

uniform sampler2D texture0;

in vec2 vert_texture0_coordinates;

out vec4 frag_color;

void main() {
  frag_color = texture(texture0, vert_texture0_coordinates);
}