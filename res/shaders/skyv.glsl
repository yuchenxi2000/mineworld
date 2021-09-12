#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 skyboxview;

void main()
{
    vec4 pos = projection * skyboxview * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}

