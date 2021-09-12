#version 330 core
layout (location = 0) in vec3 _position;
layout (location = 1) in vec2 _texcoord;
layout (location = 2) in vec3 _normal;

out vec3 position;
out vec2 texcoord;
out vec3 normal;

out float fogfactor;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 positionoffset;

const float fogdistance = 48.0;

void main(){
    position = _position - positionoffset;
    texcoord = _texcoord;
    normal = _normal;
    
    gl_Position = projection * view * vec4(position, 1.0);
    float d = length(position);
    fogfactor = pow(clamp(d / fogdistance, 0.0, 1.0), 4.0);
}
