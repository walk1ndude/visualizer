#version 410
in fData {
    highp vec3 color;
} frag;

out highp vec4 fragColor;

void main(void) {
    fragColor = vec4(frag.color, 1.0f);
}
