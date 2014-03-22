#version 130
in highp vec4 fragTex;

uniform float rBottom;
uniform float rTop;
uniform sampler3D texSample;

void main(void) {
    gl_FragColor.rgba = texture(texSample, fragTex.stp).rrrr;

    if (gl_FragColor.r < rBottom || gl_FragColor.r > rTop) {
        discard;
    }
}
