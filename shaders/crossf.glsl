#version 330 core
in vec2 uv;
out vec4 color;
uniform sampler2D crosstex;

void main() {
    vec4 c = texture(crosstex, uv);
    if (c.a < 0.1) discard;
    color = vec4(0, 0, 0, 1);
}
