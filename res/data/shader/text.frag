#version 300 es
precision mediump float;

uniform sampler2D texture_sampler;

in vec2 vert_texture_coordinates;

out vec4 frag_color;

void main() {
  frag_color = texture(texture_sampler, vert_texture_coordinates);
}