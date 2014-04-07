#version 330 core
in highp vec4 fragTex;

uniform float rBottom;
uniform float rTop;
uniform sampler3D texSample;

out vec4 fragColor;

void main(void) {
    fragColor = texture(texSample, fragTex.stp);

    if (fragColor.r < rBottom || fragColor.r > rTop) {
        discard;
    }
}
