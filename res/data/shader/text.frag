#version 300 es
precision mediump float;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float visibility;

in vec2 vert_texture_coordinates;

out vec4 frag_color;

void main() {
  vec4 c0 = texture(texture0, vert_texture_coordinates);
  vec4 c1 = texture(texture1, vert_texture_coordinates);
  frag_color = mix(c0, c1, visibility);
}