#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 _uv;

out vec2 uv;

void main(){
    gl_Position = vec4(position, 1);
    uv = _uv;
}
