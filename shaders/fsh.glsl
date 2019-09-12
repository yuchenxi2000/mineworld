#version 330 core
in vec3 position;
in vec2 texcoord;
in vec3 normal;
in float fogfactor;

out vec4 color;

uniform sampler2D BlockTexture;
uniform samplerCube skyboxtexture;

void main() {
    vec4 blockcolor = texture(BlockTexture, texcoord);
    if (blockcolor.a < 0.1) {
        discard;
    }
    vec4 skyboxcolor = texture(skyboxtexture, position);
    color = mix(blockcolor, skyboxcolor, fogfactor);
}
