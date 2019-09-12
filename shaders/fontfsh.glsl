#version 330 core
in vec2 texcoord;
out vec4 color;
uniform sampler2D fonttexture;
void main() {
    vec4 _color = texture(fonttexture, texcoord);
    _color.a = max(_color.a, 0.4);
    color = _color;
}
