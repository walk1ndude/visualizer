#version 410
in fData {
    highp vec3 color;
} frag;

out highp vec4 outColor;

void main(void) {
    outColor = vec4(frag.color, 1.0f);
}
